#include "raylib.h"
#include <cmath>
#include <cstdio>
#include <cstring>

// Game states
enum GameState {
    MENU,
    PLAYING,
    GAME_OVER_WIN,
    GAME_OVER_LOSE,
    LEADERBOARD
};

// Leaderboard
const int LEADERBOARD_SIZE = 5;

struct ScoreEntry {
    int score;
};

struct Leaderboard {
    char magic[4];  // "NR01"
    ScoreEntry entries[LEADERBOARD_SIZE];
};

void init_leaderboard(Leaderboard &lb) {
    memcpy(lb.magic, "NR01", 4);
    for (int i = 0; i < LEADERBOARD_SIZE; i++) {
        lb.entries[i].score = 0;
    }
}

bool load_leaderboard(const char *path, Leaderboard &lb) {
    FILE *f = fopen(path, "rb");
    if (!f) return false;
    size_t read = fread(&lb, sizeof(Leaderboard), 1, f);
    fclose(f);
    if (read != 1 || memcmp(lb.magic, "NR01", 4) != 0) return false;
    return true;
}

void save_leaderboard(const char *path, const Leaderboard &lb) {
    FILE *f = fopen(path, "wb");
    if (!f) return;
    fwrite(&lb, sizeof(Leaderboard), 1, f);
    fclose(f);
}

