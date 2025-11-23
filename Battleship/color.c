#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>
#include "tools.h"

#define LEVELS 6
#define CELL_STRIDE 4   /* separación horizontal entre celdas */
#define SLICE_COLS 3    /* cuántas slices de B por fila para que entre en pantalla */

/* geometría derivada */
#define LABEL_OFFSET 0
#define CELL_OFFSET 7
#define SLICE_WIDTH (CELL_OFFSET + LEVELS * CELL_STRIDE)
#define SLICE_HEIGHT (LEVELS + 2)
#define SLICE_GAP 3

static const uint8_t level_values[LEVELS] = {0, 51, 102, 153, 204, 255};

static inline void slice_origin(int base_x, int base_y, int b_idx, int *out_x, int *out_y)
{
    int row = b_idx / SLICE_COLS;
    int col = b_idx % SLICE_COLS;
    *out_x = base_x + col * (SLICE_WIDTH + SLICE_GAP);
    *out_y = base_y + row * SLICE_HEIGHT;
}

static inline void cell_position(int base_x, int base_y, int r_idx, int g_idx, int b_idx, int *x, int *y)
{
    int ox, oy;
    slice_origin(base_x, base_y, b_idx, &ox, &oy);
    *x = ox + CELL_OFFSET + r_idx * CELL_STRIDE;
    *y = oy + 1 + g_idx;
}

static void draw_cell(int x, int y, int r_idx, int g_idx, int b_idx, int selected)
{
    T_move(x - 1, y);
    fputs(selected ? "[" : " ", stdout);
    T_placeStr(x, y, "██", level_values[r_idx], level_values[g_idx], level_values[b_idx]);
    T_move(x + 2, y);
    fputs(selected ? "]" : " ", stdout);
    fflush(stdout);
    T_resetColor();
}

static void draw_palette(int base_x, int base_y)
{
    for (int b = 0; b < LEVELS; ++b)
    {
        int ox, oy;
        slice_origin(base_x, base_y, b, &ox, &oy);

        T_move(ox + LABEL_OFFSET, oy);
        printf("B=%3d", level_values[b]);
        fflush(stdout);

        for (int g = 0; g < LEVELS; ++g)
        {
            for (int r = 0; r < LEVELS; ++r)
            {
                int x, y;
                cell_position(base_x, base_y, r, g, b, &x, &y);
                draw_cell(x, y, r, g, b, 0);
            }
        }
    }
}

int main(void)
{
    system("clear");
    print("Gradiente RGB 6x6x6 (0-255). Flechas para moverte, Enter para elegir, q para salir.\n");

    int base_x = 2;
    int base_y = T_curs_y + 1;

    draw_palette(base_x, base_y);

    int r_idx = 0, g_idx = 0, b_idx = 0;
    int sel_x, sel_y;
    cell_position(base_x, base_y, r_idx, g_idx, b_idx, &sel_x, &sel_y);
    draw_cell(sel_x, sel_y, r_idx, g_idx, b_idx, 1);

    T_hideCursor();
    enable_raw_mode();

    int done = 0;
    while (!done)
    {
        int key = read_key();
        if (key == 0)
            continue;

        if (key == 'q' || key == 'Q')
        {
            T_move(1, base_y + 6 + (LEVELS + 2) + 2);
            print("Cancelado.\n");
            return 0;
        }

        if (key == '\n')
        {
            int slice_rows = (LEVELS + SLICE_COLS - 1) / SLICE_COLS; /* 2 filas para 6 niveles */
            int palette_height = slice_rows * SLICE_HEIGHT;
            int final_y = base_y + palette_height + 2;
            T_move(1, final_y);
            T_resetColor();
            print("Seleccionaste -> R: %3d\tG: %3d\tB: %3d\n",
                level_values[r_idx], level_values[g_idx], level_values[b_idx]);
            //done = 1;
            //break;
        }

        int old_r = r_idx;
        int old_g = g_idx;
        int old_b = b_idx;

        switch (key)
        {
        case 'L':
            if (r_idx > 0)
                r_idx--;
            break;
        case 'R':
            if (r_idx < LEVELS - 1)
                r_idx++;
            break;
        case 'U':
            if (g_idx > 0)
            {
                g_idx--;
            }
            else if (b_idx > 0)
            {
                b_idx--;
                g_idx = LEVELS - 1;
            }
            break;
        case 'D':
            if (g_idx < LEVELS - 1)
            {
                g_idx++;
            }
            else if (b_idx < LEVELS - 1)
            {
                b_idx++;
                g_idx = 0;
            }
            break;
        default:
            break;
        }

        if (old_r != r_idx || old_g != g_idx || old_b != b_idx)
        {
            cell_position(base_x, base_y, old_r, old_g, old_b, &sel_x, &sel_y);
            draw_cell(sel_x, sel_y, old_r, old_g, old_b, 0);
            cell_position(base_x, base_y, r_idx, g_idx, b_idx, &sel_x, &sel_y);
            draw_cell(sel_x, sel_y, r_idx, g_idx, b_idx, 1);
        }
    }
    T_showCursor();
    restore_terminal();
    return 0;
}
