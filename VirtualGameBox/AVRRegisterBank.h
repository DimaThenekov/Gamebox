#ifndef AVRREGISTERBANK_H
#define AVRREGISTERBANK_H

#include <array>
#include "AVRRegister.h"
#include <cstdint>


class AVRRegisterBank {
public:
    static const int AMOUNT = 32;

private:
    std::array<uint8_t, AMOUNT> regs;
    uint8_t sreg;
    uint16_t sp;
    uint8_t rampz;
    uint8_t eind;
    uint32_t pc;
    uint64_t irq;
    unsigned long long cycles;

public:
    AVRRegisterBank();

    unsigned int get(AVRRegister reg);

    void set(AVRRegister reg, unsigned int value);

    bool getSREG(AVRStatusBit bit);

    void setSREG(AVRStatusBit bit, bool value);

    void reset();

    void debug();

    void triggerIRQ(unsigned int irqn);

    void cancelIRQ(unsigned int irqn);

    bool hasIRQ();

    unsigned int getIRQ();

    void tick(unsigned long long ticks);

    unsigned long long getCycles();
};

#endif // AVRREGISTERBANK_H
