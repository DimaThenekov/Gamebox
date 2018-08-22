#pragma GCC optimize ("-O3")

#include "libgame.h"
#include "graphics.h"
#include "binary.h"
#include "controls.h"
#include "music.h"
#include "font.h"
#include "tunes.h"

#ifdef FRAME_BUFFER

#define MAXLEN 128

#define ROTLEFT BUTTON_SE
#define ROTRIGHT BUTTON_SW

#define LEFT BUTTON_LEFT
#define RIGHT BUTTON_RIGHT
#define UP BUTTON_UP
#define DOWN BUTTON_DOWN

#define GAMEOVER_X 16
#define GAMEOVER_Y 24

#define FIELD_WIDTH 32
#define FIELD_HEIGHT 32

static const uint8_t gameOverLines[] PROGMEM = {
    B00111110, B00111000, B11000110, B11111110,
    B01100000, B01101100, B11101110, B11000000,
    B11000000, B11000110, B11111110, B11000000,
    B11001110, B11000110, B11111110, B11111100,
    B11000110, B11111110, B11010110, B11000000,
    B01100110, B11000110, B11000110, B11000000,
    B00111110, B11000110, B11000110, B11111110,
    B00000000, B00000000, B00000000, B00000000,
    B01111100, B11000110, B11111110, B11111100,
    B11000110, B11000110, B11000000, B11000110,
    B11000110, B11000110, B11000000, B11000110,
    B11000110, B11000110, B11111100, B11001110,
    B11000110, B01101100, B11000000, B11111000,
    B11000110, B00111000, B11000000, B11011100,
    B01111100, B00010000, B11111110, B11001110
};

static const game_sprite gameOver PROGMEM = {
    31, 15, gameOverLines
};

enum Phases
{
    PHASE_GAME,
    PHASE_GAMEOVER
};

struct SnakeData
{
    uint8_t phase;
    uint8_t snakeX[MAXLEN + 1];
    uint8_t snakeY[MAXLEN + 1];
    int8_t velX;
    int8_t velY;
    bool rotLeftPressed;
    bool rotRightPressed;
    uint8_t snakeBegin;
    uint8_t snakeEnd;
    uint8_t hiscore;

};
static SnakeData* data;

static void Snake_draw_rect(int x, int y, int c)
{
    game_draw_pixel(x * 2, y * 2, c);
    game_draw_pixel((x * 2 + 1) % WIDTH, y * 2, c);
    game_draw_pixel(x * 2, (y * 2 + 1) % HEIGHT, c);
    game_draw_pixel((x * 2 + 1) % WIDTH, (y * 2 + 1) % HEIGHT, c);
}

static void generateItem(int color)
{
    while (1)
    {
        int x = DIGIT_WIDTH + rand() % (FIELD_WIDTH - 2 * DIGIT_WIDTH);
        int y = DIGIT_HEIGHT + rand() % (FIELD_HEIGHT - 2 * DIGIT_HEIGHT);
        if (game_get_pixel(2 * x, 2 * y) == BLACK)
        {
            Snake_draw_rect(x, y, color);
            return;
        }
    }
}

static void generateFood()
{
    generateItem(RED);
}

static void Snake_render()
{
    // draw score
    game_draw_digits((MAXLEN + data->snakeEnd - data->snakeBegin) % MAXLEN,
        3, 0, 0, WHITE, BLACK);
    // draw hiscore
    game_draw_digits(data->hiscore, 3, WIDTH - (DIGIT_WIDTH + 1) * 3,
        0, WHITE, BLACK);
    if (data->phase == PHASE_GAMEOVER)
        game_draw_sprite(&gameOver, GAMEOVER_X, GAMEOVER_Y, WHITE);
}

