#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <math.h>
#include "tools.h"

#define TYPE_NONE -1
#define FIRE 0
#define WATER 1
#define PLANT 2
#define POISON 3
#define GROUND 4
#define FLYING 5
#define DRAGON 6
#define ICE 7
#define SINESTER 8
#define NORMAL 9

/*
VENOSAUR:
-Giga Drain
-Sludge Bomb
-Earthquake
-Sleep Powder

CHARIZARD:
-Flamethrower
-Air Slash
-Dragon Pulse
-Roost

BLASTOISE:
-Surf
-Ice Beam
-Dark Pulse
-Rapid Spin

*/


typedef struct move_{
    char *name;
    int type;
    unsigned int power;
}move_t;

typedef struct pokemon_{
    //Informacion base
    const char *name;
    char **ascii;
    int n_ascii;
    int w;
    
    //Estadisticas
    int type1;
    int type2;
    
    move_t move_set[4];
    int attack;
    int defense;
    int speed;
    int hp;
    int level;
}pkmn;

typedef struct player_{
    char *name;
    pkmn *monster;
}ply;

ply initPly();
void pkmnSet(pkmn **monster, const char *name);
void printPkmn(pkmn *monster, int x, int y);
WINDOW *movePkmnWindow(pkmn *monster);
WINDOW *printPkmnW(pkmn *monster, int x, int y);
void clearWin(WINDOW *win);
int formula(pkmn p, move_t used, pkmn e);

/**
 * @brief Inicializar estructura jugador
 * 
 * @return struct ply 
 */
ply initPly()
{
    ply current;
    current.name = NULL;
    current.monster = NULL;
    return current;
}

void pkmnSet(pkmn **monster, const char *name)
{
    if (!monster || !name)
        return;

    if (*monster)
    {
        if ((*monster)->ascii)
        {
            for (int i = 0; i < (*monster)->n_ascii; i++)
                free((*monster)->ascii[i]);
            free((*monster)->ascii);
        }
        free(*monster);
    }

    pkmn *m = calloc(1, sizeof(pkmn));
    if (!m)
        return;

    m->name = name;
    m->level = 50;
    m->type1 = TYPE_NONE;
    m->type2 = TYPE_NONE;
    if(!strcmp(name, "Venosaur"))
    {
        m->ascii = readText("art/venosaur.txt");
        m->n_ascii = fileLines("art/venosaur.txt", 0);
        const char *moves[] = {
            "Giga Drain",
            "Sludge Bomb",
            "Earthquake",
            "Sleep Powder"
        };
        for (int i = 0; i < 4; i++)
            m->move_set[i].name = (char *)moves[i];
        m->move_set[0].type = PLANT;
        m->move_set[0].power = 75; /* Giga Drain */
        m->move_set[1].type = POISON;
        m->move_set[1].power = 90; /* Sludge Bomb */
        m->move_set[2].type = GROUND;
        m->move_set[2].power = 100; /* Earthquake */
        m->move_set[3].type = PLANT;
        m->move_set[3].power = 0; /* Sleep Powder, movimiento de estado */

        m->type1 = PLANT;
        m->type2 = POISON;
        
        m->attack = 102;
        m->defense = 103;
        m->speed = 100;
        m->hp = 155;
    }
    else if(!strcmp(name, "Charizard"))
    {
        m->ascii = readText("art/charizard.txt");
        m->n_ascii = fileLines("art/charizard.txt", 0);
        const char *moves[] = {
            "Flamethrower",
            "Air Slash",
            "Dragon Pulse",
            "Roost"
        };
        for (int i = 0; i < 4; i++)
            m->move_set[i].name = (char *)moves[i];
        m->move_set[0].type = FIRE;
        m->move_set[0].power = 90; /* Flamethrower */
        m->move_set[1].type = FLYING;
        m->move_set[1].power = 75; /* Air Slash */
        m->move_set[2].type = DRAGON;
        m->move_set[2].power = 85; /* Dragon Pulse */
        m->move_set[3].type = FLYING;
        m->move_set[3].power = 0; /* Roost, movimiento de recuperacion */

        m->type1 = FIRE;
        m->type2 = FLYING;

        m->attack = 104;
        m->defense = 98;
        m->speed = 120;
        m->hp = 153;
    }
    else if(!strcmp(name, "Blastoise"))
    {
        m->ascii = readText("art/blastoise.txt");
        m->n_ascii = fileLines("art/blastoise.txt", 0);
        const char *moves[] = {
            "Surf",
            "Ice Beam",
            "Dark Pulse",
            "Rapid Spin"
        };
        for (int i = 0; i < 4; i++)
            m->move_set[i].name = (char *)moves[i];
        m->move_set[0].type = WATER;
        m->move_set[0].power = 90; /* Surf */
        m->move_set[1].type = ICE;
        m->move_set[1].power = 90; /* Ice Beam */
        m->move_set[2].type = SINESTER;
        m->move_set[2].power = 80; /* Dark Pulse */
        m->move_set[3].type = NORMAL;
        m->move_set[3].power = 50; /* Rapid Spin (poder actualizado) */

        m->type1 = WATER;
        m->type2 = TYPE_NONE;

        m->attack = 103;
        m->defense = 120;
        m->speed = 98;
        m->hp = 154;
    }
    m->w = largestStr_bra(m->ascii, m->n_ascii);
    *monster = m;
}

