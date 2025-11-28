#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <stdbool.h>
#include <ncurses.h>
#include "sound.h"
#include "structs.h"
#include "tools.h"
#include "opt.h"

#define WIN_X_MIN 216
#define WIN_Y_MIN 45

void warn()
{
    const char *warn_msg = "Por favor, maximice su pantalla y presione 'ctrl -'";

    while (1)
    {
        int yM, xM;
        getmaxyx(stdscr, yM, xM);

        if (xM == WIN_X_MIN && yM == WIN_Y_MIN)
        {
            clear();
            refresh();
            break;
        }

        int pos_y = yM / 4;
        int pos_x = (xM - (int)strlen(warn_msg)) / 2;
        clear();
        mvprintw(pos_y, pos_x, "%s", warn_msg);
        refresh();
        napms(1);
    }
}

static int initGame()
{
    setlocale(LC_ALL, "");
    initscr();
    curs_set(0);
    noecho();
    cbreak();
    //warn();
    if(!initAudio())
    {
        endwin();
        return 0;
    }
    return 1;
}

int main(void)
{
    if(!initGame())
        return 1;
    printTitle();
    ply Player = initPly();
    //askName(&Player);


    while(mainMenu(&Player) != 3);
    free(Player.name);
    endwin();
    quitAudio();
    return 0;
}
