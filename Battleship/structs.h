#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include <stdlib.h>

typedef char array;

struct player_{
    char *name;
    int id;
    array table[10][10];
};

typedef struct player_ ply;
/**
 * @brief Inicializar estructura jugador
 * 
 * @return struct ply 
 */
ply initPly(){
    ply current;
    current.id = 0;
    for(int y = 0; y < 10; y++){
        for(int x = 0; x < 10; x++){
            current.table[y][x] = '0';
        }
    }
    return current;
}
#endif /* STRUCTS_H */
