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

// need some space for stack and system variables
#define AVAIL_SPACE 1024

static uint8_t memory[AVAIL_SPACE];

static game_instance* ptr;

void application_setup()
{
    menu_setup(main_menu);
}

void update(unsigned long delta)
{
    if (!ptr)
    {
        ptr = menu_update(delta);
        if (ptr)
        {
            random_setup();
            // run game
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
    if (!ptr)
    {
        menu_render();
    }
    else
    {
        ptr->render();
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

