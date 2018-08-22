#pragma GCC optimize ("-O3")

#include "libgame.h"
#include "graphics.h"
#include "binary.h"
#include "controls.h"

#ifdef FRAME_BUFFER

#define MAX_GOLD 32
#define MAX_BREAKING 16

#define BLOCK_WIDTH 6
#define BLOCK_HEIGHT 6

#define SCREEN_WIDTH 10
#define SCREEN_HEIGHT 10
#define SCREEN_PADDING_X 2
#define SCREEN_PADDING_Y 2

// how many different levels of breaking (0 - solid, (BREAKING - 1) - almost broken)
#define BREAKING 4

#define ANIM_INTERVAL 100
#define MOVE_INTERVAL 50
#define DIG_INTERVAL 250
#define REGEN_TIMEOUT 5000
#define FAST_REGEN_TIMEOUT 500

// character states
#define LAND 0
#define LADDER 1
#define ROPE 2

enum Animation {
    STANDING = 0,
    WALKING0,
    WALKING1,
    HANGING,
    CLIMBING0,
    CLIMBING1,
    ROPECLIMBING0,
    ROPECLIMBING1,

    ANIMATION_MAX
};

static const uint8_t player_standing_lines[] PROGMEM {
    B00110000,
    B10110100,
    B01111000,
    B00110000,
    B01001000,
    B01001000,
};

static const uint8_t player_walking0_lines[] PROGMEM {
    B00110000,
    B10110100,
    B01111000,
    B00110000,
    B01001000,
    B00001000,
};

static const uint8_t player_walking1_lines[] PROGMEM {
    B00110000,
    B10110100,
    B01111000,
    B00110000,
    B01001000,
    B01000000,
};

static const uint8_t player_hanging_lines[] PROGMEM {
    B10110100,
    B10110100,
    B01111000,
    B00110000,
    B01001000,
    B01001000,
};

static const uint8_t player_climbing0_lines[] PROGMEM {
    B10110000,
    B10110100,
    B01111000,
    B00110000,
    B01001000,
    B01000000,
};

static const uint8_t player_climbing1_lines[] PROGMEM {
    B00110100,
    B10110100,
    B01111000,
    B00110000,
    B01001000,
    B00001000,
};

static const uint8_t player_ropeclimbing0_lines[] PROGMEM {
    B10110000,
    B10110100,
    B01111000,
    B00110000,
    B01001000,
    B01001000,
};

static const uint8_t player_ropeclimbing1_lines[] PROGMEM {
    B00110100,
    B10110100,
    B01111000,
    B00110000,
    B01001000,
    B01001000,
};

const game_sprite player_sprites[ANIMATION_MAX] PROGMEM  {
    {BLOCK_WIDTH, BLOCK_HEIGHT, player_standing_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, player_walking0_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, player_walking1_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, player_hanging_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, player_climbing0_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, player_climbing1_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, player_ropeclimbing0_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, player_ropeclimbing1_lines},
};

static const uint8_t ladder_lines[] PROGMEM {
    B10000100,
    B11111100,
    B10000100,
    B10000100,
    B11111100,
    B10000100,
};

const game_sprite ladder_sprite PROGMEM {
    BLOCK_WIDTH,
    BLOCK_HEIGHT,
    ladder_lines,
};

static const uint8_t ladder_partial_lines[] PROGMEM {
    B11111100,
    B10000100,
};

const game_sprite ladder_partial_sprite PROGMEM {
    BLOCK_WIDTH,
    SCREEN_PADDING_Y,
    ladder_partial_lines,
};

static const uint8_t brick_lines[] PROGMEM {
    B10111100,
    B10111100,
    B00000000,
    B11110100,
    B11110100,
    B00000000,
};

const game_sprite brick_sprite PROGMEM {
    BLOCK_WIDTH,
    BLOCK_HEIGHT,
    brick_lines,
};

static const uint8_t rope_lines[] PROGMEM {
    B11111100,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
};

const game_sprite rope_sprite PROGMEM {
    BLOCK_WIDTH,
    BLOCK_HEIGHT,
    rope_lines,
};

static const uint8_t gold_lines[] PROGMEM {
    B00000000,
    B00000000,
    B00000000,
    B01111000,
    B01001000,
    B01111000,
};

const game_sprite gold_sprite PROGMEM {
    BLOCK_WIDTH,
    BLOCK_HEIGHT,
    gold_lines,
};

static const uint8_t solid_lines[] PROGMEM {
    B11111100,
    B11111100,
    B11111100,
    B11111100,
    B11111100,
    B11111100,
};

