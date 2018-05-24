#pragma GCC optimize ("-O3")

#include "libgame.h"
#include "storage.h"
#include "graphics.h"
#include "controls.h"
#include "random.h"
#include "menu.h"

/* List of all games */

extern game_instance BackspaceInvaders;
extern game_instance Snake;
extern game_instance Flappy;
extern game_instance Tester;
extern game_instance Raycaster;
extern game_instance BreakOut;
extern game_instance Saper;
#ifndef EMULATED /* for use only on real hardware */
extern game_instance Dump;
extern game_instance Player;
#endif
/* Register your game like so:
 * 
 * extern game_instance YOUR_GAME_NAME;
 */

static const MenuItem main_menu[] PROGMEM = {
    { "Invaders", &BackspaceInvaders },
    { "Snake", &Snake },
    { "Flappy", &Flappy },
    { "BreakOut", &BreakOut },
    { "Saper", &Saper },
    { "3D", &Raycaster },
    { "Font", &Tester },
#ifndef EMULATED /* for use only on real hardware */
    { "EEPROM", &Dump },
    { "Music", &Player },
#endif
    /* Register your game like so:
     * 
     * { "YOUR_GAME_NAME", &YOUR_GAME_NAME },
     */
    { NULL, NULL }
};

#define UP BITMASK(BUTTON_NE) | BITMASK(BUTTON_UP)
#define DOWN BITMASK(BUTTON_SE) | BITMASK(BUTTON_DOWN)
#define SELECT BITMASK(BUTTON_SW) | BITMASK(BUTTON_START)
#define PAUSE BITMASK(BUTTON_SELECT)

// need some space for stack and system variables
#define AVAIL_SPACE 1024

static uint8_t memory[AVAIL_SPACE];

static Menu *menu;
static game_instance* ptr;
static long btn_timeout;
static bool is_paused;

void pause_continue()
{
    is_paused = false;
    menu_finish(menu);
    menu = NULL;
    btn_timeout = BUTTON_DELAY;
}

void pause_exit()
{
#ifdef EMULATED
#else
    void (*f)() = 0;
    f();
#endif
}

#define CONTINUE ((void*)0xC0)
#define EXIT     ((void*)0xE)

static const MenuItem pause_menu[] PROGMEM = {
    { "Continue", CONTINUE },
    { "Exit game", EXIT },
    { NULL, NULL }
};

void application_setup()
{
    menu = menu_setup(main_menu);
}

void update(unsigned long delta)
{
    if (!ptr)
    {
        ptr = (game_instance*)menu_update(menu, delta);
        if (ptr)
        {
            menu_finish(menu);
            menu = NULL;
            random_setup();
            // run game
            *(ptr->data) = memory;
            ptr->prepare();
        }
    }
    else
    {
        btn_timeout -= delta;
        if (btn_timeout <= 0)
        {
            btn_timeout = 0;
        }
        if (!is_paused && !btn_timeout && game_is_any_button_pressed(PAUSE))
        {
            is_paused = true;
            menu = menu_setup(pause_menu, 5, 20, BLUE);
            btn_timeout = BUTTON_DELAY;
        }
        else if (is_paused && !btn_timeout && game_is_any_button_pressed(PAUSE))
        {
            pause_continue();
        }
        else if (!is_paused)
        {
            // shouldn't use SELECT in games
            game_reset_buttons(PAUSE);
            ptr->update(delta);
        }
        else
        {
            void *r = menu_update(menu, delta);
            if (r == CONTINUE)
            {
                pause_continue();
            }
            else if (r == EXIT)
            {
                pause_exit();
            }
        }
    }
}

void render()
{
    if (ptr)
    {
        ptr->render();
    }
    if (menu)
    {
        menu_render(menu);
    }
}

//////////////////////////
// Storage
//////////////////////////

void game_save(const void *buf, uint16_t size)
{
    storage_write((const char*)ptr->name, buf, size);
}

void game_load(void *buf, uint16_t size)
{
    storage_read((const char*)ptr->name, buf, size);
}

