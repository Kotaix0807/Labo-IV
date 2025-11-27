#ifndef OPT_H
#define OPT_H

#include "tools.h"
#include "box.h"
#include "structs.h"

int mainMenu(ply *Player);
int askName(ply *Player);
void printTitle();
void credits();

int mainMenu(ply *Player)
{
    printTitle();
    char *choices[] = {
        "Local",
        "Cambiar nombre",
        "Creditos",
        "Salir"
    };
    int selection = menu("Menu", choices, 4);
    switch(selection)
    {
        case 0:
            break;
        case 1:
            askName(Player);
            break;
        case 2:
            credits();
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
    if (xMax < length + 50)
    {
        char *title = "Pokemon";
        mvwprintw(stdscr, 0, (xMax - (int)strlen(title)) / 2, "%s", title);
        wrefresh(stdscr);
        return;
    }
    for(int i = 0; i < rows; i++)
        mvwprintw(stdscr, i, ((xMax / 2) - length / 2), "%s", lines[i]);
    wrefresh(stdscr);
}


void credits()
{
    wclear(stdscr);
    wrefresh(stdscr);

    char *txt[] = {
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

    int rows = (int)(sizeof(txt) / sizeof(char *));
    dialFromStr(txt, rows, "Creditos", 0, 0, ALIGN_CENTER, TXT_CENTER);
}

#endif
