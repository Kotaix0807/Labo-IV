#ifndef STRUCTS_H
#define STRUCTS_H

#include <ncurses.h>

#define TYPE_NONE -1
#define FIRE 0
#define WATER 1
#define PLANT 2
#define POISON 3
#define GROUND 4
#define FLYING 5
#define DRAGON 6
#define ICE 7
#define SINESTER 8
#define NORMAL 9



typedef struct move_{
    char name[64];
    int type;
    unsigned int power;
}move_t;

typedef struct pokemon_{
    //Informacion base
    char name[32];
    char ascii[200][200];
    int n_ascii;
    int w;
    
    //Estadisticas
    int type1;
    int type2;
    
    move_t move_set[4];
    int attack;
    int defense;
    int speed;
    int hp;
    int level;
}pkmn;

typedef struct player_{
    char name[32];
    pkmn monster;
}ply;

ply initPly();
void pkmnSet(pkmn *monster, const char *name, int reverse);
void printPkmn(pkmn *monster, int x, int y);
WINDOW *movePkmnWindow(pkmn *monster);
WINDOW *printPkmnW(pkmn *monster, int x, int y);
void clearWin(WINDOW *win);
int formula(pkmn p, move_t used, pkmn e);

#endif /* STRUCTS_H */

/*
VENOSAUR:
-Giga Drain
-Sludge Bomb
-Earthquake
-Sleep Powder

CHARIZARD:
-Flamethrower
-Air Slash
-Dragon Pulse
-Roost

BLASTOISE:
-Surf
-Ice Beam
-Dark Pulse
-Rapid Spin

*/
