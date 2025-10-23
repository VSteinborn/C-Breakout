#include "raylib.h"
#include "constants.h"
#include "util.h"


int main(void)
{
    // Window
    // -------
    InitWindow(SCREEN_WIDTH , SCREEN_HEIGHT, "C Breakout");
    SetTargetFPS(60);
    // Game State
    // -------
    enum State game_state = PLAYING;
    // Peddal
    // -------
    Paddle paddle;
    initialize_paddle(&paddle);
    // Ball
    // -------
    Ball ball;
    initialize_ball(&ball);
    // Blocks
    // -------
    struct Block blocks[BLOCK_COUNT];
    initialize_blocks(blocks);
    // Score
    // -------
    ScoreTotal score;
    initialize_score(&score);
    // Score Popups
    // -------
    struct ScorePopup score_popups[MAX_SCORE_POPUPS];
    initialize_popups(score_popups);

    // Audio
    InitAudioDevice();      // Initialize audio device
    Sound paddle_sfx = LoadSound("./assets/paddle_bounce.ogg");
    Sound block_sfx = LoadSound("./assets/block_bounce.ogg");

    // Game Loop
    // -------
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Move paddle
        if (game_state == PLAYING)
        {
            if (IsKeyDown(KEY_RIGHT)) paddle.rec.x += paddle.speed;
            if (IsKeyDown(KEY_LEFT)) paddle.rec.x -= paddle.speed;
        }
        // Bounce ball on window
        bounce_window(&ball);
        // Bounce ball on pedal
        bounce_paddle(&ball, &paddle, &paddle_sfx);
        // Bounce ball on block
        bounce_block(&ball, blocks, score_popups, &score, &block_sfx);
        // Update popups
        update_popups(score_popups);
        // Update score animation
        update_score_animation(&score);
        // Update ball position
        if (game_state == PLAYING)
        {
            ball_position_update(&ball.pos, &ball.vel);
        }
        // Check Lose State
        if (check_lose(&ball, &paddle))
        {
            game_state = LOSE;
        }
        // Check Win State
        if (check_win(blocks))
        {
            game_state = WIN;
        }
        // Draw Game
        // -------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            // Ball
            // -------
            DrawCircleV(ball.pos, ball.rad, ball.col);
            DrawCircleV(ball.pos, ball.rad_small, ball.col_small);
            DrawText("C", (int)ball.pos.x-6, (int)ball.pos.y-9, 20, paddle.col);
            // Paddle
            // -------
            DrawRectangleRec(paddle.rec,  paddle.col);
            // Blocks
            // -------
            for (int i=0; i<BLOCK_COUNT; i++) {
                if (!blocks[i].hit) {
                    DrawRectangleRec(blocks[i].rec, blocks[i].col);
                }
            }
            // Score
            // -------
            DrawText(TextFormat("Score: %i",score.value), score.x, score.y, score.font_size, score.col);
            // Score Popups
            // -------
            for (int i=0; i<MAX_SCORE_POPUPS; i++){
                if (score_popups[i].active) {
                    DrawText(
                        TextFormat("+%d", score_popups[i].value),
                        (int)score_popups[i].pos.x,
                        (int)score_popups[i].pos.y,
                        20,
                        Fade(score_popups[i].col, score_popups[i].alpha)
                    );
                }
            }
            // End-of-game messages
            // -------
            if (game_state != PLAYING)
            {
                if (game_state == LOSE)
                {
                    DrawText("Game Over!", SCREEN_WIDTH/3, SCREEN_HEIGHT/2, 50, RED);
                }
                else if (game_state == WIN)
                {
                    DrawText("You Win!", SCREEN_WIDTH/3, SCREEN_HEIGHT/2, 50, RED);
                }
                DrawText(TextFormat("Final Score: %d",score.value), SCREEN_WIDTH/4, 3*SCREEN_HEIGHT/5, 50, BLACK);
                DrawText("Quit: ESC or Restart: SPACE", SCREEN_WIDTH/5, SCREEN_HEIGHT/20, 30, BLACK);
                if (IsKeyDown(KEY_SPACE))
                {
                    // Restart
                    game_state = PLAYING;
                    initialize_game(&paddle, &ball, blocks, score_popups, &score);
                }
            }

        EndDrawing();
    }
    // De-Initialization
    UnloadSound(paddle_sfx);     // Unload sound data
    UnloadSound(block_sfx);     // Unload sound data
    CloseAudioDevice();     // Close audio device
    CloseWindow();        // Close window and OpenGL context
    return 0;
}
