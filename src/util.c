#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include "constants.h"
#include "util.h"

float square(float x)
{
   return x*x;
}

float eucledian_distance_squared(Vector2 vector)
{
    return square(vector.x) + square(vector.y);
}

float eucledian_distance(Vector2 vector)
{
    return sqrtf(eucledian_distance_squared(vector));
}

float draw_uniform_dist()
{
    return (float)((float)rand())/((float)RAND_MAX);
}

void bounce_window(Ball *ball)
{
    // Ball bounce on window
    if (ball->pos.x > SCREEN_WIDTH)
    {
        ball->vel.x = (float)-fabs(ball->vel.x);
    } else if (ball->pos.x < 0) {
        ball->vel.x = (float) fabs(ball->vel.x);
    }
    if (ball->pos.y < 0)
    {
        ball->vel.y = (float) fabs(ball->vel.y);
    } else if (ball->pos.y > SCREEN_HEIGHT)
    {
        ball->vel.y = (float)-fabs(ball->vel.y);
    }
}

void ball_position_update(Vector2 *ball, Vector2 *ball_velocity)
{
    ball->x += ball_velocity->x * TIME_TICK;
    ball->y += ball_velocity->y * TIME_TICK;
}

void initialize_blocks(Block *blocks)
{
    const float MARGIN = 5.0f;
    const float VSPACE = 40.0f;
    const int ROWS = 3;
    const int ELEMENTS_PER_ROW = BLOCK_COUNT/ROWS;
    enum Layer layer;
    for (int i=0; i < BLOCK_COUNT; i++) // block_count needs to be a multiple of 3
    {
        layer = i % ROWS;
        blocks[i].rec.x = MARGIN + ((float)(i%ELEMENTS_PER_ROW))/(float)(ELEMENTS_PER_ROW) * ((float)SCREEN_WIDTH);
        blocks[i].rec.y = SCREEN_HEIGHT / 8.0f + VSPACE * (float) layer ;
        blocks[i].rec.width = 65.0f;
        blocks[i].rec.height = 20.0f;
        blocks[i].hit = false;
        blocks[i].rec_center_x = blocks[i].rec.x + blocks[i].rec.width/2.0f;
        blocks[i].rec_center_y = blocks[i].rec.y + blocks[i].rec.height/2.0f;
        blocks[i].rec_diagonal = sqrtf(square(blocks[i].rec.width)+square(blocks[i].rec.height));
        blocks[i].value = layer == BOTTOM ? BLOCK_SCORE_VALUE : layer == MIDDLE ? BLOCK_SCORE_VALUE*2 : BLOCK_SCORE_VALUE*3;
        blocks[i].col = layer == BOTTOM ? DARKGREEN : layer == MIDDLE ? DARKBLUE : DARKPURPLE;
    }
}

void initialize_popups(ScorePopup *score_popups)
{
    for (int i=0; i < MAX_SCORE_POPUPS; i++)
    {
        score_popups[i].active = false;
    }
}

void initialize_paddle(Paddle *paddle)
{
    *paddle = (Paddle) {
       .rec.x = SCREEN_WIDTH/2.0f,
       .rec.y = SCREEN_HEIGHT*7.0f/8.0f,
       .rec.width = 40.0f,
       .rec.height = 10.0f,
       .speed = 5.0f,
       .col = MAROON,
    };
}

void initialize_ball(Ball *ball)
{
    *ball= (Ball) {
        .pos.x = SCREEN_WIDTH/2.0f,
        .pos.y = SCREEN_HEIGHT/2.0f,
        .vel.x = 1.0f,
        .vel.y = 1.0f,
        .rad = 12.0f,
        .rad_small = 10.0f,
        .col = GRAY,
        .col_small = RAYWHITE,
    };
}
void initialize_score(ScoreTotal *score)
{
    *score = (ScoreTotal) {
        .x = SCREEN_WIDTH/15,
        .y = SCREEN_HEIGHT/15,
        .value = 0,
        .animate = false,
        .scale = 1.0f,
        .velocity = 3.0f,
        .base_font_size = 20,
        .font_size = 20,
        .col = BLACK,
        .col_animate = RED,
        .col_base = BLACK,
    };
}

