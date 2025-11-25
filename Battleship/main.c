#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "structs.h"
#include "tools.h"

int main(void)
{
    initscr();
    curs_set(0);
    noecho();
    cbreak();
    printTitle();

    ply Player = initPly();
    menu_box *options = malloc(sizeof(menu_box));
    if(!options)
    {
        endwin();
        return 1;
    }
    char *choices[] = {
        "Local",
        "Salir"
    };
    Player.name = Ask("Ingresa tu nombre:", 64);
    if(!Player.name)
    {
        endwin();
        return 1;
    }
    initMenu(options, "Menu", choices, sizeof(choices) / sizeof(char *));
    int selection;
    while ((selection = run_menu(options)) == -1); /* keep handling input */
    endwin();
    printf("Seleccionaste opcion %d\n", selection);
    printf("El nombre ingresado fue: %s\n", Player.name);
    free(Player.name);
    return 0;
}
