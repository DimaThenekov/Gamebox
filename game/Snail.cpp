#pragma GCC optimize ("-O3")

#include "libgame.h"
#include "graphics.h"
#include "binary.h"
#include "controls.h"
#include "menu.h"
#include "music.h"

#ifdef FRAME_BUFFER

#define LEFT BUTTON_LEFT
#define RIGHT BUTTON_RIGHT
#define UP BUTTON_UP
#define DOWN BUTTON_DOWN
#define MENU BUTTON_START

#define R RED_DARK
#define G GREEN
#define T TRANSPARENT
#define K BLACK
#define Y YELLOW
#define W WHITE_DARK
static const uint8_t snail_lines[] PROGMEM = {
    T, R, R, R, T, T, T, T,
    R, K, K, K, R, G, T, G,
    R, K, R, K, R, G, G, G,
    R, K, R, R, R, W, G, W,
    R, K, K, K, K, G, G, G,
    T, R, R, R, R, G, G, G,
    T, T, G, G, G, G, G, G,
    T, G, G, G, G, G, G, T,
};

static const uint8_t block_lines[] PROGMEM = {
    R, R, W, R, R, R, R, R,
    R, R, W, R, R, R, R, R,
    R, R, W, R, R, R, R, R,
    W, W, W, W, W, W, W, W,
    R, R, R, R, R, W, R, R,
    R, R, R, R, R, W, R, R,
    R, R, R, R, R, W, R, R,
    W, W, W, W, W, W, W, W,
};

#undef R
#undef G
#undef T
#undef K
#undef W

static const uint8_t crate_lines[] PROGMEM = {
    B11111111,
    B10000001,
    B10111101,
    B10111101,
    B10111101,
    B10111101,
    B10000001,
    B11111111,
};

static const game_color_sprite snail PROGMEM = {
    8, 8, snail_lines
};

static const game_color_sprite block PROGMEM = {
    8, 8, block_lines
};

static const game_sprite crate PROGMEM = {
    8, 8, crate_lines
};


static const uint8_t level1[] PROGMEM = {
    "BBBBB..."
    "BS..B..."
    "B.C.B..."
    "B..BB..."
    "B..B...."
    "BXCB...."
    "BX.B...."
    "BBBB...."
};

static const uint8_t level2[] PROGMEM = {
    "BBBBBBBB"
    "BS.BXXXB"
    "B..CC..B"
    "B.C...BB"
    "B..BBBB."
    "B..B...."
    "BBBB...."
    "........"
};

static const uint8_t level3[] PROGMEM = {
    "...BBBBB"
    ".BBB...B"
    ".BSC.B.B"
    "BBCC...B"
    "B..BB.BB"
    "BX.X...B"
    "BB...X.B"
    ".BBBBBBB"
};

static const uint8_t level4[] PROGMEM = {
    "BBBBBBBB"
    "B...BS.B"
    "B....C.B"
    "B...B..B"
    "BBBBBCXB"
    "....B.XB"
    "....BBBB"
    "........"
};

static const uint8_t level5[] PROGMEM = {
    "BBBBBBBB"
    "BS..B..B"
    "B..CCC.B"
    "BXX....B"
    "BBBBBBBB"
    "........"
    "........"
    "........"
};

static const uint8_t level6[] PROGMEM = {
    ".BBBBBBB"
    ".B.....B"
    ".B.C...B"
    ".B..B..B"
    "BB.BBBBB"
    "BSC...B."
    "BX.X..B."
    "BBBBBBB."
};

static const uint8_t level7[] PROGMEM = {
    "BBBBBBBB"
    "BS.B...B"
    "B..B...B"
    "B.CB..BB"
    "BX.B...B"
    "B.CC..CB"
    "BX.B...B"
    "BBBBBBBB"
};

static const uint8_t level8[] PROGMEM = {
    "BBBBBBBB"
    "BS.B...B"
    "B..B...B"
    "B.CB..BB"
    "BX.B...B"
    "B.CC..DB"
    "BX.B...B"
    "BBBBBBBB"
};

static const uint8_t level9[] PROGMEM = {
    ".BBBBBBB"
    "BB..BS.B"
    "B..CXD.B"
    "B..BB..B"
    "B..B...B"
    "B..B...B"
    "B..B...B"
    "BBBBBBBB"
};

static const uint8_t level10[] PROGMEM = {
    "..BBBBB."
    "BBBSXXB."
    "B..C..B."
    "B..C..B."
    "B..BBBB."
    "B.CC.XB."
    "B.....B."
    "BBBBBBB."
};

static const uint8_t level11[] PROGMEM = {
    "BBBBBBB."
    "BS....B."
    "BXB...B."
    "B....BBB"
    "B...C..B"
    "B.B.C..B"
    "BX..BBBB"
    "BBBBB..."
};

static const uint8_t * const levels[] PROGMEM = {
    level1,
    level2,
    level4,
    level5,
    level6,
    level7,
    level8,
    level9,
    level10,
    level11,
    level3
};

static const MenuItem level_menu[] PROGMEM = {
    { "Too easy", (void*)1 },
    { "Still easy", (void*)2 },
    { "Deja vu?", (void*)3 },
    { "3 4 2", (void*)4 },
    { "Seems easy", (void*)5 },
    { "Not easy", (void*)6 },
    { "Harder", (void*)7 },    
    { "Almost done", (void*)8 },
    { "One spare", (void*)9 },
    { "Dead end", (void*)10 },
    { "Very hard", (void*)11 },
    { "", NULL }
};

