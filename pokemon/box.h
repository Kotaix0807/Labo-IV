#ifndef BOX_H
#define BOX_H

#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include "tools.h"

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
void dialFromFile(const char *file, const char *title, int x, int y, int align, int center);
void dialFromStr(char *txt[], int n, const char *title, int x, int y, int align);

void previewWindow(int width, int height);

txt_box *TxtBox_str(char *txt[], int n, const char *title, int x, int y, int align, int center);
txt_box *TxtBox_file(const char *file, const char *title, int x, int y, int align, int center);

void initCustTxtBox(txt_box *t, char *txt[], int n ,const char *title, int x, int y, int w, int h, int align);
txt_box *custTxtBox_str(char *txt[], int n, const char *title, int x, int y, int w, int h, int align);
txt_box *custTxtBox_file(const char *file, const char *title, int x, int y, int w, int h, int align, int center);

/**
 * @brief Inicializa un menu de opciones
 * 
 * @param m estructura menu
 * @param title titulo
 * @param choices arreglo con las opciones disponibles
 * @param n cantidad de opciones
 */
void initMenu(menu_box *m, const char *title, char *choices[], int n, int x, int y)
{
    int xM, yM;
    getmaxyx(stdscr, yM, xM);

    m->sel = 0;
    m->title = title;
    m->choices = choices;
    m->n_choice = n;
    m->w = largestOpt(m->choices, m->n_choice, m->title);
    m->h = n + 2;

    int pos_x, pos_y;

    if(x < 0 || x > xM)
        pos_x = (xM - m->w) / 2;
    else
        pos_x = x;

    if(y < 0 || y > yM)
        pos_y = (yM - m->h) / 2;
    else
        pos_y = y;

    m->win = newwin(m->h, m->w, pos_y, pos_x);
    keypad(m->win, TRUE);
    printMenu(m);
}
/**
 * @brief Imprimir menu de opciones
 * 
 * @param m estructura menu
 */
void printMenu(menu_box *m)
{
    int start = (m->w - (int)strlen(m->title)) / 2;
    box(m->win, 0, 0);
    if(m->title)
        mvwprintw(m->win, 0, start, "%s", m->title);
    for(int i = 0; i < m->n_choice; i++)
    {
        if(i == m->sel)
        {
            wattron(m->win , A_REVERSE);
            mvwprintw(m->win, i + 1, 1, "%s", m->choices[i]);
            wattroff(m->win, A_REVERSE);
        }
        else{
            mvwprintw(m->win, i + 1, 1, "%s", m->choices[i]);
        }
    }
    wrefresh(m->win);
}
/**
 * @brief iteracion del menu
 * 
 * @param m estructura menu
 * @return int -> opcion escogida
 */
int run_menu(menu_box *m)
{
    int c = wgetch(m->win);
    switch(c)
    {	
        case KEY_UP:
            if(m->sel == 0)
                m->sel = m->n_choice - 1;
            else
                --m->sel;
            break;
        case KEY_DOWN:
            if(m->sel == m->n_choice - 1)
                m->sel = 0;
            else 
                ++m->sel;
            break;
        case '\n':
            return m->sel;
        default:
            break;
    }
    printMenu(m);
    return -1;
}
/**
 * @brief liberar memoria del menu
 * 
 * @param m estructura menu
 */
