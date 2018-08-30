#pragma GCC optimize ("-O3")

#ifndef EMULATED
#include "libgame.h"
#include "font.h"
#include "graphics.h"
#include "controls.h"
#include "music.h"
#include "menu.h"
#include "tunes.h"
#include "random.h"
#include "tunes.h"

struct PlayerData
{
    Menu *menu;
};
static PlayerData* data;

#define TUNE(A,B)                      \
    uint32_t TUNE_ADDR(B)()            \
    {                                  \
        return pgm_get_far_address(B); \
    }
#include "tunes_list.h"
#undef TUNE

static const MenuItem tunes[] PROGMEM = {
#    define TUNE(A,B) { A, TUNE_ADDR(B) },
#    include "tunes_list.h"
#    undef TUNE
  {"", NULL}
};

void Player_prepare()
{
    game_set_ups(50);
    data->menu = menu_setup(tunes);
}

static void Player_setup_melody(int i)
{
    tune_disable();
    tune_init(((TuneAddr)pgm_read_pointer(&tunes[i].opaque))());
    tune_enable();
}

void Player_setup_random_melody()
{
    Player_setup_melody(rand() % (sizeof(tunes) / sizeof(MenuItem) - 1));
}

static void Player_render()
{
    menu_render(data->menu);
}

static void Player_update(unsigned long delta)
{
    TuneAddr p = (TuneAddr)menu_update(data->menu, delta);
    if (p)
    {
        tune_disable();
        tune_init(p());
        tune_enable();
    }
}

const game_instance Player PROGMEM = {
    "Player",
    Player_prepare,
    Player_render,
    Player_update,
    sizeof(PlayerData),
    (void**)(&data)
};
#endif
