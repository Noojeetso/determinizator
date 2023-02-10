#include "graph.h"
#include "graphviz.h"

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

void print_adjacency_list(const graph_t *graph)
{
    node_t *root;
    const adjacecny_list_t *list = &graph->adjacency_list;
    int count = 0;

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

        count++;
    }

    if (count == 0)
    {
        fputs("List is empty", stdout);
    }

    putc('\n', stdout);
}
