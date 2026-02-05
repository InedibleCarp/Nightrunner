#include "raylib.h"

// Game states
enum GameState {
    MENU,
    PLAYING,
    GAME_OVER_WIN,
    GAME_OVER_LOSE
};

struct AnimData{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float update_time;
    float running_time;
};

AnimData updateAnimData(AnimData data, float delta_time, int max_frame){
    data.running_time += delta_time;
    if (data.running_time >= data.update_time){
        data.running_time = 0.0;
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > max_frame){
            data.frame = 0;
        }
    }
    return data;
}

int main(){
    const int window_width{512};
    const int window_height{380};

    InitWindow(window_width, window_height, "Nightrunner");

    // acceleration due to gravity in (prixels/second)/second
    const int gravity{1000};

    // nebula obsticle variables
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    const int size_of_nebulae = 10;
    AnimData nebulae[size_of_nebulae]{};
    for (int i{0}; i < size_of_nebulae; i++){
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = float((nebula.width / 8.0));
        nebulae[i].rec.height = float((nebula.height / 8.0));
        nebulae[i].pos.x = window_width + (i * 300);
        nebulae[i].pos.y = (window_height - nebulae[i].rec.height);
        nebulae[i].frame = 0;
        nebulae[i].running_time = 0.0;
        nebulae[i].update_time = (1.0 / 16.0);
    }

    float finish_line{nebulae[size_of_nebulae - 1].pos.x};

    // player sprite variables
    Texture2D player = LoadTexture("textures/scarfy.png");
    AnimData player_data;
    player_data.rec.width = player.width / 6;
    player_data.rec.height = player.height;
    player_data.rec.x = 0;
    player_data.rec.y = 0;
    player_data.pos.x = (window_width / 2) - (player_data.rec.width / 2);
    player_data.pos.y = window_height - player_data.rec.height;
    player_data.frame = 0;
    player_data.update_time = 1.0 / 12.0;
    player_data.running_time = 0.0;

    // is the rectangle in the air
    bool is_in_air{false};

    // jump velocity (pixels/second)
    const int jump_vel{-600};
    // nebula x velocit (pixels/second)
    int neb_vel{-200};

    int velocity{};

    Texture2D background = LoadTexture("textures/far-buildings.png");
    float bgX{};
    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float mgX{};
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float fgX{};
    
    // Game state - start at menu
    GameState game_state{MENU};

    // Score based on distance traveled
    int score{0};
    float distance_accumulator{0.0f};
    const float distance_per_point{50.0f};  // pixels traveled per score point

    SetTargetFPS(60);

    // -------------------main game loop-------------------------------
    while(!WindowShouldClose()){
        
        // delta time (time since last frame)
        const float dT{GetFrameTime()};

        BeginDrawing();
        ClearBackground(WHITE);

        // scroll background (only during menu and playing)
        if (game_state == MENU || game_state == PLAYING){
            bgX -= 20 * dT;
            if (bgX <= -background.width * 2){
                bgX = 0.0;
            }

            mgX -= 40 * dT;
            if (mgX <= -midground.width * 2){
                mgX = 0.0;
            }

            fgX -= 80 * dT;
            if (fgX <= -foreground.width * 2){
                fgX = 0.0;
            }
        }

        // draw background
        Vector2 bg1Pos{bgX, 0.0};
        Vector2 bg2Pos{bgX + background.width * 2, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(background, bg2Pos, 0.0, 2.0, WHITE);

        Vector2 mg1Pos{mgX, 0.0};
        Vector2 mg2Pos{mgX + midground.width * 2, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(midground, mg2Pos, 0.0, 2.0, WHITE);

        Vector2 fg1Pos{fgX, 0.0};
        Vector2 fg2Pos{fgX + foreground.width * 2, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 2.0, WHITE);
        DrawTextureEx(foreground, fg2Pos, 0.0, 2.0, WHITE);

        // Handle game states
        switch (game_state) {
            case MENU:
            {
                // Draw title and prompt
                DrawText("NIGHTRUNNER", window_width / 2 - MeasureText("NIGHTRUNNER", 50) / 2, window_height / 3, 50, WHITE);
                DrawText("Press SPACE to start", window_width / 2 - MeasureText("Press SPACE to start", 20) / 2, window_height / 2 + 20, 20, LIGHTGRAY);

                // Draw player on menu screen (idle animation)
                player_data = updateAnimData(player_data, dT, 5);
                DrawTextureRec(player, player_data.rec, player_data.pos, WHITE);

                // Start game when SPACE is pressed
                if (IsKeyPressed(KEY_SPACE)) {
                    game_state = PLAYING;
                }
                break;
            }

            case PLAYING:
            {
                // update score based on distance traveled
                distance_accumulator += -neb_vel * dT;
                while (distance_accumulator >= distance_per_point) {
                    score++;
                    distance_accumulator -= distance_per_point;
                }

                // perform ground check
                if (player_data.pos.y >= window_height - player_data.rec.height){
                    // rectangle is on the ground
                    velocity = 0;
                    is_in_air = false;
                } else {
                    // rectangle is in the air
                    velocity += gravity * dT;
                    is_in_air = true;
                }

                // jump check
                if (IsKeyPressed(KEY_SPACE) && !is_in_air){
                    velocity += jump_vel;
                }

                // update nebula position
                for (int i{0}; i < size_of_nebulae; i++){
                    nebulae[i].pos.x += neb_vel * dT;
                }

                // update player position
                player_data.pos.y += velocity * dT;

                // update finish line position
                finish_line += neb_vel * dT;

                // update player animation frame
                if (!is_in_air){
                    player_data = updateAnimData(player_data, dT, 5);
                }

                // update nebula animation frame
                for (int i{0}; i < size_of_nebulae; i++){
                    nebulae[i] = updateAnimData(nebulae[i], dT, 7);
                }

                // collision detection
                for (AnimData nebula : nebulae){
                    float pad{50};
                    Rectangle neb_rec{
                        nebula.pos.x + pad,
                        nebula.pos.y + pad,
                        nebula.rec.width - 2 * pad,
                        nebula.rec.height - 2 * pad
                    };
                    Rectangle player_rec{
                        player_data.pos.x,
                        player_data.pos.y,
                        player_data.rec.width,
                        player_data.rec.height
                    };
                    if (CheckCollisionRecs(neb_rec, player_rec)){
                        game_state = GAME_OVER_LOSE;
                    }
                }

                // check win condition
                if (player_data.pos.x >= finish_line){
                    game_state = GAME_OVER_WIN;
                }

                // draw nebulae
                for (int i{0}; i < size_of_nebulae; i++){
                    DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
                }

                // draw player
                DrawTextureRec(player, player_data.rec, player_data.pos, WHITE);

                // draw score HUD
                DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);
                break;
            }

            case GAME_OVER_WIN:
            {
                // Draw win screen
                DrawText("You Win!", window_width / 2 - MeasureText("You Win!", 40) / 2, window_height / 2 - 40, 40, GREEN);
                DrawText(TextFormat("Final Score: %d", score), window_width / 2 - MeasureText(TextFormat("Final Score: %d", score), 24) / 2, window_height / 2 + 10, 24, WHITE);
                DrawText("Press SPACE to play again", window_width / 2 - MeasureText("Press SPACE to play again", 20) / 2, window_height / 2 + 50, 20, LIGHTGRAY);
                DrawText("Press M for menu", window_width / 2 - MeasureText("Press M for menu", 16) / 2, window_height / 2 + 80, 16, LIGHTGRAY);

                // Handle restart
                if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_M)){
                    // reset player position
                    player_data.pos.y = window_height - player_data.rec.height;
                    player_data.frame = 0;
                    player_data.running_time = 0.0;
                    velocity = 0;
                    is_in_air = false;

                    // reset nebulae positions
                    for (int i{0}; i < size_of_nebulae; i++){
                        nebulae[i].pos.x = window_width + (i * 300);
                        nebulae[i].frame = 0;
                        nebulae[i].running_time = 0.0;
                    }

                    // reset finish line
                    finish_line = nebulae[size_of_nebulae - 1].pos.x;

                    // reset background positions
                    bgX = 0.0;
                    mgX = 0.0;
                    fgX = 0.0;

                    // reset score
                    score = 0;
                    distance_accumulator = 0.0f;

                    // transition to appropriate state
                    game_state = IsKeyPressed(KEY_M) ? MENU : PLAYING;
                }
                break;
            }

            case GAME_OVER_LOSE:
            {
                // Draw game over screen
                DrawText("Game Over!", window_width / 2 - MeasureText("Game Over!", 40) / 2, window_height / 2 - 40, 40, RED);
                DrawText(TextFormat("Final Score: %d", score), window_width / 2 - MeasureText(TextFormat("Final Score: %d", score), 24) / 2, window_height / 2 + 10, 24, WHITE);
                DrawText("Press SPACE to try again", window_width / 2 - MeasureText("Press SPACE to try again", 20) / 2, window_height / 2 + 50, 20, LIGHTGRAY);
                DrawText("Press M for menu", window_width / 2 - MeasureText("Press M for menu", 16) / 2, window_height / 2 + 80, 16, LIGHTGRAY);

                // Handle restart
                if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_M)){
                    // reset player position
                    player_data.pos.y = window_height - player_data.rec.height;
                    player_data.frame = 0;
                    player_data.running_time = 0.0;
                    velocity = 0;
                    is_in_air = false;

                    // reset nebulae positions
                    for (int i{0}; i < size_of_nebulae; i++){
                        nebulae[i].pos.x = window_width + (i * 300);
                        nebulae[i].frame = 0;
                        nebulae[i].running_time = 0.0;
                    }

                    // reset finish line
                    finish_line = nebulae[size_of_nebulae - 1].pos.x;

                    // reset background positions
                    bgX = 0.0;
                    mgX = 0.0;
                    fgX = 0.0;

                    // reset score
                    score = 0;
                    distance_accumulator = 0.0f;

                    // transition to appropriate state
                    game_state = IsKeyPressed(KEY_M) ? MENU : PLAYING;
                }
                break;
            }
        }

        EndDrawing();
    }
    UnloadTexture(player);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();

    return 0;

}