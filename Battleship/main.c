#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "structs.h"
#include "tools.h"
#include "options.h"

int main(void)
{
    int GAME = initGame();
    if(GAME <= 0)
        return 1;
    /*while (GAME > 0)
    {

    }*/
    return 0;
}
