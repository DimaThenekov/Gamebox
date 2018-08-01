#ifndef AVRIO_H
#define AVRIO_H

#include <cstdint>
#include "AVRRegisterBank.h"
#include "periph/Display.h"
#include "periph/AVRTimer8.h"
#include "periph/AVRTimer16.h"


class AVRIO {
private:
    AVRRegisterBank &rbank;
    Display &display;

    AVRGPIOA gpioa;
    AVRGPIOB gpiob;
    AVRGPIOE gpioe;
    AVRGPIOF gpiof;
    AVRGPIOK gpiok;
    AVRTimer8 timer0;
    AVRTimer16 timer1, timer3;

public:
    AVRIO(AVRRegisterBank &rbank, Display &display);

    void set(uint16_t reg, uint8_t value);

    uint8_t get(uint16_t reg);

    void clearIRQ(unsigned int irq);

    void reset();

    void tick(unsigned int cycles);
};

#endif // AVRIO_H
