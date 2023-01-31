#include "array.h"

struct array
{
    int *data;
    size_t size;
    size_t max_size;
};

array_t *new_array(size_t init_size)
{
    array_t *array = malloc(sizeof(array_t));
    if (array == NULL)
    {
        fputs("Ошибка выделения памяти под структуру массива\n", stderr);
        return NULL;
    }

    array->data = malloc(init_size * sizeof(int));
    if (array->data == NULL)
    {
        fputs("Ошибка выделения памяти под элементы массива\n", stderr);
        free(array);
        return NULL;
    }

    array->max_size = init_size;
    array->size = 0;

    return array;
}

void clear_array(array_t *array)
{
    free(array->data);
}

void free_array(array_t *array)
{
    clear_array(array);
    free(array);
}

int array_expand(array_t *array)
{
    int *new_data;
    size_t new_max_size;

    new_max_size = array->max_size * ARRAY_EXPAND_FACTOR;

    new_data = realloc(array->data, new_max_size * sizeof(int));
    if (new_data == NULL)
    {
        fputs("Ошибка выделения памяти для расширения матрицы\n", stderr);
        return ERR_NO_MEMORY;
    }

    array->data = new_data;
    array->max_size = new_max_size;

    return EXIT_SUCCESS;
}

int array_append(array_t *array, int value)
{
    int rc;

    if (array->size == array->max_size)
    {
        rc = array_expand(array);
        if (rc != EXIT_SUCCESS)
            return rc;
    }

    array->data[array->size] = value;
    array->size++;

    return EXIT_SUCCESS;
}
