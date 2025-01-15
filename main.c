#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREENWIDTH     800
#define SCREENHEIGHT    800
#define BOARDCOLS       10
#define BOARDROWS       16
#define OGBLOCKSIZE     12.649 
#define BOARDAREA       (SCREENWIDTH * (3.0/4.0))
#define BLOCKSIZE       ((BOARDAREA * OGBLOCKSIZE)/160.0)
#define PADDINGX        (SCREENWIDTH - BOARDCOLS * BLOCKSIZE)
#define PADDINGY        (SCREENHEIGHT - BOARDROWS * BLOCKSIZE)

typedef enum {
        B,
        J,
        O,
        T,
        I,
        L,
        Z,
        S
} Tetrominoes;

typedef struct {
        int x;
        int y;
} Vector;

typedef struct {
        Tetrominoes type;
        Vector coordinates[4];

} Player;

void updateBoard(int rows, int cols, int board[rows][cols]);
void createTetromino();
void fallingTetromino();
void printBoard();
void debugboard();
bool hasCollide();

int board[BOARDROWS][BOARDCOLS] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};


int tetromino_j[4][4]= {
        {1,0,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
};

int tetromino_i[4][4]= {
        {1,1,1,1},
        {0,0,0,0},
        {0,0,0,0},
        {0,0,0,0}
};

int tetromino_l[4][4]= {
        {0,0,0,1},
        {0,1,1,1},
        {0,0,0,0},
        {0,0,0,0}
};

int tetromino_o[4][4]= {
        {1,1,0,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
};

int tetromino_s[4][4]= {
        {0,0,1,1},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
};

int tetromino_t[4][4]= {
        {0,0,1,0},
        {0,1,1,1},
        {0,0,0,0},
        {0,0,0,0}
};

int tetromino_z[4][4]= {
        {0,1,1,0},
        {0,0,1,1},
        {0,0,0,0},
        {0,0,0,0}
};


Color colors[8] = {BLACK, BLUE, YELLOW, MAGENTA, SKYBLUE, ORANGE, RED, GREEN};
Player player = {0};
int frame = 0;
bool oncreation = false;
bool isfalling = true;

int main(void) {

        InitWindow(SCREENWIDTH, SCREENHEIGHT, "tetris!");
        SetTargetFPS(1);

        createTetromino();
        while (!WindowShouldClose()) {
                debugboard();

                if (!isfalling)
                        createTetromino();

                frame++;
                BeginDrawing();
                updateBoard(BOARDROWS, BOARDCOLS, board);
                ClearBackground(BLACK);
                EndDrawing();
                fallingTetromino();

        }

        CloseWindow();

        return 0;
}


bool hasCollide() {

        int col = 0;
        int row = 0;

        for (int i = 0; i < 4; i++) {
                row = player.coordinates[i].y;
                col = player.coordinates[i].x;

                // Has collide with the limit 
                if (row >= BOARDROWS) 
                        return true;
                
                // Has collide with another tetrominoe
                if (board[row][col] != 0) 
                        return true;
        }

        return false;
}

void fallingTetromino() {
        int *col = 0;
        int *row = 0;
        Vector oldCoordinates[4];

        // TODO: 2 for loops, not optimal; fix it.

        for (int i = 0; i < 4; i++) {
                row = &(player.coordinates[i].y);
                col = &(player.coordinates[i].x);
                oldCoordinates[i].y = *row;
                oldCoordinates[i].x = *col;

                printf("player coordinates %d: ROW-> %d, COL-> %d \n", 
                                i, *row, *col);


                board[*row][*col] = 0;
                (*row)++;
        }

        if (hasCollide()) {
                isfalling = false;

                for (int i = 0; i < 4; i++) {
                        player.coordinates[i].y = oldCoordinates[i].y;
                        player.coordinates[i].x = oldCoordinates[i].x;
                        board[player.coordinates[i].y][player.coordinates[i].x] = player.type;
                }

        } else {
                for (int i = 0; i < 4; i++) {
                        row = &(player.coordinates[i].y);
                        col = &(player.coordinates[i].x);
                        board[*row][*col] = player.type;
                }
        }


}

void printBoard() {
        for (int row = 0; row < BOARDROWS; row++) {
                printf("%c", '\n');
                for (int col = 0; col < BOARDCOLS; col++) {
                        printf("%d ", board[row][col]);
                }
        }
}

void debugboard() {

        printf("FRAME: %d\n", frame);
        printBoard();
        printf("\n");
}

void move() {
}

void createTetromino() {
        int i = 0;
        int (*tetromino)[4];

        isfalling = true;

        /* TODO: Fix random generation: I should generate batches of 7 figures and
               deliver them one by one 
         */

        player.type = rand() / (RAND_MAX / (6 - 0 + 1));

        switch (player.type) {
                case B:
                case J:
                        tetromino = tetromino_j;
                        break;
                case O:
                        tetromino = tetromino_o;
                        break;
                case T:
                        tetromino = tetromino_t;
                        break;
                case I:
                        tetromino = tetromino_i;
                        break;
                case L:
                        tetromino = tetromino_l;
                        break;
                case Z:
                        tetromino = tetromino_z;
                        break;
                case S:
                        tetromino = tetromino_s;
                        break;
                default:
                        exit(1);
        }

        for (int row = 0; row < 4; row++) {
                for (int col = 0; col < 4; col++) {
                        int tetro_cell = tetromino[row][col];
                        // If the cell is not zero it saves the coordinate
                        if (tetro_cell) {
                                board[row][col + 4] = player.type;
                                player.coordinates[i].y = row;
                                player.coordinates[i].x = col + 4;
                                i++;
                        }
                }
        }
}

void updateBoard(int rows, int cols, int board[rows][cols]) {

        int offsetx = PADDINGX/2;
        int offsety = PADDINGY/2;

        Rectangle block = {
                .width = BLOCKSIZE,
                .height = BLOCKSIZE,
        };

        for (int row = 0; row < rows; row++) {
                for (int col = 0; col < cols; col++) {

                        block.x = (col * BLOCKSIZE) + offsetx;
                        block.y = (row * BLOCKSIZE) + offsety;

                        int cell = board[row][col];

                        switch(cell) {
                                case B:
                                        DrawRectangleRec(block, colors[B]);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                                case J:
                                        DrawRectangleRec(block, colors[J]);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                                case O:
                                        DrawRectangleRec(block, colors[O]);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                                case T:
                                        DrawRectangleRec(block, colors[T]);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                                case I:
                                        DrawRectangleRec(block, colors[I]);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                                case L:
                                        DrawRectangleRec(block, colors[L]);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                                case Z:
                                        DrawRectangleRec(block, colors[Z]);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                                case S:
                                        DrawRectangleRec(block, colors[S]);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                                default:
                                        DrawRectangleRec(block, LIGHTGRAY);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                        }
                }
        }
}