const game_sprite solid_sprite PROGMEM {
    BLOCK_WIDTH,
    BLOCK_HEIGHT,
    solid_lines,
};

static const uint8_t breaking1_lines[] PROGMEM {
    B01111000,
    B00110000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
};

static const uint8_t breaking2_lines[] PROGMEM {
    B11111100,
    B01111000,
    B00110000,
    B00000000,
    B00000000,
    B00000000,
};

static const uint8_t breaking3_lines[] PROGMEM {
    B11111100,
    B11111100,
    B01111000,
    B01111000,
    B00110000,
    B00110000,
};

const game_sprite breaking_sprites[BREAKING - 1] PROGMEM {
    {BLOCK_WIDTH, BLOCK_HEIGHT, breaking1_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, breaking2_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, breaking3_lines},
};

/*static const uint8_t level0[] PROGMEM {
    "......F..."
    "...RRRFG.."
    "BLBBGBBB.."
    ".L..G...G."
    "BBBBBBLBBB"
    "......LB.."
    "....P.LBRR"
    "G.LBBBBB.G"
    "..L..G...."
    "BBBBBBBBBB"
};
*/

static const uint8_t level0[] PROGMEM {
    ".....PF..."
    "SBBBBBBBSL"
    "SBBBBBBBSL"
    "SG.GBG.GSL"
    "SGGGBGGGSL"
    "SBBBBBBBSL"
    "SBBBBBBBSL"
    "SG.GBG.GSL"
    "SGGGGGGGGL"
    "SSSSSSSSSS"
};


struct LodeRunnerData
{
    int8_t player_x, player_y;
    uint8_t level;
    uint8_t level_width, level_height;
    uint8_t level_gold;
    uint8_t *level_ptr;
    uint8_t player_anim;
    uint8_t player_state;

    unsigned long anim_update;
    unsigned long move_update;
    unsigned long dig_update;
    unsigned long time;

    int8_t picked_gold_x[MAX_GOLD];
    int8_t picked_gold_y[MAX_GOLD];
    uint8_t picked_gold;

    int8_t breaking_x[MAX_BREAKING];
    int8_t breaking_y[MAX_BREAKING];
    uint8_t breaking_br[MAX_BREAKING];
    unsigned long breaking_regen[MAX_BREAKING];
};

static LodeRunnerData* data;

static uint8_t get(int8_t x, int8_t y)
{
    if (x < 0 || x >= data->level_width || y < 0 || y >= data->level_height) return 'S';
    return pgm_read_byte(&data->level_ptr[x + y * 10]);
}

static void draw_brick(int8_t x, int8_t y, int8_t br) {
    int8_t sx = SCREEN_PADDING_X + x * BLOCK_WIDTH;
    int8_t sy = SCREEN_PADDING_Y + y * BLOCK_HEIGHT;
    if (br >= BREAKING) {
        game_draw_sprite(&solid_sprite, sx, sy, BLACK);
    } else {
        game_draw_sprite(&solid_sprite, sx, sy, WHITE_DARK);
        game_draw_sprite(&brick_sprite, sx, sy, RED_DARK);
        if (br > 0) {
            game_draw_sprite(&breaking_sprites[br - 1], sx, sy, BLACK);
        }
    }
}

static void dig(int8_t x, int8_t y) {
    for (uint8_t i = 0; i < MAX_BREAKING; ++i) {
        if (data->breaking_x[i] == x && data->breaking_y[i] == y) {
            if (data->breaking_br[i] < BREAKING) {
                data->breaking_br[i]++;
                data->breaking_regen[i] = data->time + FAST_REGEN_TIMEOUT;
                if (data->breaking_br[i] == BREAKING) {
                    data->breaking_regen[i] = data->time + REGEN_TIMEOUT;
                }
                draw_brick(x, y, data->breaking_br[i]);
            }
            return;
        }
    }
    for (uint8_t i = 0; i < MAX_BREAKING; ++i) {
        if (!data->breaking_br[i]) {
            data->breaking_br[i] = 1;
            data->breaking_regen[i] = data->time + FAST_REGEN_TIMEOUT;
            data->breaking_x[i] = x;
            data->breaking_y[i] = y;
            draw_brick(x, y, 1);
            return;
        }
    }
}

