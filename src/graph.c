#include "graph.h"
#include <unistd.h>
#include "graphviz.h"

typedef struct node node_t;

struct node
{
    int number;
    char value;
    node_t *next;
};

typedef struct adjacency_list
{
    node_t **vertices;
    size_t max_size;
} adjacecny_list_t;

struct graph
{
    adjacecny_list_t adjacency_list;
    operation_t *last_operation;
};

node_t *new_node(int number, char value)
{
    node_t *node = malloc(sizeof(node_t));
    if (node == NULL)
    {
        fputs("Ошибка выделения памяти под узел списка\n", stderr);
        return NULL;
    }
    node->number = number;
    node->value = value;
    node->next = NULL;
    return node;
}

int list_push_end(node_t *head, node_t *push_node)
{
    node_t *curr;

    if (head == NULL)
        return EXIT_FAILURE;

    curr = head;
    while (curr->next != NULL)
    {
        curr = curr->next;
    }

    curr->next = push_node;

    return EXIT_SUCCESS;
}

graph_t *create_graph(size_t init_size)
{
    node_t **vertices;
    graph_t *new_graph;

    new_graph = malloc(sizeof(graph_t));
    if (new_graph == NULL)
    {
        fputs("Ошибка выделения памяти под структуру графа в виде списка\n", stderr);
        return NULL;
    }

    vertices = malloc(init_size * sizeof(node_t*));
    if (vertices == NULL)
    {
        free(new_graph);
        fputs("Ошибка выделения памяти под список указателей на вершины списка смежности\n", stderr);
        return NULL;
    }
    new_graph->adjacency_list.vertices = vertices;
    new_graph->adjacency_list.max_size = init_size;

    for (size_t i = 0; i < init_size; i++)
    {
        vertices[i] = NULL;
    }

    new_graph->last_operation = NULL;

    return new_graph;
}

int graph_expand(graph_t *graph, size_t new_size)
{
    node_t **new_vertices;
    size_t new_max_size = (size_t)(new_size * GRAPH_EXPAND_FACTOR);
    new_vertices = realloc(graph->adjacency_list.vertices, sizeof(int*) * new_max_size);
    if (new_vertices == NULL)
    {
        fputs("Error while expanding graph\n", stderr);
        return EXIT_FAILURE;
    }
    graph->adjacency_list.vertices = new_vertices;

    for (size_t i = graph->adjacency_list.max_size; i < new_max_size; i++)
    {
        new_vertices[i] = NULL;
    }

    graph->adjacency_list.max_size = new_max_size;

    return EXIT_SUCCESS;
}

void free_list(node_t *head)
{
    node_t *curr;
    node_t *next;

    curr = head;
    while (curr != NULL)
    {
        next = curr->next;
        free(curr);
        curr = next;
    }
}

void free_graph(graph_t *graph)
{
    for (size_t i = 0; i < graph->adjacency_list.max_size; i++)
    {
        free_list(graph->adjacency_list.vertices[i]);
    }
    free(graph->adjacency_list.vertices);
    free(graph);
}

int graph_remove_vertex(graph_t *graph, int number, vertex_type_t vertex_type)
{
    adjacecny_list_t *list;
    list = &graph->adjacency_list;
    node_t *root;

    root = list->vertices[number];
    if (root == NULL)
        return ERR_NULL_POINTER;

    root->value -= root->value & vertex_type;
    if (root->value == NO_VERTEX)
    {
        if (root->next != NULL)
            return EXIT_FAILURE;
        free(root);
        list->vertices[number] = NULL;
    }
    else
    {
        // root->value |= INTER_VERTEX;
    }

    return EXIT_SUCCESS;
}

int graph_remove_arc(graph_t *graph, const int from_number, const int to_number, char value)
{
    adjacecny_list_t *list;
    node_t *curr;
    node_t *prev;

    list = &graph->adjacency_list;
    prev = list->vertices[from_number];

    if (prev == NULL)
        return ERR_NULL_POINTER;

    while (prev->next != NULL)
    {
        curr = prev->next;
        // printf("from: %d to: %d val: %d\n", from_number, curr->number, curr->value);
        if (to_number == curr->number && value == curr->value)
        {
            prev->next = curr->next;
            // printf("deleted: from: %d to: %d val: %d\n", from_number, curr->number, curr->value);
            free(curr);
            return EXIT_SUCCESS;
        }
        prev = prev->next;
    }

    return EXIT_FAILURE;
}

