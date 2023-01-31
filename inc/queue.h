#ifndef __LIST_H__
#define __LIST_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "presets.h"
#include "errors.h"

#define LISTS_EQUAL 0
#define LISTS_DIFFER 1
#define OBJECT_EQUAL 0
#define OBJECTS_DIFFER 1

typedef struct queue queue_t;

queue_t *new_queue(void);

void free_queue(queue_t *queue);

int queue_insert(queue_t *queue, int value, ssize_t index);

int queue_push(queue_t *queue, int value);

int queue_pop(queue_t *queue, int *value);

int queue_contains(const queue_t *queue, int value);

int is_queue_empty(const queue_t *queue);

size_t get_queue_size(const queue_t *queue);

void queue_print(queue_t *queue);

#endif  // __LIST_H__
