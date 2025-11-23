#ifndef OPT_H
#define OPT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <sys/select.h>
#include "structs.h"
#include "tools.h"

int initGame();
void initLocal();
void initPrintMat(uint8_t baseR[10][10], uint8_t baseG[10][10], uint8_t baseB[10][10]);
void renderMat(uint8_t baseR[10][10], uint8_t baseG[10][10], uint8_t baseB[10][10], int frame, int start_y);
int unlKb();

int initGame()
{
    opt menu[] = {
        {.str = "Multijugador Local", .r = 255, .g = 204, .b = 51},
        {.str = "Multijugador Online", .r = 0, .g = 102, .b = 204},
        {.str = "Salir", .r = 255, .g = 51, .b = 51}
    };
    ply player = initPly();
    print("Bienvenido a Battleship, ingresa tu nombre: ");
    player.name = readLine();
    if (!player.name)
    {
        fprintf(stderr, "No se pudo leer el nombre.\n");
        return 0;
    }
    print("Hola %s!\n", player.name);
    print("Elige un modo de juego:\n\n");
    int mode = optUpd(menu, (int)(sizeof(menu) / sizeof(menu[0])));
    switch (mode)
    {
        case 0:
            print("Local\n");
            initLocal(&player);
            return 1;
            break;
        case 1:
            print("En construccion...\n");
            return 1;
            break;
        case 2:
            print("Bye!\n");
            break;
        default:
            print("Mmmm raro...\n");
            return 1;
            break;
    }

    free(player.name);
    return 1;
}

void initLocal(){
    printf("\033c");
    T_curs_x = 1;
    T_curs_y = 1;
    uint8_t baseR[10][10];
    uint8_t baseG[10][10];
    uint8_t baseB[10][10];
    initPrintMat(baseR, baseG, baseB);
    int init = 1, frame = 0;
    int start_y = T_curs_y + 1;
    T_hideCursor();
    enable_raw_mode();
    print("Elige la posicion de tus barcos:\n");
    T_placeStr(1, 14, "Escribe tu coordenada: ", 255, 255, 255);
    char *buf = NULL;
    size_t len = 0, cap = 0;
    while(init)
    {
        renderMat(baseR, baseG, baseB, frame, start_y);
        fflush(stdout);
        usleep(80000);

        frame = (frame + 1) % 120;
        int done = scanstr(&buf, &len, &cap);
        if(buf){
            T_placeStr(24, 14, buf, 255, 255, 255);
            //print("%s", buf);
        }
        if (done)
            init = 0;

    }
    T_placeStr(1, 17, buf, 255, 255, 255);
    restore_terminal();
    T_showCursor();
    free(buf);
    return;
}


void initPrintMat(uint8_t baseR[10][10], uint8_t baseG[10][10], uint8_t baseB[10][10]){
    static int seeded = 0;
    if (!seeded)
    {
        seeded = 1;
        srand((unsigned)time(NULL));
    }

    for (int r = 0; r < 10; ++r)
    {
        for (int c = 0; c < 10; ++c)
        {
            baseR[r][c] = (uint8_t)(rand() % 40);
            baseG[r][c] = (uint8_t)(rand() % 90);
            baseB[r][c] = (uint8_t)(150 + rand() % 106);
        }
    }
}

void renderMat(uint8_t baseR[10][10], uint8_t baseG[10][10], uint8_t baseB[10][10], int frame, int start_y){
    static int phases_init = 0;
    static double phase[10][10];
    if (!phases_init)
    {
        phases_init = 1;
        for (int r = 0; r < 10; ++r)
        {
            for (int c = 0; c < 10; ++c)
            {
                /* phase offset aleatorio para que cada celda parpadee desfasada */
                phase[r][c] = ((double)rand() / (double)RAND_MAX) * 2.0 * M_PI;
            }
        }
    }
    T_move(1, start_y);
    print("    1  2  3  4  5  6  7  8  9  10\n");

    for (int row = 0; row < 10; ++row)
    {
        T_move(1, start_y + 1 + row);
        print("%c   ", 'A' + row);
        for (int col = 0; col < 10; ++col)
        {
            double factor = 0.6 + 0.4 * ((sin(frame * 0.25 + phase[row][col]) + 1.0) * 0.5);
            uint8_t r = (uint8_t)(baseR[row][col] * factor);
            uint8_t g = (uint8_t)(baseG[row][col] * factor);
            uint8_t b = (uint8_t)(baseB[row][col] * factor);
            T_setColor(r, g, b);
            print("██ ");
            T_resetColor();
        }
        print("\n");
    }
    T_move(1, start_y + 12);
}

int unlKb(){
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);
    struct timeval tv = {0, 0};
    return select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv);
}
#endif /* OPT_H */

/***
 * Recordar:
 * Liberar el nombre del player al final
 * 
 * 
 */
