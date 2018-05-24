#include "libgame.h"
#include "menu.h"
#include "graphics.h"
#include "controls.h"
#include "font.h"

#define UP (BITMASK(BUTTON_NE) | BITMASK(BUTTON_UP))
#define DOWN (BITMASK(BUTTON_SE) | BITMASK(BUTTON_DOWN))
#define SELECT (BITMASK(BUTTON_SW) | BITMASK(BUTTON_A) | BITMASK(BUTTON_B))

#define LINES (HEIGHT / (FONT_HEIGHT + 1))

struct Menu
{
    long btn_timeout;
    uint16_t sel;
    bool btn_pressed;
    uint8_t menux, menuy;
    uint8_t bg;
    const MenuItem *menu;
};

static Menu menu[2];
static uint8_t current;

Menu *menu_setup(const MenuItem *items, uint8_t x, uint8_t y, uint8_t bg)
{
    //assert(current < 2);
    Menu *m = &menu[current];
    ++current;
    m->menu = items;
    m->sel = 0;
    m->btn_pressed = false;
    m->btn_timeout = 0;
    m->menux = x;
    m->menuy = y;
    m->bg = bg;

    return m;
}

void *menu_update(Menu *m, unsigned long delta)
{
    m->btn_timeout -= delta;
    if (m->btn_timeout > 0)
    {
        return NULL;
    }
    m->btn_timeout = 0;
    if (game_is_any_button_pressed(DOWN)
        && pgm_read_pointer(&m->menu[m->sel + 1].name))
    {
        m->sel++;
        m->btn_timeout = BUTTON_DELAY;
    }
    else if (game_is_any_button_pressed(UP) && m->sel > 0)
    {
        m->sel--;
        m->btn_timeout = BUTTON_DELAY;
    }
    else if (game_is_any_button_pressed(SELECT))
    {
        m->btn_pressed = true;
    }
    else if (m->btn_pressed)
    {
        m->btn_pressed = false;
        return pgm_read_pointer(&m->menu[m->sel].opaque);
    }

    return NULL;
}

void menu_render(Menu *m)
{
    uint16_t page = m->sel / LINES * LINES;
    uint8_t iter = 0;
    for ( ; pgm_read_pointer(&m->menu[page + iter].name) && iter < LINES ; ++iter)
    {
        game_draw_text((const uint8_t *)pgm_read_pointer(&m->menu[page + iter].name),
            m->menux, (FONT_HEIGHT + 1) * iter + m->menuy,
            (page + iter == m->sel) ? RED : WHITE, m->bg);
    }
    if (page > 0)
    {
        game_draw_char(0x18, WIDTH - FONT_WIDTH, 0, GREEN);
    }
    if (iter == LINES && pgm_read_pointer(&m->menu[page + iter].name))
    {
        game_draw_char(0x19, WIDTH - FONT_WIDTH, HEIGHT - FONT_HEIGHT, GREEN);
    }
}

void menu_finish(Menu *m)
{
    --current;
    //assert(&menu[current] == m);
}
