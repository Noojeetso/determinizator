#ifndef __HISTORY_H__
#define __HISTORY_H__

#include <stdlib.h>
#include <stdio.h>

#define NO_VALUE -1

typedef enum operation_type
{
    ADD = 1,
    REMOVE = 2
} operation_type_t;

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
    int from_number;
    int to_number;
    char value;
    operation_type_t operation_type;
    operation_t *prev;
    operation_t *next;
};

typedef struct history
{
    operation_t *head;
    operation_t *tail;
    operation_t *prev;
    operation_t *curr;
} history_t;

history_t *create_history(void);

void free_history(history_t *history);

void history_push(history_t *history, operation_t *operation);

operation_t *operation_add_vertex(int number, char value);

operation_t *operation_add_arc(int number_from, int number_to, char value);

operation_t *operation_remove_vertex(int number, char value);

operation_t *operation_remove_arc(int number_from, int number_to, char value);

#endif  // __HISTORY_H__
