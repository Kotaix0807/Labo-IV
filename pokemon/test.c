#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include "structs.h"
#include "tools.h"
#include "opt.h"
/*
int main(void)
{
    initscr();
    curs_set(0);
    noecho();
    cbreak();

    char *name = Ask("HOLA DARIO", 20);
    free(name);

    menu_box *m = malloc(sizeof(menu_box));
    char *choices[] = {
        "Opcion1",
        "Opcion2",
        "Luciano",
        "Te",
        "Extranhamos"
    };
    initMenu(m, "Hola mundo", choices, sizeof(choices) / sizeof(char *));
    int select;
    while((select = run_menu(m)) == -1);
    endwin();
    return 0;
}*/
/*
int main(void)
{
    setlocale(LC_ALL, "");
    initscr();
    curs_set(0);
    noecho();
    cbreak();
    unsigned long art_lines = fileLines("pokemon.txt", 0);
    char **arr = readText("pokemon.txt");
    if (!arr) {
        fprintf(stderr, "No se pudo leer pokemon.txt\n");
        return 1;
    }
    for (unsigned long i = 0; i < art_lines; i++)
        mvprintw(i, 0, "%s", arr[i]);
    for (unsigned long i = 0; i < art_lines; i++)
        free(arr[i]);
    free(arr);
    refresh();
    getch();
    endwin();
    return 0;
}*/

int main(void)
{
    setlocale(LC_ALL, "");
    initscr();
    curs_set(0);
    noecho();
    cbreak();

    int yM, xM;
    getmaxyx(stdscr, yM, xM);
    printw("Window size x = %d, y = %d\n", xM, yM);

    getch();
    endwin();
    return 0;
}

/*
int main(void)
{
    setlocale(LC_ALL, "");
    initscr();
    curs_set(0);
    noecho();
    cbreak();

    char *txt[] = {
        "Hola",
        "Prueba",
        "De",
        "texto"
    };
    txt_box *box1 = TxtBox_str(txt, sizeof(txt)/sizeof(char*), NULL, 0, 0, ALIGN_CENTER, TXT_CENTER);
    getch();
    delBox(box1);
    endwin();
}*/