static void regen() {
    for (uint8_t i = 0; i < MAX_BREAKING; ++i) {
        if (data->breaking_br[i] && data->breaking_regen[i]) {
            if (data->breaking_regen[i] > data->time) {
                continue;
            }

            if (!--data->breaking_br[i]) {
                data->breaking_regen[i] = 0;
            } else {
                data->breaking_regen[i] = data->time + FAST_REGEN_TIMEOUT;
            }
            draw_brick(data->breaking_x[i], data->breaking_y[i], data->breaking_br[i]);
        }
    }
}

static void draw_map()
{
    game_clear_screen();
    int8_t x;
    int8_t y = SCREEN_PADDING_Y;
    for (int8_t j = 0; j < SCREEN_HEIGHT; ++j) {
        x = SCREEN_PADDING_X;
        for (int8_t i = 0; i < SCREEN_WIDTH; ++i) {
            uint8_t block = get(i, j);
            const game_sprite *spr = NULL;
            uint8_t color;
            uint8_t bg = 0xff;
            switch (block) {
                case 'B':
                    spr = &brick_sprite;
                    color = RED_DARK;
                    bg = WHITE_DARK;
                    break;

                case 'S':
                    spr = &solid_sprite;
                    color = RED_DARK;
                    break;

                case 'R':
                    spr = &rope_sprite;
                    color = WHITE;
                    break;

                case 'L':
                    spr = &ladder_sprite;
                    color = WHITE;
                    break;

                case 'F':
                    if (data->level_gold == data->picked_gold) {
                        spr = &ladder_sprite;
                        color = CYAN;
                        if (j == 0) {
                            game_draw_sprite(&ladder_partial_sprite, x, 0, color);
                        }
                    }
                    break;

                case 'G':
                    spr = &gold_sprite;
                    color = YELLOW;
                    break;
            }
            if (bg != 0xff) {
                game_draw_sprite(&solid_sprite, x, y, bg);
            }
            if (spr) {
                game_draw_sprite(spr, x, y, color);
            }
            x += BLOCK_WIDTH;
        }
        y += BLOCK_HEIGHT;
    }
    for (uint8_t i = 0; i < data->picked_gold; ++i) {
        int8_t bx = data->picked_gold_x[i];
        int8_t by = data->picked_gold_y[i];
        game_draw_sprite(&solid_sprite, SCREEN_PADDING_X + BLOCK_WIDTH * bx, SCREEN_PADDING_Y + BLOCK_HEIGHT * by, BLACK);
    }
    for (uint8_t i = 0; i < MAX_BREAKING; ++i) {
        if (data->breaking_br[i]) {
            draw_brick(data->breaking_x[i], data->breaking_y[i], data->breaking_br[i]);
        }
    }
}

static bool wall_at(int8_t x, int8_t y)
{
    int8_t bx = x / BLOCK_WIDTH;
    int8_t by = y / BLOCK_HEIGHT;
    if (x % BLOCK_WIDTH != 0) {
        uint8_t c = get(bx + 1, by);
        if (c == 'S') {
            return true;
        }

        if (c == 'B') {
            bool flag = true;
            for (uint8_t i = 0; i < MAX_BREAKING; ++i) {
                if (data->breaking_x[i] == bx + 1 && data->breaking_y[i] == by) {
                    if (data->breaking_br[i] >= BREAKING) {
                        flag = false;
                        break;
                    }
                }
            }
            if (flag) {
                return true;
            }
        }
    }
    uint8_t c = get(bx, by);
    if (c == 'S') {
        return true;
    }

    if (c == 'B') {
        bool flag = true;
        for (uint8_t i = 0; i < MAX_BREAKING; ++i) {
            if (data->breaking_x[i] == bx && data->breaking_y[i] == by) {
                if (data->breaking_br[i] >= BREAKING) {
                    flag = false;
                    break;
                }
            }
        }
        if (flag) {
            return true;
        }
    }
    
    return false;
}

static bool ladder_at(int8_t x, int8_t y)
{
    int8_t bx = x / BLOCK_WIDTH;
    int8_t by = y / BLOCK_HEIGHT;
    if (x % BLOCK_WIDTH != 0) {
        uint8_t c = get(bx + 1, by);
        if (c == 'L' || (c == 'F' && data->level_gold == data->picked_gold))
            return true;
    }
    uint8_t c = get(bx, by);
    if (c == 'L' || (c == 'F' && data->level_gold == data->picked_gold))
        return true;
    return false;
}

static void nearest_ladder(int8_t x, int8_t y)
{
    int8_t bx = (x + 3) / BLOCK_WIDTH;
    int8_t by = y / BLOCK_HEIGHT;
    uint8_t c = get(bx, by);
    if (c == 'L' || (c == 'F' && data->level_gold == data->picked_gold)) {
        data->player_x = bx * BLOCK_WIDTH;
        data->player_state = LADDER;
        data->player_anim = CLIMBING0;
        return;
    }
}

