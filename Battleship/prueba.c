#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>
#include "tools.h"

int main(){
    system("clear");
    opt menu[] = {
        {.str = "Comenzar", .r = 255, .g = 215, .b = 0},
        {.str = "Salir", .r = 255, .g = 99, .b = 71},
        {.str = "menu principal.", .r = 100, .g = 149, .b = 237},
        {.str = "menu principal.", .r = 100, .g = 149, .b = 237},
        {.str = "menu principal.", .r = 100, .g = 149, .b = 237},
        {.str = "menu principal.", .r = 100, .g = 149, .b = 237},
        {.str = "menu principal.", .r = 100, .g = 149, .b = 237}
    };

    optUpd(menu, (int)(sizeof(menu) / sizeof(menu[0])));
    print("Terminado\n");
    return 0;
}