void printPkmn(pkmn *monster, int x, int y)
{
    int yM, xM;
    getmaxyx(stdscr, yM, xM);
    int h = monster->n_ascii;
    int w = monster->w;
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
}

/**
 * @brief Permite mover la ventana del pokemon con flechas; con 'r' muestra la posicion actual.
 *        Salir con ENTER o 'q'.
 */
WINDOW *movePkmnWindow(pkmn *monster)
{
    int yM, xM;
    getmaxyx(stdscr, yM, xM);
    int win_h = monster->n_ascii;
    int win_w = monster->w;
    int pos_y = (yM - win_h) / 2;
    int pos_x = (xM - win_w) / 2;

    WINDOW *win = printPkmnW(monster, pos_x, pos_y);
    keypad(win, TRUE);

    while (1)
    {
        werase(win);
        box(win, 0, 0);
        for (int i = 0; i < win_h; i++)
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

        if (pos_y < 0)
            pos_y = 0;
        if (pos_x < 0)
            pos_x = 0;
        if (pos_y > yM - win_h)
            pos_y = yM - win_h;
        if (pos_x > xM - win_w)
            pos_x = xM - win_w;

        wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
        wrefresh(win);
        mvwin(win, pos_y, pos_x);
    }
    mvprintw(yM - 1, 0, " ");
    clrtoeol();
    //wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    //wclear(win);
    //wrefresh(win);
    //delwin(win);
    refresh();
    return win;
}

WINDOW *printPkmnW(pkmn *monster, int x, int y)
{
    int yM, xM;
    getmaxyx(stdscr, yM, xM);
    int h = monster->n_ascii;
    int w = monster->w;
    int win_h = h + 2;
    int win_w = w + 2;

    int win_y, win_x;

    if(y < 0)
        win_y = (yM - win_h) / 2;
    else
        win_y = y;

    if(x < 0)
        win_x = (xM - win_w) / 2;
    else
        win_x = x;

    WINDOW *win = newwin(win_h, win_w, win_y, win_x);
    //box(win, 0, 0);
    for(int i = 0; i < h; i++)
        mvwprintw(win, i + 1, 1, "%s", monster->ascii[i]);
    mvwprintw(win, 0, 2, "[%s]/[HP: %d]", monster->name, monster->hp);
    wrefresh(win);
    return win;

}

void clearWin(WINDOW *win)
{
    wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wclear(win);
    wrefresh(win);
    delwin(win);
}

static double type_multiplier_single(int move_type, int target_type)
{
    if (target_type == TYPE_NONE)
        return 1.0;

    switch (move_type)
    {
        case FIRE:
            switch (target_type)
            {
                case PLANT:
                case ICE:
                    return 2.0;
                case WATER:
                case FIRE:
                case DRAGON:
                    return 0.5;
                default:
                    return 1.0;
            }
        case WATER:
            switch (target_type)
            {
                case FIRE:
                case GROUND:
                    return 2.0;
                case WATER:
                case PLANT:
                case DRAGON:
                    return 0.5;
                default:
                    return 1.0;
            }
        case PLANT:
            switch (target_type)
            {
                case WATER:
                case GROUND:
                    return 2.0;
                case FIRE:
                case PLANT:
                case POISON:
                case FLYING:
                case DRAGON:
                    return 0.5;
                default:
                    return 1.0;
            }
        case POISON:
            switch (target_type)
            {
                case PLANT:
                    return 2.0;
                case POISON:
                case GROUND:
                    return 0.5;
                default:
                    return 1.0;
            }
        case GROUND:
            switch (target_type)
            {
                case FIRE:
                case POISON:
                    return 2.0;
                case PLANT:
                    return 0.5;
                case FLYING:
                    return 0.0;
                default:
                    return 1.0;
            }
        case FLYING:
            switch (target_type)
            {
                case PLANT:
                    return 2.0;
                default:
                    return 1.0;
            }
        case DRAGON:
            if (target_type == DRAGON)
                return 2.0;
            return 1.0;
        case ICE:
            switch (target_type)
            {
                case PLANT:
                case DRAGON:
                    return 2.0;
                case FIRE:
                case WATER:
                case ICE:
                    return 0.5;
                default:
                    return 1.0;
            }
        case SINESTER:
            if (target_type == SINESTER)
                return 0.5;
            return 1.0;
        case NORMAL:
            return 1.0;
        default:
            return 1.0;
    }
}

static double type_multiplier(int move_type, const pkmn *def)
{
    double modifier = type_multiplier_single(move_type, def->type1);
    modifier *= type_multiplier_single(move_type, def->type2);
    return modifier;
}

int formula(pkmn p, move_t used, pkmn e)
{
    if (used.power == 0)
        return 0;

    double stab;
    if (used.type == p.type1 || used.type == p.type2)
        stab = 1.5;
    else
        stab = 1.0;
    double effectiveness = type_multiplier(used.type, &e);
    if (effectiveness == 0.0)
        return 0;
    double defense;
    if (e.defense > 0)
        defense = (double)e.defense;
    else
        defense = 1.0;

    double base = (((2.0 * p.level) / 5.0) + 2.0) * used.power * ((double)p.attack / defense);
    base = (base / 50.0) + 2.0;

    double damage = floor(base * stab * effectiveness);
    if (damage < 1.0)
        damage = 1.0;
    return (int)damage;
}


#endif /* STRUCTS_H */