static void pick_gold()
{
    int8_t bx = (data->player_x + 3) / BLOCK_WIDTH;
    int8_t by = (data->player_y + 2) / BLOCK_HEIGHT;
    uint8_t c = get(bx, by);
    if (c == 'G') {
        for (uint8_t i = 0; i < data->picked_gold; ++i) {
            if (data->picked_gold_x[i] == bx && data->picked_gold_y[i] == by) {
                return;
            }
        }
        data->picked_gold_x[data->picked_gold] = bx;
        data->picked_gold_y[data->picked_gold] = by;
        data->picked_gold++;
        draw_map();
        return;
    }
}

static bool collide(int8_t x, int8_t y)
{
    if (x < 0) return true;
    if (x + BLOCK_WIDTH > data->level_width * BLOCK_WIDTH) return true; 
    if (wall_at(x, y + BLOCK_HEIGHT - 1)) return true;
    return false;
}

static void init_level()
{
    data->level_width = 10;
    data->level_height = 10;
    data->picked_gold = 0;
    data->level_ptr = (uint8_t*)level0;
    for (int8_t i = 0; i < MAX_BREAKING; ++i) {
        data->breaking_br[i] = 0;
    }
    for (int8_t i = 0; i < data->level_width; ++i) {
        for (int8_t j = 0; j < data->level_height; ++j) {
            if (get(i, j) == 'P') {
                data->player_x = BLOCK_WIDTH * i;
                data->player_y = BLOCK_HEIGHT * j;
            }

            if (get(i, j) == 'G') {
                data->level_gold++;
            }
        }
    }
    draw_map();
}

static bool can_move()
{
    return (data->player_state == ROPE ||
            collide(data->player_x, data->player_y + 1) ||
            ladder_at(data->player_x, data->player_y + BLOCK_HEIGHT));
}

static void LodeRunner_prepare()
{
    game_enable_frame_buffer();
    data->level = 0;

    data->anim_update = 0;
    data->move_update = 0;
    data->time = 0;

    init_level();
}

static void LodeRunner_render()
{
    game_draw_sprite(&player_sprites[data->player_anim],
                    data->player_x + SCREEN_PADDING_X,
                    data->player_y + SCREEN_PADDING_Y, GREEN);
}

