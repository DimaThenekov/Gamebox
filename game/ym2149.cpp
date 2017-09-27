#include "music.h"

#include <util/delay.h>
#include <avr/io.h>

// +5V is connected to BC2
// MSB (A7) is connected to BDIR
// LSB (A6) is connected to BC1
#define DATA_READ (0x01 << 6)
#define DATA_WRITE (0x02 << 6)
#define ADDRESS_MODE (0x03 << 6)
#define MASK 0xc0

void music_setup(void) {
  DDRF |= MASK;
  DDRC = 0xff;
}

static void set_address(char addr) {
  PORTF = (PORTF & ~MASK) | ADDRESS_MODE;
  PORTC = addr;
  _delay_us(1.); //tAS = 300ns
  PORTF = (PORTF & ~MASK); // inactive
  _delay_us(1.); //tAH = 80ns
}

static void set_data(uint8_t data) {
  PORTF = (PORTF & ~MASK) | DATA_WRITE;
  PORTC = data;
  _delay_us(1.); // 300ns < tDW < 10us
  PORTF = (PORTF & ~MASK); // inactive
  _delay_us(1.); // tDH = 80ns
}

void music_send_data(uint8_t addr, uint8_t data) {
  set_address(addr);
  set_data(data);
}

