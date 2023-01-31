#include "queue.h"

typedef struct node
{
    int value;
    struct node *next;
} node_t;

struct queue
{
    node_t *head;
    node_t *tail;
};

queue_t *new_queue(void)
{
    queue_t *new_queue = malloc(sizeof(queue_t));
    if (new_queue == NULL)
    {
        fputs("Ошибка выделения памяти под структуру списка\n", stderr);
        return NULL;
    }

    new_queue->head = NULL;
    new_queue->tail = NULL;

    return new_queue;
}

void clear_queue(queue_t *queue)
{
    node_t *current = queue->head;
    node_t *prev;

    while (current != NULL)
    {
        prev = current;
        current = current->next;
        free(prev);
    }
}

void free_queue(queue_t *queue)
{
    clear_queue(queue);
    free(queue);
}

int queue_insert(queue_t *queue, int value, ssize_t index)
{
    size_t counter;
    node_t *prev;
    node_t *current;
    node_t *new_node;

    new_node = malloc(sizeof(node_t));
    if (new_node == NULL)
    {
        fputs("Ошибка выделения памяти под новый узел списка\n", stderr);
        return ERR_NO_MEMORY;
    }

    new_node->value = value;
    new_node->next = NULL;

    if (queue->head == NULL)
    {
        queue->head = new_node;
        queue->tail = new_node;
        return EXIT_SUCCESS;
    }

    if (index == -1)
    {
        queue->tail->next = new_node;
        queue->tail = new_node;
        return EXIT_SUCCESS;
    }

    counter = 0;
    prev = NULL;
    current = queue->head;
    while (current != NULL && counter++ < index)
    {
        prev = current;
        current = current->next;
    }

    if (prev == NULL)
    {
        new_node->next = queue->head;
        queue->head = new_node;
    }
    else
    {
        if (prev->next == NULL)
            queue->tail = new_node;
        new_node->next = prev->next;
        prev->next = new_node;
    }

    return EXIT_SUCCESS;
}

int queue_push(queue_t *queue, int value)
{
    node_t *new_node;

    new_node = malloc(sizeof(node_t));
    if (new_node == NULL)
    {
        fputs("Ошибка выделения памяти под новый узел списка\n", stderr);
        return ERR_NO_MEMORY;
    }

    new_node->value = value;
    new_node->next = NULL;

    if (queue->head == NULL)
    {
        queue->head = new_node;
        queue->tail = new_node;
        return EXIT_SUCCESS;
    }

    queue->tail->next = new_node;
    queue->tail = new_node;
    return EXIT_SUCCESS;
}

int queue_pop(queue_t *queue, int *value)
{
    node_t *current;

    current = queue->head;
    if (current == NULL)
        return ERR_NULL_POINTER;

    *value = current->value;
    queue->head = current->next;
    free(current);

    if (queue->head == NULL)
        queue->tail = NULL;

    return EXIT_SUCCESS;
}

int queue_contains(const queue_t *queue, int value)
{
    for (node_t *curr = queue->head; curr != NULL; curr = curr->next)
        if (curr->value == value)
            return 1;
    return 0;
}

int is_queue_empty(const queue_t *queue)
{
    return queue->head == NULL;
}

size_t get_queue_size(const queue_t *queue)
{
    size_t size = 0;
    node_t *curr = queue->head;

    while (curr != NULL)
    {
        size++;
        curr = curr->next;
    }

    return size;
}

void queue_print(queue_t *queue)
{
    node_t *curr;

    curr = queue->head;
    if (curr == NULL)
    {
        puts("Очередь пуста");
        return;
    }

    fputs("Очередь: ", stdout);
    while (curr != NULL)
    {
        printf("%d |%c", curr->value, curr->next != NULL ? ' ' : '\n');
        curr = curr->next;
    }
}
