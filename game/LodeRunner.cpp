#pragma GCC optimize ("-O3")

#include "libgame.h"
#include "graphics.h"
#include "binary.h"
#include "controls.h"

#ifdef FRAME_BUFFER

#define BLOCK_WIDTH 6
#define BLOCK_HEIGHT 6

#define SCREEN_WIDTH 10
#define SCREEN_HEIGHT 10
#define SCREEN_PADDING_X 2
#define SCREEN_PADDING_Y 2

#define ANIM_INTERVAL 100
#define MOVE_INTERVAL 50

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
    {BLOCK_WIDTH, BLOCK_HEIGHT, 1, player_standing_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, 1, player_walking0_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, 1, player_walking1_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, 1, player_hanging_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, 1, player_climbing0_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, 1, player_climbing1_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, 1, player_ropeclimbing0_lines},
    {BLOCK_WIDTH, BLOCK_HEIGHT, 1, player_ropeclimbing1_lines},
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
    1,
    ladder_lines,
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
    1,
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
    1,
    rope_lines,
};

static const uint8_t gold_lines[] PROGMEM {
    B00000000,
    B00000000,
    B11111100,
    B10000100,
    B10000100,
    B11111100,
};

const game_sprite gold_sprite PROGMEM {
    BLOCK_WIDTH,
    BLOCK_HEIGHT,
    1,
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
    1,
    solid_lines,
};

static const uint8_t level0[] PROGMEM {
    ".........."
    "...RRR...."
    "BLBB.BBB.."
    ".L........"
    "BBBBBBLBBB"
    "......L..."
    "....P.LBRR"
    "..LBBBBB.."
    "..L......."
    "BBBBBBBBBB"
};


struct LodeRunnerData
{
    int8_t player_x, player_y;
    uint8_t level;
    uint8_t level_width, level_height;
    uint8_t *level_ptr;
    int8_t camera_x, camera_y;
    uint8_t player_anim;
    uint8_t player_state;

    unsigned long anim_update;
    unsigned long move_update;
    unsigned long time;
};
static LodeRunnerData* data;

inline static uint8_t get(int8_t x, int8_t y) {
    if (x < 0 || x >= data->level_width || y < 0 || y >= data->level_height) return 'S';
    return pgm_read_byte(&data->level_ptr[x + y * 10]);
}

inline static void draw_map()
{
    game_clear_screen();
    int8_t x;
    int8_t y = SCREEN_PADDING_Y;
    for (int8_t j = data->camera_y; j < data->camera_y + SCREEN_HEIGHT; ++j) {
        x = SCREEN_PADDING_X;
        for (int8_t i = data->camera_x; i < data->camera_x + SCREEN_WIDTH; ++i) {
            uint8_t block = get(i, j);
            const game_sprite *spr = NULL;
            uint8_t color;
            switch (block) {
                case 'B':
                    spr = &brick_sprite;
                    color = RED_DARK;
                    break;

                case 'S':
                    spr = &solid_sprite;
                    color = RED_DARK;
                    break;

                case 'R':
                    spr = &rope_sprite;
                    color = WHITE_DARK;
                    break;

                case 'L':
                    spr = &ladder_sprite;
                    color = WHITE_DARK;
                    break;
            }
            if (spr) {
                game_draw_sprite(spr, x, y, color);
            }
            x += BLOCK_WIDTH;
        }
        y += BLOCK_HEIGHT;
    }
}

static inline bool wall_at(int8_t x, int8_t y)
{
    int8_t bx = x / BLOCK_WIDTH;
    int8_t by = y / BLOCK_HEIGHT;
    if (x % BLOCK_WIDTH != 0) {
        uint8_t c = get(bx + 1, by);
        if (c == 'B' || c == 'S')
            return true;
    }
    uint8_t c = get(bx, by);
    return (c == 'B' || c == 'S');
}

static inline bool ladder_at(int8_t x, int8_t y)
{
    int8_t bx = x / BLOCK_WIDTH;
    int8_t by = y / BLOCK_HEIGHT;
    if (x % BLOCK_WIDTH != 0) {
        uint8_t c = get(bx + 1, by);
        if (c == 'L')
            return true;
    }
    uint8_t c = get(bx, by);
    return (c == 'L');
}

static inline void nearest_ladder(int8_t x, int8_t y)
{
    int8_t bx = (x + 3) / BLOCK_WIDTH;
    int8_t by = y / BLOCK_HEIGHT;
    uint8_t c = get(bx, by);
    if (c == 'L') {
        data->player_x = bx * BLOCK_WIDTH;
        data->player_state = LADDER;
        data->player_anim = CLIMBING0;
        return;
    }
}

static inline bool collide(int8_t x, int8_t y) {
    if (x < 0) return true;
    if (x + BLOCK_WIDTH > data->level_width * BLOCK_WIDTH) return true; 
    if (wall_at(x, y + BLOCK_HEIGHT - 1)) return true;
    return false;
}

static void init_level()
{
    data->level_width = 10;
    data->level_height = 10;
    data->level_ptr = (uint8_t*)level0;
    for (int i = 0; i < data->level_width; ++i) {
        for (int j = 0; j < data->level_height; ++j) {
            if (get(i, j) == 'P') {
                data->player_x = BLOCK_WIDTH * i;
                data->player_y = BLOCK_HEIGHT * j;
                break;
            }
        }
    }
    draw_map();
}

static bool can_move() {
    return (data->player_state == ROPE ||
            collide(data->player_x, data->player_y + 1) ||
            ladder_at(data->player_x, data->player_y + BLOCK_HEIGHT));
}

static void LodeRunner_prepare()
{
    game_enable_frame_buffer();
    data->level = 0;
    data->camera_x = 0;
    data->camera_y = 0;

    data->anim_update = 0;
    data->move_update = 0;
    data->time = 0;

    init_level();
}

static void LodeRunner_render()
{
    game_draw_sprite(&player_sprites[data->player_anim],
                    data->player_x + SCREEN_PADDING_X,
                    data->player_y + SCREEN_PADDING_Y, WHITE);
}

static void LodeRunner_update(unsigned long delta)
{
    bool left = game_is_button_pressed(BUTTON_LEFT);
    bool right = game_is_button_pressed(BUTTON_RIGHT);
    bool up = game_is_button_pressed(BUTTON_UP);
    bool down = game_is_button_pressed(BUTTON_DOWN);

    data->time += delta;

    bool do_anim = false;
    bool do_move = false;

    if (data->time - data->anim_update > ANIM_INTERVAL) {
        data->anim_update = data->time;
        do_anim = true;
    }
    
    if (data->time - data->move_update > MOVE_INTERVAL) {
        do_move = true;
        data->move_update = data->time;
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
}

game_instance LodeRunner = {
    "LodeRunner",
    LodeRunner_prepare,
    LodeRunner_render,
    LodeRunner_update,
    sizeof(LodeRunnerData),
    (void**)(&data)
};

#endif
