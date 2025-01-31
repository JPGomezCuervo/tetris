#include "tetris.h"

float lock_interval = 0.5f; 
float lock_timer = 0.0f; 
float fall_interval = 1.0f;
float fall_timer = 0.0f;
int64_t allocated_size = 1024;

int main(void) {
        init_board();
        srand(time(NULL));
        player.hold = NULL;
        player.can_hold = true;

        // TODO: Manage allocated size and resize it! this should be in a function
        entities = malloc(sizeof(Tetromino *) * allocated_size);

        InitWindow(SCREENWIDTH, SCREENHEIGHT, "tetrix!");
        SetTargetFPS(60);

        player.game_state = GENERATION;
        while (!WindowShouldClose()) {
                fall_timer += GetFrameTime();
                lock_timer += GetFrameTime();

                switch(player.game_state) {
                        case GENERATION:
                                Tetromino *t = get_tetromino();
                                entities[entities_len - 1] = t;
                                player.tetromino = entities[entities_len - 1];

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

                /*print_board();*/
                refresh_board();
                BeginDrawing();
                ClearBackground(BLACK);
                EndDrawing();
        }

        CloseWindow();
        cleanup();

        return 0;
}
