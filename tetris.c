#include "tetris.h"

// TODO: memset is not that good with values different a -0 CHECK
// TODO: Implement ghost tetromino
// TODO: Implement score
// TODO: Implement keypressed loop
// TODO: Implement a memory management system
// TODO: Implement resizable window
// TODO: Implement queue visualizer
// TODO: fix I rotation
// TODO: REFACTOR THE WHOLE CODE SPECIALLY THE PARTS OF GAME_STATE

int64_t entities_len = 0;
Tetromino **entities = 0;
Game player = {0};
Color colors[8] = {BLUE, YELLOW, MAGENTA, SKYBLUE, ORANGE, RED, GREEN};
int board[BOARDROWS][BOARDCOLS]; 

Vector shapes[7][4] = {
        { {0,0}, {0,1}, {1,1}, {2,1} }, // J
        { {0,0}, {1,0}, {0,1}, {1,1} }, // O
        { {1,0}, {0,1}, {1,1}, {2,1} }, // T
        { {0,0}, {1,0}, {2,0}, {3,0} }, // I
        { {2,0}, {2,1}, {1,1}, {0,1} }, // L
        { {0,0}, {1,0}, {1,1}, {2,1} }, // Z
        { {1,0}, {2,0}, {0,1}, {1,1} }, // S
};

Vector srs_offsets_right[4][4] = {
        { {-1,0}, {-1,1}, {0,-2}, {-1,-2} },
        { {1,0}, {1,-1}, {0,2}, {1,2} },
        { {1,0}, {1,1}, {0,-2}, {1,-2} },
        { {-1,0}, {-1,-1}, {0,2}, {-1,2} },
};

Vector srs_offsets_left[4][4] = {
        { {1,0}, {1,1}, {0,-2}, {1,-2} },
        { {-1,0}, {-1,-1}, {0,2}, {-1,2} },
        { {-1,0}, {-1,1}, {0,-2}, {-1,-2} },
        { {1,0}, {1,-1}, {0,2}, {1,2} },
};

Vector srs_offsets_i_right[4][4] = {
        { {-2,0}, {1,0}, {-2,-1}, {1,2} },
        { {-1,0}, {2,0}, {-1,2}, {2,-1} },
        { {2,0}, {-1,0}, {2,1}, {-1,-2} },
        { {1,0}, {-2,0}, {1,-2}, {-2,1} },
};

Vector srs_offsets_i_left[4][4] = {
        { {-1,0}, {2,0}, {-1,2}, {2,-1} },
        { {-2,0}, {1,0}, {-2,-1}, {1,2} },
        { {1,0}, {-2,0}, {1,-2}, {-2,1} },
        { {2,0}, {-1,0}, {2,1}, {-1,-2} },
};


Vector vector_rotate(Vector v, float angle)
{
    Vector result = { 0 };

    float cosres = cosf(angle);
    float sinres = sinf(angle);

    result.x = roundf(v.x * cosres - v.y * sinres);
    result.y = roundf(v.x * sinres + v.y * cosres);

    return result;
}

void add_xy_to_tetromino(Tetromino *t, int x, int y)
{

        for (int i = 0; i < 4; i++) {
                if (t->coord[i].x == EMPTYCELL || t->coord[i].y == EMPTYCELL)
                        continue;

                t->coord[i].x += x;
                t->coord[i].y += y;
        }

        t->pivot.x += x;
        t->pivot.y += y;
}

inline void copy_coordinates(Vector src[4], Vector dst[4])
{
        for (int i = 0; i < 4; i++) {
                dst[i].x = src[i].x;
                dst[i].y = src[i].y;
        }
}

void swap(Tetromino **a, Tetromino **b)
{
        Tetromino *t = *a;
        *a = *b;
        *b = t;
}

void cleanup()
{

        for (int i = 0; i < entities_len; i++) {
                free(entities[i]);
        }
        free(entities);
}


void init_board()
{
        for (int i = 0; i < BOARDROWS; i++) {
                for (int j = 0; j < BOARDCOLS; j++) {
                        board[i][j] = EMPTYCELL;
                }
        }
}

