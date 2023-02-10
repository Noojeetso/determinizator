#include "graph.h"
#include <unistd.h>

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

void free_adjacency_list(node_t *head)
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
        free_adjacency_list(graph->adjacency_list.vertices[i]);
    }
    free(graph->adjacency_list.vertices);
    free(graph);
}

graph_t *graph_copy(const graph_t *source_graph)
{
    graph_t *new_graph;
    node_t *root;
    const adjacecny_list_t *list;

    new_graph = create_graph(INIT_GRAPH_SIZE);
    if (new_graph == NULL)
        return NULL;

    list = &source_graph->adjacency_list;

    for (size_t i = 0; i < list->max_size; i++)
    {
        root = list->vertices[i];
        if (root == NULL)
            continue;

        graph_add_vertex(new_graph, root->number, root->value);

        for (node_t *curr = root->next; curr != NULL; curr = curr->next)
        {
            graph_add_arc(new_graph, root->number, curr->number, curr->value);
        }
    }

    return new_graph;
}

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

    vertex->value |= INTER_VERTEX;
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
        if (to_number == curr->number && value == curr->value)
        {
            prev->next = curr->next;
            free(curr);
            return EXIT_SUCCESS;
        }
        prev = prev->next;
    }

    return EXIT_FAILURE;
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
        PRINT_DEBUG("ALREADY AT THE FIRST HISTORY PAGE");
        return EXIT_SUCCESS;
    }

    if (history->prev != history->curr)
        history->curr = history->curr->prev;
    history->prev = history->curr->prev;

    operation = history->curr;

    if (operation == history->head)
    {
        PRINT_DEBUG("FIRST HISTORY PAGE");
    }

    // printf("UNDO %s: %d %d %d\n", operation->operation_type == ADD ? "ADD" : "REMOVE", operation->from_number, operation->to_number, operation->value);
    if (history->curr == NULL)
    {
        fputs("Previous operation is null pointer\n", stderr);
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
        PRINT_DEBUG("ALREADY AT THE LAST HISTORY PAGE");
        return EXIT_SUCCESS;
    }

    operation = history->curr;

    if (operation == history->tail)
    {
        PRINT_DEBUG("LAST HISTORY PAGE");
    }

    // printf("APPLY %s: %d %d %d\n", operation->operation_type == ADD ? "ADD" : "REMOVE", operation->from_number, operation->to_number, operation->value);
    if (history->curr == NULL)
    {
        fputs("Next operation is null pointer\n", stderr);
        return EXIT_FAILURE;
    }

    rc = apply_operation(graph, operation);
    if (rc != EXIT_SUCCESS)
    {
        fputs("Operation apply error\n", stderr);
        return rc;
    }

    graph->last_operation = operation;

    history->prev = history->curr;
    if (history->curr->next != NULL)
        history->curr = history->curr->next;

    return EXIT_SUCCESS;
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