void initialize_game(Paddle *paddle, Ball *ball, Block *blocks, ScorePopup *score_popups, ScoreTotal *score)
{
    initialize_paddle(paddle);
    initialize_ball(ball);
    initialize_blocks(blocks);
    initialize_popups(score_popups);
    initialize_score(score);
}

void update_popups(ScorePopup *score_popups)
{
    for (int i=0; i<MAX_SCORE_POPUPS; i++)
    {
        if (score_popups[i].active)
        {
            score_popups[i].lifetime -= GetFrameTime();
            score_popups[i].pos.y -= 30 * GetFrameTime();
            score_popups[i].alpha = score_popups[i].lifetime;
            if (score_popups[i].lifetime <= 0.0f)
            {
                score_popups[i].active = false;
            }
        }
    }
}

void update_score_animation(ScoreTotal *score)
{
    if (score->animate)
    {
        score->scale -= score->velocity * GetFrameTime();
        score->font_size = (int) (score->scale * (float)score->base_font_size);
        if (score->scale <= 1.0f)
        {
            score->scale = 1.0f;
            score->animate = false;
            score->col = score->col_base;
        }
    }
}

void add_popup(ScorePopup *score_popups, Block *hit_block)
{
    for (int i=0; i < MAX_SCORE_POPUPS; i++)
    {
        if (!score_popups[i].active)
        {
            score_popups[i].active = true;
            score_popups[i].pos = (Vector2){
                hit_block->rec.x + hit_block->rec.width/2,
                hit_block->rec.y
            };
            score_popups[i].value = hit_block->value;
            score_popups[i].lifetime = 1.0f;
            score_popups[i].alpha = 1.0f;
            score_popups[i].col = BLACK;
            break;
        }
    }

}

void bounce_paddle(Ball *ball, Paddle *paddle, Sound *paddle_sfx)
{
    if (CheckCollisionCircleRec(ball->pos, ball->rad, paddle->rec))
    {
        ball->vel.y = -fabsf(ball->vel.y);
        if (IsKeyDown(KEY_LEFT))
        {
            ball->vel.x += -0.3f;
        }
        else if (IsKeyDown(KEY_RIGHT))
        {
            ball->vel.x += 0.3f;
        }
        PlaySound(*paddle_sfx);
    }
}

void bounce_block(Ball *ball, Block *blocks, ScorePopup *score_popups, ScoreTotal *score, Sound *block_sfx)
{
    for (int i = 0; i<BLOCK_COUNT; i++)
    {
        if (!blocks[i].hit && CheckCollisionCircleRec(ball->pos, ball->rad, blocks[i].rec))
        {
            float dy = fabsf(blocks[i].rec_center_y - ball->pos.y);
            float dx = fabsf(blocks[i].rec_center_x - ball->pos.x);
            float corner_dx_squared = square(blocks[i].rec_diagonal+ball->rad) - square(blocks[i].rec.height/2.0f+ball->rad);
            float corner_dy_squared = square(blocks[i].rec_diagonal+ball->rad) - square(blocks[i].rec.width/2.0f+ball->rad);
            float delta_x = fabsf(square(dx) - corner_dx_squared);
            float delta_y = fabsf(square(dy) - corner_dy_squared);
            if (delta_x <= delta_y)
            {
                ball->vel.x *= -1.0f;
            }
            else
            {
                ball->vel.y *= -1.0f;
            }
            blocks[i].hit = true;
            add_popup(score_popups, &blocks[i]);
            score->value += blocks[i].value;
            score->animate = true;
            score->scale = 2.5f;
            score->col = score->col_animate;
            PlaySound(*block_sfx);
        }
    }
}

bool check_lose(Ball *ball, Paddle *paddle)
{
    float eps = 0.2f * paddle->rec.height;
    if (ball->pos.y > paddle->rec.y + eps) {
        return true;
    }
    return false;
}

bool check_win(Block *blocks)
{
    bool result = true;
    for (int i = 0; i<BLOCK_COUNT; i++){
        result = result && blocks[i].hit;
    }
    return result;
}
