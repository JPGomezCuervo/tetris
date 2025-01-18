#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO:Implement a memory management system

#define SCREENWIDTH     800
#define SCREENHEIGHT    800
#define BOARDCOLS       10
#define BOARDROWS       20
#define BOARDAREA       (SCREENHEIGHT * (3.5/4.0))
#define BLOCKSIZE       BOARDAREA/BOARDROWS
#define PADDINGX        (SCREENWIDTH - BOARDCOLS * BLOCKSIZE)
#define PADDINGY        (SCREENHEIGHT - BOARDROWS * BLOCKSIZE)
#define OFFSET          3

int board[BOARDROWS][BOARDCOLS] = {
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
};

Color colors[8] = {BLUE, YELLOW, MAGENTA, SKYBLUE, ORANGE, RED, GREEN};

typedef enum {
        J,
        O,
        T,
        I,
        L,
        Z,
        S
} enum_Tetrominoes;

typedef enum {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        ROTATE_R,
        ROTATE_L,
} enum_Movement;

typedef struct {
        int32_t x;
        int32_t y;
} Vector;

typedef struct { 
        Vector coord[4];
        int32_t id;
        enum_Tetrominoes type;
        bool alive;
} Tetromino;

typedef struct {
        Tetromino *tetromino;
} Game;

Vector shapes[7][4] = {
        { {0,0}, {0,1}, {1,1}, {2,1} }, // J
        { {0,0}, {1,0}, {0,1}, {1,1} }, // O
        { {1,0}, {0,1}, {1,1}, {2,1} }, // T
        { {0,0}, {1,0}, {2,0}, {3,0} }, // I
        { {2,0}, {2,1}, {1,1}, {0,1} }, // L
        { {0,0}, {1,0}, {1,1}, {2,1} }, // Z
        { {1,0}, {2,0}, {0,1}, {1,1} }, // S
};


int64_t allocated_size = 1024;
int64_t entities_len = 0;
Tetromino **entities;
float fallInterval = 1.0f;
float fallTimer = 0.0f;
bool falling = false;
Game player = {0};

void refreshBoard();
void cleanup();
void create_batch(Tetromino *batch[7]);
Tetromino *get_tetromino();
Tetromino *create_tetromino();
void copy_coordinates(Vector src[4], Vector dst[4]);
Tetromino clear_player_from_board();
void update_player_coordinates(Game player, enum_Movement mov);
void fall();

int main(void) {

        // TODO: Manage allocated size and resize it!
        entities = malloc(sizeof(Tetromino *) * allocated_size);

        InitWindow(SCREENWIDTH, SCREENHEIGHT, "tetrix!");
        SetTargetFPS(60);

        while (!WindowShouldClose()) {
                fallTimer += GetFrameTime();

                if (!falling) {
                        Tetromino *t = get_tetromino();
                        entities[entities_len - 1] = t;
                        player.tetromino = entities[entities_len - 1];
                        falling = true;
                }

                if (fallTimer >= fallInterval) {
                        fall();
                        fallTimer = 0.0f;
                }

                refreshBoard();
                BeginDrawing();
                ClearBackground(BLACK);
                EndDrawing();
        }

        CloseWindow();
        cleanup();

        return 0;
}

void set_board(Vector coordinates[4], enum_Tetrominoes type) {

        for (int i = 0; i < 4; i++)
                board[coordinates[i].y][coordinates[i].x] = type;

}

Tetromino clear_player_from_board() {
        Tetromino old_state = {0};
        memcpy(&old_state, &player, sizeof(Game));
        set_board(player.tetromino->coord, -1);
        return old_state;
}

bool collision_system() {

}



void update_player_coordinates(Game player, enum_Movement mov) {
        Vector vcoord[4] = {0};
        copy_coordinates(player.tetromino->coord, vcoord);

        switch(mov) {
                case UP:
                        for (int i = 0; i < 4; i++) vcoord[i].y--;

                        break;
                case DOWN:
                        for (int i = 0; i < 4; i++) vcoord[i].y++;


                        break;
                case LEFT:
                        for (int i = 0; i < 4; i++) vcoord[i].x--;


                        break;
                case RIGHT:
                        for (int i = 0; i < 4; i++) vcoord[i].x++;

                        break;

                case ROTATE_R:

                        /*if (player.tetromino.type == O) */
                        /*        break;*/
                        /**/
                        /*int t = 0;*/
                        /**/
                        /*for (int i = 0; i < 4; i++) {*/
                        /**/
                        /*        vcoord[i].x -= pivot.x;*/
                        /*        vcoord[i].y -= pivot.y;*/
                        /*        vcoord[i] = vectorRotate(vcoord[i], DEG2RAD * 90.0f);*/
                        /*        vcoord[i].x += pivot.x;*/
                        /*        vcoord[i].y += pivot.y;*/
                        /*}*/
                        /**/

                        break;

                case ROTATE_L:

                        break;
                default:
                        exit(1);
        }
        copy_coordinates(vcoord, player.tetromino->coord);
}

void fall () {
        Tetromino old_state = clear_player_from_board();

        update_player_coordinates(player, DOWN);
        set_board(player.tetromino->coord, player.tetromino->type);

}

inline void copy_coordinates(Vector src[4], Vector dst[4]) {

        for (int i = 0; i < 4; i++) {
                dst[i].x = src[i].x;
                dst[i].y = src[i].y;
        }
}

void swap(Tetromino *a, Tetromino *b) {
        Tetromino *t = a;
        a = b;
        b = t;
}

Tetromino *get_tetromino() {
        Tetromino *t;
        static Tetromino *batch[7] = {0};
        static int counter = 0;

        if (counter >= 7)
                counter = 0;

        if (counter == 0)
                create_batch(batch);

        t = batch[counter];
        t->id = entities_len;
        entities_len++;
        counter++;
        return t;
}

Tetromino *create_tetromino() {
                Tetromino *t = malloc(sizeof(Tetromino));
                t->type = 0;
                t->id = 0;
                t->alive = true;
                return t;
}

void create_batch(Tetromino *batch[7]) {

        for (int i = 0; i < 7; i++) {
                Tetromino *t = create_tetromino(); 
                t->type = i;
                copy_coordinates(shapes[i], t->coord);
                for (int j = 0; j < 4; j++) t->coord[j].x += OFFSET;
                batch[i] = t;
        }
}

void cleanup() {

        for (int i = 0; i < entities_len; i++) {
                free(entities[i]);
        }
        free(entities);
}

void refreshBoard() {

        int offsetx = PADDINGX/2;
        int offsety = PADDINGY/2;

        Rectangle block = {
                .width = BLOCKSIZE,
                .height = BLOCKSIZE,
        };

        // Check this for loop;
        for (int i = 0; i < entities_len; i++) {
                Tetromino t;
                t = *entities[i];

                if (t.alive) 
                        for (int j = 0; j < 4; j++) {
                                int x = t.coord[j].x;
                                int y = t.coord[j].y;
                                board[y][x] = t.id;
                        }
        }

        for (int row = 0; row < BOARDROWS; row++) {
                for (int col = 0; col < BOARDCOLS; col++) {
                        Tetromino t;

                        block.x = (col * BLOCKSIZE) + offsetx;
                        block.y = (row * BLOCKSIZE) + offsety;

                        int cell = board[row][col];

                        if (cell == -1) {
                                DrawRectangleRec(block, BLACK);
                                DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                        } else {
                                t = *entities[cell];
                                DrawRectangleRec(block, colors[t.type]);
                                DrawRectangleLinesEx(block, 1.0, LIGHTGRAY);
                        }
                }
        }
}
