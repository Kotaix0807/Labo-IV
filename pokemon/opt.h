#ifndef OPT_H
#define OPT_H

#include "tools.h"
#include "box.h"
#include "structs.h"
#include <time.h>
#include <stdlib.h>

int mainMenu(ply *Player);
int askName(ply *Player);
void printTitle();
void credits();
void askPkmn(ply *cur);
void preview();
void Combat(ply *cur);

int mainMenu(ply *Player)
{
    printTitle();
    char *choices[] = {
        "Combate", //Local
        "Cambiar Nombre", //Cambiar Nombre
        "Creditos", //Creditos
        "Salir" //Salir
    };
    int selection = menu("Menu", choices, sizeof(choices)/sizeof(char*), -1, -1);
    switch(selection)
    {
        case 0:
            if(!Player->name)
                askName(Player);
            askPkmn(Player);
            Combat(Player);
            break;
        case 1:
            askName(Player);
            //preview();
            break;
        case 2:
            credits();
            break;
        case 3:
            break;
        default:
            break;
    }
    return selection;
}

int askName(ply *Player)
{
    while(1)
    {
        Player->name = Ask("Ingresa tu nombre:", 10);
        if(Player->name && Player->name[0] != '\0')
            break;
        free(Player->name);
        int yMax = getmaxy(stdscr);
        mvprintw(yMax - 1, 0, "Intentalo de nuevo");
        clrtoeol();
        refresh();
    }
    if(!Player->name)
    {
        perror("Player name not set");
        endwin();
        exit(1);
        return 1;
    }
    return 0;
}

/**
 * @brief Imprimir titulo
 * 
 */
void printTitle()
{
    int xMax = getmaxx(stdscr);
    char *lines[] = {
    "                                  ,'\\",
    "    _.----.        ____         ,'  _\\   ___    ___     ____",
    "_,-'       `.     |    |  /`.   \\,-'    |   \\  /   |   |    \\  |`.",
    "\\      __    \\    '-.  | /   `.  ___    |    \\/    |   '-.   \\ |  |",
    " \\.    \\ \\   |  __  |  |/    ,','_  `.  |          | __  |    \\|  |",
    "   \\    \\/   /,' _`.|      ,' / / / /   |          ,' _`.|     |  |",
    "    \\     ,-'/  /   \\    ,'   | \\/ / ,`.|         /  /   \\  |     |",
    "     \\    \\ |   \\_/  |   `-.  \\    `'  /|  |    ||   \\_/  | |\\    |",
    "      \\    \\ \\      /       `-.`.___,-' |  |\\  /| \\      /  | |   |",
    "       \\    \\ `.__,'|  |`-._    `|      |__| \\/ |  `.__,'|  | |   |",
    "        \\_.-'       |__|    `-._ |              '-.|     '-.| |   |",
    "                                `'                            '-._|"
    };

    int rows = (int)(sizeof(lines) / sizeof(char *));
    int length = largestStr(lines, rows);
    for(int i = 0; i < rows; i++)
        mvwprintw(stdscr, i, ((xMax / 2) - length / 2), "%s", lines[i]);
    wrefresh(stdscr);
}


void credits()
{
    wclear(stdscr);
    wrefresh(stdscr);

    char *credits[] = {
        "Alguien lee los creditos?",
        "Creado por:",
        "-Eloy Quezada",
        "-Dario Duarte",
        "-Gabriel Hernandez",
        "Proyecto creado para el ramo:",
        "Introduccion a sistemas operativos.",
        "Creado en lenguaje C con la ayuda de ncurses",
        "Universidad de Magallanes 2025"
    };

    dialFromStr(credits, (int)(sizeof(credits)/sizeof(char *)), "Creditos", -1, -1, ALIGN_CENTER);
}

void askPkmn(ply *cur)
{
    char *pklist[] = {
        "Venosaur",
        "Blastoise",
        "Charizard"
    };
    int pkmn = menu("Elige tu pokemon", pklist, sizeof(pklist)/sizeof(char*), -1, -1);
    pkmnSet(&cur->monster, pklist[pkmn]);
}

void preview()
{
    int h = atoi(Ask("Alto de la ventana:", 3));
    int w = atoi(Ask("Ancho de la ventana:", 3));
    previewWindow(w, h);
}

