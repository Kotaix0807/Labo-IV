#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include <stdlib.h>

typedef char** array;

struct player_{
    char *name;
    int id;
    array table;
};

typedef struct player_ ply;
/**
 * @brief Inicializar estructura jugador
 * 
 * @return struct ply 
 */
ply initPly(){
    ply current;
    current.name = NULL;
    current.table = NULL;
    current.id = 0;
    return current;
}
#endif /* STRUCTS_H */
