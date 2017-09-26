#include "libgame.h"
#include "menu.h"
#include "graphics.h"
#include "controls.h"
#include "font.h"

#define UP (BITMASK(BUTTON_NE) | BITMASK(BUTTON_UP))
#define DOWN (BITMASK(BUTTON_SE) | BITMASK(BUTTON_DOWN))
#define SELECT (BITMASK(BUTTON_SW) | BITMASK(BUTTON_A) | BITMASK(BUTTON_B))

#define LINES (HEIGHT / (FONT_HEIGHT + 1))

#define BUTTON_DELAY 200
static unsigned long cur_time;
static unsigned long btn_timeout;
static uint16_t sel;
static bool btn_pressed;

static MenuItem *menu;

void menu_setup(MenuItem *items)
{
    menu = items;
    sel = 0;
    btn_pressed = false;
    btn_timeout = 0;
    cur_time = 0;
}

void *menu_update(unsigned long delta)
{
    cur_time += delta;
    if (cur_time < btn_timeout)
    {
        return NULL;
    }
    if ((game_is_any_button_pressed(DOWN))
        && pgm_read_pointer(&menu[sel + 1].name))
    {
        sel++;
        btn_timeout = cur_time + BUTTON_DELAY;
    }
    else if ((game_is_any_button_pressed(UP)) && sel > 0)
    {
        sel--;
        btn_timeout = cur_time + BUTTON_DELAY;
    }
    else if (game_is_any_button_pressed(SELECT))
    {
        btn_pressed = true;
    }
    else if (btn_pressed)
    {
        btn_pressed = false;
        return pgm_read_pointer(&menu[sel].opaque);
    }

    return NULL;
}

void menu_render()
{
    uint16_t page = sel / LINES * LINES;
    uint8_t iter = 0;
    for ( ; pgm_read_pointer(&menu[page + iter].name) && iter < LINES ; ++iter)
    {
        game_draw_text(pgm_read_pointer(&menu[page + iter].name), 0, (FONT_HEIGHT + 1) * iter,
            (page + iter == sel) ? RED : WHITE);
    }
    if (page > 0)
    {
        game_draw_char(0x18, WIDTH - FONT_WIDTH, 0, GREEN);
    }
    if (iter == LINES && pgm_read_pointer(&menu[page + iter].name))
    {
        game_draw_char(0x19, WIDTH - FONT_WIDTH, HEIGHT - FONT_HEIGHT, GREEN);
    }
}

