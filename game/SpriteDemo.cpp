#pragma GCC optimize ("-O3")

#include "libgame.h"
#include "graphics.h"
#include "binary.h"
#include "controls.h"
#include "music.h"
#include "tunes.h"

struct SpriteDemoData
{
};
static SpriteDemoData* data;

static void SpriteDemo_prepare()
{
}

static void SpriteDemo_render()
{
}

static void SpriteDemo_update(unsigned long delta)
{
}

const game_instance SpriteDemo PROGMEM = {
    "SpriteDemo",
    SpriteDemo_prepare,
    SpriteDemo_render,
    SpriteDemo_update,
    sizeof(SpriteDemoData),
    (void**)(&data)
};
