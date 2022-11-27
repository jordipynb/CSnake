#include <stdio.h>
#include <stdlib.h>
#include "structure.h"
#include "game.h"

int main()
{
    game gm;
    int rows, columns;
    scanf("%d %d", &rows, &columns);
    generateWorld(&gm, rows, columns);
    generateSnake(&gm);
    generateFruits(&gm);
    clearScreen();
    printWorld(&gm);
    play(&gm);
    return 0;
}
