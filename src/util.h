#ifndef UTIL_H
#define UTIL_H

#include <raylib.h>

enum State {
    PLAYING, WIN, LOSE
};
enum Layer {
    TOP, MIDDLE, BOTTOM
};
typedef struct Block
{
    Rectangle rec;
    bool hit;
    float rec_center_x;
    float rec_center_y;
    float rec_diagonal;
    int value;
    Color col;
}Block;
typedef struct Ball
{
    Vector2 pos;
    Vector2 vel;
    float rad;
    float rad_small;
    Color col;
    Color col_small;
}Ball;
typedef struct Paddle
{
    Rectangle rec;
    float speed;
    Color col;
}Paddle;
typedef struct ScorePopup
{
    Vector2 pos;
    int value;
    float lifetime;
    float alpha;
    bool active;
    Color col;
}ScorePopup;
typedef struct ScoreTotal
{
    int x;
    int y;
    int value;
    float velocity;
    bool animate;
    float scale;
    int font_size;
    int base_font_size;
    Color col;
    Color col_animate;
    Color col_base;
}ScoreTotal;
float square(float);
float eucledian_distance_squared(Vector2);
float eucledian_distance(Vector2);
float draw_uniform_dist(); // Uniform [0,1]
void bounce_window(Ball *ball);
void ball_position_update(Vector2*, Vector2*);
void initialize_blocks(Block*);
void initialize_popups(ScorePopup *score_popups);
void initialize_paddle(Paddle *paddle);
void initialize_ball(Ball *ball);
void initialize_score(ScoreTotal *score);
void initialize_game(Paddle *paddle, Ball *ball, Block *blocks, ScorePopup *score_popups, ScoreTotal *score);
void update_popups(ScorePopup *score_popups);
void update_score_animation(ScoreTotal *score);
void bounce_paddle(Ball *ball, Paddle *paddle, Sound *paddle_sfx);
void bounce_block(Ball *ball, Block *blocks, ScorePopup *score_popups, ScoreTotal *score, Sound *block_sfx);
bool check_lose(Ball *ball, Paddle *paddle);
bool check_win(Block *blocks);
#endif