int graph_add_vertex(graph_t *graph, int number, vertex_type_t vertex_type)
{
    node_t *vertex;
    adjacecny_list_t *list;

    list = &graph->adjacency_list;

    vertex = list->vertices[number];
    if (vertex != NULL)
    {
        vertex->value = vertex->value | vertex_type;
        return EXIT_SUCCESS;
    }

    if (list->max_size <= number)
    {
        fputs("Номер создаваемой вершины превышает максимальный\n", stderr);
        return ERR_INPUT_NUMBER;
    }

    vertex = new_node(number, vertex_type);
    if (vertex == NULL)
        return ERR_NO_MEMORY;

    list->vertices[number] = vertex;

    return EXIT_SUCCESS;
}

int graph_add_arc(graph_t *graph, const int from_number, const int to_number, const char value)
{
    int rc;
    node_t *list_node;
    adjacecny_list_t *list;

    list = &graph->adjacency_list;

    // Add root vertex
    if (list->vertices[from_number] == NULL)
    {
        rc = graph_add_vertex(graph, from_number, INTER_VERTEX);
        if (rc != EXIT_SUCCESS)
            return rc;
    }

    // Add arc
    list_node = new_node(to_number, value);
    if (list_node == NULL)
        return ERR_NO_MEMORY;

    rc = list_push_end(list->vertices[from_number], list_node);
    if (rc != EXIT_SUCCESS)
        return rc;

    return EXIT_SUCCESS;
}

size_t char_count(const char *str, char ch)
{
    size_t counter = 0;

    while (*str != '\0')
    {
        if (*str == ch)
            counter++;
        str++;
    }

    return counter;
}

