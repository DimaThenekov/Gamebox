//
// Created by tsarn on 7/10/18.
//

#ifndef VIRTUALGAMEBOX_AVRGPIO_H
#define VIRTUALGAMEBOX_AVRGPIO_H


#include <cstdint>
#include "../AVRRegisterBank.h"

class AVRGPIO {
protected:
    uint8_t data = 0, direction = 0;

protected:
    virtual bool getPinValue(int pin);

    virtual void setPinValue(int pin, bool value);

public:
    uint8_t pinRead();

    void pinWrite(uint8_t value);

    uint8_t ddrRead();

    void ddrWrite(uint8_t value);

    uint8_t portRead();

    void portWrite(uint8_t value);
};


#endif //VIRTUALGAMEBOX_AVRGPIO_H
