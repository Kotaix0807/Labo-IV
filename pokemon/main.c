#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <stdbool.h>
#include "sound.h"
#include "structs.h"
#include "tools.h"
#include "opt.h"

static int initGame()
{
    setlocale(LC_ALL, "");
    initscr();
    curs_set(0);
    noecho();
    cbreak();
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
    askName(&Player);
    while(mainMenu(&Player) != 3);
    free(Player.name);
    endwin();
    quitAudio();
    return 0;
}
