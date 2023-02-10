#include "graph.h"

// Leave only those vertices to which non-lambda transitions exist
void copy_non_lambda_vertices(const graph_t *source_graph, graph_t *new_graph)
{
    const adjacecny_list_t *list;
    node_t *root;

    list = &source_graph->adjacency_list;

    for (size_t i = 0; i < list->max_size; i++)
    {
        root = list->vertices[i];
        if (root == NULL)
            continue;
        // printf("root_num: %d\n", root->number);
        // printf("root_val: %c\n", root->value);
        if ((root->value & INPUT_VERTEX) == INPUT_VERTEX)
        {
            graph_add_vertex(new_graph, root->number, root->value);
        }

        for (node_t *curr = root->next; curr != NULL; curr = curr->next)
        {
            // printf("curr_num: %d\n", curr->number);
            // printf("curr_val: %c\n", curr->value);
            if (curr->value != '~')
            {
                graph_add_vertex(new_graph, curr->number, list->vertices[curr->number]->value);
            }
        }
    }
}

int add_non_lambda_arcs(const graph_t *source_graph, graph_t *new_graph, history_t *history)
{
    queue_t *queue;
    const adjacecny_list_t *list;
    const adjacecny_list_t *new_list;
    node_t *root;
    node_t *curr;
    int number;
    int rc;
    bool found_lambda;

    list = &source_graph->adjacency_list;
    new_list = &new_graph->adjacency_list;

    queue = new_queue();
    if (queue == NULL)
        return ERR_NO_MEMORY;

    found_lambda = true;
    while (found_lambda)
    {
        found_lambda = false;

        for (size_t i = 0; i < list->max_size; i++)
        {
            root = list->vertices[i];
            if (root == NULL || new_list->vertices[root->number] == NULL)
                continue;

            for (curr = root->next; curr != NULL; curr = curr->next)
            {
                if (curr->value != '~')
                {
                    if (!graph_contains_arc(new_graph, root->number, curr->number, curr->value))
                    {
                        rc = update_arc(source_graph, new_graph, history, root->number, curr->number, curr->value);
                        if (rc != EXIT_SUCCESS)
                        {
                            free_queue(queue);
                            free_history(history);
                            return rc;
                        }
                        found_lambda = true;
                    }

                    continue;
                }
                if (list->vertices[curr->number] != NULL && is_output_vertex(list->vertices[curr->number]) && !is_output_vertex(new_list->vertices[root->number]))
                {
                    rc = update_vertex(new_graph, history, root->number, list->vertices[curr->number]->value);
                    if (rc != EXIT_SUCCESS)
                    {
                        free_queue(queue);
                        free_history(history);
                        return rc;
                    }
                }
                if (!queue_contains(queue, curr->number))
                {
                    rc = queue_push(queue, curr->number);
                    if (rc != EXIT_SUCCESS)
                    {
                        free_queue(queue);
                        return rc;
                    }
                }
            }

            while (!is_queue_empty(queue))
            {
                rc = queue_pop(queue, &number);
                if (rc != EXIT_SUCCESS)
                {
                    free_queue(queue);
                    free_history(history);
                    return rc;
                }
                for (curr = list->vertices[number]->next; curr != NULL; curr = curr->next)
                {
                    if (curr->value != '~')
                    {
                        if (!graph_contains_arc(new_graph, root->number, curr->number, curr->value))
                        {
                            rc = update_arc(source_graph, new_graph, history, root->number, curr->number, curr->value);
                            if (rc != EXIT_SUCCESS)
                            {
                                free_queue(queue);
                                free_history(history);
                                return rc;
                            }
                            found_lambda = true;
                        }
                        continue;
                    }
                    if (list->vertices[curr->number] != NULL && is_output_vertex(list->vertices[curr->number]) && !is_output_vertex(new_list->vertices[root->number]))
                    {
                        rc = update_vertex(new_graph, history, root->number, list->vertices[curr->number]->value);
                        if (rc != EXIT_SUCCESS)
                        {
                            free_queue(queue);
                            free_history(history);
                            return rc;
                        }
                    }
                    if (!queue_contains(queue, curr->number) && number != curr->number && number != root->number)
                    {
                        rc = queue_push(queue, curr->number);
                        if (rc != EXIT_SUCCESS)
                        {
                            free_queue(queue);
                            free_history(history);
                            return rc;
                        }
                    }
                }
            }
        }
    }

    free_queue(queue);

    return EXIT_SUCCESS;
}

int remove_arcs_to_node(graph_t *graph, history_t *history, int number)
{
    int rc;
    node_t *root;
    node_t *next;
    operation_t *operation;
    adjacecny_list_t *list;

    list = &graph->adjacency_list;

    for (size_t i = 0; i < list->max_size; i++)
    {
        if (i == number)
            continue;

        root = list->vertices[i];
        if (root == NULL)
            continue;

        for (node_t *curr = root->next; curr != NULL; curr = next)
        {
            next = curr->next;

            if (curr->number != number)
                continue;

            operation = operation_remove_arc(root->number, curr->number, curr->value);
            if (operation == NULL)
                return ERR_NO_MEMORY;
            history_push(history, operation);

            rc = apply_operation(graph, operation);
            if (rc != EXIT_SUCCESS)
                return rc;
        }
    }

    return EXIT_SUCCESS;
}

