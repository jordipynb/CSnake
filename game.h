#ifndef GAME_H
#define GAME_H

#include "structure.h"
#include <time.h>

int moveRows[] = {-1, 1, 0, 0}; //array direccional de las filas
int moveCols[] = {0, 0, 1, -1}; //array direccional de las columnas

//método que genera el mapa donde se va a jugar
void generateWorld(game *gm, int rows, int columns);
//método que genera la serpiente en el mundo
void generateSnake(game *gm);
//método que genera 5 frutas random en el mapa
void generateFruits(game *gm);
//método principal que controla la jugabilidad
void play(game *gm);
//método que hace una búsqueda de huevos a lo ancho del mapa
myqueue *BFS(game *gm, point actual_position);
//método que ejecuta la avanzada de la serpiente por el camino devuelto
void advance(myqueue *way, game *gm, BOOL no_way);
//método que hace caminar a la serpiente por el camino más largo si no encuentra frutas
myqueue *tryToLive(game *gm);
//método que dice si es válido o no un punto en el mapa
BOOL isValidPosition(game *gm, point pos);
//método que devuelve el máximo de dos enteros
int mathMax(int a, int b);
//método que limpia la consola
void clearScreen();
//método que ejecuta la limpieza de consola cada un cierto tiempo
void refresh();
//método que dibuja el mapa
void printWorld(game *gm);

