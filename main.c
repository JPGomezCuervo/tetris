#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef enum {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        ROTATE_R,
        ROTATE_L,
} Movement;

typedef struct {
        int x;
        int y;
} Vector;

typedef struct {
        Tetrominoes type;
        Vector *coordinates;

} Player;

void refreshBoard(int rows, int cols, int board[rows][cols]);
void createTetromino();
void fallingTetromino();
void printBoard();
void debugboard();
bool hasCollide();
void move();
void copyCoordinates(Vector src[4], Vector dst[4]);
void updatePlayerCoordinates(Vector curr[4], Movement m);
void setBoard(Vector coordinates[4], Tetrominoes type);
void printCoordinates();
Vector vectorRotate(Vector v, float angle);
bool compareCoordinates(Vector new[4], Vector old[4]);

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


Vector tetromino_j[4] = { {0,0}, {0,1}, {1,1}, {2,1} };
Vector tetromino_i[4] = { {0,0}, {1,0}, {2,0}, {3,0} };
Vector tetromino_l[4] = { {2,0}, {2,1}, {1,1}, {0,1} };
Vector tetromino_o[4] = { {0,0}, {1,0}, {0,1}, {1,1} };
Vector tetromino_s[4] = { {1,0}, {2,0}, {0,1}, {1,1} };
Vector tetromino_t[4] = { {1,0}, {0,1}, {1,1}, {2,1} };
Vector tetromino_z[4] = { {0,0}, {1,0}, {1,1}, {2,1} };


Color colors[8] = {BLACK, BLUE, YELLOW, MAGENTA, SKYBLUE, ORANGE, RED, GREEN};
int frame = 0;
bool oncreation = false;
bool isfalling = false;
float fallInterval = 1.0f;
float fallTimer = 0.0f;
Player player = {0};
Player olplayer = {0};
Vector2 pivot = {0};

int main(void) {

        player.coordinates = malloc(4 * sizeof(Vector));
        olplayer.coordinates = malloc(4 * sizeof(Vector));

        InitWindow(SCREENWIDTH, SCREENHEIGHT, "tetris!");
        SetTargetFPS(60);

        while (!WindowShouldClose()) {

                copyCoordinates(player.coordinates, olplayer.coordinates);

                fallTimer += GetFrameTime();
                frame++;


                if (!isfalling)
                        createTetromino();
                move();

                if (!compareCoordinates(player.coordinates, olplayer.coordinates))
                {
                        printCoordinates();
                        printBoard();
                }

                if (fallTimer >= fallInterval) {
                        fallingTetromino();
                        debugboard();
                        fallTimer = 0.0f;
                }

                refreshBoard(BOARDROWS, BOARDCOLS, board);

                BeginDrawing();
                ClearBackground(BLACK);
                EndDrawing();
        }

        CloseWindow();

        free(player.coordinates);
        free(olplayer.coordinates);

        return 0;
}


bool compareCoordinates(Vector new[4], Vector old[4]) {

        for (int i = 0; i < 4; i++) {
                if (!(new[i].x == old[i].x && new[i].y == old[i].y))
                        return false;
        }
        return true;
}

bool hasCollide() {

        int col = 0;
        int row = 0;
        bool collided = 0;

        for (int i = 0; i < 4; i++) {
                row = player.coordinates[i].y;
                col = player.coordinates[i].x;

                // Has collide with the limit 
                if (row >= BOARDROWS || row < 0) 
                        return true;

                if (col < 0 || col >= BOARDCOLS)
                        return true;
                
                // Has collide with another tetrominoe
                if (board[row][col] != 0) 
                        return true;
        }

        return false;
}

Vector vectorRotate(Vector v, float angle) {
    Vector result = { 0 };

    float cosres = cosf(angle);
    float sinres = sinf(angle);

    result.x = roundf(v.x * cosres - v.y * sinres);
    result.y = roundf(v.x * sinres + v.y * cosres);

    return result;
}

void updatePlayerCoordinates(Vector curr[4], Movement m) {

        switch(m) {
                case UP:
                        for (int i = 0; i < 4; i++) curr[i].y--;

                        pivot.y--;
                        break;
                case DOWN:
                        for (int i = 0; i < 4; i++) curr[i].y++;

                        pivot.y++;
                        break;
                case LEFT:
                        for (int i = 0; i < 4; i++) curr[i].x--;

                        pivot.x--;
                        break;
                case RIGHT:
                        for (int i = 0; i < 4; i++) curr[i].x++;

                        pivot.x++;
                        break;

                case ROTATE_R:

                        if (player.type == O) 
                                break;

                        for (int i = 0; i < 4; i++) {

                                curr[i].x -= pivot.x;
                                curr[i].y -= pivot.y;
                                curr[i] = vectorRotate(curr[i], DEG2RAD * 90.0f);
                                curr[i].x += pivot.x;
                                curr[i].y += pivot.y;

                                if (hasCollide()) {
                                        setBoard(player.coordinates, B);
                                        if (curr[i].x >= BOARDCOLS) {
                                                updatePlayerCoordinates(player.coordinates, LEFT);
                                        } else if (curr[i].x < 0) {
                                                updatePlayerCoordinates(player.coordinates, RIGHT);
                                        }

                                        if (curr[i].y >= BOARDCOLS) {
                                                updatePlayerCoordinates(player.coordinates, UP);
                                        } else if (curr[i].y < 0) {
                                                updatePlayerCoordinates(player.coordinates, UP);
                                        }
                                }
                        }

                        break;
                case ROTATE_L:
                        if (player.type == O) 
                                break;

                        for (int i = 0; i < 4; i++) {

                                curr[i].x -= pivot.x;
                                curr[i].y -= pivot.y;
                                curr[i] = vectorRotate(curr[i], DEG2RAD * -90.0f);
                                curr[i].x += pivot.x;
                                curr[i].y += pivot.y;

                                // CHECK!!! REFACTORRRRR
                                if (hasCollide()) {
                                        setBoard(player.coordinates, B);
                                        if (curr[i].x >= BOARDCOLS) {
                                                updatePlayerCoordinates(player.coordinates, LEFT);
                                        } else if (curr[i].x < 0) {
                                                updatePlayerCoordinates(player.coordinates, RIGHT);
                                        }

                                        if (curr[i].y >= BOARDCOLS) {
                                                updatePlayerCoordinates(player.coordinates, UP);
                                        } else if (curr[i].y < 0) {
                                                updatePlayerCoordinates(player.coordinates, UP);
                                        }
                                }
                        }

                        break;
                default:
                        exit(1);
        }
}

