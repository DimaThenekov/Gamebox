#ifndef CONFIG_H
#define CONFIG_H

#ifdef EMULATED
#define PROGMEM 
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <stdint.h>
#include <string.h>
using namespace std;
#else
#include <Arduino.h>
#include <avr/pgmspace.h>
#endif

#ifdef EMULATED
#define pgm_read_byte(addr) (*(addr))
#define pgm_read_word(addr) (*(addr))
#define pgm_read_dword(addr) (*(addr))
#define pgm_read_byte_near(addr) (*((uint8_t*)(addr)))
#define pgm_read_word_near(addr) (*((uint16_t*)(addr)))
#define pgm_read_dword_near(addr) (*((uint32_t*)(addr)))
#define pgm_read_pointer(addr) (*(addr))
#else
#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
#define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif
#endif

// Button input
#ifdef ARDUINO_AVR_MEGA2560
#define CTL_CLOCK A8
#define CTL_LATCHB A9
#define CTL_LATCHJ A10
#define CTL_DATA A11
#else
#define CTL_CLOCK 11
#define CTL_LATCHB 12
#define CTL_LATCHJ 13
#define CTL_DATA A4
#endif

// Graphics defines
#ifdef ARDUINO_AVR_MEGA2560
#define GFX_PORT2
#define GFX_DATAPORT PORTB
#define GFX_DATADIR  DDRB
#define GFX_DATAPORT2 PORTA
#define GFX_DATADIR2  DDRA
#define GFX_SCLKPORT PORTE
#else
#define GFX_DATAPORT PORTD
#define GFX_DATADIR  DDRD
#define GFX_SCLKPORT PORTB
#endif

#ifdef ARDUINO_AVR_MEGA2560
// PORTB
#define GFX_IR1 53
#define GFX_IG1 52
#define GFX_IB1 51
// PORTA
#define GFX_IR2 22
#define GFX_IG2 23
#define GFX_IB2 24
// PORTE
#define GFX_CLK 5
#define GFX_LAT 2
#define GFX_OE  3
#else
// PORTD
#define GFX_IR1 2
#define GFX_IG1 3
#define GFX_IB1 4
#define GFX_IR2 5
#define GFX_IG2 6
#define GFX_IB2 7
// PORTB
#define GFX_CLK 8  // ??? MUST be on PORTB! (Use pin 11 on Mega)
#define GFX_LAT 10
#define GFX_OE  9
#endif
#define GFX_A   A0
#define GFX_B   A1
#define GFX_C   A2
#define GFX_D   A3
// 5-bit address bus support
//#define GFX_E   A5

// Whether to use 6-bit color palette
// TODO: is not supported in FRAME BUFFER mode
#define COLOR_6BIT 0


#ifdef ARDUINO_AVR_MEGA2560
// defined if frame buffer is supported
#define FRAME_BUFFER
#endif

#endif

