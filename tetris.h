#ifndef TETRIS_H
#define TETRIS_H

#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
#define OUTOFGAME       -1


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
        Tetromino *hold;
        bool can_hold;
        int game_state;
} Game;

Vector vector_rotate(Vector v, float angle);
void add_xy_to_tetromino(Tetromino *t, int x, int y);
void copy_coordinates(Vector src[4], Vector dst[4]);
void swap(Tetromino **a, Tetromino **b);
void cleanup();
void init_board();
void set_board(Vector coordinates[4], enum_Tetrominoes type);
Tetromino clear_player_from_board();
void refresh_board();
Tetromino *create_tetromino();
Tetromino *get_tetromino();
void create_batch(Tetromino *batch[7]);
void shuffle_batch(Tetromino *batch[7]);
void move(Tetromino *t);
bool update_tetromino_coordinates(Tetromino *t, enum_Movement move);
int has_collide_with_limits(const Tetromino *t);
int has_collide_with_tetromino(const Tetromino *t);
bool collision_system_ok(Tetromino *t);
bool super_rotation_system(Tetromino *t, enum_Movement move);
void clear_lines();
void swap_row(int elements, int rowa[elements], int rowb[elements]);
void print_board();
void add_xy_to_tetromino_secure(Tetromino *t, int x, int y);

extern int64_t entities_len;
extern Tetromino **entities;
extern Game player;
extern Color colors[8];

#endif
