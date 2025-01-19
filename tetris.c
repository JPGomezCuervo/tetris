#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// TODO: Implement a memory management system
// TODO: Implement SPIN LOCK
// TODO: Implement SUPER ROTATION SYSTEM 

#define SCREENWIDTH     800
#define SCREENHEIGHT    800
#define BOARDCOLS       10
#define BOARDROWS       20
#define BOARDAREA       (SCREENHEIGHT * (3.5/4.0))
#define BLOCKSIZE       BOARDAREA/BOARDROWS
#define PADDINGX        (SCREENWIDTH - BOARDCOLS * BLOCKSIZE)
#define PADDINGY        (SCREENHEIGHT - BOARDROWS * BLOCKSIZE)
#define OFFSET          3
#define EMPTYCELL       -1

int board[BOARDROWS][BOARDCOLS];

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

typedef enum {
        RIGHTLINE = 1,
        LEFTLINE,
        BOTOMLINE,
        SKYLINE,
} enum_Limits;

typedef enum {
        GENERATION,
        FALLING,
        LOCK,
        PATTERN,
        ITERATE,
        ANIMATE,
        ELIMINATE,
        COMPLETION
} enum_game_state;

typedef struct {
        int32_t x;
        int32_t y;
} Vector;

typedef struct { 
        Vector coord[4];
        int32_t id;
        enum_Tetrominoes type;
        bool alive;
        bool is_rotating;
        enum_Movement last_move;
        Vector2 pivot;
        
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
int game_state = 0;
Game player = {0};
/*int screen_width = SCREENWIDTH;*/
/*int screen_height = SCREENHEIGHT;*/

void refreshBoard();
void cleanup();
void create_batch(Tetromino *batch[7]);
Tetromino *get_tetromino();
Tetromino *create_tetromino();
void copy_coordinates(Vector src[4], Vector dst[4]);
Tetromino clear_player_from_board();
bool update_tetromino_coordinates(Tetromino *t, enum_Movement mov);
void fall();
int has_collide_with_limits(Tetromino t);
int has_collide_with_tetromino(Tetromino *t);
bool collision_system_ok(Tetromino *t);
void shuffle_batch(Tetromino *batch[7]);
void move(Tetromino *t);
void set_board(Vector coordinates[4], enum_Tetrominoes type);
Vector vector_rotate(Vector v, float angle);
void correct_if_possible(Tetromino *t);

int main(void) {
        srand(time(NULL));
        memset(board, EMPTYCELL, sizeof(board));

        // TODO: Manage allocated size and resize it!
        entities = malloc(sizeof(Tetromino *) * allocated_size);

        /*SetConfigFlags(FLAG_WINDOW_RESIZABLE);*/
        InitWindow(SCREENWIDTH, SCREENHEIGHT, "tetrix!");
        SetTargetFPS(60);

        game_state = GENERATION;
        while (!WindowShouldClose()) {
                /*if (IsWindowResized() && !IsWindowFullscreen()) {*/
                /*        screen_width = GetScreenWidth();*/
                /*        screen_height = GetScreenHeight();*/
                /*}*/
                fallTimer += GetFrameTime();

                switch(game_state) {
                        case GENERATION:
                                Tetromino *t = get_tetromino();
                                entities[entities_len - 1] = t;
                                player.tetromino = entities[entities_len - 1];
                                game_state = FALLING;
                                break;
                        case FALLING:
                                if (fallTimer >= fallInterval) {
                                        fall();
                                        fallTimer = 0.0f;
                                }
                                break;
                        case LOCK:
                                break;
                        case PATTERN:
                                break;
                        case ITERATE:
                                break;
                        case ANIMATE:
                                break;
                        case ELIMINATE:
                                break;
                        case COMPLETION:
                                break;
                        default:
                }

                move(player.tetromino);

                refreshBoard();
                BeginDrawing();
                ClearBackground(BLACK);
                EndDrawing();
        }

        CloseWindow();
        cleanup();

        return 0;
}

// CHECK THIS FOR A POSSIBLE REFACTOR
void correct_if_possible(Tetromino *t) {
        if (update_tetromino_coordinates(t, DOWN)) {
                update_tetromino_coordinates(t, UP);
                set_board(t->coord, t->type);
        }
}

void move(Tetromino *t) {
        int direction = 0;
        bool keypressed = true;

        if (IsKeyPressed(KEY_RIGHT)) {
                direction = RIGHT;
        } else if (IsKeyPressed(KEY_LEFT)) {
                direction = LEFT;
        } else if (IsKeyDown(KEY_DOWN)) {
                direction = DOWN;
        } else if (IsKeyPressed(KEY_UP)) {
                direction = ROTATE_R;
        } else {
                keypressed = false;
        }

        if (keypressed) {
                if(!update_tetromino_coordinates(t, direction)) {
                        correct_if_possible(t);
                }
        }
}

Vector vector_rotate(Vector v, float angle) {
    Vector result = { 0 };

    float cosres = cosf(angle);
    float sinres = sinf(angle);

    result.x = roundf(v.x * cosres - v.y * sinres);
    result.y = roundf(v.x * sinres + v.y * cosres);

    return result;
}

void set_board(Vector coordinates[4], enum_Tetrominoes type) {

        for (int i = 0; i < 4; i++)
                board[coordinates[i].y][coordinates[i].x] = type;

}

Tetromino clear_player_from_board() {
        Tetromino old_state = {0};
        memcpy(&old_state, &player, sizeof(Game));
        set_board(player.tetromino->coord, EMPTYCELL);
        return old_state;
}

int has_collide_with_limits(Tetromino t) {

        int row = 0;
        int col = 0;

        for (int i = 0; i < 4; i++) {
                col = t.coord[i].x;
                row = t.coord[i].y;

                if (row >= BOARDROWS)
                        return BOTOMLINE;

                if (row < 0) 
                        return SKYLINE;

                if(col >= BOARDCOLS)
                        return RIGHTLINE;

                if (col < 0)
                        return LEFTLINE;
        }

                return 0;
}

int has_collide_with_tetromino(Tetromino *t) {

        int8_t row = 0;
        int8_t col = 0;
        int8_t cell = 0;

        for (int i = 0; i < 4; i++) {
                col = t->coord[i].x;
                row = t->coord[i].y;
                cell = board[row][col];

                if (cell != EMPTYCELL) {
                        return cell;
                }
        }
                return t->id;
}

bool collision_system_ok(Tetromino *t) {

        switch(has_collide_with_limits(*t)) {
                case BOTOMLINE:
                        game_state = GENERATION;
                        return false;
                        break;
                case SKYLINE:
                case RIGHTLINE:
                case LEFTLINE:
                        return false;
                        break;
                default:
                        break;
        }

        if (has_collide_with_tetromino(t) != t->id && !t->is_rotating) {
                game_state = GENERATION;
                return false;
        } else if (has_collide_with_tetromino(t) == t->id) {
                game_state = FALLING;
                return true;
        } 

        return true;
}


bool update_tetromino_coordinates(Tetromino *t, enum_Movement mov) {
        Tetromino vt = {0};
        clear_player_from_board();
        memcpy(&vt, t, sizeof(*t));

        switch(mov) {
                case UP:
                        for (int i = 0; i < 4; i++) vt.coord[i].y--;
                        vt.pivot.y--;

                        break;
                case DOWN:
                        for (int i = 0; i < 4; i++) vt.coord[i].y++;
                        vt.pivot.y++;

                        break;
                case LEFT:
                        for (int i = 0; i < 4; i++) vt.coord[i].x--;
                        vt.pivot.x--;

                        break;
                case RIGHT:
                        for (int i = 0; i < 4; i++) vt.coord[i].x++;
                        vt.pivot.x++;

                        break;

                case ROTATE_R:

                        if (vt.type == O) 
                                break;

                        for (int i = 0; i < 4; i++) {
                                vt.coord[i].x -= vt.pivot.x;
                                vt.coord[i].y -= vt.pivot.y;
                                vt.coord[i] = vector_rotate(vt.coord[i], DEG2RAD * 90.0f);
                                vt.coord[i].x += vt.pivot.x;
                                vt.coord[i].y += vt.pivot.y;
                        }

                        break;

                case ROTATE_L:

                        break;
                default:
                        exit(1);
        }

        if(collision_system_ok(&vt)) {
                copy_coordinates(vt.coord, t->coord);
                t->pivot = vt.pivot;
                set_board(t->coord, t->type);
                return true;
        };

        return false;
}

void fall () {
        update_tetromino_coordinates(player.tetromino, DOWN);
}

inline void copy_coordinates(Vector src[4], Vector dst[4]) {

        for (int i = 0; i < 4; i++) {
                dst[i].x = src[i].x;
                dst[i].y = src[i].y;
        }
}

void swap(Tetromino **a, Tetromino **b) {
        Tetromino *t = *a;
        *a = *b;
        *b = t;
}

void shuffle_batch(Tetromino *batch[7]) {

        for (int i = 6; i > 0; i--) {
                int j = rand() % (i + 1);
                swap(&batch[i], &batch[j]);
        }
}

Tetromino *get_tetromino() {
        Tetromino *t;
        static Tetromino *batch[7] = {0};
        static int counter = 0;

        if (counter >= 7)
                counter = 0;

        if (counter == 0) {
                create_batch(batch);
                shuffle_batch(batch);
        }

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
                t->is_rotating = false;
                t->last_move = 0;
                t->pivot.x = 1.0f + 3.0f;
                t->pivot.y = 1.0f; 
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

                        if (cell == EMPTYCELL) {
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
