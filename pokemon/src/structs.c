#include <ncurses.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "structs.h"
#include "tools.h"

/**
 * @brief Inicializar estructura jugador
 * 
 * @return struct ply 
 */
ply initPly()
{
    ply current;
    memset(&current, 0, sizeof(ply));
    return current;
}

static void copy_ascii(pkmn *m, char **src, int lines)
{
    if (!src || lines <= 0)
        return;
    int max_lines;
    if (lines < 200)
        max_lines = lines;
    else
        max_lines = 200;
    for (int i = 0; i < max_lines; i++)
    {
        strncpy(m->ascii[i], src[i], sizeof(m->ascii[i]) - 1);
        m->ascii[i][sizeof(m->ascii[i]) - 1] = '\0';
    }
    m->n_ascii = max_lines;
}

void pkmnSet(pkmn *monster, const char *name, int reverse)
{
    if (!monster || !name)
        return;

    char name_copy[sizeof(monster->name)];
    strncpy(name_copy, name, sizeof(name_copy) - 1);
    name_copy[sizeof(name_copy) - 1] = '\0';

    memset(monster, 0, sizeof(pkmn));
    strncpy(monster->name, name_copy, sizeof(monster->name) - 1);
    monster->name[sizeof(monster->name) - 1] = '\0';
    monster->level = 50;
    monster->type1 = TYPE_NONE;
    monster->type2 = TYPE_NONE;

    char **ascii_lines = NULL;
    int lines = 0;

    if(!strcmp(name, "Venosaur"))
    {
        if(reverse)
        {
            ascii_lines = readText("art/en_venosaur.txt");
            lines = (int)fileLines("art/en_venosaur.txt", 0);
        }
        else {
            ascii_lines = readText("art/venosaur.txt");
            lines = (int)fileLines("art/venosaur.txt", 0);
        }
        const char *moves[] = {
            "Giga Drain",
            "Sludge Bomb",
            "Earthquake",
            "Sleep Powder"
        };
        for (int i = 0; i < 4; i++)
            strcpy(monster->move_set[i].name, moves[i]);
        monster->move_set[0].type = PLANT;
        monster->move_set[0].power = 75; /* Giga Drain */
        monster->move_set[1].type = POISON;
        monster->move_set[1].power = 90; /* Sludge Bomb */
        monster->move_set[2].type = GROUND;
        monster->move_set[2].power = 100; /* Earthquake */
        monster->move_set[3].type = PLANT;
        monster->move_set[3].power = 0; /* Sleep Powder, movimiento de estado */

        monster->type1 = PLANT;
        monster->type2 = POISON;
        
        monster->attack = 102;
        monster->defense = 103;
        monster->speed = 100;
        monster->hp = 155;
    }
    else if(!strcmp(name, "Charizard"))
    {
        if(reverse)
        {
            ascii_lines = readText("art/en_charizard.txt");
            lines = (int)fileLines("art/en_charizard.txt", 0);
        }
        else {
            ascii_lines = readText("art/charizard.txt");
            lines = (int)fileLines("art/charizard.txt", 0);
        }
        const char *moves[] = {
            "Flamethrower",
            "Air Slash",
            "Dragon Pulse",
            "Roost"
        };
        for (int i = 0; i < 4; i++)
            strcpy(monster->move_set[i].name, moves[i]);
        monster->move_set[0].type = FIRE;
        monster->move_set[0].power = 90; /* Flamethrower */
        monster->move_set[1].type = FLYING;
        monster->move_set[1].power = 75; /* Air Slash */
        monster->move_set[2].type = DRAGON;
        monster->move_set[2].power = 85; /* Dragon Pulse */
        monster->move_set[3].type = FLYING;
        monster->move_set[3].power = 0; /* Roost, movimiento de recuperacion */

        monster->type1 = FIRE;
        monster->type2 = FLYING;

        monster->attack = 104;
        monster->defense = 98;
        monster->speed = 120;
        monster->hp = 153;
    }
    else if(!strcmp(name, "Blastoise"))
    {
        if(reverse)
        {
            ascii_lines = readText("art/en_blastoise.txt");
            lines = (int)fileLines("art/en_blastoise.txt", 0);
        }
        else {
            ascii_lines = readText("art/blastoise.txt");
            lines = (int)fileLines("art/blastoise.txt", 0);
        }
        const char *moves[] = {
            "Surf",
            "Ice Beam",
            "Dark Pulse",
            "Rapid Spin"
        };
        for (int i = 0; i < 4; i++)
            strcpy(monster->move_set[i].name, moves[i]);
        monster->move_set[0].type = WATER;
        monster->move_set[0].power = 90; /* Surf */
        monster->move_set[1].type = ICE;
        monster->move_set[1].power = 90; /* Ice Beam */
        monster->move_set[2].type = SINESTER;
        monster->move_set[2].power = 80; /* Dark Pulse */
        monster->move_set[3].type = NORMAL;
        monster->move_set[3].power = 50; /* Rapid Spin */

        monster->type1 = WATER;
        monster->type2 = TYPE_NONE;

        monster->attack = 103;
        monster->defense = 120;
        monster->speed = 98;
        monster->hp = 154;
    }
    else {
        clear();
        printw("ERROR [%s]", name);
        refresh();
    }

    copy_ascii(monster, ascii_lines, lines);

    if (monster->n_ascii > 0)
    {
        char *ascii_ptrs[200];
        for (int i = 0; i < monster->n_ascii; i++)
            ascii_ptrs[i] = monster->ascii[i];
        monster->w = largestStr_bra(ascii_ptrs, monster->n_ascii);
    }

    if (ascii_lines)
    {
        for (int i = 0; i < lines; i++)
            free(ascii_lines[i]);
        free(ascii_lines);
    }
}

void printPkmn(pkmn *monster, int x, int y)
{
    int yM, xM;
    getmaxyx(stdscr, yM, xM);
    int h = monster->n_ascii;
    int w = monster->w;
    int win_h = h + 2;
    int win_w = w + 2;

    int win_y = 0, win_x = 0;

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
