#ifndef MUSIC_H
#define MUSIC_H

#include <stdint.h>

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
void music_set_ram(const uint8_t *p, uint16_t offs);
void music_set_ay_reg(uint8_t reg, uint8_t val);
void music_send_ay();
void music_clear_ay();
uint8_t music_peek(uint16_t addr);
uint16_t music_peek2(uint16_t addr);

// Public interface
void tune_init(const uint8_t *tune);
void tune_enable();
void tune_disable();
void music_mute();

/*
// fxm must point to the program memory
void fxm_init(const uint8_t *fxm);
// enables interrupt-driven playing
void fxm_enable();
// disables sound and interrupts
void fxm_disable();
void fxm_mute();

// pt3 must point to the program memory
void pt3_init(const uint8_t *pt3);
// enables interrupt-driven playing
void pt3_enable();
// disables sound and interrupts
void pt3_disable();
void pt3_mute();
*/
// TODO: improve this interface
void Player_setup_melody(int i);
void Player_setup_random_melody();

#endif
