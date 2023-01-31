#ifndef __STACK_H__
#define __STACK_H__

#include <stdlib.h>
#include <stdio.h>
#include "presets.h"
#include "errors.h"
#include "scan.h"

#define STACK_EXPAND_FACTOR 2

typedef struct stack array_stack_t;

array_stack_t *stack_create(size_t init_size);

void stack_free(array_stack_t *stack);

int stack_push(array_stack_t *stack, int value);

int stack_pop(array_stack_t *stack, int *value);

void stack_print(array_stack_t *stack);

#endif  // __STACK_H__
