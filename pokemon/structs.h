#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "tools.h"

typedef struct pokemon_{
    char *name;
    char *move_set[4];
    char **ascii;
    int n_ascii;
    int w;
    unsigned int attack;
    unsigned int defense;
    unsigned int speed;
    unsigned int hp;
}pkmn;

typedef struct player_{
    char *name;
    pkmn *monster;
}ply;

/**
 * @brief Inicializar estructura jugador
 * 
 * @return struct ply 
 */
ply initPly(){
    ply current;
    current.name = NULL;
    current.monster = NULL;
    return current;
}

void pkmnSet(ply *curr, char *name)
{
    curr->monster = malloc(sizeof(pkmn));
    curr->monster->name = name;
    if(!strcmp(name, "Venosaur"))
    {
        curr->monster->ascii = readText("art/venosaur.txt");
        curr->monster->n_ascii = fileLines("art/venosaur.txt", 0);
        curr->monster->w = largestStr_bra(curr->monster->ascii, curr->monster->n_ascii);
        const char *moves[] = {
            "Movimiento_1",
            "Movimiento_2",
            "Movimiento_3",
            "Movimiento_4"
        };
        for (int i = 0; i < 4; i++)
            curr->monster->move_set[i] = (char *)moves[i];
        curr->monster->attack = 50;
        curr->monster->defense = 50;
        curr->monster->speed = 50;
        curr->monster->hp = 100;
    }
    else if(!strcmp(name, "Charizard"))
    {
        curr->monster->ascii = readText("art/charizard.txt");
        curr->monster->n_ascii = fileLines("art/charizard.txt", 0);
        curr->monster->w = largestStr_bra(curr->monster->ascii, curr->monster->n_ascii);
        const char *moves[] = {
            "Movimiento_1",
            "Movimiento_2",
            "Movimiento_3",
            "Movimiento_4"
        };
        for (int i = 0; i < 4; i++)
            curr->monster->move_set[i] = (char *)moves[i];
        curr->monster->attack = 50;
        curr->monster->defense = 50;
        curr->monster->speed = 50;
        curr->monster->hp = 100;
    }
    else if(!strcmp(name, "Blastoise"))
    {
        curr->monster->ascii = readText("art/blastoise.txt");
        curr->monster->n_ascii = fileLines("art/blastoise.txt", 0);
        curr->monster->w = largestStr_bra(curr->monster->ascii, curr->monster->n_ascii);
        const char *moves[] = {
            "Movimiento_1",
            "Movimiento_2",
            "Movimiento_3",
            "Movimiento_4"
        };
        for (int i = 0; i < 4; i++)
            curr->monster->move_set[i] = (char *)moves[i];
        curr->monster->attack = 50;
        curr->monster->defense = 50;
        curr->monster->speed = 50;
        curr->monster->hp = 100;
    }
}

void printPkmn(pkmn *monster, int x, int y)
{
    int yM, xM;
    getmaxyx(stdscr, yM, xM);
    int h = monster->n_ascii;
    int w = 50;
    int win_h = h + 2;
    int win_w = w + 2;

    int win_y, win_x;

    if(y < 0)
        win_y = (yM - win_h) / 2;
    if(x < 0)
        win_x = (xM - win_w) / 2;

    WINDOW *win = newwin(win_h, win_w, win_y, win_x);
    box(win, 0, 0);
    for(int i = 0; i < h; i++)
        mvwprintw(win, i + 1, 1, "%s", monster->ascii[i]);
    wrefresh(win);
    //wgetch(win);
    //wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    //wclear(win);
    //wrefresh(win);
    //delwin(win);
}

/**
 * @brief Permite mover la ventana del pokemon con flechas; con 'r' muestra la posicion actual.
 *        Salir con ENTER o 'q'.
 */
void movePkmnWindow(pkmn *monster)
{
    int yM, xM;
    getmaxyx(stdscr, yM, xM);
    int h = monster->n_ascii;
    int w = 50;
    int win_h = h + 2;
    int win_w = w + 2;
    int pos_y = (yM - win_h) / 2;
    int pos_x = (xM - win_w) / 2;

    WINDOW *win = newwin(win_h, win_w, pos_y, pos_x);
    keypad(win, TRUE);

    while (1)
    {
        werase(win);
        box(win, 0, 0);
        for (int i = 0; i < h; i++)
            mvwprintw(win, i + 1, 1, "%s", monster->ascii[i]);
        wrefresh(win);

        int ch = wgetch(win);
        if (ch == 'q' || ch == '\n')
            break;

        switch (ch)
        {
            case KEY_UP:    
                pos_y--;
                break;
            case KEY_DOWN:
                pos_y++;
                break;
            case KEY_LEFT:  
                pos_x--;
                break;
            case KEY_RIGHT:
                pos_x++;
                break;
            case 'r':
                mvprintw(yM - 1, 0, "Ventana en x = %d, y = %d", pos_x, pos_y);
                clrtoeol();
                refresh();
                break;
            default:
                break;
        }

        /* límites para que no se salga de la pantalla */
        if (pos_y < 0)
            pos_y = 0;
        if (pos_x < 0)
            pos_x = 0;
        if (pos_y > yM - win_h)
            pos_y = yM - win_h;
        if (pos_x > xM - win_w)
            pos_x = xM - win_w;

        /* limpiar la posición anterior antes de mover */
        wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
        wrefresh(win);
        mvwin(win, pos_y, pos_x);
    }
    mvprintw(yM - 1, 0, " ");
    clrtoeol();
    //wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    //wclear(win);
    //wrefresh(win);
    refresh();
    delwin(win);
}
#endif /* STRUCTS_H */
