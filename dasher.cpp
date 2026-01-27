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
        } else {
            // rectangle is in the air
            velocity += gravity;
        }
        
        if (IsKeyPressed(KEY_SPACE)){
            velocity -= 10;
        }
        
        // update position
        posY += velocity;

        DrawRectangle(window_width / 2, posY, width, height, BLUE);

        EndDrawing();
    }

    CloseWindow();

    return 0;

}