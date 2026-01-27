#include "raylib.h"

int main(){
    int window_width{800};
    int window_height{450};

    InitWindow(window_width, window_height, "Nightrunner");
    
    SetTargetFPS(60);

    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(WHITE);
        EndDrawing();
    }

    CloseWindow();

    return 0;

}