#pragma GCC optimize ("-O3")

#include "libgame.h"
#include "graphics.h"
#include "binary.h"
#include "controls.h"

#ifdef FRAME_BUFFER

#define LEFT BUTTON_LEFT
#define RIGHT BUTTON_RIGHT
#define UP BUTTON_UP
#define DOWN BUTTON_DOWN

#define S RED
#define B GREEN
#define O TRANSPARENT
#define Z BLACK
#define E WHITE
static const uint8_t snail_lines[] PROGMEM = {
    O, S, S, S, O, O, O, O,
    S, Z, Z, Z, S, B, O, B,
    S, Z, S, Z, S, B, B, B,
    S, Z, S, S, S, E, B, E,
    S, Z, Z, Z, Z, B, B, B,
    O, S, S, S, S, B, B, B,
    O, O, B, B, B, B, B, B,
    O, B, B, B, B, B, B, O,
};
#undef S
#undef B
#undef O
#undef Z
#undef E

static const game_color_sprite snail PROGMEM = {
    8, 8, snail_lines
};

#define CW 8
#define CH 8
#define W (WIDTH / CW)
#define H (HEIGHT / CH)

struct SnailData
{
    int8_t x, y;
    long btn_timeout;
};
static SnailData* data;

static void Snail_draw()
{
    game_draw_color_sprite(&snail, data->x * CW, data->y * CH);
}

static void Snail_prepare()
{
    game_set_ups(15);
    game_enable_frame_buffer();
    data->x = 0;
    data->y = 0;
    data->btn_timeout = 0;
    Snail_draw();
}

static void Snail_render()
{
}

static void Snail_update(unsigned long delta)
{
    data->btn_timeout -= delta;
    if (data->btn_timeout > 0)
        return;
    bool update = false;
    int px = data->x;
    int py = data->y;
    data->btn_timeout = 0;
    if (game_is_button_pressed(DOWN) && data->y < H - 1)
    {
        ++data->y;
        update = true;
    }
    else if (game_is_button_pressed(UP) && data->y > 0)
    {
        --data->y;
        update = true;
    }
    else if (game_is_button_pressed(RIGHT) && data->x < W - 1)
    {
        ++data->x;
        update = true;
    }
    else if (game_is_button_pressed(LEFT) && data->x > 0)
    {
        --data->x;
        update = true;
    }
    if (update)
    {
        game_draw_rect(px * CW, py * CH, CW, CH, BLACK);
        Snail_draw();
    }
}

game_instance Snail = {
    "Snail",
    Snail_prepare,
    Snail_render,
    Snail_update,
    sizeof(SnailData),
    (void**)(&data)
};

#endif
