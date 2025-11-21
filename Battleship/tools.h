#ifndef TOOLS_H
#define TOOLS_H

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


static inline void T_move(int x, int y)
{
    /* No usamos print para no alterar las coordenadas al emitir secuencias ANSI */
    fprintf(stdout, "\033[%d;%dH", y, x);
    fflush(stdout);
    T_cursor_x = x;
    T_cursor_y = y;
}

static inline void T_setColor(uint8_t r, uint8_t g, uint8_t b)
{
    fprintf(stdout, "\033[38;2;%u;%u;%um", r, g, b);
}

static inline void T_resetColor(void)
{
    fputs("\033[0m", stdout);
}

static inline void T_placeC(int x, int y, char ch, uint8_t r, uint8_t g, uint8_t b)
{
    T_move(x, y);
    T_setColor(r, g, b);
    if(ch != '\n'){
        T_cursor_x++;
    }
    else{
        T_cursor_y++;
        T_cursor_x = 1;
    }
    putchar(ch);
    T_resetColor();
    fflush(stdout);
}

static inline void T_placeStr(int x, int y, const char *ch, uint8_t r, uint8_t g, uint8_t b)
{
    T_move(x, y);
    T_setColor(r, g, b);
    for(size_t i = 0; i < strlen(ch); ++i)
    {
        if (ch[i] != '\n'){
            T_cursor_x++;
        }
        else{
            T_cursor_y++;
            T_cursor_x = 1;
        }
    }
    fputs(ch, stdout);
    T_resetColor();
    fflush(stdout);
}

static inline void T_replace(int x, int y, char ch, uint8_t r, uint8_t g, uint8_t b)
{
    T_move(x, y);
    T_setColor(r, g, b);
    putchar(ch);
    T_resetColor();
    fflush(stdout);
}

static inline char *readLine(void)
{
    size_t capacity = 32;
    size_t length = 0;
    char *buffer = malloc(capacity);

    if (!buffer)
        return NULL;

    int ch;
    while ((ch = getchar()) != EOF && ch != '\n')
    {
        if (length + 1 >= capacity)
        {
            capacity *= 2;
            char *tmp = realloc(buffer, capacity);
            if (!tmp)
            {
                free(buffer);
                return NULL;
            }
            buffer = tmp;
        }
        buffer[length++] = (char)ch;
    }

    if (length == 0 && ch == EOF)
    {
        free(buffer);
        return NULL;
    }

    buffer[length] = '\0';
    return buffer;
}

#endif /* TOOLS_H */
