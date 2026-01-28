#include "raylib.h"

int main(){
    const int window_width{512};
    const int window_height{380};

    InitWindow(window_width, window_height, "Nightrunner");

    // acceleration due to gravity in (prixels/second)/second
    const int gravity{1000};

    // player sprite variables
    Texture2D player = LoadTexture("textures/scarfy.png");
    Rectangle player_rec;
    player_rec.width = player.width / 6;
    player_rec.height = player.height;
    player_rec.x = 0;
    player_rec.y = 0;
    Vector2 player_pos;
    player_pos.x = (window_width / 2) - (player_rec.width / 2);
    player_pos.y = window_height - player_rec.height;

    // animation frame
    int frame{0};
    // amount of time before updating animation frame
    const float updateTime{1.0 / 12.0};
    float runningTime{};

    // is the rectangle in the air
    bool is_in_air{false};

    // jump velocity (pixels/second)
    const int jump_vel{-600};

    int velocity{};
    
    SetTargetFPS(60);

    // -------------------main game loop-------------------------------
    while(!WindowShouldClose()){
        
        // delta time (time since last frame)
        const float dT{GetFrameTime()};
        runningTime += dT;

        BeginDrawing();
        ClearBackground(WHITE);

        // perform ground check
        if (player_pos.y >= window_height - player_rec.height){
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
        
        // update position
        player_pos.y += velocity * dT;

        // update animation frame
        if (runningTime >= updateTime){
            runningTime = 0.0;
            player_rec.x = frame * player_rec.width;
            frame++;
            if (frame > 5){
                frame = 0;
            }
        }
        
        DrawTextureRec(player, player_rec, player_pos, WHITE);

        EndDrawing();
    }
    UnloadTexture(player);
    CloseWindow();

    return 0;

}