int remove_lambda_nodes(graph_t *new_graph, graph_t *no_lambda_graph, history_t *history)
{
    int rc;
    const adjacecny_list_t *new_list;
    const adjacecny_list_t *no_lambda_list;
    node_t *root;
    node_t *next;
    operation_t *operation;

    new_list = &new_graph->adjacency_list;
    no_lambda_list = &no_lambda_graph->adjacency_list;

    // puts("remove lambda nodes");
    for (size_t i = 0; i < new_list->max_size; i++)
    {
        root = new_list->vertices[i];
        if (root == NULL || no_lambda_list->vertices[i] != NULL)
            continue;

        // Remove lambda node's arcs
        for (node_t *curr = root->next; curr != NULL; curr = next)
        {
            next = curr->next;

            // printf("deleted arc: from: %d to: %d val: %d\n", root->number, curr->number, curr->value);
            operation = operation_remove_arc(root->number, curr->number, curr->value);
            if (operation == NULL)
                return ERR_NO_MEMORY;
            history_push(history, operation);

            rc = apply_operation(new_graph, operation);
            if (rc != EXIT_SUCCESS)
            {
                fputs("Remove arc operation from lambda node error\n", stderr);
                return rc;
            }
        }

        // Remove arcs to lambda node
        rc = remove_arcs_to_node(new_graph, history, root->number);
        if (rc != EXIT_SUCCESS)
        {
            fputs("Remove arcs to lambda node error\n", stderr);
            return rc;
        }

        // Remove lambda node
        // printf("deleted node: from: %d val: %d\n", root->number, root->value);
        operation = operation_remove_vertex(root->number, root->value);
        if (operation == NULL)
            return ERR_NO_MEMORY;
        history_push(history, operation);

        rc = apply_operation(new_graph, operation);
        if (rc != EXIT_SUCCESS)
        {
            fputs("Remove lambda node operation error\n", stderr);
            return rc;
        }
    }

    return EXIT_SUCCESS;
}

int remove_lambda_arcs(graph_t *new_graph, graph_t *no_lambda_graph, history_t *history)
{
    int rc;
    const adjacecny_list_t *new_list;
    const adjacecny_list_t *no_lambda_list;
    node_t *root;
    node_t *next;
    operation_t *operation;

    new_list = &new_graph->adjacency_list;
    no_lambda_list = &no_lambda_graph->adjacency_list;

    // puts("remove lambda arcs");
    // print_adjacency_list(new_graph);
    for (size_t i = 0; i < new_list->max_size; i++)
    {
        root = new_list->vertices[i];
        if (root == NULL || no_lambda_list->vertices[root->number] == NULL)
            continue;

        for (node_t *curr = root->next; curr != NULL; curr = next)
        {
            next = curr->next;
            if (curr->value != '~' || new_list->vertices[curr->number] == NULL)
                continue;

            // printf("removed arc: %d->%d (%c)\n", root->number, curr->number, curr->value);
            operation = operation_remove_arc(root->number, curr->number, curr->value);
            if (operation == NULL)
                return ERR_NO_MEMORY;
            history_push(history, operation);

            rc = apply_operation(new_graph, operation);
            if (rc != EXIT_SUCCESS)
            {
                fputs("Remove lambda arc from non-lambda node operation error\n", stderr);
                return rc;
            }
        }
    }

    return EXIT_SUCCESS;
}

history_t *get_lambda_transition_deletion_history(const graph_t *source_graph)
{
    int rc;
    graph_t *no_lambda_graph;
    graph_t *new_graph;
    history_t *history;

    history = create_history();
    if (history == NULL)
        return NULL;

    new_graph = graph_copy(source_graph);
    if (new_graph == NULL)
        return NULL;

    no_lambda_graph = create_graph(INIT_GRAPH_SIZE);
    if (no_lambda_graph == NULL)
        return NULL;

    copy_non_lambda_vertices(source_graph, no_lambda_graph);

    rc = add_non_lambda_arcs(source_graph, no_lambda_graph, history);
    if (rc != EXIT_SUCCESS)
    {
        fputs("Error creating history\n", stderr);
        free_graph(no_lambda_graph);
        return NULL;
    }

    rc = remove_lambda_nodes(new_graph, no_lambda_graph, history);
    if (rc != EXIT_SUCCESS)
    {
        fputs("Error removing lambda nodes\n", stderr);
        free_graph(no_lambda_graph);
        return NULL;
    }

    // Remove lambda arcs from remaining non-lambda vertices
    rc = remove_lambda_arcs(new_graph, no_lambda_graph, history);
    if (rc != EXIT_SUCCESS)
    {
        fputs("Error removing lambda arcs\n", stderr);
        free_graph(no_lambda_graph);
        return NULL;
    }

    // print_history(history);

    free_graph(no_lambda_graph);
    free_graph(new_graph);

    return history;
}
