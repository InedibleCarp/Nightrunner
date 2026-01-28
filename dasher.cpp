#include "raylib.h"

int main(){
    const int window_width{512};
    const int window_height{380};

    InitWindow(window_width, window_height, "Nightrunner");

    // acceleration due to gravity in (prixels/frame)/frame
    const int gravity{1};

    // rectangle dimensions
    const int width{50};
    const int height{80};
    
    // is the rectangle in the air
    bool is_in_air{false};

    // jump velocity
    const int jump_vel{-22};


    int posY{window_height - height};
    int velocity{0};
    
    SetTargetFPS(60);

    // main game loop
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(WHITE);

        // perform ground check
        if (posY >= window_height - height){
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
        posY += velocity;

        DrawRectangle(window_width / 2, posY, width, height, BLUE);

        EndDrawing();
    }

    CloseWindow();

    return 0;

}