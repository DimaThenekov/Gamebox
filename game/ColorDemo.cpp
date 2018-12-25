#pragma GCC optimize ("-O3")

#include "libgame.h"
#include "graphics.h"
#include "binary.h"
#include "controls.h"
#include "music.h"
#include "tunes.h"

#ifdef FRAME_BUFFER

struct ColorDemoData
{
};
static ColorDemoData* data;

static void ColorDemo_prepare()
{
    game_enable_frame_buffer();
    for (int c = 0 ; c < WIDTH ; ++c)
        for (int r = 0 ; r < HEIGHT ; ++r)
            game_draw_pixel(c, r, (c / (WIDTH / 8)) * 16 + (r / (HEIGHT / 8)));
}

static void ColorDemo_render()
{
}

static void ColorDemo_update(unsigned long delta)
{
}

const game_instance ColorDemo PROGMEM = {
    "ColorDemo",
    ColorDemo_prepare,
    ColorDemo_render,
    ColorDemo_update,
    sizeof(ColorDemoData),
    (void**)(&data)
};

#endif
