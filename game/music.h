#ifndef MUSIC_H
#define MUSIC_H

#include <stdint.h>

// base YM functions
void music_setup(void);
void music_send_data(uint8_t addr, uint8_t data);

// fxm must point to the program memory
void fxm_init(const uint8_t *fxm);
// enables interrupt-driven playing
void fxm_enable_int();
void fxm_loop();

#endif
