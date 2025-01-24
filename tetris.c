#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// TODO: Implement ghost tetromino
// TODO: Implement score
// TODO: Implement keypressed loop
// TODO: Implement hard drop
// TODO: Implement a memory management system
// TODO: Implement resizable window
// TODO: Implement Holding mechanic
// TODO: Refactor SRS
// TODO: Implement queue visualizer

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
        int rotation_state;
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

/* 
   0 = spawn state
   1 = state resulting from a clockwise rotation ("right") from spawn
   2 = state resulting from 2 successive rotations in either direction from spawn.
   3 = state resulting from a counter-clockwise ("left") rotation from spawn
*/
Vector srs_offsets_right[][4] = {
        { {-1,0}, {-1,1}, {0,-2}, {-1,-2} },
        { {1,0}, {1,-1}, {0,2}, {1,2} },
        { {1,0}, {1,1}, {0,-2}, {1,-2} },
        { {-1,0}, {-1,-1}, {0,2}, {-1,2} },
};

Vector srs_offsets_left[][4] = {
        { {1,0}, {1,1}, {0,-2}, {1,-2} },
        { {-1,0}, {-1,-1}, {0,2}, {-1,2} },
        { {-1,0}, {-1,1}, {0,-2}, {-1,-2} },
        { {1,0}, {1,-1}, {0,2}, {1,2} },
};

Vector srs_offsets_i_right[][4] = {
        { {-2,0}, {1,0}, {-2,-1}, {1,2} },
        { {-1,0}, {2,0}, {-1,2}, {2,-1} },
        { {2,0}, {-1,0}, {2,1}, {-1,-2} },
        { {1,0}, {-2,0}, {1,-2}, {-2,1} },
};

Vector srs_offsets_i_left[][4] = {
        { {-1,0}, {2,0}, {-1,2}, {2,-1} },
        { {-2,0}, {1,0}, {-2,-1}, {1,2} },
        { {1,0}, {-2,0}, {1,-2}, {-2,1} },
        { {2,0}, {-1,0}, {2,1}, {-1,-2} },
};


int64_t allocated_size = 1024;
int64_t entities_len = 0;
Tetromino **entities;
float lock_interval = 0.5f; 
float lock_timer = 0.0f; 
float fall_interval = 1.0f;
float fall_timer = 0.0f;
int game_state = 0;
Game player = {0};
/*int screen_width = SCREENWIDTH;*/
/*int screen_height = SCREENHEIGHT;*/

