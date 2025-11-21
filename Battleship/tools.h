#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

typedef struct options{
    char *str;
    uint8_t r;
    uint8_t g;
    uint8_t b;
}opt;


static int T_curs_x = 1;
static int T_curs_y = 1;

static struct termios original_termios;

static void restore_terminal(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

static void enable_raw_mode(void)
{
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1)
        return;

    struct termios raw = original_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    atexit(restore_terminal);
}

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
            T_curs_y++;
            T_curs_x = 1;
        }
        else
        {
            T_curs_x++;
        }
    }

    free(buffer);
}


static inline void T_move(int x, int y)
{
    int target_y = y;
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_row > 0 && y > ws.ws_row)
    {
        int extra = y - ws.ws_row;
        for (int i = 0; i < extra; ++i)
        {
            putchar('\n');
        }
        fflush(stdout);
        y = ws.ws_row;
    }

    /* No usamos print para no alterar las coordenadas al emitir secuencias ANSI */
    fprintf(stdout, "\033[%d;%dH", y, x);
    fflush(stdout);
    T_curs_x = x;
    T_curs_y = target_y;
}

static inline void T_setColor(uint8_t r, uint8_t g, uint8_t b)
{
    fprintf(stdout, "\033[38;2;%u;%u;%um", r, g, b);
}

static inline void T_resetColor(void)
{
    fputs("\033[0m", stdout);
}

static inline void T_hideCursor(void)
{
    fputs("\033[?25l", stdout);
    fflush(stdout);
}

static inline void T_showCursor(void)
{
    fputs("\033[?25h", stdout);
    fflush(stdout);
}

static inline void T_placeC(int x, int y, char ch, uint8_t r, uint8_t g, uint8_t b)
{
    T_move(x, y);
    T_setColor(r, g, b);
    if(ch != '\n'){
        T_curs_x++;
    }
    else{
        T_curs_y++;
        T_curs_x = 1;
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
            T_curs_x++;
        }
        else{
            T_curs_y++;
            T_curs_x = 1;
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

static int read_key(void)
{
    int ch = getchar();
    if (ch == '\033')
    {
        int next1 = getchar();
        int next2 = getchar();
        if (next1 == '[')
        {
            switch (next2)
            {
                case 'A':
                    return 'U';
                case 'B':
                    return 'D';
                case 'C':
                    return 'R';
                case 'D':
                    return 'L';
                default:
                    return 0;
            }
        }
        return 0;
    }
    if (ch == '\r' || ch == '\n')
        return '\n';
    return ch;
}

int optUpd(opt menu[], int n){
    if(n <= 0 || n == 1)
    {
        printf("Error: Invalid option value\n");
        return -1; 
    }
    enable_raw_mode();
    int flag = 0, current = 0, Y = T_curs_y + 2;

    for(int i = 0; i < n; i++)
    {
        T_placeStr(2, Y + i, menu[i].str, menu[i].r, menu[i].g, menu[i].b);
    }
    T_placeC(1, Y + current, '>', 0, 0, 0);

    while(!flag)
    {
        char ch = read_key();
        int prev = current;

        if(ch == 'D')
        {
            if (current >= n - 1)
                current = 0;
            else
                current = current + 1;
        }
        else if(ch == 'U')
        {
            if (current <= 0)
                current = n - 1;
            else
                current = current - 1;
        }
        else if(ch == '\n')
        {
            int target_y = Y + n + 1;
            T_move(1, target_y);
            T_curs_x = 1;
            //T_curs_y = target_y;
            restore_terminal();
            return current;
        }
        else
        {
            continue;
        }

        if (current != prev)
        {
            T_placeC(1, Y + prev, ' ', 0, 0, 0);
            T_placeC(1, Y + current, '>', 0, 0, 0);
        }
    }
}

#endif /* TOOLS_H */
