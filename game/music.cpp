#pragma GCC optimize ("-O3")
#include <Arduino.h>
#include <avr/pgmspace.h>
#include "music.h"

static uint32_t ram;
static uint16_t ram_addr;
static bool mute;
static uint8_t ayregs[14];
static bool fxm;

bool fxm_init(uint32_t fxm);
void fxm_loop();
bool pt3_init(uint32_t pt3);
void pt3_loop();


uint8_t music_peek(uint16_t addr)
{
    return pgm_read_byte(ram + addr - ram_addr);
}

uint16_t music_peek2(uint16_t addr)
{
    return music_peek(addr) + (music_peek(addr + 1) << 8);
}

void music_set_ram(uint32_t p, uint16_t offs)
{
    ram = p;
    ram_addr = offs;
}

void music_set_ay_reg(uint8_t reg, uint8_t val)
{
    ayregs[reg] = val;
}

void music_clear_ay()
{
    for (int i = 13 ; i >= 0 ; --i)
    {
        ayregs[i] = 0;
        music_send_data(i, 0);
    }
}

void music_send_ay()
{
    for (int i = 13 ; i >= 0 ; --i)
        music_send_data(i, ayregs[i]);
}

void tune_init(uint32_t tune)
{
    noInterrupts();

    if (fxm_init(tune))
    {
        fxm = true;
        // init hardware YM
        music_setup();
        interrupts();
        return;
    }
    else if (pt3_init(tune))
    {
        fxm = false;
        // init hardware YM
        music_setup();
        interrupts();
        return;
    }
}

void tune_enable()
{
    // Set up Timer3 for interrupt:
    TCCR3A  = _BV(WGM31); // Mode 14 (fast PWM), OC3A off
    TCCR3B  = _BV(WGM33) | _BV(WGM32) | _BV(CS32); // Mode 14, div 256
    ICR3    = 0;
    TIMSK3 |= _BV(TOIE3); // Enable Timer3 interrupt
}

// ISR_BLOCK important
ISR(TIMER3_OVF_vect, ISR_BLOCK)
{
    if (!mute && ram)
    {
        if (fxm)
            fxm_loop();
        else
            pt3_loop();
    }
    TIFR3 |= TOV3;                  // Clear Timer3 interrupt flag
    ICR3      = 1250;        // Set interval for next interrupt
    TCNT3     = 0;        // Restart interrupt timer
}

void tune_disable()
{
    TIMSK3 &= ~_BV(TOIE3); // Disable Timer3 interrupt
    music_clear_ay();
}

void music_mute()
{
    mute = !mute;
    if (mute)
        music_clear_ay();
}
