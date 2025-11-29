#ifndef OPT_H
#define OPT_H

#include "tools.h"
#include "box.h"
#include "structs.h"

int mainMenu(ply *Player);
int askName(ply *Player);
void printTitle();
void credits();
void askPkmn(ply *cur);
void preview();
void Combat(ply *cur);

int mainMenu(ply *Player)
{
    //printTitle();
    char *choices[] = {
        "Combat", //Local
        "Ventana", //Cambiar Nombre
        "Creditos", //Creditos
        "Salir" //Salir
    };
    int selection = menu("Menu", choices, sizeof(choices)/sizeof(char*), -1, -1);
    switch(selection)
    {
        case 0:
            askPkmn(Player);
            Combat(Player);
            break;
        case 1:
            //askName(Player);
            preview();
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
    pkmnSet(cur, pklist[pkmn]);
}


void preview()
{
    int h = atoi(Ask("Alto de la ventana:", 3));
    int w = atoi(Ask("Ancho de la ventana:", 3));
    previewWindow(w, h);
}

void Combat(ply *cur)
{
    wclear(stdscr);
    wrefresh(stdscr);

    int xM, yM;
    getmaxyx(stdscr, yM, xM);

   //Por ahora se queda asi hasta que se incluya la comunicacion de procesos
    pkmn *enemy = malloc(sizeof(pkmn));
    if(!enemy)
        return;
    enemy->name = "Charizard";
    enemy->ascii = readText("art/charizard.txt");
    enemy->n_ascii = (int)fileLines("art/charizard.txt", 0);

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
    enemy->w = largestStr_bra(enemy->ascii, enemy->n_ascii);


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
        int mv = menu("Que deseas hacer?", cur->monster->move_set, 4, 1, (yM - 6));
        delBox(combat_box);

        char *used[] = {
            "%s uso:",
            "%s!"
        };
        replace_fmt(used, 0, cur->monster->name);
        replace_fmt(used, 1, cur->monster->move_set[mv]);
        int used_lines = (int)(sizeof(used)/sizeof(char*));
        int used_h = 10 - used_lines;
        txt_box *use = custTxtBox_str(used, used_lines, NULL, 1, (yM - used_h - 2), 40, used_h, ALIGN_CENTER);
        napms(1000);
        flushinp(); /* descartar teclas pulsadas durante la pausa */
        delBox(use);
        enemy->hp -= cur->monster->attack;
        if(enemy->hp <= 0)
        {
            int l = enemy->n_ascii;
            for(int i = 0; i < l; i++)
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


        printw("Movimiento enemigo");
        refresh();
        getch();
        clrtoeol();
        refresh();

        clearWin(enemy_win);
        clearWin(player_win);
    }
    char *end[] = {
        "Felicidades %s!",
        "Haz ganado el combate pokémon"
    };
    dialFromStr(end, 2, "Winner!", -1, -1, ALIGN_CENTER);

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