#define CW 8
#define CH 8
#define W (WIDTH / CW)
#define H (HEIGHT / CH)

enum GameState
{
    GS_PLAY,
    GS_MENU,
};

struct SnailData
{
    int8_t x, y;
    uint8_t level;
    long btn_timeout;
    uint8_t field[H][W];
    Menu *menu;
    GameState state;
};
static SnailData* data;

static void Snail_draw()
{
    game_draw_color_sprite(&snail, data->x * CW, data->y * CH);
}

static void Snail_draw_field_cell(int c, int r)
{
    switch (data->field[r][c])
    {
    case 'B':
        game_draw_color_sprite(&block, c * CW, r * CH);
        break;
    case 'D':
        game_draw_rect(c * CW + 1, r * CH + 1, CW - 2, CH - 2, BLUE);
        /* fall through */
    case 'C':
        game_draw_sprite(&crate, c * CW, r * CH, YELLOW_DARK);
        break;
    case 'X':
        game_draw_rect(c * CW + 1, r * CH + 1, CW - 2, CH - 2, BLUE);
        break;
    default:
        game_draw_rect(c * CW, r * CH, CW, CH, BLACK);
        break;
    }
}


static void Snail_draw_field()
{
    for (int r = 0 ; r < H ; ++r)
        for (int c = 0 ; c < W ; ++c)
            Snail_draw_field_cell(c, r);
    Snail_draw();
}

void Snail_prepare_level(int lev)
{
    data->level = lev;
    uint8_t *ptr = (uint8_t*)pgm_read_pointer(&levels[lev]);
    for (int r = 0 ; r < H ; ++r)
        for (int c = 0 ; c < W ; ++c)
        {
            game_draw_rect(c * CW, r * CH, CW, CH, BLACK);
            char cell = pgm_read_byte(ptr++);
            data->field[r][c] = cell;
            if (cell == 'S')
            {
                data->x = c;
                data->y = r;
                data->field[r][c] = 0;
            }
            else if (cell == '.')
            {
                data->field[r][c] = 0;
            }
        }
    Snail_draw_field();
#ifndef EMULATED
    Player_setup_random_melody();
#endif
}

static void Snail_prepare()
{
    game_set_ups(8);
    game_enable_frame_buffer();
    data->x = 0;
    data->y = 0;
    data->btn_timeout = 0;
    data->state = GS_MENU;
    data->menu = menu_setup(level_menu);
}

void Snail_move(int dx, int dy)
{
    if (!dx && !dy)
        return;
    int newx = data->x + dx;
    int newy = data->y + dy;
    if (newx < 0 || newx >= W || newy < 0 || newy >= H)
        return;
    char next = data->field[newy][newx];
    if (next == 'C' || next == 'D')
    {
        int cx = newx + dx;
        int cy = newy + dy;
        if (cx < 0 || cx >= W || cy < 0 || cy >= H)
            return;
        char next2 = data->field[cy][cx];
        if (next2 && next2 != 'X')
            return;
        // move crate
        game_draw_rect(cx * CW, cy * CH, CW, CH, BLACK);
        data->field[cy][cx] = next2 ? 'D' : 'C';
        data->field[newy][newx] = next == 'C' ? 0 : 'X';
        Snail_draw_field_cell(cx, cy);
    }
    else if (next && next != 'X')
    {
        return;
    }
    game_draw_rect(newx * CW, newy * CH, CW, CH, BLACK);
    game_draw_rect(data->x * CW, data->y * CH, CW, CH, BLACK);
    Snail_draw_field_cell(data->x, data->y);
    data->x = newx;
    data->y = newy;
    Snail_draw();
}

static void Snail_check_win()
{
    bool hasX = false;
    for (int r = 0 ; r < H ; ++r)
        for (int c = 0 ; c < W ; ++c)
        {
            char ch = data->field[r][c];
            if (ch == 'X')
                hasX = true;
        }
    if (!hasX)
        Snail_prepare_level(data->level + 1);
}

static void Snail_render()
{
    if (data->state == GS_MENU)
    {
        menu_render(data->menu);
    }
}

static void Snail_update(unsigned long delta)
{
    if (data->state == GS_MENU)
    {
        ptrdiff_t p = (ptrdiff_t)menu_update(data->menu, delta);
        if (p)
        {
            Snail_prepare_level(p - 1);
            data->state = GS_PLAY;
        }
        return;
    }
    data->btn_timeout -= delta;
    if (data->btn_timeout > 0)
        return;
    int dx = 0;
    int dy = 0;
    data->btn_timeout = 0;
    if (game_is_button_pressed(MENU))
    {
        game_draw_rect(0, 0, WIDTH, HEIGHT, BLACK);
        data->state = GS_MENU;
        return;
    }
    else if (game_is_button_pressed(DOWN))
    {
        dy = 1;
    }
    else if (game_is_button_pressed(UP))
    {
        dy = -1;
    }
    else if (game_is_button_pressed(RIGHT))
    {
        dx = 1;
    }
    else if (game_is_button_pressed(LEFT))
    {
        dx = -1;
    }
    Snail_move(dx, dy);
    Snail_check_win();
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
