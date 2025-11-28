#ifndef OPT_H
#define OPT_H

#include "tools.h"
#include "box.h"
#include "structs.h"

int mainMenu(ply *Player);
int askName(ply *Player);
void printTitle();
void credits();
void local(ply *cur);
void preview();
void render_combat_scene(ply *cur);

int mainMenu(ply *Player)
{
    //printTitle();
    char *choices[] = {
        "Ver Pokemon", //Local
        "Ventana", //Cambiar Nombre
        "Creditos", //Creditos
        "Render scene",
        "Salir" //Salir
    };
    int selection = menu("Menu", choices, sizeof(choices)/sizeof(char*));
    switch(selection)
    {
        case 0:
            local(Player);
            break;
        case 1:
            //askName(Player);
            preview();
            break;
        case 2:
            credits();
            break;
        case 3:
            local(Player);
            render_combat_scene(Player);
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

void local(ply *cur)
{
    char *pklist[] = {
        "Venosaur",
        "Blastoise",
        "Charizard"
    };
    int pkmn = menu("Entrenador, elige tu pokemon", pklist, sizeof(pklist)/sizeof(char*));
    pkmnSet(cur, pklist[pkmn]);
}


void preview()
{
    int h = atoi(Ask("Alto de la ventana:", 3));
    int w = atoi(Ask("Ancho de la ventana:", 3));
    previewWindow(w, h);
}


void render_combat_scene(ply *cur)
{
    wclear(stdscr);
    wrefresh(stdscr);

    int yM, xM;
    getmaxyx(stdscr, yM, xM);

    pkmn *enemy = malloc(sizeof(pkmn));
    if(!enemy)
        return;
    enemy->name = "Blastoise";
    enemy->ascii = readText("art/blastoise.txt");
    enemy->n_ascii = (int)fileLines("art/blastoise.txt", 0);
    const char *moves_enemy[] = {
        "Movimiento_1",
        "Movimiento_2",
        "Movimiento_3",
        "Movimiento_4"
    };
    for (int i = 0; i < 4; i++)
        enemy->move_set[i] = (char *)moves_enemy[i];
    enemy->attack = 50;
    enemy->defense = 50;
    enemy->speed = 50;
    enemy->hp = 100;

    int enemy_w = largestStr_bra(enemy->ascii, enemy->n_ascii);
    int enemy_h = enemy->n_ascii + 2;

    int player_w = largestStr_bra(cur->monster->ascii, cur->monster->n_ascii);
    int player_h = cur->monster->n_ascii + 2;

    int enemy_x = xM - enemy_w - 2;
    if (enemy_x < 0)
        enemy_x = 0;
    int enemy_y = 1;

    int player_x = 2;
    int player_y = yM - player_h - 2;
    if (player_y < 0)
        player_y = 0;

    WINDOW *enemy_win = newwin(enemy_h, enemy_w, enemy_y, enemy_x);
    box(enemy_win, 0, 0);
    mvwprintw(enemy_win, 0, 2, " %s ", enemy->name);
    for (int i = 0; i < enemy->n_ascii; i++)
        mvwprintw(enemy_win, i + 1, 1, "%s", enemy->ascii[i]);
    wrefresh(enemy_win);

    WINDOW *player_win = newwin(player_h, player_w, player_y, player_x);
    box(player_win, 0, 0);
    mvwprintw(player_win, 0, 2, " %s ", cur->monster->name);
    for (int i = 0; i < cur->monster->n_ascii; i++)
        mvwprintw(player_win, i + 1, 1, "%s", cur->monster->ascii[i]);
    wrefresh(player_win);

    mvprintw(yM - 1, 2, "Presiona ENTER o 'q' para volver al menu");
    refresh();

    int ch;
    while ((ch = getch()) != '\n' && ch != 'q');

    /* limpiar y liberar recursos */
    wborder(enemy_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wclear(enemy_win);
    wrefresh(enemy_win);
    delwin(enemy_win);

    wborder(player_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wclear(player_win);
    wrefresh(player_win);
    delwin(player_win);

    if (enemy->ascii)
    {
        for (int i = 0; i < enemy->n_ascii; i++)
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
