#pragma GCC optimize ("-O3")

#include "libgame.h"
#include "storage.h"

#define _OUTPUT_INSTANCES
#include "games.h"
#undef _OUTPUT_INSTANCES

#define UP BITMASK(BUTTON_NE) | BITMASK(BUTTON_UP)
#define DOWN BITMASK(BUTTON_SE) | BITMASK(BUTTON_DOWN)
#define SELECT BITMASK(BUTTON_SW) | BITMASK(BUTTON_START)

#define AVAIL_SPACE 1400

static uint8_t N_GAMES;

static uint8_t sel = 0;

static uint8_t memory[AVAIL_SPACE];

static unsigned long cur_time = 0;

static unsigned long btn_timeout = 0;
#define BUTTON_DELAY 200

static bool game_running = false;
static bool btn_pressed = false;
static game_instance* ptr;
void prepare()
{
    N_GAMES = sizeof(instances) / sizeof(game_instance);
    game_setup();
    storage_init();
    game_set_ups(60);
}

void update(unsigned long delta)
{
    cur_time += delta;
    if (!game_running)
    {
        if (cur_time < btn_timeout) return;
        if ((game_is_any_button_pressed(DOWN)) && sel < (N_GAMES - 1))
        {
            sel++;
            btn_timeout = cur_time + BUTTON_DELAY;
        }
        if ((game_is_any_button_pressed(UP)) && sel > 0)
        {
            sel--;
            btn_timeout = cur_time + BUTTON_DELAY;
        }
        if (game_is_any_button_pressed(SELECT)) btn_pressed = true;
        if (!game_is_any_button_pressed(SELECT) && btn_pressed)
        {
            // run game
            ptr = instances + sel;
            game_running = true;
            *(ptr->data) = memory;
            ptr->prepare();
        }
    }
    else
    {
        ptr->update(delta);
    }
}

void render()
{
    if (!game_running)
    {
        for (uint8_t iter = 0; iter < N_GAMES; ++iter)
        {
            if (instances[iter].name)
            {
                game_draw_text(instances[iter].name, 0, 8 * iter, (iter == sel) ? RED : WHITE);
            }
        }
    }
    else
    {
        ptr->render();
    }
}
