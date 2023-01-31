#include "sparse_matrix.h"

void free_sparse_matrix(sparse_matrix_t *matrix)
{
    free(matrix->a);
    free(matrix->ia);
    free(matrix->ja);
}

int allocate_sparse_matrix(sparse_matrix_t *matrix)
{
    sparse_matrix_t tmp;

    tmp.a = calloc(matrix->a_len, sizeof(int));
    if (tmp.a == NULL)
        return ERR_NO_MEMORY;

    tmp.ja = calloc(matrix->a_len, sizeof(int));
    if (tmp.ja == NULL)
    {
        free(tmp.a);
        return ERR_NO_MEMORY;
    }

    tmp.ia = calloc(matrix->ia_len + 1, sizeof(int));
    if (tmp.ia == NULL)
    {
        free(tmp.a);
        free(tmp.ja);
        return ERR_NO_MEMORY;
    }

    matrix->a = tmp.a;
    matrix->ja = tmp.ja;
    matrix->ia = tmp.ia;

    return EXIT_SUCCESS;
}

/*
int convert_matrix(matrix_t *matrix, sparse_matrix_t *sparse_matrix)
{
    sparse_matrix_t tmp = {.a_len = matrix.no_zeroes, .ia_len = matrix.x_len};

    int rc = EXIT_SUCCESS;

    rc = allocate_sparse_matrix(&tmp);

    if (rc != EXIT_SUCCESS)
    {
        puts("Ошибка: память под матрицу выделить не удалось");
        return ERR_NO_MEMORY;
    }

    int elem = 0, flag = 0;
    int rem_elem = -1;

    for (int i = 0; i < matrix.x_len; i++)
    {
        flag = 0;

        for (int j = 0; j < matrix.y_len; j++)
        {
            if (matrix.data[i][j] != 0)
            {
                tmp.a[elem] = matrix.data[i][j];

                tmp.ja[elem] = j;

                if (flag == 0)
                {
                    tmp.ia[i] = elem;
                    rem_elem = elem;
                    flag = 1;
                }

                elem++;
            }
        }

        if (flag == 0)
        {
            tmp.ia[i] = rem_elem;
        }
    }

    print_sparse_matrix(tmp);

    tmp.size = matrix.x_len * matrix.y_len;
    tmp.no_zeroes = matrix.no_zeroes;

    *sparse_matrix = tmp;

    return EXIT_SUCCESS;
}
*/

void print_sparse_matrix(sparse_matrix_t matrix)
{
    if (matrix.a_len > 50)
    {
        puts("\n\nСлишком много ненулевых элементов, поэтому иформация о них на экран выведена не будет\n");
        return;
    }

    puts("\n\n\n------------------\nРазреженная матрица\n------------------\nНенулевые элементы:\n");

    for (int i = 0; i < matrix.a_len; i++)
    {
        printf("%2d ", matrix.a[i]);
    }

    puts("\n\nСтолбцы ненулевых элементов:\n");

    for (int i = 0; i < matrix.a_len; i++)
    {
        printf("%2d ", matrix.ja[i]);
    }

    // puts("\n\nИндекс в массиве ненулевых элементов первого вхождения элемента в строку:\n");

    // for (int i = 0; i < matrix.ia_len; i++)
    // {
    //     printf("%3d ", matrix.ia[i]);
    // }

    puts("\n\n");
}

/*
void print_sparse_list(list_t list)
{
    sparse_matrix_t tmp = {.a_len = list.length, .ia_len = 1};

    int elem = 0;

    int rc = allocate_sparse_matrix(&tmp);

    if (rc != EXIT_SUCCESS)
    {
        return;
    }

    for (int i = 0; i < list.length; i++)
    {
        if (list.array[i] != 0)
        {
            tmp.a[elem] = list.array[i];
            tmp.ja[elem++] = i;
        }
    }

    if (elem > 35)
    {
        puts("Слишком много ненулевых элементов, поэтому результат не будет выведен на экран\n");
        return;
    }

    puts("Ненулевые элементы:\n");
    for (int i = 0; i < elem; i++)
    {
        printf("%2d ", tmp.a[i]);
    }

    puts("\n\nПозиции ненулевых элементов:\n");

    for (int i = 0; i < elem; i++)
    {
        printf("%2d ", tmp.ja[i]);
    }

    free_sparse_matrix(&tmp);
}
*/
