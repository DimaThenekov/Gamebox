//
// Created by tsarn on 7/10/18.
//

#ifndef VIRTUALGAMEBOX_AVRTIMER8_H
#define VIRTUALGAMEBOX_AVRTIMER8_H


#include <cstdint>
#include "../AVRRegisterBank.h"

class AVRTimer8 {
private:
    AVRRegisterBank &rbank;

public:
    uint8_t cnt;
    uint8_t irq, mask;
    uint8_t ctrl0, ctrl1;

    uint8_t irq_ovf, irq_compa, irq_compb;

    unsigned int ticks;

public:
    AVRTimer8(AVRRegisterBank &rbank, uint8_t irq_ovf);

    void tick(unsigned int cycles);

    void reset();
};


#endif //VIRTUALGAMEBOX_AVRTIMER8_H
