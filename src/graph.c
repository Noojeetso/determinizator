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

typedef struct alias
{
    char name;
    char *array;
    size_t length;
    size_t max_length;
} alias_t;

typedef struct operation operation_t;

struct operation
{
    node_t *removed;
    node_t *created;
    operation_t *next;
    operation_t *prev;
};

typedef struct history
{
    operation_t *head;
    operation_t *tail;
    operation_t *curr;
} history_t;

struct graph
{
    adjacecny_list_t adjacency_list;
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

    return new_graph;
}

int
graph_expand(graph_t *graph, size_t new_size)
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

void
free_graph(graph_t *graph)
{
    for (size_t i = 0; i < graph->adjacency_list.max_size; i++)
    {
        free_list(graph->adjacency_list.vertices[i]);
    }
    free(graph->adjacency_list.vertices);
    free(graph);
}

int graph_add_vertex(graph_t *graph, int number, vertex_type_t vertex_type)
{
    node_t *vertex;

    vertex = graph->adjacency_list.vertices[number];
    if (vertex != NULL)
    {
        vertex->value = vertex->value | vertex_type;
        return EXIT_SUCCESS;
    }

    if (graph->adjacency_list.max_size <= number)
    {
        fputs("Номер создаваемой вершины превышает максимальный\n", stderr);
        return ERR_INPUT_NUMBER;
    }

    vertex = new_node(number, vertex_type);
    if (vertex == NULL)
        return ERR_NO_MEMORY;

    graph->adjacency_list.vertices[number] = vertex;

    return EXIT_SUCCESS;
}