void Combat(ply *cur)
{
    srand(time(NULL));
    wclear(stdscr);
    wrefresh(stdscr);

    int xM, yM;
    getmaxyx(stdscr, yM, xM);

    key_t llave = (key_t)atoi(Ask("Ingrese la clave", 4));

   //Por ahora se queda asi hasta que se incluya la comunicacion de procesos
    pkmn *enemy = NULL;
    pkmnSet(&enemy, "Charizard");
    int enemy_h = enemy->n_ascii;
    //int player_h = cur->monster->n_ascii;

    while(1)
    {
        WINDOW *player_win = printPkmnW(cur->monster, 45, 15);
        WINDOW *enemy_win = printPkmnW(enemy, (xM - enemy->w - 2), 0);
        char *init[] = {
            "Jugador: %s",
            "Tu oponente es: %s"
        };
        replace_fmt(init, 0, cur->name);
        replace_fmt(init, 1, enemy->name);
        int init_lines = (int)(sizeof(init)/sizeof(char*));
        int init_h = 10 - init_lines;
        txt_box *combat_box = custTxtBox_str(init, init_lines, NULL, 1, (yM - init_h - 2), 40, init_h, ALIGN_CENTER);
        char *move_names[4];
        for (int i = 0; i < 4; i++)
            move_names[i] = cur->monster->move_set[i].name;
        int mv = menu("Que deseas hacer?", move_names, 4, 1, (yM - 6));
        delBox(combat_box);

        char *used[] = {
            "%s uso:",
            "%s!"
        };
        replace_fmt(used, 0, cur->monster->name);
        replace_fmt(used, 1, cur->monster->move_set[mv].name);
        int used_lines = (int)(sizeof(used)/sizeof(char*));
        int used_h = 10 - used_lines;
        txt_box *use = custTxtBox_str(used, used_lines, NULL, 1, (yM - used_h - 2), 40, used_h, ALIGN_CENTER);
        napms(1000);
        flushinp(); /* descartar teclas pulsadas durante la pausa */
        delBox(use);
        int damage = formula(*(cur->monster), cur->monster->move_set[mv], *enemy);
        if(enemy->hp > damage)
            enemy->hp -= damage;
        else
        {
            enemy->hp = 0;
            for(int i = 0; i < enemy_h; i++)
            {
                if(enemy->n_ascii > 0)
                    enemy->n_ascii--;
                printPkmnW(enemy, (xM - enemy->w - 2), 0);
                napms(45);
                flushinp();
            }
            clearWin(enemy_win);
            clearWin(player_win);
            break;
        }
        printPkmnW(enemy, (xM - enemy->w - 2), 0);
        /**
        txt_box *use = custTxtBox_str(used, used_lines, NULL, 1, (yM - used_h - 2), 40, used_h, ALIGN_CENTER);

        for (int i = 0; i < 4; i++)
            free(move_names[i]);
        free(move_names);

        for (int i = 0; i < 4; i++)
            move_names[i] = enemy->move_set[i].name;
        mv = rand() % 3;
        replace_fmt(used, 0, enemy->name);
        replace_fmt(used, 1, enemy->move_set[mv].name);
        napms(1000);
        int damage = formula(*enemy, enemy->move_set[mv], *(cur->monster));
        if(cur->monster->hp > damage)
            cur->monster->hp -= damage;
        else
        {
            cur->monster->hp = 0;
            for(int i = 0; i < player_h; i++)
            {
                if(cur->monster->n_ascii > 0)
                    cur->monster->n_ascii--;
                printPkmnW(cur->monster, (xM - cur->monster->w - 2), 0);
                napms(45);
                flushinp();
            }
            clearWin(enemy_win);
            clearWin(player_win);
            break;
        }

        */
        clearWin(enemy_win);
        clearWin(player_win);
    }
    char *end[] = {
        "Felicidades %s!",
        "Haz ganado el combate pokemon"
    };
    replace_fmt(end, 0, cur->name);
    dialFromStr(end, 2, "Winner!", -1, -1, ALIGN_CENTER);

    if (enemy->ascii)
    {
        for (int i = 0; i < enemy_h; i++)
            free(enemy->ascii[i]);
        free(enemy->ascii);
    }
    free(enemy);

    wclear(stdscr);
    wrefresh(stdscr);
}

/*
    ENEMIGO: X = 195, Y = 0


*/


/*
     MIN_PANTALLA:

*/
#endif