void freeMenu(menu_box *m)
{
    if(!m)
        return;
    wborder(m->win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wclear(m->win);
    wrefresh(m->win);
    if(m->win)
    {
        keypad(m->win, FALSE);
        delwin(m->win);
        m->win = NULL;
    }
    m->choices = NULL; /* choices/title owned by caller */
    m->title = NULL;
    m->n_choice = 0;
    m->sel = 0;
    free(m);
}

int menu(const char *title, char *choices[], int n, int x, int y)
{
    int selection = -1;
    menu_box *options = malloc(sizeof(menu_box));
    if(!options)
        return -2;
    initMenu(options, title, choices, n, x, y);
    while ((selection = run_menu(options)) == -1);
    freeMenu(options);
    return selection;
}

/**
 * @brief Inicializar cuadro de respuesta
 * 
 * @param b estructura respuesta
 * @param title titulo
 * @param n Caracteres esperados
 */
void initAwns(awns_box *b, const char *title, int n)
{
    int xM, yM;
    getmaxyx(stdscr, yM, xM);
    b->t_len = (int)strlen(title);
    b->title = title;
    b->MAX = n;
    b->h = 3;
    if(n < b->t_len)
        b->w = b->t_len;
    else
        b->w = n;
    b->w += 4;
    b->win = newwin(b->h, b->w, (yM - b->h) / 2, (xM - b->w) / 2);
    keypad(b->win, TRUE);
}    
/**
 * @brief Imprimir cuadro de respuesta
 * 
 * @param b estructura respuesta
 */
void printAwns(awns_box *b)
{
    int start = (b->w - b->t_len) / 2;
    box(b->win, 0, 0);
    mvwprintw(b->win, 0, start, "%s", b->title);
    wmove(b->win, 1, 2);
    curs_set(1);
    echo();
    wrefresh(b->win);
}
/**
 * @brief Liberar cuadro de respuesta
 * 
 * @param b estructura respuesta
 */
void freeAwns(awns_box *b)
{
    if(!b)
        return;
    wborder(b->win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wclear(b->win);
    wrefresh(b->win);
    int yMax = getmaxy(stdscr);
    mvprintw(yMax - 1, 0, " ");
    clrtoeol();
    refresh();
    if(b->win)
    {
        keypad(b->win, FALSE);
        delwin(b->win);
        b->win = NULL;
    }
    b->title = NULL;
    b->w = 0;
    b->h = 0;
    noecho();
    curs_set(0);
    free(b);
}
/**
 * @brief Genera un cuadro de respuesta
 * 
 * @param question Titulo/pregunta
 * @param chars caracteres esperados
 * @return char* -> Respuesta
 */
char *Ask(const char *question, int chars)
{
    awns_box *b = malloc(sizeof(awns_box));
    if(!b)
        return NULL;
    initAwns(b, question, chars);
    printAwns(b);
    char awnser[chars];
    wgetnstr(b->win, awnser, chars);
    freeAwns(b);
    return strdup(awnser);
}

void initTxtBox(txt_box *t, char *txt[], int n ,const char *title, int x, int y, int align)
{
    /* title es opcional; evita strlen(NULL) */
    if (title) {
        t->title = title;
        t->title_len = (int)strlen(title);
    } else {
        t->title = NULL;
        t->title_len = 0;
    }

    t->align = align;

    t->txt = txt;
    t->w = largestStr(txt, n);
    t->h = n;
    t->owns_txt = 0; /* caller owns the backing strings */
    t->win = newwin(t->h + 2, t->w + 2, y, x);
    keypad(t->win, TRUE);
}

static int txt_align_start(txt_box *t, int line_len)
{
    int space = t->w - line_len;
    if (space < 0)
        space = 0;
    switch (t->align) {
        case ALIGN_RIGHT:
            return 1 + space;
        case ALIGN_CENTER:
            return 1 + space / 2;
        case ALIGN_LEFT:
        default:
            return 1;
    }
}

void printBox(txt_box *t)
{
    box(t->win, 0, 0);
    for (int i = 0; i < t->h; i++) {
        int len = (int)strlen(t->txt[i]);
        int start = txt_align_start(t, len);
        mvwprintw(t->win, i + 1, start, "%s", t->txt[i]);
    }
    if(t->title != NULL)
        mvwprintw(t->win, 0, (t->w - t->title_len) / 2, "%s", t->title);
    wrefresh(t->win);
}

void delBox(txt_box *t)
{
    if(!t)
        return;
    if (t->owns_txt && t->txt) {
        for (int i = 0; i < t->h; i++)
            free(t->txt[i]);
        free(t->txt);
    }
    t->txt = NULL;
    t->owns_txt = 0;
    wborder(t->win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wclear(t->win);
    wrefresh(t->win);
    if(t->win)
    {
        keypad(t->win, FALSE);
        delwin(t->win);
        t->win = NULL;
    }
    t->title = NULL;
    t->w = 0;
    t->h = 0;
    free(t);
}

void dialFromFile(const char *file, const char *title, int x, int y, int align, int center)
{
    txt_box *blob = malloc(sizeof(txt_box));
    if(center)
    {
        int box_w = (int)fileLines(file, 1) + 2;
        int box_h = (int)fileLines(file, 0) + 2;
        int x_ = (getmaxx(stdscr) - box_w) / 2;
        int y_ = (getmaxy(stdscr) - box_h) / 2;
        initTxtBox(blob, readText(file), (int)fileLines(file, 0), title, x_, y_, align);
        blob->owns_txt = 1;
    }
    else
    {
        initTxtBox(blob, readText(file), (int)fileLines(file, 0), title, x, y, align);
        blob->owns_txt = 1;
    }
    printBox(blob);
    wgetch(blob->win);
    delBox(blob);
}

void dialFromStr(char *txt[], int n, const char *title, int x, int y, int align)
{
    txt_box *blob = malloc(sizeof(txt_box));
    int xM, yM;
    getmaxyx(stdscr, yM, xM);
    if(x < 0 || x > xM || y < 0 || y > yM)
    {
        int box_w = largestStr(txt, n) + 2;
        int box_h = n + 2;
        int x_ = (xM - box_w) / 2;
        int y_ = (yM - box_h) / 2;
        initTxtBox(blob, txt, n, title, x_, y_, align);
    }
    else
        initTxtBox(blob, txt, n, title, x, y, align);
    printBox(blob);
    wgetch(blob->win);
    delBox(blob);
}

void previewWindow(int width, int height)
{
    int yM, xM;
    getmaxyx(stdscr, yM, xM);

    int win_h = height + 2;
    int win_w = width + 2;

    if (win_h > yM) win_h = yM;
    if (win_w > xM) win_w = xM;

    int pos_y = (yM - win_h) / 2;
    int pos_x = (xM - win_w) / 2;

    WINDOW *win = newwin(win_h, win_w, pos_y, pos_x);
    keypad(win, TRUE);

    while (1)
    {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 0, 2, "Preview (%dx%d)", win_h - 2, win_w - 2);
        wrefresh(win);

        int ch = wgetch(win);
        if (ch == 'q' || ch == '\n')
            break;

        switch (ch)
        {
            case KEY_UP:    pos_y--; break;
            case KEY_DOWN:  pos_y++; break;
            case KEY_LEFT:  pos_x--; break;
            case KEY_RIGHT: pos_x++; break;
            case 'r':
                mvprintw(yM - 1, 1, "Ventana en x = %d, y = %d", pos_x, pos_y);
                clrtoeol();
                refresh();
                break;
            default:
                break;
        }

        /* límites para que no se salga de la pantalla */
        if (pos_y < 0) pos_y = 0;
        if (pos_x < 0) pos_x = 0;
        if (pos_y > yM - win_h) pos_y = yM - win_h;
        if (pos_x > xM - win_w) pos_x = xM - win_w;

        /* limpiar la posición anterior antes de mover */
        wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
        wrefresh(win);
        mvwin(win, pos_y, pos_x);
    }

    mvprintw(yM - 1, 0, " ");
    clrtoeol();
    refresh();
    delwin(win);
}


txt_box *TxtBox_str(char *txt[], int n, const char *title, int x, int y, int align, int center)
{
    txt_box *blob = malloc(sizeof(txt_box));
    if(center)
    {
        int box_w = largestStr(txt, n) + 2;
        int box_h = n + 2;
        int x_ = (getmaxx(stdscr) - box_w) / 2;
        int y_ = (getmaxy(stdscr) - box_h) / 2;
        initTxtBox(blob, txt, n, title, x_, y_, align);
    }
    else
        initTxtBox(blob, txt, n, title, x, y, align);
    printBox(blob);
    return blob;
}

txt_box *TxtBox_file(const char *file, const char *title, int x, int y, int align, int center)
{
    txt_box *blob = malloc(sizeof(txt_box));
    if(center)
    {
        int box_w = (int)fileLines(file, 1) + 2;
        int box_h = (int)fileLines(file, 0) + 2;
        int x_ = (getmaxx(stdscr) - box_w) / 2;
        int y_ = (getmaxy(stdscr) - box_h) / 2;
        initTxtBox(blob, readText(file), (int)fileLines(file, 0), title, x_, y_, align);
        blob->owns_txt = 1;
    }
    else
    {
        initTxtBox(blob, readText(file), (int)fileLines(file, 0), title, x, y, align);
        blob->owns_txt = 1;
    }
    printBox(blob);
    return blob;
}


void initCustTxtBox(txt_box *t, char *txt[], int n ,const char *title, int x, int y, int w, int h, int align)
{
    //int yM, xM;
    //getmaxyx(stdscr, yM, xM);

    if (title) {
        t->title = title;
        t->title_len = (int)strlen(title);
    } else {
        t->title = NULL;
        t->title_len = 0;
    }

    t->align = align;
    t->txt = txt;

    int temp_w = largestStr(txt, n);
    int temp_h = n;
    if (w < temp_w)
        w = temp_w;
    if (h < temp_h)
        h = temp_h;

    t->w = w;
    t->h = h;
    t->owns_txt = 0;

    if (x < 0)
        x = 0;

    if (y < 0)
        y = 0;

    t->win = newwin(t->h + 2, t->w + 2, y, x);
    keypad(t->win, TRUE);
}

void custPrintBox(txt_box *t, int n)
{
    box(t->win, 0, 0);
    for (int i = 0; i < n; i++)
    {
        //int len = (int)strlen(t->txt[i]);
        int start = 1;//txt_align_start(t, len);
        mvwprintw(t->win, i + 1, start, "%s", t->txt[i]);
    }
    if(t->title != NULL)
        mvwprintw(t->win, 0, (t->w - t->title_len) / 2, "%s", t->title);
    wrefresh(t->win);
}

txt_box *custTxtBox_str(char *txt[], int n, const char *title, int x, int y, int w, int h, int align)
{
    txt_box *blob = malloc(sizeof(txt_box));
    int xM, yM;
    getmaxyx(stdscr, yM, xM);
    if(x < 0 || x > xM || y < 0 || y > yM)
    {
        int box_w = largestStr(txt, n) + 2;
        int box_h = n + 2;
        int x_ = (xM - box_w) / 2;
        int y_ = (yM - box_h) / 2;
        initCustTxtBox(blob, txt, n, title, x_, y_, w, h, align);
    }
    else
        initCustTxtBox(blob, txt, n, title, x, y, w, h, align);
    custPrintBox(blob, n);
    return blob;
}


txt_box *custTxtBox_file(const char *file, const char *title, int x, int y, int w, int h, int align, int center)
{
    txt_box *blob = malloc(sizeof(txt_box));
    if(center)
    {
        int box_w = (int)fileLines(file, 1) + 2;
        int box_h = (int)fileLines(file, 0) + 2;
        int x_ = (getmaxx(stdscr) - box_w) / 2;
        int y_ = (getmaxy(stdscr) - box_h) / 2;
        initCustTxtBox(blob, readText(file), (int)fileLines(file, 0), title, x_, y_, w, h, align);
        blob->owns_txt = 1;
    }
    else
    {
        initCustTxtBox(blob, readText(file), (int)fileLines(file, 0), title, x, y, w, h, align);
        blob->owns_txt = 1;
    }
    printBox(blob);
    return blob;
}



#endif
