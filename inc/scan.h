#ifndef __SCAN_H__
#define __SCAN_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "errors.h"
// #include "print.h"

int
scan_double(double *value,
            double min_value,
            double max_value);

int
scan_int(int *value,
         int min_value,
         int max_value);

int
scan_integer_file(FILE *file,
                  int *number,
                  size_t min,
                  size_t max);

int
scan_menu(int *key);

int
flush_input(void);

int
scan_string(char *string,
            size_t max_size);

# endif  // __SCAN_H__
