#include "history.h"

// operation_t *create_operation(operation_node_t *created, operation_node_t *removed)
operation_t *create_operation(int from_number, int to_number, char value)
{
    operation_t *operation;
    operation = malloc(sizeof(*operation));
    if (operation == NULL)
    {
        fputs("Ошибка выделения памяти под новую операцию\n", stderr);
        return NULL;
    }

    operation->from_number = from_number;
    operation->to_number = to_number;
    operation->value = value;
    // operation->created = created;
    // operation->removed = removed;
    operation->prev = NULL;
    operation->next = NULL;

    return operation;
}
/*
void free_operation_list(operation_node_t *head)
{
    operation_node_t *curr;
    operation_node_t *next;

    curr = head;
    while (curr != NULL)
    {
        next = curr->next;
        free(curr);
        curr = next;
    }
}
*/

// for alias free in future
void free_operation(operation_t *operation)
{
    // free_operation_list(operation->created);
    // free_operation_list(operation->removed);
    free(operation);
}

operation_t *operation_add_vertex(int number, char value)
{
    operation_t *operation = create_operation(number, NO_VALUE, value);
    if (operation == NULL)
        return NULL;

    operation->operation_type = ADD;
    return operation;
}

operation_t *operation_add_arc(int from_number, int to_number, char value)
{
    operation_t *operation = create_operation(from_number, to_number, value);
    if (operation == NULL)
        return NULL;

    operation->operation_type = ADD;
    return operation;
}

operation_t *operation_remove_vertex(int number, char value)
{
    operation_t *operation = create_operation(number, NO_VALUE, value);
    if (operation == NULL)
        return NULL;

    operation->operation_type = REMOVE;
    return operation;
}

operation_t *operation_remove_arc(int from_number, int to_number, char value)
{
    operation_t *operation = create_operation(from_number, to_number, value);
    if (operation == NULL)
        return NULL;

    operation->operation_type = REMOVE;
    return operation;
}

history_t *create_history(void)
{
    history_t *history;

    history = malloc(sizeof(*history));
    if (history == NULL)
    {
        fputs("Ошибка выделения памяти под узел списка\n", stderr);
        return NULL;
    }

    history->head = NULL;
    history->tail = NULL;
    history->prev = NULL;
    history->curr = NULL;

    return history;
}

void free_history(history_t *history)
{
    operation_t *curr;
    operation_t *next;

    curr = history->head;
    while (curr != NULL)
    {
        next = curr->next;
        free_operation(curr);
        curr = next;
    }

    free(history);
}

void history_push(history_t *history, operation_t *operation)
{
    if (history->head == NULL)
    {
        history->head = operation;
        history->tail = operation;
        history->curr = operation;
        // operation->prev = operation;
        // operation->next = operation;
        return;
    }

    history->tail->next = operation;
    operation->prev = history->tail;
    // operation->next = history->head;
    history->tail = operation;
}

void print_history(history_t *history)
{
    operation_t *operation;

    if (history == NULL)
        return;

    operation = history->head;

    if (operation == NULL)
    {
        puts("History is empty");
        return;
    }

    int count = 0;
    puts("History:");
    while (operation != NULL)
    {
        count++;
        printf("%d: %s: %d %d %d\n", count, operation->operation_type == ADD ? "ADD" : "REMOVE", operation->from_number, operation->to_number, operation->value);
        operation = operation->next;
    }
    putc('\n', stdout);
}