// Returns the rank (0-4) if the score made the board, or -1 if not.
int insert_score(Leaderboard &lb, int score) {
    // Find insertion point (sorted descending)
    int pos = -1;
    for (int i = 0; i < LEADERBOARD_SIZE; i++) {
        if (score > lb.entries[i].score) {
            pos = i;
            break;
        }
    }
    if (pos < 0) return -1;
    // Shift lower scores down
    for (int i = LEADERBOARD_SIZE - 1; i > pos; i--) {
        lb.entries[i] = lb.entries[i - 1];
    }
    lb.entries[pos].score = score;
    return pos;
}

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
    const int window_width{768};
    const int window_height{480};

    InitWindow(window_width, window_height, "Nightrunner");
    InitAudioDevice();

    // acceleration due to gravity in (prixels/second)/second
    const int gravity{1000};

    // difficulty scaling constants
    const float base_neb_vel{-200.0f};   // starting obstacle speed (px/s)
    const float vel_scale{50.0f};        // log curve scaling factor for speed ramp
    const float max_neb_vel{-600.0f};    // speed cap (fastest obstacles can move)
    const float base_max_gap{600.0f};    // starting max gap between obstacles (px)
    const float min_gap{300.0f};         // minimum gap (must always be jumpable)
    const float gap_shrink_scale{20.0f}; // log curve scaling factor for gap tightening
    const float min_max_gap{250.0f};     // floor for max gap (never tighter than this)

    // nebula obsticle variables
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");

    const int size_of_nebulae = 10;
    AnimData nebulae[size_of_nebulae]{};
    for (int i{0}; i < size_of_nebulae; i++){
        nebulae[i].rec.x = 0.0;
        nebulae[i].rec.y = 0.0;
        nebulae[i].rec.width = float((nebula.width / 8.0));
        nebulae[i].rec.height = float((nebula.height / 8.0));
        nebulae[i].pos.x = window_width + (i * 500);
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
    bool can_double_jump{true};  // reset when landing, used when jumping in air

    // jump velocity (pixels/second)
    const int jump_vel{-600};
    // nebula x velocity (pixels/second) - derived from score each frame
    float neb_vel{base_neb_vel};

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

    // Lives
    int lives{3};
    float invincibility_timer{0.0f};
    const float invincibility_duration{1.1f};

    // Leaderboard
    const char *scores_path = "scores.dat";
    Leaderboard leaderboard;
    init_leaderboard(leaderboard);
    load_leaderboard(scores_path, leaderboard);
    int last_rank{-1};  // rank of last inserted score (-1 = didn't place)

    SetTargetFPS(60);

    Music menu_music = LoadMusicStream("audio/menu_theme.mp3");
    Music game_music = LoadMusicStream("audio/gameplay_theme.mp3");

    // -------------------main game loop-------------------------------
    while(!WindowShouldClose()){
        
        // delta time (time since last frame)
        const float dT{GetFrameTime()};

        // Menu music: play during MENU state, stop otherwise
        if (game_state == MENU) {
            if (!IsMusicStreamPlaying(menu_music)) {
                PlayMusicStream(menu_music);
            }
            UpdateMusicStream(menu_music);
        } else {
            if (IsMusicStreamPlaying(menu_music)) {
                StopMusicStream(menu_music);
            }
        }

        // Game music: play during PLAYING state, stop otherwise
        if (game_state == PLAYING) {
            if (!IsMusicStreamPlaying(game_music)) {
                PlayMusicStream(game_music);
            }
            UpdateMusicStream(game_music);
        } else {
            if (IsMusicStreamPlaying(game_music)) {
                StopMusicStream(game_music);
            }
        }

        BeginDrawing();
        ClearBackground(WHITE);

        // scroll background (during menu, playing, and leaderboard)
        if (game_state == MENU || game_state == PLAYING || game_state == LEADERBOARD){
            bgX -= 20 * dT;
            if (bgX <= -background.width * 3){
                bgX = 0.0;
            }

            mgX -= 40 * dT;
            if (mgX <= -midground.width * 3){
                mgX = 0.0;
            }

            fgX -= 80 * dT;
            if (fgX <= -foreground.width * 3){
                fgX = 0.0;
            }
        }

        // draw background
        Vector2 bg1Pos{bgX, 0.0};
        Vector2 bg2Pos{bgX + background.width * 3, 0.0};
        DrawTextureEx(background, bg1Pos, 0.0, 3.0, WHITE);
        DrawTextureEx(background, bg2Pos, 0.0, 3.0, WHITE);

        Vector2 mg1Pos{mgX, 0.0};
        Vector2 mg2Pos{mgX + midground.width * 3, 0.0};
        DrawTextureEx(midground, mg1Pos, 0.0, 3.0, WHITE);
        DrawTextureEx(midground, mg2Pos, 0.0, 3.0, WHITE);

        Vector2 fg1Pos{fgX, 0.0};
        Vector2 fg2Pos{fgX + foreground.width * 3, 0.0};
        DrawTextureEx(foreground, fg1Pos, 0.0, 3.0, WHITE);
        DrawTextureEx(foreground, fg2Pos, 0.0, 3.0, WHITE);

        // Handle game states
        switch (game_state) {
            case MENU:
            {
                // Draw title and prompt
                DrawText("NIGHTRUNNER", window_width / 2 - MeasureText("NIGHTRUNNER", 50) / 2, window_height / 3, 50, WHITE);
                DrawText("Press SPACE to start", window_width / 2 - MeasureText("Press SPACE to start", 20) / 2, window_height / 2 + 20, 20, LIGHTGRAY);
                DrawText("Press L for Leaderboard", window_width / 2 - MeasureText("Press L for Leaderboard", 16) / 2, window_height / 2 + 50, 16, LIGHTGRAY);

                // Draw player on menu screen (idle animation)
                player_data = updateAnimData(player_data, dT, 5);
                DrawTextureRec(player, player_data.rec, player_data.pos, WHITE);

                // Start game when SPACE is pressed
                if (IsKeyPressed(KEY_SPACE)) {
                    game_state = PLAYING;
                }
                if (IsKeyPressed(KEY_L)) {
                    last_rank = -1;
                    game_state = LEADERBOARD;
                }
                break;
            }

            case PLAYING:
            {
                // scale difficulty based on score (logarithmic curve)
                neb_vel = base_neb_vel - vel_scale * logf(1.0f + score);
                if (neb_vel < max_neb_vel) neb_vel = max_neb_vel;

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
                    can_double_jump = true;  // reset double jump when landing
                } else {
                    // rectangle is in the air
                    velocity += gravity * dT;
                    is_in_air = true;
                }

                // jump check
                if (IsKeyPressed(KEY_SPACE)){
                    if (!is_in_air){
                        // first jump from ground
                        velocity += jump_vel;
                    } else if (can_double_jump){
                        // double jump in air
                        velocity = jump_vel;  // reset velocity for consistent jump height
                        can_double_jump = false;
                    }
                }

                // update nebula position
                for (int i{0}; i < size_of_nebulae; i++){
                    nebulae[i].pos.x += neb_vel * dT;
                }

                // recycle nebulae that scroll off-screen left
                for (int i{0}; i < size_of_nebulae; i++){
                    if (nebulae[i].pos.x < -nebulae[i].rec.width){
                        // find the rightmost nebula
                        float rightmost_x = 0.0f;
                        for (int j{0}; j < size_of_nebulae; j++){
                            if (nebulae[j].pos.x > rightmost_x){
                                rightmost_x = nebulae[j].pos.x;
                            }
                        }
                        // compute randomized gap that tightens with score
                        float current_max_gap = base_max_gap - gap_shrink_scale * logf(1.0f + score);
                        if (current_max_gap < min_max_gap) current_max_gap = min_max_gap;
                        float gap = (float)GetRandomValue((int)min_gap, (int)current_max_gap);
                        nebulae[i].pos.x = rightmost_x + gap;
                    }
                }

                // update player position
                player_data.pos.y += velocity * dT;

                // clamp player to screen bounds
                if (player_data.pos.y < 0){
                    player_data.pos.y = 0;
                    velocity = 0;
                }

                // tick down invincibility
                if (invincibility_timer > 0.0f) {
                    invincibility_timer -= dT;
                }

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
                    if (CheckCollisionRecs(neb_rec, player_rec) && invincibility_timer <= 0.0f){
                        lives--;
                        if (lives <= 0) {
                            game_state = GAME_OVER_LOSE;
                        } else {
                            player_data.pos.y = window_height - player_data.rec.height;
                            velocity = 0;
                            is_in_air = false;
                            can_double_jump = true;
                            invincibility_timer = invincibility_duration;
                        }
                    }
                }

                // draw nebulae
                for (int i{0}; i < size_of_nebulae; i++){
                    DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
                }

                // draw player (flash while invincible)
                if (invincibility_timer <= 0.0f || (int)(invincibility_timer * 10) % 2 == 0) {
                    DrawTextureRec(player, player_data.rec, player_data.pos, WHITE);
                }

                // draw score HUD
                DrawText(TextFormat("Score: %d", score), 10, 10, 20, WHITE);
                DrawText(TextFormat("Lives: %d", lives), 10, 35, 20, WHITE);
                break;
            }

            case GAME_OVER_WIN:
            {
                // Reserved for future high score celebration
                break;
            }

            case GAME_OVER_LOSE:
            {
                // Save score to leaderboard on first frame of game over
                static bool score_saved = false;
                if (!score_saved) {
                    last_rank = insert_score(leaderboard, score);
                    if (last_rank >= 0) {
                        save_leaderboard(scores_path, leaderboard);
                    }
                    score_saved = true;
                }

                // Draw panel behind text
                DrawRectangle(window_width / 2 - 200, window_height / 2 - 70, 400, 200, Color{0, 0, 0, 180});

                // Draw game over screen
                DrawText("Game Over!", window_width / 2 - MeasureText("Game Over!", 40) / 2, window_height / 2 - 50, 40, RED);
                DrawText(TextFormat("Final Score: %d", score), window_width / 2 - MeasureText(TextFormat("Final Score: %d", score), 24) / 2, window_height / 2, 24, WHITE);
                if (last_rank >= 0) {
                    const char *rank_text = TextFormat("New high score! Rank #%d", last_rank + 1);
                    DrawText(rank_text, window_width / 2 - MeasureText(rank_text, 20) / 2, window_height / 2 + 30, 20, GOLD);
                }
                DrawText("Press SPACE to try again", window_width / 2 - MeasureText("Press SPACE to try again", 20) / 2, window_height / 2 + 60, 20, LIGHTGRAY);
                DrawText("Press L for Leaderboard  |  Press M for Menu", window_width / 2 - MeasureText("Press L for Leaderboard  |  Press M for Menu", 16) / 2, window_height / 2 + 90, 16, LIGHTGRAY);

                // Handle input
                if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_M) || IsKeyPressed(KEY_L)){
                    // reset player position
                    player_data.pos.y = window_height - player_data.rec.height;
                    player_data.frame = 0;
                    player_data.running_time = 0.0;
                    velocity = 0;
                    is_in_air = false;
                    can_double_jump = true;

                    // reset nebulae positions
                    for (int i{0}; i < size_of_nebulae; i++){
                        nebulae[i].pos.x = window_width + (i * 500);
                        nebulae[i].frame = 0;
                        nebulae[i].running_time = 0.0;
                    }

                    // reset difficulty
                    neb_vel = base_neb_vel;

                    // reset background positions
                    bgX = 0.0;
                    mgX = 0.0;
                    fgX = 0.0;

                    // reset score
                    score = 0;
                    distance_accumulator = 0.0f;
                    score_saved = false;

                    // reset lives
                    lives = 3;
                    invincibility_timer = 0.0f;

                    // transition to appropriate state
                    if (IsKeyPressed(KEY_L)) {
                        game_state = LEADERBOARD;
                    } else if (IsKeyPressed(KEY_M)) {
                        game_state = MENU;
                    } else {
                        game_state = PLAYING;
                    }
                }
                break;
            }

            case LEADERBOARD:
            {
                // Draw leaderboard panel
                int panel_w = 400;
                int panel_h = 280;
                int panel_x = window_width / 2 - panel_w / 2;
                int panel_y = window_height / 2 - panel_h / 2;
                DrawRectangle(panel_x, panel_y, panel_w, panel_h, Color{0, 0, 0, 180});

                DrawText("LEADERBOARD", window_width / 2 - MeasureText("LEADERBOARD", 36) / 2, panel_y + 15, 36, GOLD);

                // Draw each entry
                for (int i = 0; i < LEADERBOARD_SIZE; i++) {
                    int y = panel_y + 65 + i * 35;
                    Color row_color = (i == last_rank) ? GOLD : WHITE;
                    const char *entry_text;
                    if (leaderboard.entries[i].score > 0) {
                        entry_text = TextFormat("#%d    %d", i + 1, leaderboard.entries[i].score);
                    } else {
                        entry_text = TextFormat("#%d    ---", i + 1);
                    }
                    DrawText(entry_text, window_width / 2 - MeasureText(entry_text, 24) / 2, y, 24, row_color);
                }

                DrawText("Press M for Menu", window_width / 2 - MeasureText("Press M for Menu", 16) / 2, panel_y + panel_h - 30, 16, LIGHTGRAY);

                if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_SPACE)) {
                    game_state = MENU;
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
    UnloadMusicStream(menu_music);
    UnloadMusicStream(game_music);
    CloseAudioDevice();
    CloseWindow();

    return 0;

}