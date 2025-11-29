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

#endif
