#ifndef __SPARSE_MATRIX_H__
#define __SPARSE_MATRIX_H__

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "errors.h"

typedef struct
{
    int *a;
    int *ja;
    int *ia;

    int a_len;
    int ia_len;
    size_t size;
    size_t no_zeroes;
} sparse_matrix_t;

int allocate_sparse_matrix(sparse_matrix_t *matrix);

void free_sparse_matrix(sparse_matrix_t *matrix);

// int convert_matrix(matrix_t matrix, sparse_matrix_t *sparse_matrix);

void print_sparse_matrix(sparse_matrix_t matrix);

#endif  // __SPARSE_MATRIX_H__
