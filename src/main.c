#include <stdlib.h>
// #include <stdio.h>
#include "graphviz.h"
#include "history.h"
#include <termios.h>
#include <sys/ioctl.h>

// Инициализация настроек терминала
static struct termios old, new;
void initTermios(int echo)
{
    tcgetattr(0, &old); // Получение старых настроек терминала
    new = old; // Копирование старых настроек терминала
    new.c_lflag &= ~ICANON; // Отключение буфера ввода/вывода
    new.c_lflag &= echo ? ECHO : ~ECHO; // Отключение печати вводимых символов
    tcsetattr(0, TCSANOW, &new); // Установка новых настроек терминала
}

// Восстановление старых настроек терминала
void resetTermios(void)
{
    tcsetattr(0, TCSANOW, &old);
}

void scan_key(char *current_char)
{
    initTermios(0);
    char ch = '\0';
    // while (ch != 'q')
    // {
    read(STDIN_FILENO, &ch, 1);
    *current_char = ch;
    // }
    resetTermios();
}

int main(void)
{
    int rc;
    graph_t *graph;
    history_t *history;
    char current_symbol;

    graph = graph_from_file("graph_in.txt");
    if (graph == NULL)
        return EXIT_FAILURE;

    // rc = graph_visualize(graph, "graph");

    history = get_lambda_transition_deletion_history(graph);
    if (history == NULL)
    {
        fputs("Error creating history\n", stderr);
        return EXIT_FAILURE;
    }

    print_history(history);

    // rc = graph_visualize(new_graph, "graph_wo_lambda");
    print_adjacency_list(graph);

    rc = EXIT_SUCCESS;
    while (rc == EXIT_SUCCESS)
    {
        rc = graph_visualize(graph, "graph");
        if (rc != EXIT_SUCCESS)
            break;
        scan_key(&current_symbol);
        // printf("curr: %c\n", current_symbol);
        switch (current_symbol)
        {
            case 'd':
                rc = history_next(history, graph);
                break;
            case 'a':
                rc = history_prev(history, graph);
                break;
            // default:
            //     rc = EXIT_FAILURE;
        }
        print_adjacency_list(graph);
    }

    free_graph(graph);
    free_history(history);

    return rc;
}
