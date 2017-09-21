#include "music.h"

#include <util/delay.h>
#include <avr/io.h>

// +5V is connected to BC2
// MSB (PA1) is connected to BDIR
// LSB (PA0) is connected to BC1
#define DATA_READ 0x01
#define DATA_WRITE 0x02
#define ADDRESS_MODE 0x03

void music_setup(void) {
  DDRA |= 3;
  DDRC = 0xff;
}

static void set_address(char addr) {
  PORTA = (PORTA & ~3) | ADDRESS_MODE;
  PORTC = addr;
  _delay_us(1.); //tAS = 300ns
  PORTA = (PORTA & ~3); // inactive
  _delay_us(1.); //tAH = 80ns
}

static void set_data(uint8_t data) {
  PORTA = (PORTA & ~3) | DATA_WRITE;
  PORTC = data;
  _delay_us(1.); // 300ns < tDW < 10us
  PORTA = (PORTA & ~3); // inactive
  _delay_us(1.); // tDH = 80ns
}

void music_send_data(uint8_t addr, uint8_t data) {
  set_address(addr);
  set_data(data);
}