inline void copyCoordinates(Vector src[4], Vector dst[4]) {

        for (int i = 0; i < 4; i++) {
                dst[i].x = src[i].x;
                dst[i].y = src[i].y;
        }
}

void setBoard(Vector coordinates[4], Tetrominoes type) {

        for (int i = 0; i < 4; i++)
                board[coordinates[i].y][coordinates[i].x] = type;

}

void fallingTetromino() {

        setBoard(player.coordinates, B);
        updatePlayerCoordinates(player.coordinates, DOWN);

        if (hasCollide()) {
                isfalling = false;
                updatePlayerCoordinates(player.coordinates, UP);
        }

        setBoard(player.coordinates, player.type);
}

void printBoard() {
        for (int row = 0; row < BOARDROWS; row++) {
                printf("%c", '\n');
                for (int col = 0; col < BOARDCOLS; col++) {
                        printf("%d ", board[row][col]);
                }
        }
}

void printCoordinates() {

        char str[255] = {0};
        str[255] = '\0';
        int x = 0;
        int y = 0;
        int len = 0;

        printf("Player coordinates:\n");

        for (int i = 0; i < 4; i++) {
                len = strlen(str);
                x = player.coordinates[i].x;
                y = player.coordinates[i].y;

                sprintf(str+len, "ROW-> %d, COL-> %d\n", y, x);
        }
        printf(str);
}

void debugboard() {

        printf("FRAME: %d\n", frame);
        printBoard();
        printf("\n");
        printCoordinates();
        printf("\n");
}

void move() {
        int direction = 0;
        int opposite = 0;
        bool keypressed = true;

        if (IsKeyPressed(KEY_RIGHT)) {
                direction = RIGHT;
                opposite = LEFT;
        } else if (IsKeyPressed(KEY_LEFT)) {
                direction = LEFT;
                opposite = RIGHT;
        } else if (IsKeyDown(KEY_DOWN)) {
                direction = DOWN;
                opposite = UP;
        } else if (IsKeyPressed(KEY_UP)) {
                direction = ROTATE_R;
                opposite = ROTATE_L;
        } else {
                keypressed = false;
        }

        if (keypressed) {
                setBoard(player.coordinates, B);
                updatePlayerCoordinates(player.coordinates, direction);
                if(hasCollide()) {
                        updatePlayerCoordinates(player.coordinates, opposite);
                        if (direction == DOWN)
                                isfalling = false;
                }
                setBoard(player.coordinates, player.type);
        }
}

void createTetromino() {
        Vector tetromino[4];
        int x = 0;
        int y = 0;

        isfalling = true;

        /* TODO: Fix random generation: I should generate batches of 7 figures and
               deliver them one by one 
         */

        player.type = rand() / (RAND_MAX / (7 - 0 + 1));

        switch (player.type) {
                case B:
                case J:
                        copyCoordinates(tetromino_j, tetromino);
                        pivot.x = 1.0f + 4;
                        pivot.y = 1.0f;
                        break;
                case O:
                        copyCoordinates(tetromino_o,tetromino);
                        break;
                case T:
                        copyCoordinates(tetromino_t,tetromino);
                        pivot.x = 1.0f + 4;
                        pivot.y = 1.0f;
                        break;
                case I:
                        copyCoordinates(tetromino_i,tetromino);
                        pivot.x = roundf(1.5f + 4);
                        pivot.y = 0.0f;
                        break;
                case L:
                        copyCoordinates(tetromino_l,tetromino);
                        pivot.x = 1.0f + 4;
                        pivot.y = 1.0f;
                        break;
                case Z:
                        copyCoordinates(tetromino_z,tetromino);
                        pivot.x = 1.0f + 4;
                        pivot.y = 1.0f;
                        break;
                case S:
                        copyCoordinates(tetromino_s,tetromino);
                        pivot.x = 1.0f + 4;
                        pivot.y = 1.0f;
                        break;
                default:
                        exit(1);
        }

        for (int i = 0; i < 4; i++) {
                y = tetromino[i].y;
                // TODO: replace magic number
                // 4 is the offset.
                x = tetromino[i].x + 4;
                board[y][x] = player.type;
                player.coordinates[i].x = x;
                player.coordinates[i].y = y;
        }
}

void refreshBoard(int rows, int cols, int board[rows][cols]) {

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
