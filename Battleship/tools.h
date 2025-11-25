#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

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
    int Tlen;
    int MAX;
    int w;
    int h;
}awns_box;

int largestStr(char *arr[], int n);
int largestOpt(menu_box m);
void initMenu(menu_box *m, const char *title, char *choices[], int n);
void printMenu(menu_box *m);
int run_menu(menu_box *m);
void freeMenu(menu_box *m);

void printTitle();

void initAwns(awns_box *b, const char *title, int n);
void printAwns(awns_box *b);
void freeAwns(awns_box *b);
char *Ask(const char *question, int chars);

int largestStr(char *arr[], int n)
{
    if(n <= 0)
    {
        perror("No str length");
        exit(1);
    }
    int max = 0, count = 0;
    for(int i = 0; i < n; i++)
    {
        count = (int)strlen(arr[i]);
        if(count > max)
            max = count;
    }
    return max;
}

int largestOpt(menu_box m)
{
    int length = largestStr(m.choices, m.n_choice);
    if(length < (int)strlen(m.title))
        return (int)strlen(m.title) + 4;
    else
        return length + 6;
}
void initMenu(menu_box *m, const char *title, char *choices[], int n)
{
    int xM, yM;
    getmaxyx(stdscr, yM, xM);

    m->sel = 0;
    m->title = title;
    m->choices = choices;
    m->n_choice = n;
    m->w = largestOpt(*m);
    m->h = n + 2;
    m->win = newwin(m->h, m->w, ((yM / 2) - m->h / 2), ((xM / 2) - m->w / 2));
    keypad(m->win, TRUE);
    printMenu(m);
}

void printMenu(menu_box *m)
{
    int start = (m->w - (int)strlen(m->title)) / 2;
    box(m->win, 0, 0);
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
            int sel = m->sel;
            freeMenu(m);
            return sel;
        default:
            break;
    }
    printMenu(m);
    return -1;
}

void freeMenu(menu_box *m)
{
    if(!m)
        return;
    wborder(m->win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
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

void printTitle()
{
    int xMax = getmaxx(stdscr);
    char *lines[] = {
        " _           _   _   _           _     _       ",
        "| |         | | | | | |         | |   (_)      ",
        "| |__   __ _| |_| |_| | ___  ___| |__  _ _ __  ",
        "| '_ \\ / _` | __| __| |/ _ \\/ __| '_ \\| | '_ \\ ",
        "| |_) | (_| | |_| |_| |  __/\\__ \\ | | | | |_) |",
        "|_.__/ \\__,_|\\__|\\__|_|\\___||___/_| |_|_| .__/ ",
        "                                        | |    ",
        "                                        |_|    "
    };
    int length = largestStr(lines, sizeof(lines) / sizeof(char *));
    if (xMax < length + 50)
    {
        char *title = "Battleship";
        mvwprintw(stdscr, 0, ((xMax / 2) - (int)strlen(title) / 2), "%s", title);
        wrefresh(stdscr);
        return;
    }
    for(int i = 0; i < 8; i++)
        mvwprintw(stdscr, i, ((xMax / 2) - length / 2), "%s", lines[i]);
    wrefresh(stdscr);
}


void initAwns(awns_box *b, const char *title, int n)
{
    int xM, yM;
    getmaxyx(stdscr, yM, xM);
    b->Tlen = (int)strlen(title);
    b->title = title;
    b->MAX = n;
    b->h = 3;
    if(n < b->Tlen)
    b->w = b->Tlen;
    else
        b->w = n;
    b->w += 4;
    b->win = newwin(b->h, b->w, ((yM / 2) - b->h / 2), ((xM / 2) - b->w / 2));
    keypad(b->win, TRUE);
}    

void printAwns(awns_box *b)
{
    int start = (b->w - b->Tlen) / 2;
    box(b->win, 0, 0);
    mvwprintw(b->win, 0, start, "%s", b->title);
    wmove(b->win, 1, 2);
    curs_set(1);
    echo();
    wrefresh(b->win);
}

void freeAwns(awns_box *b)
{
    if(!b)
        return;
    wborder(b->win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    for(int i = 1; i < b->MAX; i++)
    {
        mvwprintw(b->win, 1, i, " ");
    }
    wrefresh(b->win);
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


/*

 _           _   _   _           _     _       
| |         | | | | | |         | |   (_)      
| |__   __ _| |_| |_| | ___  ___| |__  _ _ __  
| '_ \ / _` | __| __| |/ _ \/ __| '_ \| | '_ \ 
| |_) | (_| | |_| |_| |  __/\__ \ | | | | |_) |
|_.__/ \__,_|\__|\__|_|\___||___/_| |_|_| .__/ 
                                        | |    
                                        |_|   
*/