void refresh_board();
void cleanup();
void create_batch(Tetromino *batch[7]);
Tetromino *get_tetromino();
Tetromino *create_tetromino();
void copy_coordinates(Vector src[4], Vector dst[4]);
Tetromino clear_player_from_board();
bool update_tetromino_coordinates(Tetromino *t, enum_Movement mov);
int has_collide_with_limits(Tetromino t);
int has_collide_with_tetromino(Tetromino *t);
bool collision_system_ok(Tetromino *t);
void shuffle_batch(Tetromino *batch[7]);
void move(Tetromino *t);
void set_board(Vector coordinates[4], enum_Tetrominoes type);
Vector vector_rotate(Vector v, float angle);
bool super_rotation_system(Tetromino *t, enum_Movement mov);
void add_xy_to_tetromino(Tetromino *t, int x, int y);

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
                fall_timer += GetFrameTime();
                lock_timer += GetFrameTime();

                switch(game_state) {
                        case GENERATION:
                                Tetromino *t = get_tetromino();
                                entities[entities_len - 1] = t;
                                player.tetromino = entities[entities_len - 1];
                                /*if (player.tetromino->type == I) {*/
                                /*        update_tetromino_coordinates(player.tetromino, DOWN);*/
                                /*}*/

                                game_state = FALLING;
                                break;
                        case FALLING:
                                if (fall_timer >= fall_interval) {
                                        update_tetromino_coordinates(player.tetromino, DOWN);
                                        fall_timer = 0.0f;
                                }
                                break;
                        case LOCK:
                                if (lock_timer >= lock_interval) {
                                        game_state = GENERATION;
                                        lock_timer = 0.0f;
                                        break;
                                } else {
                                        move(player.tetromino);
                                }
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

                refresh_board();
                BeginDrawing();
                ClearBackground(BLACK);
                EndDrawing();
        }

        CloseWindow();
        cleanup();

        return 0;
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
        } else if (IsKeyPressed(KEY_Z)) {
                direction = ROTATE_L;
        } else {
                keypressed = false;
        }

        if (keypressed) {
                update_tetromino_coordinates(t, direction);
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

bool super_rotation_system(Tetromino *t, enum_Movement mov) {

        if (t->type != I) {
                if (mov == ROTATE_R) {
                        for (int i = 0; i < 4; i++) {
                                add_xy_to_tetromino(t,
                                                srs_offsets_right[t->rotation_state][i].x,
                                                srs_offsets_right[t->rotation_state][i].y);

                                if (t->rotation_state == 3) {
                                        t->rotation_state = 0;
                                } else {
                                        t->rotation_state++;
                                }

                                if (collision_system_ok(t)) {
                                        return true;
                                } else {
                                        add_xy_to_tetromino(t,
                                                        srs_offsets_right[t->rotation_state][i].x * -1,
                                                        srs_offsets_right[t->rotation_state][i].y * -1);
                                }
                        }
                } else if (mov == ROTATE_L) {
                        for (int i = 0; i < 4; i++) {
                                if (t->rotation_state >= 4) {
                                        t->rotation_state = 0;
                                } else {
                                        t->rotation_state++;
                                }
                                add_xy_to_tetromino(t,
                                                srs_offsets_left[t->rotation_state][i].x,
                                                srs_offsets_left[t->rotation_state][i].y);

                                if (collision_system_ok(t)) {
                                        return true;
                                } else {
                                        add_xy_to_tetromino(t,
                                                        srs_offsets_left[t->rotation_state][i].x * -1,
                                                        srs_offsets_left[t->rotation_state][i].y * -1);
                                }
                        }
                }
        } else {
                if (mov == ROTATE_R) {
                        for (int i = 0; i < 4; i++) {
                                add_xy_to_tetromino(t,
                                                srs_offsets_i_right[t->rotation_state][i].x,
                                                srs_offsets_i_right[t->rotation_state][i].y);

                                if (t->rotation_state == 3) {
                                        t->rotation_state = 0;
                                } else {
                                        t->rotation_state++;
                                }

                                if (collision_system_ok(t)) {
                                        return true;
                                } else {
                                        add_xy_to_tetromino(t,
                                                        srs_offsets_i_right[t->rotation_state][i].x * -1,
                                                        srs_offsets_i_right[t->rotation_state][i].y * -1);
                                }
                        }
                } else if (mov == ROTATE_L) {
                        for (int i = 0; i < 4; i++) {
                                if (t->rotation_state >= 4) {
                                        t->rotation_state = 0;
                                } else {
                                        t->rotation_state++;
                                }
                                add_xy_to_tetromino(t,
                                                srs_offsets_i_left[t->rotation_state][i].x,
                                                srs_offsets_i_left[t->rotation_state][i].y);

                                if (collision_system_ok(t)) {
                                        return true;
                                } else {
                                        add_xy_to_tetromino(t,
                                                        srs_offsets_i_left[t->rotation_state][i].x * -1,
                                                        srs_offsets_i_left[t->rotation_state][i].y * -1);
                                }
                        }
                }
        }
        return false;
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

        int collision_id = 0;
        switch(has_collide_with_limits(*t)) {
                case BOTOMLINE:
                        if (game_state != LOCK)
                                game_state = LOCK;
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

        collision_id = has_collide_with_tetromino(t);

        if (collision_id != t->id) {
                game_state = LOCK;
                return false;
        } else {
                return true;
        }
}


void add_xy_to_tetromino(Tetromino *t, int x, int y) {

        for (int i = 0; i < 4; i++) {
                t->coord[i].x += x;
                t->coord[i].y += y;
        }

        t->pivot.x += x;
        t->pivot.y += y;
}

bool update_tetromino_coordinates(Tetromino *t, enum_Movement mov) {
        Tetromino vt = {0};
        clear_player_from_board();
        memcpy(&vt, t, sizeof(*t));

        switch(mov) {
                case UP:
                        add_xy_to_tetromino(&vt, 0, -1);
                        break;
                case DOWN:
                        add_xy_to_tetromino(&vt, 0, 1);

                        break;
                case LEFT:
                        add_xy_to_tetromino(&vt, -1, 0);

                        break;
                case RIGHT:
                        add_xy_to_tetromino(&vt, +1, 0);

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

                        if (vt.type == O) 
                                break;

                        for (int i = 0; i < 4; i++) {
                                vt.coord[i].x -= vt.pivot.x;
                                vt.coord[i].y -= vt.pivot.y;
                                vt.coord[i] = vector_rotate(vt.coord[i], DEG2RAD * -90.0f);
                                vt.coord[i].x += vt.pivot.x;
                                vt.coord[i].y += vt.pivot.y;
                        }

                        break;
                default:
                        exit(1);
        }

        if ( (collision_system_ok(&vt))) {
                copy_coordinates(vt.coord, t->coord);
                t->pivot = vt.pivot;
                set_board(t->coord, t->type);

                if (mov == ROTATE_R) {
                        if (t->rotation_state == 3)
                                t->rotation_state = 0;
                        else 
                                t->rotation_state++;
                } else if (mov == ROTATE_L) {
                        if (t->rotation_state == 3)
                                t->rotation_state = 0;
                        else 
                                t->rotation_state--;
                } 

                game_state = FALLING;
                return true;
        } else {
                if ((mov == ROTATE_R || mov == ROTATE_L) && super_rotation_system(&vt, mov)) {
                        game_state = FALLING;
                        copy_coordinates(vt.coord, t->coord);
                        t->pivot = vt.pivot;
                        set_board(t->coord, t->type);
                        t->rotation_state = 0;
                        return true;
                }
        }

        return false;
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

        if (t->type == I) {
                add_xy_to_tetromino(t, 0, 1);
        }

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
                t->rotation_state = 0;
                t->pivot.x = 1.0f + 3.0f;
                t->pivot.y = 1.0f; 
                return t;
}

void create_batch(Tetromino *batch[7]) {

        for (int i = 0; i < 7; i++) {
                Tetromino *t = create_tetromino(); 
                t->type = i;
                copy_coordinates(shapes[i], t->coord);

                if ( t->type == I) {
                        t->pivot.y = 0.0f;
                }

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

void refresh_board() {

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
