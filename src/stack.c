#include "stack.h"

struct stack
{
    int *data;
    size_t max_size;
    size_t size;
};

array_stack_t *stack_create(size_t init_size)
{
    array_stack_t *new_array_stack = malloc(sizeof(array_stack_t));
    if (new_array_stack == NULL)
        return NULL;
    new_array_stack->data = malloc(init_size * sizeof(int));
    if (new_array_stack->data == NULL)
    {
        free(new_array_stack);
        return NULL;
    }
    new_array_stack->max_size = init_size;
    new_array_stack->size = 0;
    return new_array_stack;
}

void stack_free(array_stack_t *stack)
{
    free(stack->data);
    free(stack);
}

int stack_expand(array_stack_t *stack)
{
    int *new_data;
    size_t new_max_size;

    new_max_size = stack->max_size * STACK_EXPAND_FACTOR;
    new_data = realloc(stack->data, new_max_size * sizeof(int));
    if (new_data == NULL)
    {
        fputs("Ошибка выделения памяти для расширения стека\n", stderr);
        return ERR_NO_MEMORY;
    }

    stack->data = new_data;
    stack->max_size = new_max_size;

    return EXIT_SUCCESS;
}

int stack_push(array_stack_t *stack, int value)
{
    int rc;

    if (stack->size == stack->max_size)
    {
        rc = stack_expand(stack);
        if (rc != EXIT_SUCCESS)
            return rc;
    }

    stack->data[stack->size] = value;
    stack->size++;

    return EXIT_SUCCESS;
}

int stack_pop(array_stack_t *stack, int *value)
{
    if (stack->size == 0)
    {
        fputs("Стек пуст\n", stderr);
        return ERR_STACK_EMPTY;
    }

    *value = stack->data[stack->size];
    stack->size--;

    return EXIT_SUCCESS;
}

void stack_print(array_stack_t *stack)
{
    if (stack->size == 0)
    {
        puts("Стек пуст");
        return;
    }

    for (size_t i = 0; i < stack->size; i++)
    {
        printf("Элемент %lu: %d\n", stack->size - i + 1, stack->data[i]);
    }
}