static void LodeRunner_update(unsigned long delta)
{
    bool left = game_is_button_pressed(BUTTON_LEFT);
    bool right = game_is_button_pressed(BUTTON_RIGHT);
    bool up = game_is_button_pressed(BUTTON_UP);
    bool down = game_is_button_pressed(BUTTON_DOWN);
    bool dig_left = game_is_button_pressed(BUTTON_B);
    bool dig_right = game_is_button_pressed(BUTTON_A);

    data->time += delta;

    bool do_anim = false;
    bool do_move = false;
    bool do_dig = false;

    if (data->time - data->anim_update > ANIM_INTERVAL) {
        data->anim_update = data->time;
        do_anim = true;
    }
    
    if (data->time - data->move_update > MOVE_INTERVAL) {
        do_move = true;
        data->move_update = data->time;
    }

    if (data->time - data->dig_update > DIG_INTERVAL) {
        do_dig = true;
    }

    if (do_dig) {
        if (dig_left ^ dig_right) {
            int8_t y = (data->player_y + BLOCK_HEIGHT) / BLOCK_HEIGHT;
            int8_t x;
            if (dig_left) {
                x = (data->player_x - 3) / BLOCK_WIDTH;
            } else {
                x = (data->player_x + BLOCK_WIDTH + 3) / BLOCK_WIDTH;
            }
            uint8_t b = get(x, y);
            if (b == 'B' && !wall_at(x * BLOCK_WIDTH, (y - 1) * BLOCK_HEIGHT)) {
                data->dig_update = data->time;
                dig(x, y);
            }
        }
    }

    if (do_move) {
        if (data->player_state != ROPE) {
            if (!collide(data->player_x, data->player_y + 1) && !ladder_at(data->player_x, data->player_y + BLOCK_HEIGHT)) {
                data->player_y++;
            }
        }

        if (can_move() && (up ^ down)) {
            if (up) {
                if (data->player_state != LADDER) {
                    nearest_ladder(data->player_x, data->player_y);
                }

                if (data->player_state == LADDER) {
                    if (data->player_anim == CLIMBING0) data->player_anim = CLIMBING1;
                    else if (data->player_anim == CLIMBING1) data->player_anim = CLIMBING0;
                    data->player_y--;
                    if (!ladder_at(data->player_x, data->player_y + BLOCK_HEIGHT - 1)) {
                        data->player_state = LAND;
                        data->player_anim = STANDING;
                    }
                }
            } else {
                if (data->player_state != LADDER) {
                    nearest_ladder(data->player_x, data->player_y + BLOCK_HEIGHT);
                }

                if (data->player_state == LADDER) {
                    if (data->player_anim == CLIMBING0) data->player_anim = CLIMBING1;
                    else if (data->player_anim == CLIMBING1) data->player_anim = CLIMBING0;
                    if (!ladder_at(data->player_x, data->player_y + BLOCK_HEIGHT)) {
                        data->player_state = LAND;
                        data->player_anim = STANDING;
                    } else {
                        data->player_y++;
                    }
                }

                if (data->player_state == ROPE) {
                    data->player_state = LAND;
                    data->player_anim = STANDING;
                    if (!collide(data->player_x, data->player_y + 1) && !ladder_at(data->player_x, data->player_y + BLOCK_HEIGHT)) {
                        data->player_y++;
                    }
                }
            }
        }
    }

    if (left ^ right) {
        if (data->player_state == LAND) {
            if (do_anim) {
                if (data->player_anim == WALKING0) data->player_anim = WALKING1;
                else if (data->player_anim == WALKING1) data->player_anim = WALKING0;
            }

            if (do_move && can_move()) {
                if (data->player_anim == STANDING) {
                    data->player_anim = WALKING0;
                }
                if (left) {
                    if (!collide(data->player_x - 1, data->player_y)) {
                        data->player_x--;
                    }
                } else {
                    if (!collide(data->player_x + 1, data->player_y)) {
                        data->player_x++;
                    }
                }
            }
        }

        if (data->player_state == LADDER) {
            if (left) {
                if (!collide(data->player_x - 1, data->player_y) && !collide(data->player_x - 1, data->player_y - BLOCK_HEIGHT + 1)) {
                    data->player_x--;
                    data->player_state = LAND;
                    data->player_anim = WALKING0;
                }
            } else {
                if (!collide(data->player_x + 1, data->player_y) && !collide(data->player_x + 1, data->player_y - BLOCK_HEIGHT + 1)) {
                    data->player_x++;
                    data->player_state = LAND;
                    data->player_anim = WALKING0;
                }
            }
        }

        if (data->player_state == ROPE) {
            if (do_anim) {
                if (data->player_anim == ROPECLIMBING0) data->player_anim = ROPECLIMBING1;
                else if (data->player_anim == ROPECLIMBING1) data->player_anim = ROPECLIMBING0;
            }

            if (do_move) {
                if (data->player_anim == HANGING) {
                    data->player_anim = ROPECLIMBING0;
                }
                if (left) {
                    if (!collide(data->player_x - 1, data->player_y)) {
                        data->player_x--;
                    }
                } else {
                    if (!collide(data->player_x + 1, data->player_y)) {
                        data->player_x++;
                    }
                }
            }
        }
    }

    if (data->player_state == LAND && !(left ^ right)) {
        data->player_anim = STANDING;
    }
    
    if (data->player_state != ROPE) {
        int8_t bx = data->player_x / BLOCK_WIDTH;
        int8_t bx2 = (data->player_x + BLOCK_WIDTH - 1) / BLOCK_WIDTH;
        int8_t by = data->player_y / BLOCK_HEIGHT;
        if (by * BLOCK_HEIGHT == data->player_y && get(bx, by) == 'R' && get(bx2, by) == 'R') {
            data->player_state = ROPE;
            data->player_anim = HANGING;
        }
    } else {
        int8_t bx = data->player_x / BLOCK_WIDTH;
        int8_t bx2 = (data->player_x + BLOCK_WIDTH - 1) / BLOCK_WIDTH;
        int8_t by = data->player_y / BLOCK_HEIGHT;
        if (by * BLOCK_HEIGHT == data->player_y && (get(bx, by) != 'R' || get(bx2, by) != 'R')) {
            data->player_state = LAND;
            data->player_anim = STANDING;
        }
    }

    if (do_move) {
        pick_gold();
    }

    if (do_dig) {
        regen();
    }
}

const game_instance LodeRunner PROGMEM {
    "LodeRunner",
    LodeRunner_prepare,
    LodeRunner_render,
    LodeRunner_update,
    sizeof(LodeRunnerData),
    (void**)(&data)
};

#endif