void set_board(Vector coordinates[4], enum_Tetrominoes type)
{

        for (int i = 0; i < 4; i++)
                board[coordinates[i].y][coordinates[i].x] = type;

}

Tetromino clear_player_from_board()
{
        Tetromino old_state = {0};
        memcpy(&old_state, &player, sizeof(Game));
        set_board(player.tetromino->coord, EMPTYCELL);
        return old_state;
}

void refresh_board()
{

        int offsetx = PADDINGX/2;
        int offsety = PADDINGY/2;

        Rectangle block = {
                .width = BLOCKSIZE,
                .height = BLOCKSIZE,
        };

        // Check this for loop;
        for (int i = 0; i < entities_len; i++) {
                Tetromino *t;
                t = entities[i];

                // TODO: remove alive prop from tetromino
                if (t != player.hold) 
                        for (int j = 0; j < 4; j++) {
                                int x = t->coord[j].x;
                                int y = t->coord[j].y;
                                if (x == EMPTYCELL || y == EMPTYCELL) 
                                        continue;

                                board[y][x] = t->id;
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

Tetromino *create_tetromino()
{
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

Tetromino *get_tetromino()
{
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

void create_batch(Tetromino *batch[7])
{

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

void shuffle_batch(Tetromino *batch[7])
{

        for (int i = 6; i > 0; i--) {
                int j = rand() % (i + 1);
                swap(&batch[i], &batch[j]);
        }
}

void move(Tetromino *t)
{
        int direction = 0;
        bool should_move = true;

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
        } else if (IsKeyPressed(KEY_SPACE)) {
                while (update_tetromino_coordinates(t, DOWN));
                should_move = false;
        } else if (player.can_hold && IsKeyPressed(KEY_C)) {
                player.can_hold = false;
                clear_player_from_board();
                if (player.hold == NULL) {
                        player.hold = t;
                        player.game_state = GENERATION;
                } else {
                        swap(&player.hold, &player.tetromino);
                        copy_coordinates(shapes[player.tetromino->type], player.tetromino->coord);
                        add_xy_to_tetromino(player.tetromino, OFFSET, 0);

                        if (player.tetromino->type != I) {
                                player.tetromino->pivot.x = 1.0f + 3.0f;
                                player.tetromino->pivot.y = 1.0f; 
                        } else {
                                player.tetromino->pivot.y = 0.0f;
                        }
                }
                should_move = false;
        } else {
                should_move = false;
        }

        if (should_move) {
                if (!update_tetromino_coordinates(t, direction)) {
                        player.game_state = FALLING;
                }
        }
}

bool update_tetromino_coordinates(Tetromino *t, enum_Movement move)
{
        Tetromino vt = {0};
        clear_player_from_board();
        memcpy(&vt, t, sizeof(*t));

        switch(move) {
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

                if (move == ROTATE_R) {
                        if (t->rotation_state == 3)
                                t->rotation_state = 0;
                        else 
                                t->rotation_state++;
                } else if (move == ROTATE_L) {
                        if (t->rotation_state == 3)
                                t->rotation_state = 0;
                        else 
                                t->rotation_state--;
                } 

                player.game_state = FALLING;
                return true;
        } else {
                if ((move == ROTATE_R || move == ROTATE_L) && super_rotation_system(&vt, move)) {
                        player.game_state = FALLING;
                        copy_coordinates(vt.coord, t->coord);
                        t->pivot = vt.pivot;
                        set_board(t->coord, t->type);
                        t->rotation_state = 0;
                        return true;
                }
        }

        return false;
}

int has_collide_with_limits(const Tetromino *t)
{

        int row = 0;
        int col = 0;

        for (int i = 0; i < 4; i++) {
                col = t->coord[i].x;
                row = t->coord[i].y;

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

int has_collide_with_tetromino(const Tetromino *t)
{

        int8_t row = 0;
        int8_t col = 0;
        int8_t cell = 0;

        for (int i = 0; i < 4; i++) {
                col = t->coord[i].x;
                row = t->coord[i].y;
                cell = board[row][col];

                if (row == EMPTYCELL || col == EMPTYCELL)
                        continue;

                if (cell != EMPTYCELL) {
                        return cell;
                }
        }
                return t->id;
}

bool collision_system_ok(Tetromino *t)
{

        int collision_id = 0;
        switch(has_collide_with_limits(t)) {
                case BOTOMLINE:
                        if (player.game_state != LOCK)
                                player.game_state = LOCK;
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
                player.game_state = LOCK;
                return false;
        } else {
                return true;
        }
}

bool super_rotation_system(Tetromino *t, enum_Movement move)
{

        Vector (*ptr_srs_offsets)[4][4];

        if (t->type == I) {
                if (move == ROTATE_R)
                        ptr_srs_offsets = &srs_offsets_i_right;
                else
                        ptr_srs_offsets = &srs_offsets_i_left;

        } else {
                if (move == ROTATE_R)
                        ptr_srs_offsets = &srs_offsets_right;
                else
                        ptr_srs_offsets = &srs_offsets_left;
        }


        for (int i = 0; i < 4; i++) {
                add_xy_to_tetromino(t,
                                (*ptr_srs_offsets)[t->rotation_state][i].x,
                                (*ptr_srs_offsets)[t->rotation_state][i].y);

                if (t->rotation_state == 3)
                        t->rotation_state = 0;
                else
                        t->rotation_state++;


                if (collision_system_ok(t))
                        return true;
                else
                        add_xy_to_tetromino(t,
                                        (*ptr_srs_offsets)[t->rotation_state][i].x * -1,
                                        (*ptr_srs_offsets)[t->rotation_state][i].y * -1);
        }


        return false;
}

void clear_lines()
{

        int lines_erased = 0;
        bool clean_row = true;

        for (int i = 0; i < 4; i++) {
                clean_row = true;
                Vector pos = player.tetromino->coord[i];

                for (int col = 0; col < BOARDCOLS; col++) {
                        if (board[pos.y][col] == EMPTYCELL) {
                                clean_row = false;
                                break;
                        }
                }

                if (clean_row) {
                        for (int col = 0; col < BOARDCOLS; col++) {
                                int id = board[pos.y][col];
                                for (int i = 0; i < 4; i++) {
                                        if (entities[id]->coord[i].x == col 
                                                        &&
                                                        entities[id]->coord[i].y == pos.y) {
                                                entities[id]->coord[i].x = EMPTYCELL;
                                                entities[id]->coord[i].y = EMPTYCELL;
                                        }
                                }
                                board[pos.y][col] = EMPTYCELL;
                        }

                        lines_erased++;
                }
        }

        if (lines_erased > 0) {
                size_t row_size = sizeof(board[0]); // Size of one row
                memmove(board + lines_erased, board, (BOARDROWS - lines_erased) * row_size);

                for (int row = 0; row < lines_erased; row++) {
                        for (int col = 0; col < BOARDCOLS; col++) {
                                board[row][col] = EMPTYCELL;
                        }
                }
                for (int i = 0; i < entities_len; i++) {
                        add_xy_to_tetromino_secure(entities[i], 0, lines_erased);
                }
        }
}

void print_board()
{
        for (int row = 0; row < BOARDROWS; row++) {
                printf("%c", '\n');
                for (int col = 0; col < BOARDCOLS; col++) {
                        printf("%d\t", board[row][col]);
                }
        }
        printf("\nEND OF PRINT\n");
}

void swap_row(int elements, int rowa[elements], int rowb[elements])
{
        int temp[elements];

        memcpy(temp, rowa, elements * sizeof(int));

        memcpy(rowa, rowb, elements * sizeof(int));
        memcpy(rowb, temp, elements * sizeof(int));
}


void add_xy_to_tetromino_secure(Tetromino *t, int x, int y)
{

        for (int i = 0; i < 4; i++) {
                if (t->coord[i].x == EMPTYCELL || t->coord[i].y == EMPTYCELL)
                        continue;

                if (t->coord[i].y < BOARDROWS -1) {
                        t->coord[i].y += y;
                        t->pivot.y += y;
                }

                if (t->coord[i].x < BOARDCOLS -1) {
                        t->coord[i].x += x;
                        t->pivot.x += x;
                }
        }
}