int graph_add_arc(graph_t *graph, int number_from, int number_to, char value)
{
    int rc;
    node_t *list_node;

    // Add root vertex
    if (graph->adjacency_list.vertices[number_from] == NULL)
    {
        rc = graph_add_vertex(graph, number_from, INTER_VERTEX);
        if (rc != EXIT_SUCCESS)
            return rc;
    }

    // Add arc
    list_node = new_node(number_to, value);
    if (list_node == NULL)
        return ERR_NO_MEMORY;

    rc = list_push_end(graph->adjacency_list.vertices[number_from], list_node);
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
    int number_from;
    int number_to;
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
            rc = sscanf(str, "%d->%d %c", &number_from, &number_to, &value);
            if (rc != 3)
            {
                free(str);
                return ERR_INPUT_NUMBER;
            }
            free(str);
            // printf("%d: %d->%d %c\n", rc, number_from, number_to, value);

            max_number = MAX(number_from, number_to);
            if (graph->adjacency_list.max_size < max_number)
            {
                rc = graph_expand(graph, max_number);
                if (rc != EXIT_SUCCESS)
                    return rc;
            }

            rc = graph_add_vertex(graph, number_to, INTER_VERTEX);
            if (rc != EXIT_SUCCESS)
                return rc;

            rc = graph_add_arc(graph, number_from, number_to, value);
            if (rc != EXIT_SUCCESS)
                return rc;
            break;
        case 2:
            if (str[0] == '-')
            {
                rc = sscanf(str, "-->%d", &number_to);
                if (rc != 1)
                {
                    free(str);
                    return ERR_INPUT_NUMBER;
                }
                free(str);

                rc = graph_add_vertex(graph, number_to, INPUT_VERTEX);
                if (rc != EXIT_SUCCESS)
                    return rc;
            }
            else
            {
                rc = sscanf(str, "%d-->", &number_from);
                if (rc != 1)
                {
                    free(str);
                    return ERR_INPUT_NUMBER;
                }
                free(str);

                rc = graph_add_vertex(graph, number_from, OUTPUT_VERTEX);
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

history_t *new_history(void)
{
    history_t *history;
    history = malloc(sizeof(history_t));
    if (history == NULL)
    {
        fputs("Ошибка выделения памяти под узел списка\n", stderr);
        return NULL;
    }
    history->curr = NULL;
    history->curr = NULL;
    return history;
}

int graph_contains_arc(graph_t *graph, int from_number, int to_number, char value)
{
    node_t *root;
    node_t *curr;
    adjacecny_list_t *adjacency_list;
    adjacency_list = &graph->adjacency_list;

    root = adjacency_list->vertices[from_number];
    if (root == NULL)
        return 0;

    for (curr = root->next; curr != NULL; curr = curr->next)
    {
        if (curr->number == to_number && curr->value == value)
            return 1;
    }

    return 0;
}

graph_t *remove_lambda_transitions(graph_t *graph)  // , history_t *history)
{
    // int rc;
    adjacecny_list_t *adjacency_list;
    adjacecny_list_t *new_adjacency_list;
    node_t *root;
    node_t *curr;
    int number;
    queue_t *queue = new_queue();
    graph_t *new_graph = create_graph(INIT_GRAPH_SIZE);

    adjacency_list = &graph->adjacency_list;
    // history = new_history();

    // Filter vertices
    for (size_t i = 0; i < adjacency_list->max_size; i++)
    {
        root = adjacency_list->vertices[i];
        if (root == NULL)
            continue;
        // printf("root_num: %d\n", root->number);
        // printf("root_val: %c\n", root->value);
        if (root->value != '\0')
        {
            graph_add_vertex(new_graph, root->number, root->value);
        }

        for (node_t *curr = root->next; curr != NULL; curr = curr->next)
        {
            // printf("curr_num: %d\n", curr->number);
            // printf("curr_val: %c\n", curr->value);
            if (curr->value != '~')
            {
                graph_add_vertex(new_graph, curr->number, '\0');
            }
        }
    }
    new_adjacency_list = &new_graph->adjacency_list;

    int found_lambda = 1;
    while (found_lambda)
    {
        found_lambda = 0;

        for (size_t i = 0; i < adjacency_list->max_size; i++)
        {
            root = adjacency_list->vertices[i];
            if (root == NULL || new_adjacency_list->vertices[root->number] == NULL)
                continue;

            for (curr = root->next; curr != NULL; curr = curr->next)
            {
                if (curr->value != '~')
                {
                    if (!graph_contains_arc(new_graph, root->number, curr->number, curr->value))
                    {
                        found_lambda = 1;
                        graph_add_arc(new_graph, root->number, curr->number, curr->value);
                    }
                    continue;
                }
                if (!queue_contains(queue, curr->number))
                    queue_push(queue, curr->number);
            }

            while (!is_queue_empty(queue))
            {
                queue_pop(queue, &number);
                for (curr = adjacency_list->vertices[number]->next; curr != NULL; curr = curr->next)
                {
                    if (curr->value != '~')
                    {
                        if (!graph_contains_arc(new_graph, root->number, curr->number, curr->value))
                        {
                            found_lambda = 1;
                            graph_add_arc(new_graph, root->number, curr->number, curr->value);
                        }
                        continue;
                    }
                    if (!queue_contains(queue, curr->number) && number != curr->number && number != root->number)
                        queue_push(queue, curr->number);
                }
            }
        }
    }

    free_queue(queue);

    return new_graph;
}

int write_graph_connections(FILE *file, graph_t *graph, void *arg)
{
    int rc;
    char *from_color;
    char *from_style;
    adjacecny_list_t *adjacency_list;
    node_t *root;
    // graph_t *new_graph = (graph_t*)arg;

    adjacency_list = &graph->adjacency_list;

    for (size_t i = 0; i < adjacency_list->max_size; i++)
    {
        root = adjacency_list->vertices[i];
        if (root == NULL)
            continue;
        // printf("root_num: %d\n", root->number);
        // printf("root_val: %c\n", root->value);
        if ((root->value & VERTEX_MASK) == INPUT_VERTEX)
        {
            rc = fprintf(file, "0->%d;\n", root->number);
            if (rc == EOF)
                return rc;
            rc = fprintf(file, "0 [style=\"invisible\"];\n");
            if (rc == EOF)
                return rc;
            from_style = FILLED;
            from_color = GREEN;
            rc = fprintf(file, "%d [style=\"%s\", fillcolor=\"%s\"];\n",
                         root->number, from_style, from_color);
        }
        else if ((root->value & VERTEX_MASK) == OUTPUT_VERTEX)
        {
            rc = fprintf(file, "%d->%d;\n", root->number, -root->number);
            if (rc == EOF)
                return rc;
            rc = fprintf(file, "%d [style=\"invisible\"];\n", -root->number);
            if (rc == EOF)
                return rc;
            from_style = FILLED;
            from_color = RED;
            rc = fprintf(file, "%d [style=\"%s\", fillcolor=\"%s\"];\n",
                         root->number, from_style, from_color);
        }
        else if ((root->value & VERTEX_MASK) == INPUT_VERTEX &&
                 (root->value & VERTEX_MASK) == OUTPUT_VERTEX)
        {
            rc = fprintf(file, "0->%d;\n", root->number);
            if (rc == EOF)
                return rc;
            rc = fprintf(file, "0 [style=\"invisible\"];\n");
            if (rc == EOF)
                return rc;
            rc = fprintf(file, "%d->%d;\n", root->number, -root->number);
            if (rc == EOF)
                return rc;
            rc = fprintf(file, "%d [style=\"invisible\"];\n", -root->number);
            if (rc == EOF)
                return rc;
            from_style = "wedged";
            from_color = GREEN":"RED;
            rc = fprintf(file, "%d [style=\"%s\", fillcolor=\"%s\"];\n",
                         root->number, from_style, from_color);
        }

        for (node_t *curr = root->next; curr != NULL; curr = curr->next)
        {
            // printf("curr_num: %d\n", curr->number);
            // printf("curr_val: %c\n", curr->value);

            // printf("root: %d curr: %d val: %d\n", root->number, curr->number, root->value);
            from_style = "";
            from_color = "";

            if (graph->adjacency_list.vertices[curr->number] != NULL)
            {
                from_style = FILLED;
                from_color = ORANGE;
            }

            if (curr->value != '~')
            {
                rc = fprintf(file, "%d -> %d [label=\"%c\"];\n", root->number, curr->number, curr->value);
            }
            else
            {
                rc = fprintf(file, "%d -> %d [label=\"λ\"];\n", root->number, curr->number);
            }
            if (rc == EOF)
                return rc;

            if (adjacency_list->vertices[curr->number] != NULL && adjacency_list->vertices[curr->number]->value != '\0')
                continue;

            rc = fprintf(file, "%d [style=\"%s\", fillcolor=\"%s\"];\n",
                         curr->number, from_style, from_color);
            if (rc == EOF)
                return rc;
        }
    }

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