int scan_graph_arc(FILE *file, graph_t *graph)
{
    int rc;
    int from_number;
    int to_number;
    int max_number;
    char value;
    char *str = NULL;
    size_t str_len = 0;
    size_t dashes;

    rc = getline(&str, &str_len, file);
    if (rc == -1)
    {
        free(str);
        return EXIT_FAILURE;
    }

    dashes = char_count(str, '-');
    // printf("dashes: %lu\n", dashes);

    switch (dashes)
    {
        case 1:
            rc = sscanf(str, "%d->%d %c", &from_number, &to_number, &value);
            if (rc != 3)
            {
                free(str);
                return ERR_INPUT_NUMBER;
            }
            free(str);
            // printf("%d: %d->%d %c\n", rc, from_number, to_number, value);

            max_number = MAX(from_number, to_number);
            if (graph->adjacency_list.max_size < max_number)
            {
                rc = graph_expand(graph, max_number);
                if (rc != EXIT_SUCCESS)
                    return rc;
            }

            rc = graph_add_vertex(graph, to_number, INTER_VERTEX);
            if (rc != EXIT_SUCCESS)
                return rc;

            rc = graph_add_arc(graph, from_number, to_number, value);
            if (rc != EXIT_SUCCESS)
                return rc;
            break;
        case 2:
            if (str[0] == '-')
            {
                rc = sscanf(str, "-->%d", &to_number);
                if (rc != 1)
                {
                    free(str);
                    return ERR_INPUT_NUMBER;
                }
                free(str);

                rc = graph_add_vertex(graph, to_number, INPUT_VERTEX);
                if (rc != EXIT_SUCCESS)
                    return rc;
            }
            else
            {
                rc = sscanf(str, "%d-->", &from_number);
                if (rc != 1)
                {
                    free(str);
                    return ERR_INPUT_NUMBER;
                }
                free(str);

                rc = graph_add_vertex(graph, from_number, OUTPUT_VERTEX);
                if (rc != EXIT_SUCCESS)
                    return rc;
            }
            break;
        default:
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

graph_t *graph_from_file(char *filename)
{
    graph_t *graph;
    FILE *file;
    int rc;
    size_t counter;
    char ch;

    file = fopen(filename, READ_MODE);
    if (file == NULL)
    {
        fprintf(stderr, "Ошибка открытия файла \"%s\"\n", filename);
        return NULL;
    }
    graph = create_graph(INIT_GRAPH_SIZE);
    if (graph == NULL)
        return NULL;

    counter = 0;
    do
    {
        rc = scan_graph_arc(file, graph);
        if (rc == EXIT_SUCCESS)
            counter++;
    }
    while (rc == EXIT_SUCCESS);

    ch = fgetc(file);
    if (ch != EOF)
    {
        fputs("Файл повреждён\n", stderr);
        free_graph(graph);
        fclose(file);
        return NULL;
    }

    rc = fclose(file);
    if (rc == EOF)
    {
        fputs("Ошибка закрытия файла\n", stderr);
        free_graph(graph);
        return NULL;
    }

    if (counter == 0)
    {
        fputs("Не было введено ни одной вершины\n", stderr);
        free_graph(graph);
        return NULL;
    }

    return graph;
}

int graph_contains_arc(const graph_t *graph, int from_number, int to_number, char value)
{
    node_t *root;
    node_t *curr;
    const adjacecny_list_t *list;
    list = &graph->adjacency_list;

    root = list->vertices[from_number];
    if (root == NULL)
        return 0;

    for (curr = root->next; curr != NULL; curr = curr->next)
    {
        if (curr->number == to_number && curr->value == value)
            return 1;
    }

    return 0;
}

// Leave only those vertices to which non-lambda transitions exist
void filter_vertices(const graph_t *source_graph, graph_t *new_graph)
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

int update_vertex(graph_t *graph, history_t *history, int number, char value)
{
    operation_t *operation;
    int rc;
    char current_value = 0;

    if (graph->adjacency_list.vertices[number] != NULL)
        current_value = graph->adjacency_list.vertices[number]->value;

    // printf("update| num: %d, curr: %d, new: %d\n", number, current_value, value);

    operation = operation_add_vertex(number, value - (value & current_value));
    if (operation == NULL)
        return ERR_NO_MEMORY;
    history_push(history, operation);

    rc = graph_add_vertex(graph, number, value);
    if (rc != EXIT_SUCCESS)
        return rc;

    return EXIT_SUCCESS;
}

int update_arc(const graph_t *source_graph, graph_t *new_graph, history_t *history, int from_number, int to_number, char value)
{
    operation_t *operation;
    int rc;

    rc = graph_add_arc(new_graph, from_number, to_number, value);
    if (rc != EXIT_SUCCESS)
        return rc;

    if (!graph_contains_arc(source_graph, from_number, to_number, value))
    {
        operation = operation_add_arc(from_number, to_number, value);
        if (operation == NULL)
            return ERR_NO_MEMORY;
        history_push(history, operation);
    }

    return EXIT_SUCCESS;
}

bool is_output_vertex(node_t *vertex)
{
    return (vertex->value & OUTPUT_VERTEX) == OUTPUT_VERTEX;
}

int add_non_lambda_transitions(const graph_t *source_graph, graph_t *new_graph, history_t *history)
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

int remove_arcs_to_node(const adjacecny_list_t *list, history_t *history, int number)
{
    node_t *root;
    operation_t *operation;

    for (size_t i = 0; i < list->max_size; i++)
    {
        if (i == number)
            continue;

        root = list->vertices[i];
        if (root == NULL)
            continue;

        for (node_t *curr = root->next; curr != NULL; curr = curr->next)
        {
            if (curr->number != number)
                continue;

            operation = operation_remove_arc(root->number, curr->number, curr->value);
            if (operation == NULL)
                return ERR_NO_MEMORY;
            history_push(history, operation);
        }
    }

    return EXIT_SUCCESS;
}

int remove_lambda_nodes(const graph_t *source_graph, graph_t *new_graph, history_t *history)
{
    int rc;
    const adjacecny_list_t *list;
    const adjacecny_list_t *new_list;
    node_t *root;
    operation_t *operation;

    list = &source_graph->adjacency_list;
    new_list = &new_graph->adjacency_list;

    for (size_t i = 0; i < list->max_size; i++)
    {
        root = list->vertices[i];
        if (root == NULL || new_list->vertices[root->number] != NULL)
            continue;

        rc = remove_arcs_to_node(list, history, root->number);
        if (rc != EXIT_SUCCESS)
            return rc;

        for (node_t *curr = root->next; curr != NULL; curr = curr->next)
        {
            operation = operation_remove_arc(root->number, curr->number, curr->value);
            if (operation == NULL)
                return ERR_NO_MEMORY;
            history_push(history, operation);
        }
        operation = operation_remove_vertex(root->number, root->value);
        if (operation == NULL)
            return ERR_NO_MEMORY;
        history_push(history, operation);
    }

    return EXIT_SUCCESS;
}

graph_t *remove_lambda_transitions(const graph_t *graph, history_t *history)
{
    int rc;
    graph_t *new_graph = create_graph(INIT_GRAPH_SIZE);

    filter_vertices(graph, new_graph);

    rc = add_non_lambda_transitions(graph, new_graph, history);
    if (rc != EXIT_SUCCESS)
    {
        free_graph(new_graph);
        return NULL;
    }

    rc = remove_lambda_nodes(graph, new_graph, history);
    if (rc != EXIT_SUCCESS)
    {
        free_graph(new_graph);
        return NULL;
    }

    return new_graph;
}

int apply_operation(graph_t *graph, operation_t *operation)
{
    int rc;

    if (operation->operation_type == REMOVE)
    {
        if (operation->to_number == NO_VALUE)
        {
            rc = graph_remove_vertex(graph, operation->from_number, operation->value);
        }
        else
        {
            rc = graph_remove_arc(graph, operation->from_number, operation->to_number, operation->value);
        }
    }
    else if (operation->operation_type == ADD)
    {
        if (operation->to_number == NO_VALUE)
        {
            rc = graph_add_vertex(graph, operation->from_number, operation->value);
        }
        else
        {
            rc = graph_add_arc(graph, operation->from_number, operation->to_number, operation->value);
        }
    }
    else
    {
        return EXIT_FAILURE;
    }

    return rc;
}

int undo_operation(graph_t *graph, operation_t *operation)
{
    int rc;

    if (operation->operation_type == REMOVE)
    {
        if (operation->to_number == NO_VALUE)
        {
            rc = graph_add_vertex(graph, operation->from_number, operation->value);
        }
        else
        {
            rc = graph_add_arc(graph, operation->from_number, operation->to_number, operation->value);
        }
    }
    else if (operation->operation_type == ADD)
    {
        if (operation->to_number == NO_VALUE)
        {
            rc = graph_remove_vertex(graph, operation->from_number, operation->value);
        }
        else
        {
            rc = graph_remove_arc(graph, operation->from_number, operation->to_number, operation->value);
        }
    }
    else
    {
        return EXIT_FAILURE;
    }

    return rc;
}

int history_prev(history_t *history, graph_t *graph)
{
    operation_t *operation;
    int rc;

    if (history->curr == history->head)
    {
        puts("ALREADY AT THE FIRST HISTORY PAGE");
        return EXIT_SUCCESS;
    }

    if (history->prev != history->curr)
        history->curr = history->curr->prev;
    history->prev = history->curr->prev;

    operation = history->curr;

    if (operation == history->head)
    {
        puts("FIRST HISTORY PAGE");
    }

    printf("UNDO %s: %d %d %d\n", operation->operation_type == ADD ? "ADD" : "REMOVE", operation->from_number, operation->to_number, operation->value);
    if (history->curr == NULL)
    {
        fputs("Previous operation is null pointer", stderr);
        return EXIT_FAILURE;
    }

    rc = undo_operation(graph, operation);
    if (rc != EXIT_SUCCESS)
        return rc;

    graph->last_operation = operation->prev;

    return EXIT_SUCCESS;
}

int history_next(history_t *history, graph_t *graph)
{
    operation_t *operation;
    int rc;

    if (history->prev == history->curr)
    {
        puts("ALREADY AT THE LAST HISTORY PAGE");
        return EXIT_SUCCESS;
    }

    operation = history->curr;

    if (operation == history->tail)
    {
        puts("LAST HISTORY PAGE");
    }

    printf("APPLY %s: %d %d %d\n", operation->operation_type == ADD ? "ADD" : "REMOVE", operation->from_number, operation->to_number, operation->value);
    if (history->curr == NULL)
        return EXIT_FAILURE;

    rc = apply_operation(graph, operation);
    if (rc != EXIT_SUCCESS)
        return rc;

    graph->last_operation = operation;

    history->prev = history->curr;
    if (history->curr->next != NULL)
        history->curr = history->curr->next;

    return EXIT_SUCCESS;
}

int write_graph_connections(FILE *file, const graph_t *graph, void *arg)
{
    int rc;
    char *color;
    char *style;
    char *arc_color;
    const adjacecny_list_t *list;
    node_t *root;
    // graph_t *new_graph = (graph_t*)arg;

    list = &graph->adjacency_list;

    for (size_t i = 0; i < list->max_size; i++)
    {
        root = list->vertices[i];
        if (root == NULL)
            continue;

        if ((root->value & INPUT_VERTEX) == INPUT_VERTEX)
        {
            rc = fprintf(file, "0->%d;\n", root->number);
            if (rc == EOF)
                return rc;
            rc = fprintf(file, "0 [style=\"invisible\"];\n");
            if (rc == EOF)
                return rc;
        }

        if ((root->value & OUTPUT_VERTEX) == OUTPUT_VERTEX)
        {
            arc_color = "";

            if (graph->last_operation != NULL &&
                graph->last_operation->from_number == root->number &&
                graph->last_operation->to_number == NO_VALUE &&
                (graph->last_operation->value & OUTPUT_VERTEX) == OUTPUT_VERTEX)
            {
                arc_color = RED;
            }

            rc = fprintf(file, "%d->%d [color=\"%s\"];\n", root->number, -root->number, arc_color);
            if (rc == EOF)
                return rc;
            rc = fprintf(file, "%d [style=\"invisible\"];\n", -root->number);
            if (rc == EOF)
                return rc;
        }

        if ((root->value & INPUT_VERTEX) == INPUT_VERTEX &&
            (root->value & OUTPUT_VERTEX) == OUTPUT_VERTEX)
        {
            style = WEDGED;
            color = RED":"GREEN;
            rc = fprintf(file, "%d [style=\"%s\", fillcolor=\"%s\"];\n",
                         root->number, style, color);
        }
        else if ((root->value & INPUT_VERTEX) == INPUT_VERTEX)
        {
            style = FILLED;
            color = GREEN;
            rc = fprintf(file, "%d [style=\"%s\", fillcolor=\"%s\"];\n",
                         root->number, style, color);
        }
        else if ((root->value & OUTPUT_VERTEX) == OUTPUT_VERTEX)
        {
            style = FILLED;
            color = RED;
            rc = fprintf(file, "%d [style=\"%s\", fillcolor=\"%s\"];\n",
                         root->number, style, color);
        }

        for (node_t *curr = root->next; curr != NULL; curr = curr->next)
        {
            style = "";
            color = "";
            arc_color = "";

            if (graph->last_operation != NULL &&
                graph->last_operation->from_number == root->number &&
                graph->last_operation->to_number == curr->number &&
                graph->last_operation->value == curr->value)
            {
                arc_color = RED;
            }

            if (list->vertices[curr->number] != NULL)
            {
                style = FILLED;
                color = ORANGE;
            }

            if (curr->value != '~')
                rc = fprintf(file, "%d -> %d [label=\"%c\" color=\"%s\"];\n", root->number, curr->number, curr->value, arc_color);
            else
                rc = fprintf(file, "%d -> %d [label=\"λ\"];\n", root->number, curr->number);
            if (rc == EOF)
                return rc;

            if (list->vertices[curr->number] != NULL && list->vertices[curr->number]->value != INTER_VERTEX)
                continue;

            rc = fprintf(file, "%d [style=\"%s\", fillcolor=\"%s\"];\n",
                         curr->number, style, color);
            if (rc == EOF)
                return rc;
        }
    }

    return EXIT_SUCCESS;
}

void print_adjacency_list(graph_t *graph)
{
    node_t *root;
    adjacecny_list_t *list = &graph->adjacency_list;
    for (size_t i = 0; i < list->max_size; i++)
    {
        root = list->vertices[i];
        if (root == NULL)
            continue;
        printf("%d (%d):", root->number, root->value);

        for (node_t *curr = root->next; curr != NULL; curr = curr->next)
        {
            printf(" %d (%c)|", curr->number, curr->value);
        }

        putc('\n', stdout);
    }
    putc('\n', stdout);
}

int compare_graphs(graph_t *graph_a, graph_t *graph_b)
{
    adjacecny_list_t *adjacency_list_a;
    adjacecny_list_t *adjacency_list_b;
    node_t *root_a;
    node_t *root_b;

    if (graph_a == NULL || graph_b == NULL)
        return GRAPHS_DIFFER;

    adjacency_list_a = &graph_a->adjacency_list;
    adjacency_list_b = &graph_b->adjacency_list;

    if (adjacency_list_a->max_size != adjacency_list_b->max_size)
        return GRAPHS_DIFFER;

    for (size_t i = 0; i < adjacency_list_a->max_size; i++)
    {
        root_a = adjacency_list_a->vertices[i];
        root_b = adjacency_list_b->vertices[i];

        if (root_a == NULL && root_b == NULL)
            continue;
        if (root_a == NULL || root_b == NULL)
            return GRAPHS_DIFFER;

        if (root_a->value != root_b->value)
            return GRAPHS_DIFFER;

        for (node_t *curr = root_a->next; curr != NULL; curr = curr->next)
        {
            if (!graph_contains_arc(graph_b, root_a->number, curr->number, curr->value))
                return GRAPHS_DIFFER;
        }

        for (node_t *curr = root_b->next; curr != NULL; curr = curr->next)
        {
            if (!graph_contains_arc(graph_a, root_b->number, curr->number, curr->value))
                return GRAPHS_DIFFER;
        }
    }

    return GRAPHS_EQUAL;
}
