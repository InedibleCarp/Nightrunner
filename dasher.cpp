#include "raylib.h"

int main(){
    const int window_width{512};
    const int window_height{380};

    InitWindow(window_width, window_height, "Nightrunner");

    // acceleration due to gravity in (prixels/frame)/frame
    const int gravity{1};

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

    
    // is the rectangle in the air
    bool is_in_air{false};

    // jump velocity
    const int jump_vel{-22};

    int velocity{0};
    
    SetTargetFPS(60);

    // main game loop
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(WHITE);

        // perform ground check
        if (player_pos.y >= window_height - player_rec.height){
            // rectangle is on the ground
            velocity = 0;
            is_in_air = false;
        } else {
            // rectangle is in the air
            velocity += gravity;
            is_in_air = true;
        }
        
        // jump check
        if (IsKeyPressed(KEY_SPACE) && !is_in_air){
            velocity += jump_vel;
        }
        
        // update position
        player_pos.y += velocity;

        DrawTextureRec(player, player_rec, player_pos, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;

}