static void Snake_reset()
{
    game_clear_screen();
    data->phase = PHASE_GAME;
    data->snakeX[0] = 17;
    data->snakeY[0] = 16;
    data->snakeX[1] = 16;
    data->snakeY[1] = 16;
    data->snakeX[2] = 15;
    data->snakeY[2] = 16;
    data->snakeBegin = 0;
    data->snakeEnd = 3;
    data->velX = 1;
    data->velY = 0;
    data->rotLeftPressed = false;
    data->rotRightPressed = false;
    for (int i = 0 ; i < 3 ; ++i)
        generateFood();
    for (int i = 0 ; i < 8 ; ++i)
        generateItem(BLUE);
}

static void Snake_prepare()
{
    game_set_ups(15);
    game_enable_frame_buffer();
    Snake_reset();
    data->hiscore = 0;
    game_load(&data->hiscore, sizeof(data->hiscore));
#ifndef EMULATED
    tune_init(where_time_dropped_dead);
    tune_enable();
#endif
}

static void Snake_update(unsigned long delta)
{
    if (data->phase == PHASE_GAME)
    {
        // move snake forward
        int newX = (data->snakeX[data->snakeBegin] + data->velX) % FIELD_WIDTH;
        int newY = (data->snakeY[data->snakeBegin] + data->velY) % FIELD_HEIGHT;
        data->snakeBegin = (data->snakeBegin + MAXLEN - 1) % MAXLEN;
        data->snakeEnd = (data->snakeEnd + MAXLEN - 1) % MAXLEN;
        Snake_draw_rect(data->snakeX[data->snakeEnd], data->snakeY[data->snakeEnd], BLACK);
        data->snakeX[data->snakeBegin] = newX;
        data->snakeY[data->snakeBegin] = newY;
        uint8_t pixel = game_get_pixel(newX * 2, newY * 2);
        if (pixel == RED)
        {
            if ((data->snakeEnd + 1) % MAXLEN != data->snakeBegin)
            {
                Snake_draw_rect(data->snakeX[data->snakeEnd], data->snakeY[data->snakeEnd], GREEN);
                data->snakeEnd = (data->snakeEnd + 1) % MAXLEN;
            }
            generateFood();
        }
        else if (pixel != BLACK)
        {
            // game over
            int score = (MAXLEN + data->snakeEnd - data->snakeBegin) % MAXLEN;
            if (score > data->hiscore)
            {
                data->hiscore = score;
                game_save(&data->hiscore, sizeof(data->hiscore));
            }
            data->phase = PHASE_GAMEOVER;
        }
        Snake_draw_rect(newX, newY, GREEN);

        if (game_is_button_pressed(ROTLEFT) && !data->rotLeftPressed)
        {
            int newVelX = -data->velY;
            int newVelY = data->velX;
            data->velX = newVelX;
            data->velY = newVelY;
        }
        else if (game_is_button_pressed(ROTRIGHT) && !data->rotRightPressed)
        {
            int newVelX = data->velY;
            int newVelY = -data->velX;
            data->velX = newVelX;
            data->velY = newVelY;
        }
        else if (game_is_button_pressed(LEFT) && data->velY != 0)
        {
            data->velX = -1;
            data->velY = 0;
        }
        else if (game_is_button_pressed(RIGHT) && data->velY != 0)
        {
            data->velX = 1;
            data->velY = 0;
        }
        else if (game_is_button_pressed(UP) && data->velX != 0)
        {
            data->velX = 0;
            data->velY = -1;
        }
        else if (game_is_button_pressed(DOWN) && data->velX != 0)
        {
            data->velX = 0;
            data->velY = 1;
        }
    }
    if (data->phase == PHASE_GAMEOVER)
    {
        if (game_is_any_button_pressed(-1))
        {
            Snake_reset(); 
        }
    }
    data->rotLeftPressed = game_is_button_pressed(ROTLEFT);
    data->rotRightPressed = game_is_button_pressed(ROTRIGHT);
}

const game_instance Snake2 PROGMEM = {
    "Snake2",
    Snake_prepare,
    Snake_render,
    Snake_update,
    sizeof(SnakeData),
    (void**)(&data)
};

#endif
