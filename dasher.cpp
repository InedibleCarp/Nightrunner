#include "raylib.h"

int main(){
    const int window_width{512};
    const int window_height{380};

    InitWindow(window_width, window_height, "Nightrunner");
    
    SetTargetFPS(60);

    // main game loop
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(WHITE);
        EndDrawing();
    }

    CloseWindow();

    return 0;

}