void generateWorld(game *gm, int rows, int columns) {
    gm->map = (char **) malloc(rows * sizeof(char *));
    int i, j;
    for (i = 0; i < rows; i++) {
        gm->map[i] = (char *) malloc(columns * sizeof(char));
        for (j = 0; j < columns; j++) {
            gm->map[i][j] = '.';
        }
    }
    gm->rows = rows;
    gm->columns = columns;
    gm->score = 0;
    gm->itsover = FALSE;
}
void generateSnake(game *gm) {
    gm->snake = (myqueue *) malloc(sizeof(myqueue));
    gm->snake->first = NULL;
    gm->snake->last = gm->snake->first;
    gm->snake->counter = 0;
    gm->increase = 0;
    int new_r = (int) gm->rows / 2;
    int new_c = (int) gm->columns / 2;
    point p;
    p.x = new_r;
    p.y = new_c - 2;
    enqueue(gm->snake, p);
    p.y++;
    enqueue(gm->snake, p);
    p.y++;
    enqueue(gm->snake, p);
    int i;
    node_t *current = gm->snake->last;
    for (i = 0; i < gm->snake->counter; i++) {
        if (i == 0) {
            gm->map[current->pos.x][current->pos.y] = '@';
        } else {
            gm->map[current->pos.x][current->pos.y] = 'o';
        }
        current = current->previous;
    }
    free(current);
}
void generateFruits(game *gm) {
    int r = gm->rows;
    int c = gm->columns;
    srand((unsigned) time(NULL));
    int counter = 0;
    while (counter < 5) {
        int i = rand() % r;
        int j = rand() % c;
        if (gm->map[i][j] == '.') {
            gm->map[i][j] = 'x';
            counter++;
        }
    }
    gm->fruitsonmap = 5;
}
void play(game *gm) {
    point currentposition = gm->snake->last->pos;
    BOOL no_way = FALSE;
    while (gm->itsover == FALSE) {
        myqueue *nextway = BFS(gm, currentposition);
        if (nextway->counter == 0) {
            if (no_way == TRUE) {
                gm->itsover = TRUE;
                printf("GAME OVER!!\n");
                return;
            }
            no_way = TRUE;
            nextway = tryToLive(gm);
            if (nextway->counter == 0) {
                gm->itsover = TRUE;
                printf("GAME OVER!!\n");
                return;
            }
            advance(nextway, gm, no_way);
            currentposition = gm->snake->last->pos;
        } else {
            no_way = FALSE;
            gm->fruitsonmap--;
            advance(nextway, gm, no_way);
            currentposition = gm->snake->last->pos;
        }
    }
}
myqueue *BFS(game *gm, point actual_position) {
    myqueue *positions = (myqueue *) malloc(sizeof(myqueue));
    positions->first = NULL;
    positions->last = NULL;
    positions->counter = 0;

    myqueue *moves = (myqueue *) malloc(sizeof(myqueue));
    moves->last = NULL;
    moves->first = NULL;
    moves->counter = 0;

    int totalsize = gm->rows * gm->columns;
    point *p = (point *) malloc(totalsize * sizeof(point));
    point head_snake = actual_position;
    BOOL visited[totalsize];

    for (int i = 0; i < totalsize; ++i) {
        visited[i] = FALSE;
    }

    enqueue(positions, actual_position);
    while (positions->counter > 0) {
        actual_position = dequeue(positions);
        for (int i = 0; i < 4; ++i) {
            point next_pos;
            next_pos.x = actual_position.x + moveRows[i];
            next_pos.y = actual_position.y + moveCols[i];

            if (isValidPosition(gm, next_pos) == TRUE) {
                int index = (gm->columns * next_pos.x) + next_pos.y;
                if (visited[index] == FALSE) {
                    p[index] = actual_position;
                    if (gm->map[next_pos.x][next_pos.y] == 'x') {
                        enqueue(moves, next_pos);
                        while (p[index].x != head_snake.x || p[index].y != head_snake.y) {
                            next_pos = p[index];
                            enqueue(moves, next_pos);
                            index = (gm->columns * next_pos.x) + next_pos.y;
                        }
                        free(positions);
                        return moves;
                    }
                    visited[index] = TRUE;
                    enqueue(positions, next_pos);
                }
            }
        }
    }
    free(positions);
    return moves;
}
void advance(myqueue *way, game *gm, BOOL no_way) {
    point currentposition;
    while (way->counter > 0) {
        if (no_way == TRUE)
            currentposition = dequeue(way);
        else
            currentposition = pop(way);
        if (gm->increase == 0) {
            point first = dequeue(gm->snake);
            gm->map[first.x][first.y] = '.';
        }

        gm->map[gm->snake->last->pos.x][gm->snake->last->pos.y] = 'o';
        enqueue(gm->snake, currentposition);
        gm->map[currentposition.x][currentposition.y] = '@';
        if (way->counter == 0) {
            int totalsize = (gm->rows) * (gm->columns);
            int fullspaces = gm->fruitsonmap + gm->snake->counter;
            if ((gm->fruitsonmap == 0 && totalsize - fullspaces >= 5))generateFruits(gm);
            gm->score++;
        }
        refresh();
        printWorld(gm);
        if (gm->increase > 0)
            gm->increase--;
    }
    gm->increase += 3;
}
myqueue *tryToLive(game *gm) {
    myqueue *way = (myqueue *) malloc(sizeof(myqueue));
    way->first = NULL;
    way->last = NULL;
    way->counter = 0;

    point head = gm->snake->last->pos;
    for (int i = 0; i < 4; ++i) {
        myqueue *temp_way = (myqueue *) malloc(sizeof(myqueue));
        temp_way->first = NULL;
        temp_way->last = NULL;
        temp_way->counter = 0;
        for (int j = 1; j < mathMax(gm->columns, gm->rows); ++j) {
            point temp_pos;
            temp_pos.x = head.x + j * moveRows[i];
            temp_pos.y = head.y + j * moveCols[i];
            if (isValidPosition(gm, temp_pos) == TRUE) {
                enqueue(temp_way, temp_pos);
            } else {
                if (temp_way->counter > way->counter) {
                    way = temp_way;
                }
                break;
            }
        }
    }
    return way;
}
BOOL isValidPosition(game *gm, point pos) {
    if (pos.x >= 0 && pos.x < gm->rows && pos.y >= 0 && pos.y < gm->columns && gm->map[pos.x][pos.y] != '@' &&
        gm->map[pos.x][pos.y] != 'o')
        return TRUE;
    return FALSE;
}
int mathMax(int a, int b) {
    if (a > b) return a;
    return b;
}
void clearScreen() {
    system("clear");
}
void refresh() {
    time_t secondsfirst;
    time_t secondslast;
    time(&secondsfirst);
    time(&secondslast);
    while (secondslast - secondsfirst < 1) {
        time(&secondslast);
    }
    clearScreen();
}
void printWorld(game *gm) {
    int i, j;
    int r = gm->rows;
    int c = gm->columns;
    for (i = 0; i < r; i++) {
        for (j = 0; j < c; j++) {
            if (j == 0) {
                printf("%c", gm->map[i][j]);
            } else {
                printf(" %c", gm->map[i][j]);
            }
        }
        printf("\n");
    }
    printf("Score: %d\n", gm->score);
}
#endif
