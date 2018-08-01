#include "AVRRegisterBank.h"
#include "atmega2560.h"

AVRRegisterBank::AVRRegisterBank() {
    reset();
}

void AVRRegisterBank::reset() {
    for (int i = 0; i < AMOUNT; ++i) {
        regs[i] = 0x00;
    }
    sreg = 0;
    sp = 0x21ff;
    rampz = 0;
    eind = 0;
    pc = 0;
    irq = 0;
    cycles = 0;
}

unsigned int AVRRegisterBank::get(AVRRegister reg) {
    switch (reg) {
        case AVRRegister::SREG:
            return sreg;
        case AVRRegister::SP:
            return sp;
        case AVRRegister::RAMPZ:
            return rampz;
        case AVRRegister::EIND:
            return eind;
        case AVRRegister::PC:
            return pc;

        case AVRRegister::X:
            return regs[26] | (static_cast<unsigned int>(regs[27]) << 8);
        case AVRRegister::Y:
            return regs[28] | (static_cast<unsigned int>(regs[29]) << 8);
        case AVRRegister::Z:
            return regs[30] | (static_cast<unsigned int>(regs[31]) << 8);
        default:
            return regs[static_cast<unsigned int>(reg)];
    }
}

void AVRRegisterBank::set(AVRRegister reg, unsigned int value) {
    switch (reg) {
        case AVRRegister::SREG:
            sreg = static_cast<uint8_t>(value);
            break;
        case AVRRegister::SP:
            sp = static_cast<uint16_t>(value);
            break;
        case AVRRegister::RAMPZ:
            rampz = static_cast<uint8_t>(value);
            break;
        case AVRRegister::EIND:
            eind = static_cast<uint8_t>(value);
            break;
        case AVRRegister::PC:
            pc = value % FLASH_SIZE;
            break;

        case AVRRegister::X:
            regs[26] = static_cast<uint8_t>(value & 0xff);
            regs[27] = static_cast<uint8_t>(value >> 8);
            break;
        case AVRRegister::Y:
            regs[28] = static_cast<uint8_t>(value & 0xff);
            regs[29] = static_cast<uint8_t>(value >> 8);
            break;
        case AVRRegister::Z:
            regs[30] = static_cast<uint8_t>(value & 0xff);
            regs[31] = static_cast<uint8_t>(value >> 8);
            break;
        default:
            regs[static_cast<unsigned int>(reg)] = static_cast<uint8_t>(value);
    }
}

bool AVRRegisterBank::getSREG(AVRStatusBit bit) {
    return static_cast<bool>((sreg >> static_cast<unsigned int>(bit)) & 1);
}

void AVRRegisterBank::setSREG(AVRStatusBit bit, bool value) {
    if (value) {
        sreg |= (1 << static_cast<unsigned int>(bit));
    } else {
        sreg &= ~(1 << static_cast<unsigned int>(bit));
    }
}

void AVRRegisterBank::debug() {
    /*fprintf(stderr, "--REGISTERS--\n");
    for (int i = 0; i < 32; ++i) {
        fprintf(stderr, "R%d = 0x%02x\n", i, regs[i]);
    }
    fprintf(stderr, "(PC) = 0x%06x", pc);*/
    fprintf(stderr, "(R8:R11 - start) = %d\n", *(uint32_t*)(&regs[0] + 8));
    fprintf(stderr, "(R12:R15 - ms) = %d\n", *(uint32_t*)(&regs[0] + 12));
    fprintf(stderr, "(R24:R27 - micros()) = %d\n", *(uint32_t*)(&regs[0] + 24));
}

void AVRRegisterBank::triggerIRQ(unsigned int irqn) {
    irq |= (1ull << irqn);
}

bool AVRRegisterBank::hasIRQ() {
    return (sreg & 0x80) && (irq != 0);
}

unsigned int AVRRegisterBank::getIRQ() {
    unsigned long long res = __builtin_ctzll(irq);
    irq &= ~(1ull << res);
    return res;
}

void AVRRegisterBank::cancelIRQ(unsigned int irqn) {
    irq &= ~(1ull << irqn);
}

void AVRRegisterBank::tick(unsigned long long ticks) {
    cycles += ticks;
}

unsigned long long AVRRegisterBank::getCycles() {
    return cycles;
}
