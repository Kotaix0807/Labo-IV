#ifndef BOX_H
#define BOX_H

#include <ncurses.h>

#define ALIGN_LEFT 0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT 2

typedef struct menu_{
    WINDOW *win;
    const char *title;
    char **choices;
    int w;
    int h;
    int n_choice;
    int sel;
}menu_box;

typedef struct awns_{
    WINDOW *win;
    const char *title;
    int t_len;
    int MAX;
    int w;
    int h;
}awns_box;

typedef struct txt_{
    WINDOW *win;
    const char *title;
    char **txt;
    int title_len;
    int owns_txt;
    int align;
    int w;
    int h;
}txt_box;

void initMenu(menu_box *m, const char *title, char *choices[], int n, int x, int y);
void printMenu(menu_box *m);
int run_menu(menu_box *m);
void freeMenu(menu_box *m);
int menu(const char *title, char *choices[], int n, int x, int y);

void initAwns(awns_box *b, const char *title, int n);
void printAwns(awns_box *b);
void freeAwns(awns_box *b);
char *Ask(const char *question, int chars);

void initTxtBox(txt_box *t, char *txt[], int n ,const char *title, int x, int y, int align);
void printBox(txt_box *t);
void delBox(txt_box *t);
void dialFromFile(const char *file, const char *title, int x, int y, int align, int center);
void dialFromStr(char *txt[], int n, const char *title, int x, int y, int align);

void previewWindow(int width, int height);

txt_box *txtBox_str(char *txt[], int n, const char *title, int x, int y, int align);
txt_box *txtBox_file(const char *file, const char *title, int x, int y, int align);

void initCustTxtBox(txt_box *t, char *txt[], int n ,const char *title, int x, int y, int w, int h, int align);
txt_box *custTxtBox_str(char *txt[], int n, const char *title, int x, int y, int w, int h, int align);
txt_box *custTxtBox_file(const char *file, const char *title, int x, int y, int w, int h, int align, int center);

#endif
