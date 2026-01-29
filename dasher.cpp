#include "raylib.h"

struct AnimData{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float update_time;
    float running_time;
};

int main(){
    const int window_width{512};
    const int window_height{380};

    InitWindow(window_width, window_height, "Nightrunner");

    // acceleration due to gravity in (prixels/second)/second
    const int gravity{1000};

    // nebula obsticle variables
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    AnimData neb_data{
        {0.0, 0.0, float((nebula.width / 8.0)), float((nebula.height / 8.0))}, // Rectangle rec
        {window_width, (window_height - neb_data.rec.height)}, // Vector 2 pos
        0, (1.0 / 12.0), 0 // frame, update_time, running_time
    };

    AnimData neb2_data{
        {0.0, 0.0, float((nebula.width / 8.0)), float((nebula.height / 8.0))}, // Rectangle rec
        {window_width + 300, (window_height - neb_data.rec.height)}, // Vector 2 pos
        0, (1.0 / 16.0), 0 // frame, update_time, running_time
    };
    
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
    
    SetTargetFPS(60);

    // -------------------main game loop-------------------------------
    while(!WindowShouldClose()){
        
        // delta time (time since last frame)
        const float dT{GetFrameTime()};
        player_data.running_time += dT;
        neb_data.running_time += dT;
        neb2_data.running_time += dT;

        BeginDrawing();
        ClearBackground(WHITE);

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
        neb_data.pos.x += neb_vel * dT;
        neb2_data.pos.x += neb_vel * dT;
        
        // update player position
        player_data.pos.y += velocity * dT;

        // update player animation frame
        if (player_data.running_time >= player_data.update_time && !is_in_air){
            player_data.running_time = 0.0;
            player_data.rec.x = player_data.frame * player_data.rec.width;
            player_data.frame++;
            if (player_data.frame > 5){
                player_data.frame = 0;
            }
        }

        // update nebula animation frame
        if (neb_data.running_time >= neb_data.update_time){
            neb_data.running_time = 0.0;
            neb_data.rec.x = neb_data.frame * neb_data.rec.width;
            neb_data.frame++;
            if (neb_data.frame > 7){
                neb_data.frame = 0;
            }
        }

        // update nebula2 animation frame
        if (neb2_data.running_time >= neb2_data.update_time){
            neb2_data.running_time = 0.0;
            neb2_data.rec.x = neb2_data.frame * neb2_data.rec.width;
            neb2_data.frame++;
            if (neb2_data.frame > 7){
                neb2_data.frame = 0;
            }
        }
        
        // draw nebula
        DrawTextureRec(nebula, neb_data.rec, neb_data.pos, WHITE);
        DrawTextureRec(nebula, neb2_data.rec, neb2_data.pos, RED);
        
        // draw player
        DrawTextureRec(player, player_data.rec, player_data.pos, WHITE);

        EndDrawing();
    }
    UnloadTexture(player);
    UnloadTexture(nebula);
    CloseWindow();

    return 0;

}