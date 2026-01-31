#include "raylib.h"

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
        for (int i{0}; i < size_of_nebulae; i++){
            nebulae[i].pos.x += neb_vel * dT;
        }
        
        // update player position
        player_data.pos.y += velocity * dT;

        // update player animation frame
        if (!is_in_air){
            player_data = updateAnimData(player_data, dT, 5);
        }

        // update nebula animation frame
        for (int i{0}; i < size_of_nebulae; i++){
            nebulae[i] = updateAnimData(nebulae[i], dT, 7);
        }
        
        
        // draw nebula
        for (int i{0}; i < size_of_nebulae; i++){
            DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
        }
        
        // draw player
        DrawTextureRec(player, player_data.rec, player_data.pos, WHITE);

        EndDrawing();
    }
    UnloadTexture(player);
    UnloadTexture(nebula);
    CloseWindow();

    return 0;

}