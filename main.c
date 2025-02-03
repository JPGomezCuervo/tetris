#include "tetris.h"

float lock_interval = 0.5f; 
float lock_timer = 0.0f; 
float fall_interval = 1.0f;
float fall_timer = 0.0f;
int allocated_size = 1000000;

int main(void) {
        Arena arena_tetro = create_arena(allocated_size);
        Arena arena_entities = create_arena(allocated_size);
        /*entities = (Tetromino **) arena_alloc(&arena_entities, sizeof(Tetromino*) * (allocated_size/sizeof(Tetromino)));*/
        entities = malloc(sizeof(Tetromino *) * allocated_size);
        init_board();
        srand(time(NULL));
        player.hold = NULL;
        player.can_hold = true;
        player.ghost = create_tetromino(&arena_tetro);
        player.ghost->is_ghost = true;

        // TODO: Manage allocated size and resize it! this should be in a function

        InitWindow(SCREENWIDTH, SCREENHEIGHT, "tetrix!");
        SetTargetFPS(60);

        player.game_state = GENERATION;
        while (!WindowShouldClose()) {
                fall_timer += GetFrameTime();
                lock_timer += GetFrameTime();

                switch(player.game_state) {
                        case GENERATION:
                                Tetromino *t = get_tetromino(&arena_entities);
                                entities[entities_len - 1] = t;
                                player.tetromino = entities[entities_len - 1];
                                memcpy(player.ghost, player.tetromino, sizeof(*player.tetromino));
                                player.ghost->is_ghost = true;

                                player.game_state = FALLING;
                                break;
                        case FALLING:
                                if (fall_timer >= fall_interval) {
                                        update_tetromino_coordinates(player.tetromino, DOWN);
                                        fall_timer = 0.0f;
                                }
                                break;
                        case LOCK:
                                if (lock_timer >= lock_interval) {
                                        player.game_state = COMPLETION;
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
                                clear_lines();
                                player.game_state = GENERATION;
                                player.can_hold = true;
                                break;
                        default:
                }

                move(player.tetromino);

                BeginDrawing();
                refresh_board();
                show_ghost();
                ClearBackground(BLACK);
                EndDrawing();
        }

        CloseWindow();
        destroy_arena(&arena_tetro);
        /*free(entities);*/
        destroy_arena(&arena_entities);

        return 0;
}
