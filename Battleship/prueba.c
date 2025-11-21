#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

static int T_cursor_x = 1;
static int T_cursor_y = 1;

void print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    va_list copy;
    va_copy(copy, args);

    size_t needed = (size_t)vsnprintf(NULL, 0, format, copy);
    va_end(copy);

    if (needed < 0)
    {
        va_end(args);
        return;
    }

    char *buffer = malloc(needed + 1);
    if (!buffer)
    {
        va_end(args);
        return;
    }

    vsnprintf(buffer, needed + 1, format, args);
    va_end(args);

    fputs(buffer, stdout);

    for (size_t i = 0; i < needed; ++i)
    {
        if (buffer[i] == '\n')
        {
            T_cursor_y++;
            T_cursor_x = 1;
        }
        else
        {
            T_cursor_x++;
        }
    }

    free(buffer);
}

int main(){
    
    print("Hola, este es un st%dring\n", sizeof(int));
    print("X: %d\n", T_cursor_x);
    print("Y: %d\n", T_cursor_y);
    return 0;
}
