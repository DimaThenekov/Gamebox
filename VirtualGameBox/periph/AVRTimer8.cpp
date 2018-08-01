//
// Created by tsarn on 7/10/18.
//

#include "AVRTimer8.h"

AVRTimer8::AVRTimer8(AVRRegisterBank &rbank, uint8_t irq_ovf) :
        rbank(rbank),
        irq_ovf(irq_ovf) {
    reset();
}

void AVRTimer8::reset() {
    cnt = 0;
    irq = mask = 0;
    ctrl0 = ctrl1 = 0;
    ticks = 0;
}

void AVRTimer8::tick(unsigned int cycles) {
    unsigned int prescaler;
    switch (ctrl1 & 7) {
        case 1: prescaler = 1; break;
        case 2: prescaler = 8; break;
        case 3: prescaler = 64; break;
        case 4: prescaler = 256; break;
        case 5: prescaler = 1024; break;
        default: return;
    }
    ticks += cycles;
    if (ticks >= prescaler) {
        unsigned int triggers = ticks / prescaler;
        ticks %= prescaler;

        for (int i = 0; i < triggers; ++i) {
            cnt++;

            if (cnt == 0) {
                // overflow

                if (mask & 0x01) {
                    irq |= 0x01;
                    rbank.triggerIRQ(irq_ovf);
                }
            }
        }
    }
}