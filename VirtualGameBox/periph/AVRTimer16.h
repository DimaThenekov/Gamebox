//
// Created by tsarn on 7/12/18.
//

#ifndef VIRTUALGAMEBOX_AVRTIMER16_H
#define VIRTUALGAMEBOX_AVRTIMER16_H


#include "../AVRRegisterBank.h"

class AVRTimer16 {
private:
    AVRRegisterBank &rbank;

public:
    uint16_t cnt, icr;
    uint8_t irq, mask;
    uint8_t ctrl0, ctrl1, ctrl2;

    uint8_t irq_ovf;

    unsigned int ticks;

public:
    AVRTimer16(AVRRegisterBank &rbank, uint8_t irq_ovf);

    void tick(unsigned int cycles);

    void reset();
};


#endif //VIRTUALGAMEBOX_AVRTIMER16_H
