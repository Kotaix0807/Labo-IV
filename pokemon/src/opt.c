#include <stdlib.h>
#include <string.h>
#include "opt.h"
#include "box.h"
#include "tools.h"

int mainMenu()
{
    printTitle();
    char *choices[] = {
        "Combate", //Local
        "Cambiar Nombre", //Cambiar Nombre
        "Creditos", //Creditos
        "Salir" //Salir
    };
    return menu("Menu", choices, sizeof(choices)/sizeof(char*), -1, -1);
}

int askName(ply *Player)
{
    while(1)
    {
        char *tmp = Ask("Ingresa tu nombre:", 10);
        if(tmp && tmp[0] != '\0')
        {
            strncpy(Player->name, tmp, sizeof(Player->name) - 1);
            Player->name[sizeof(Player->name) - 1] = '\0';
            free(tmp);
            break;
        }
        free(tmp);
        int yMax = getmaxy(stdscr);
        mvprintw(yMax - 1, 0, "Intentalo de nuevo");
        clrtoeol();
        refresh();
    }
    if(Player->name[0] == '\0')
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
    char *title[] = {
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

    int title_rows = (int)(sizeof(title) / sizeof(char *));
    int title_length = largestStr(title, title_rows);
    for(int i = 0; i < title_rows; i++)
        mvwprintw(stdscr, i, ((xMax / 2) - title_length / 2), "%s", title[i]);
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
    int credits_rows = (int)(sizeof(credits)/sizeof(char *));
    dialFromStr(credits, credits_rows, "Creditos", -1, -1, ALIGN_CENTER);
}

void askPkmn(ply *cur)
{
    char *pklist[] = {
        "Venosaur",
        "Blastoise",
        "Charizard"
    };
    int pklist_rows = (int)(sizeof(pklist)/sizeof(char*));
    int pkmn = menu("Elige tu pokemon", pklist, pklist_rows, -1, -1);
    pkmnSet(&cur->monster, pklist[pkmn], 0);
}

void preview()
{
    char *h_in = Ask("Alto de la ventana:", 3);
    char *w_in = Ask("Ancho de la ventana:", 3);
    int h = atoi(h_in);
    int w = atoi(w_in);
    free(h_in);
    free(w_in);
    previewWindow(w, h);
}


/*
    ENEMIGO: X = 195, Y = 0


*/


/*
     MIN_PANTALLA:

*/
