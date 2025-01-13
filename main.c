#include <raylib.h>
#include <stdbool.h>

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

Color colors [8];
int board[BOARDCOLS][BOARDROWS] = {0};
bool oncreation = false;

int tetromino_j[4][4]= {
        {1,0,0,0},
        {1,1,1,0},
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
        {0,0,0,0},
        {1,1,0,0},
        {1,1,0,0},
        {0,0,0,0}
};

void updateBoard(int cols, int rows, int board[cols][rows]);
void createTetromino(int tetromino[4][4], Tetrominoes type );

int main(void) {

        colors[B] = BLACK;
        colors[J] = BLUE;
        colors[O] = YELLOW;
        colors[T] = MAGENTA;
        colors[I] = SKYBLUE;
        colors[L] = ORANGE;
        colors[Z] = RED;
        colors[S] = GREEN;

        InitWindow(SCREENWIDTH, SCREENHEIGHT, "tetris!");
        SetTargetFPS(60);

        while (!WindowShouldClose()) {
                createTetromino(tetromino_o, O);
                BeginDrawing();
                updateBoard(BOARDCOLS, BOARDROWS, board);
                ClearBackground(BLACK);
                EndDrawing();
        }

        CloseWindow();

        return 0;
}


void createTetromino(int tetromino[4][4], Tetrominoes type) {

        for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 4; x++) {
                        int tetro_cell = tetromino[y][x];
                        if (tetro_cell) {
                                board[x+4][y] = type;
                        }
                }
        }
}

void updateBoard(int cols, int rows, int board[cols][rows]) {

        int offsetx = PADDINGX/2;
        int offsety = PADDINGY/2;

        Rectangle block = {
                .width = BLOCKSIZE,
                .height = BLOCKSIZE,
        };

        for (int x = 0; x < cols; x++) {
                for (int y = 0; y < rows; y++) {

                        block.x = (x * BLOCKSIZE) + offsetx;
                        block.y = (y * BLOCKSIZE) + offsety;

                        int cell = board[x][y];

                        switch(cell) {
                                case 0:
                                        DrawRectangleRec(block, colors[0]);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                                case 1:
                                        DrawRectangleRec(block, colors[1]);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                                case 2:
                                        DrawRectangleRec(block, colors[2]);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                                case 3:
                                        DrawRectangleRec(block, colors[3]);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                                case 4:
                                        DrawRectangleRec(block, colors[4]);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                                case 5:
                                        DrawRectangleRec(block, colors[5]);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                                case 6:
                                        DrawRectangleRec(block, colors[6]);
                                        DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                                        break;
                                case 7:
                                        DrawRectangleRec(block, colors[7]);
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
