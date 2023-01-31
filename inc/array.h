#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <stdlib.h>
#include <stdio.h>
#include "errors.h"

#define ARRAY_EXPAND_FACTOR 2

typedef struct array array_t;

array_t *new_array(size_t init_size);

void clear_array(array_t *array);

void free_array(array_t *array);

int array_append(array_t *array, int value);

#endif  // __ARRAY_H__
