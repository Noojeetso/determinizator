#ifndef __PRESETS_H__
#define __PRESETS_H__

#define MIN(x, y) x > y ? y : x;

#define MAX(x, y) x > y ? x : y;

#define READ_MODE "r"
#define WRITE_MODE "w"
#define APPEND_MODE "a"
#define READ_WRITE_MODE "r+"

#define FILLED "filled"
#define WEDGED "wedged"
#define DEFAULT "lightgrey"
#define ORANGE "#fff2d0"
#define GREEN "#ddffb7"
#define RED "#f0a47e"

#define MAX_STR_LEN 100
#define DATABASES "databases/"

#ifdef DEBUG
#define PRINT_DEBUG(str) fprintf(stderr, "%s\n", str);
#define PRINTF_DEBUG(format, ...) fprintf(stderr, format, __VA_ARGS__);
#define PERROR_DEBUG(str) perror(str);
#else
#define PRINT_DEBUG(str) ((void) 0)
#define PRINTF_DEBUG(format, ...) ((void) 0)
#define PERROR_DEBUG(str) ((void) 0)
#endif

#endif // __PRESETS_H__
