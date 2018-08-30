#ifndef MUSIC_H
#define MUSIC_H

#include <stdint.h>

/* This is one of the last section and it won't push other progmem data
   to the upper addresses */
#define MUSICMEM __attribute__((section(".fini7")))

#define AY_TONA  0
#define AY_TONB  2
#define AY_TONC  4
#define AY_NOISE 6
#define AY_MIXER 7
#define AY_AMPA  8
#define AY_AMPB  9
#define AY_AMPC  10
#define AY_ENV   11
#define AY_ENVT  13

// base YM functions
void music_setup(void);
void music_send_data(uint8_t addr, uint8_t data);
void music_set_ram(uint32_t p, uint16_t offs);
void music_set_ay_reg(uint8_t reg, uint8_t val);
void music_send_ay();
void music_clear_ay();
uint8_t music_peek(uint16_t addr);
uint16_t music_peek2(uint16_t addr);

// Public interface
void tune_init(uint32_t tune);
void tune_enable();
void tune_disable();
void music_mute();

void Player_setup_random_melody();

#endif
