#include "scan.h"

int
flush_input(void)
{
    int amount = 0;
    char ch;
    do
    {
        scanf("%c", &ch);
        if (ch == EOF)
            break;
        amount++;
    }
    while (ch != '\n');

    return amount;
}

/*
int
scan_menu(int *key)
{
    int rc;

    printf("%s", "Номер меню: ");

    rc = scanf("%d", key);

    if (flush_input() != 1)
    {
        print_newline();
        fputs("Ошибка при считывании номера меню\n", stderr);
        return EXIT_FAILURE;
    }

    if (rc != 1)
    {
        print_newline();
        fputs("Не был введён номер меню\n", stderr);
        return ERR_INPUT_NUMBER;
    }

    return EXIT_SUCCESS;
}
*/

int
scan_double(double *value,
            double min_value,
            double max_value)
{
    printf("Введите число от %.1lf до %.1lf включительно:\n", min_value, max_value);

    if (scanf("%lf", value) != 1 || flush_input() != 1)
    {
        // print_newline();
        fputs("\nОшибка при вводе целого числа\n", stderr);
        return ERR_INPUT_NUMBER;
    }

    if (*value < min_value)
    {
        // print_newline();
        fputs("\nВведённое число меньше, чем минимально допустимое\n", stderr);
        return ERR_DOMAIN;
    }

    if (*value > max_value)
    {
        // print_newline();
        fputs("\nВведённое число больше, чем максимально допустимое\n", stderr);
        return ERR_DOMAIN;
    }

    return EXIT_SUCCESS;
}

int
scan_int(int *value,
         int min_value,
         int max_value)
{
    printf("Введите целое число от %d до %d включительно:\n", min_value, max_value);

    if (scanf("%d", value) != 1 || flush_input() != 1)
    {
        // print_newline();
        fputs("\nОшибка при вводе целого числа\n", stderr);
        return ERR_INPUT_NUMBER;
    }

    if (*value < min_value)
    {
        // print_newline();
        fputs("\nВведённое число меньше, чем минимально допустимое\n", stderr);
        return ERR_DOMAIN;
    }

    if (*value > max_value)
    {
        // print_newline();
        fputs("\nВведённое число больше, чем максимально допустимое\n", stderr);
        return ERR_DOMAIN;
    }

    return EXIT_SUCCESS;
}

int
scan_integer_file(FILE *file,
                  int *number,
                  size_t min,
                  size_t max)
{
    int rc;

    rc = fscanf(file, "%d", number);
    if (rc != 1)
    {
        fgetc(file);
        if (!feof(file))
            fputs("Ошибка ввода индекса вершины\n", stderr);
        return ERR_READING_VALUE;
    }

    if (*number < min || *number > max)
    {
        printf("err: %d\n", *number);
        fprintf(stderr, "Индекс вершины должен лежать в пределах от %lu до %lu вклюительно\n", min, max - 1);
        return ERR_READING_VALUE;
    }

    return EXIT_SUCCESS;
}

int
scan_string(char *string,
            size_t max_size)
{
    size_t str_length;

    if (fgets(string, max_size, stdin) == NULL)
    {
        fputs("Ошибка считывания строки", stderr);
        return ERR_READING_STRING;
    }

    str_length = strlen(string);
    if (string[str_length - 1] != '\n')
    {
        fputs("Введённа слишком длинная строка", stderr);
        return ERR_READING_STRING;
    }

    string[str_length - 1] = '\0';

    return EXIT_SUCCESS;
}
