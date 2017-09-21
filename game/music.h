#ifndef MUSIC_H
#define MUSIC_H

#include <stdint.h>

// base YM functions
void music_setup(void);
void music_send_data(uint8_t addr, uint8_t data);

// fxm must point to the program memory
void fxm_init(const uint8_t *fxm);
void fxm_loop();

#endif
