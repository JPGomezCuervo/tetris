#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

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
        Tetrominoes type;
        int coordinates[4][2];

} Player;

Color colors [8];
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

bool oncreation = false;
bool isfalling = true;

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
        {1,1,0,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
};

void updateBoard(int rows, int cols, int board[rows][cols]);
void createTetromino(int tetromino[4][4], Tetrominoes type);
void fallingTetromino();
void printBoard();
void debugboard();
bool hasCollide();

Player player = {0};
int frame = 0;


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
        SetTargetFPS(1);

        createTetromino(tetromino_o, O);
        while (!WindowShouldClose()) {
                debugboard();
                frame++;
                BeginDrawing();
                updateBoard(BOARDROWS, BOARDCOLS, board);
                ClearBackground(BLACK);
                EndDrawing();
                if (isfalling) {
                        fallingTetromino();
                }
        }

        CloseWindow();

        return 0;
}


bool hasCollide() {

        int col = 0;
        int row = 0;

        for (int i = 0; i < 4; i++) {
                row = player.coordinates[i][0];
                col = player.coordinates[i][1];

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
        int oldCoordinates[4][4] = {0};

        // TODO: 2 for loops, not optimal; fix it.

        for (int i = 0; i < 4; i++) {
                row = &(player.coordinates[i][0]);
                col = &(player.coordinates[i][1]);
                oldCoordinates[i][0] = *row;
                oldCoordinates[i][1] = *col;

                printf("player coordinates %d: ROW-> %d, COL-> %d \n", 
                                i, *row, *col);


                board[*row][*col] = 0;
                (*row)++;
        }

        if (hasCollide()) {
                isfalling = false;

                for (int i = 0; i < 4; i++) {
                        player.coordinates[i][0] = oldCoordinates[i][0];
                        player.coordinates[i][1] = oldCoordinates[i][1];
                        board[player.coordinates[i][0]][player.coordinates[i][1]] = player.type;
                }

        } else {
                for (int i = 0; i < 4; i++) {
                        row = &(player.coordinates[i][0]);
                        col = &(player.coordinates[i][1]);
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

void createTetromino(int tetromino[4][4], Tetrominoes type) {

        int i = 0;
        player.type = type;

        for (int row = 0; row < 4; row++) {
                for (int col = 0; col < 4; col++) {
                       int tetro_cell = tetromino[row][col];
                        if (tetro_cell) {
                                board[row][col+4] = type;
                                player.coordinates[i][0] = row;
                                player.coordinates[i][1] = col+4;
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
