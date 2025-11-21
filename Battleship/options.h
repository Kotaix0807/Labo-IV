#ifndef OPT_H
#define OPT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "tools.h"

int initGame()
{
    ply current = initPly();
    print("Bienvenido a Battleship, ingresa tu nombre: ");
    current.name = readLine();
    if (!current.name)
    {
        fprintf(stderr, "No se pudo leer el nombre.\n");
        return 0;
    }
    print("Hola %s!\n", current.name);
    print("Elige un modo de juego:\n");
    //printf("X: %d\n", T_curs_x);
    //printf("Y: %d\n", T_curs_y);
    T_placeC(T_curs_x, T_curs_y + 1, '>', 0, 0, 255);
    T_placeStr(1, T_curs_y + 1, "HOLA\n", 0, 0, 255);
    T_placeStr(T_curs_x, T_curs_y + 1, "YOU DIED\n", 255, 102, 153);

    

    return 1;
}

#endif /* OPT_H */


/***
 * Recordar:
 * Liberar el nombre del player al final
 * 
 * 
 */
