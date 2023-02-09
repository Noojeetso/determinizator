#include "graphviz.h"

char *
create_graph_file_name(char *graph_name,
                       char *EXTENSION)
{
    char *file_name = malloc(strlen(OUT_FOLDER) + strlen(graph_name) + sizeof('.') + strlen(IMAGE_EXTENSION) + sizeof('\0'));
    if (file_name == NULL)
        return NULL;
    file_name = memcpy(file_name, OUT_FOLDER, strlen(OUT_FOLDER) + 1);
    file_name = strcat(file_name, graph_name);
    file_name = strcat(file_name, ".");
    file_name = strcat(file_name, EXTENSION);
    return file_name;
}

int
graph_to_dot(const graph_t *graph,
             char *graph_name,
             int (*write_graph_connections)(FILE *file, const graph_t *graph, void *arg),
             void *arg)
{
    int rc;
    FILE *output_file;
    char *file_name;

    puts("Преобразование графа в .dot файл...");

    if (graph == NULL)
    {
        fputs("Граф не существует\n", stderr);
        return ERR_NULL_POINTER;
    }

    file_name = create_graph_file_name(graph_name, DOT_EXTENSION);
    if (file_name == NULL)
    {
        fputs("Ошибка генерации имени файла\n", stderr);
        return ERR_NO_MEMORY;
    }

    output_file = fopen(file_name, WRITE_MODE);
    free(file_name);
    if (output_file == NULL)
    {
        fputs("Ошибка при открытии файла на запись\n", stderr);
        return ERR_OPEN_FILE;
    }

    rc = fputs("digraph new_graph {\n", output_file);
    if (rc == EOF)
    {
        fputs("Ошибка при записи в файл\n", stderr);
        fclose(output_file);
        return ERR_WRITING_FILE;
    }

    rc = fputs("rankdir=LR;\n", output_file);
    if (rc == EOF)
    {
        fputs("Ошибка при записи в файл\n", stderr);
        fclose(output_file);
        return ERR_WRITING_FILE;
    }

    rc = write_graph_connections(output_file, graph, arg);
    if (rc != EXIT_SUCCESS)
    {
        fputs("Ошибка при записи в файл\n", stderr);
        fclose(output_file);
        return rc;
    }

    rc = fputs("}\n", output_file);

    if (rc == EOF)
    {
        fclose(output_file);
        return ERR_WRITING_FILE;
    }

    if (fclose(output_file) == EOF)
    {
        fputs("Ошибка при закрытии файла\n", stderr);
        return ERR_CLOSE_FILE;
    }

    return EXIT_SUCCESS;
}

int
dot_to_svg(char *graph_name)
{
    int rc;
    pid_t postprocess_id;
    char *input_file_name;
    char *output_file_name;

    puts("Преобразование .dot в .svg файл...");

    postprocess_id = fork();

    if (postprocess_id == -1)
    {
        fputs("Ошибка создания процесса для генерации изображения", stderr);
        return EXIT_FAILURE;
    }

    if (postprocess_id == 0)
    {
        input_file_name = create_graph_file_name(graph_name, DOT_EXTENSION);
        if (input_file_name == NULL)
        {
            fputs("Ошибка генерации имени файла\n", stderr);
            return ERR_NO_MEMORY;
        }

        output_file_name = create_graph_file_name(graph_name, IMAGE_EXTENSION);
        if (output_file_name == NULL)
        {
            fputs("Ошибка генерации имени файла\n", stderr);
            free(input_file_name);
            return ERR_NO_MEMORY;
        }
        // printf("in filename: %s\n", input_file_name);
        // printf("out filename: %s\n", output_file_name);

        execlp("dot", DOT_EXTENSION, "-T"IMAGE_EXTENSION, input_file_name, "-o", output_file_name, NULL);
    }

    waitpid(postprocess_id, &rc, 0);

    if (rc != EXIT_SUCCESS)
    {
        fputs("Ошибка при создании изображения графика\n", stderr);
    }

    return rc;
}

int
open_svg(char *graph_name)
{
    int rc;
    FILE *dev_null;
    char *input_file_name;

    puts("Открытие .svg файла...");

    if (fork() == 0)
    {
        input_file_name = create_graph_file_name(graph_name, IMAGE_EXTENSION);
        if (input_file_name == NULL)
        {
            fputs("Ошибка генерации имени файла\n", stderr);
            return ERR_NO_MEMORY;
        }
        dev_null = fopen("/dev/null", READ_MODE);
        dup2(fileno(dev_null), 1);
        dup2(fileno(dev_null), 2);
        rc = execlp("xdg-open", "xdg-open", input_file_name, NULL);
        free(input_file_name);
        if (rc != EXIT_SUCCESS)
        {
            fputs("Ошибка при открытии изображении графика\n", stderr);
            return rc;
        }
        fclose(dev_null);
    }

    return EXIT_SUCCESS;
}

int create_and_open_picture(char *graph_name)
{
    int rc;

    rc = dot_to_svg(graph_name);
    if (rc != EXIT_SUCCESS)
        return rc;

    rc = open_svg(graph_name);
    if (rc != EXIT_SUCCESS)
        return rc;

    return EXIT_SUCCESS;
}

int graph_visualize(graph_t *graph, char *graph_name)
{
    int rc;

    rc = graph_to_dot(graph, graph_name, write_graph_connections, NULL);
    if (rc != EXIT_SUCCESS)
        return rc;

    create_and_open_picture(graph_name);
    if (rc != EXIT_SUCCESS)
        return rc;

    return EXIT_SUCCESS;
}

int graph_visualize_highlited(graph_t *graph, graph_t *new_graph, char *graph_name)
{
    int rc;

    rc = graph_to_dot(graph, graph_name, write_graph_connections, new_graph);
    if (rc != EXIT_SUCCESS)
        return rc;

    create_and_open_picture(graph_name);
    if (rc != EXIT_SUCCESS)
        return rc;

    return EXIT_SUCCESS;
}

/*
int
graph_visualize_unreachables(graph_t *list_graph,
                             int vertex_number,
                             char *graph_name)
{
    int rc;
    int *min_paths;

    rc = find_graph_min_paths_safe(list_graph, vertex_number, &min_paths);
    if (rc != EXIT_SUCCESS)
        return rc;

    rc = graph_to_dot(list_graph, graph_name, write_graph_connections, min_paths);
    free(min_paths);
    if (rc != EXIT_SUCCESS)
        return rc;

    create_and_open_picture(graph_name);
    if (rc != EXIT_SUCCESS)
        return rc;

    return EXIT_SUCCESS;
}
*/
