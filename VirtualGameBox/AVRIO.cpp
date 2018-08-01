#include "AVRIO.h"

void AVRIO::set(uint16_t reg, uint8_t value) {
    switch (reg) {
        case 0x20:
            gpioa.pinWrite(value);
            return;

        case 0x21:
            gpioa.ddrWrite(value);
            return;

        case 0x22:
            gpioa.portWrite(value);
            return;

        case 0x23:
            gpiob.pinWrite(value);
            return;

        case 0x24:
            gpiob.ddrWrite(value);
            return;

        case 0x25:
            gpiob.portWrite(value);
            return;

        case 0x2c:
            gpioe.pinWrite(value);
            return;

        case 0x2d:
            gpioe.ddrWrite(value);
            return;

        case 0x2e:
            gpioe.portWrite(value);
            return;

        case 0x2f:
            gpiof.pinWrite(value);
            return;

        case 0x30:
            gpiof.ddrWrite(value);
            return;

        case 0x31:
            gpiof.portWrite(value);
            return;

        case 0x106:
            gpiok.pinWrite(value);
            return;

        case 0x107:
            gpiok.ddrWrite(value);
            return;

        case 0x108:
            gpiok.portWrite(value);
            return;

        case 0x35:
            timer0.irq &= ~value;
            if (!(timer0.irq & 1)) rbank.cancelIRQ(24);
            return;

        case 0x44:
            timer0.ctrl0 = value;
            return;

        case 0x45:
            timer0.ctrl1 = value;
            return;

        case 0x46:
            timer0.cnt = value;
            return;

        case 0x6e:
            timer0.mask = value;
            return;

        case 0x36:
            timer1.irq &= ~value;
            if (!(timer1.irq & 1)) rbank.cancelIRQ(21);
            return;

        case 0x80:
            timer1.ctrl0 = value;
            return;

        case 0x81:
            timer1.ctrl1 = value;
            return;

        case 0x82:
            timer1.ctrl2 = value;
            return;

        case 0x84:
            timer1.cnt = (timer1.cnt & 0xff00) | value;
            return;

        case 0x85:
            timer1.cnt = (timer1.cnt & 0x00ff) | (value << 8);
            return;

        case 0x86:
            timer1.icr = (timer1.icr & 0xff00) | value;
            return;

        case 0x87:
            timer1.icr = (timer1.icr & 0x00ff) | (value << 8);
            return;

        case 0x6f:
            timer1.mask = value;
            return;

        case 0x38:
            timer3.irq &= ~value;
            if (!(timer3.irq & 1)) rbank.cancelIRQ(36);
            return;

        case 0x90:
            timer3.ctrl0 = value;
            return;

        case 0x91:
            timer3.ctrl1 = value;
            return;

        case 0x92:
            timer3.ctrl2 = value;
            return;

        case 0x94:
            timer3.cnt = (timer1.cnt & 0xff00) | value;
            return;

        case 0x95:
            timer3.cnt = (timer1.cnt & 0x00ff) | (value << 8);
            return;

        case 0x96:
            timer3.icr = (timer3.icr & 0xff00) | value;
            return;

        case 0x97:
            timer3.icr = (timer3.icr & 0x00ff) | (value << 8);
            return;

        case 0x71:
            timer3.mask = value;
            return;

        case 0x58:
            // reserved, but we use it for display options

            display.options = value;
            return;

        case 0x5b:
            // RAMPZ

            rbank.set(AVRRegister::RAMPZ, value);
            return;

        case 0x5c:
            // EIND
            rbank.set(AVRRegister::EIND, value);
            return;

        case 0x5d:
            // SPL

            rbank.set(AVRRegister::SP, (rbank.get(AVRRegister::SP) & 0xff00) | value);
            return;

        case 0x5e:
            // SPH

            rbank.set(AVRRegister::SP, (rbank.get(AVRRegister::SP) & 0xff) | (static_cast<unsigned int>(value) << 8));
            return;

        case 0x5f:
            // SREG

            rbank.set(AVRRegister::SREG, value);
            return;
    }
}

uint8_t AVRIO::get(uint16_t reg) {
    switch (reg) {
        case 0x20:
            return gpioa.pinRead();

        case 0x21:
            return gpioa.ddrRead();

        case 0x22:
            return gpioa.portRead();

        case 0x23:
            return gpiob.pinRead();

        case 0x24:
            return gpiob.ddrRead();

        case 0x25:
            return gpiob.portRead();

        case 0x2c:
            return gpioe.pinRead();

        case 0x2d:
            return gpioe.ddrRead();

        case 0x2e:
            return gpioe.portRead();

        case 0x2f:
            return gpiof.pinRead();

        case 0x30:
            return gpiof.ddrRead();

        case 0x31:
            return gpiof.portRead();

        case 0x106:
            return gpiok.pinRead();

        case 0x107:
            return gpiok.ddrRead();

        case 0x108:
            return gpiok.portRead();

        case 0x35:
            return timer0.irq;

        case 0x44:
            return timer0.ctrl0;

        case 0x45:
            return timer0.ctrl1;

        case 0x46:
            return timer0.cnt;

        case 0x6e:
            return timer0.mask;

        case 0x36:
            return timer1.irq;

        case 0x80:
            return timer1.ctrl0;

        case 0x81:
            return timer1.ctrl1;

        case 0x82:
            return timer1.ctrl2;

        case 0x84:
            return timer1.cnt & 0xff;

        case 0x85:
            return timer1.cnt >> 8;

        case 0x6f:
            return timer1.mask;

        case 0x38:
            return timer3.irq;

        case 0x90:
            return timer3.ctrl0;

        case 0x91:
            return timer3.ctrl1;

        case 0x92:
            return timer3.ctrl2;

        case 0x94:
            return timer3.cnt & 0xff;

        case 0x95:
            return timer3.cnt >> 8;

        case 0x71:
            return timer3.mask;

        case 0x58:
            // reserved, but we use it for display options

            return display.options;

        case 0x5b:
            // RAMPZ

            return rbank.get(AVRRegister::RAMPZ);

        case 0x5c:
            // EIND

            return rbank.get(AVRRegister::EIND);

        case 0x5d:
            // SPL

            return rbank.get(AVRRegister::SP) & 0xff;

        case 0x5e:
            // SPH

            return (rbank.get(AVRRegister::SP) >> 8) & 0xff;

        case 0x5f:
            // SREG

            return rbank.get(AVRRegister::SREG);
    }
    return 0;
}

void AVRIO::reset() {
}

AVRIO::AVRIO(AVRRegisterBank &rbank, Display &display) :
        rbank(rbank),
        display(display),
        gpioa(display, rbank),
        gpiob(display, rbank),
        gpioe(display, rbank),
        gpiof(display, rbank),
        gpiok(display, rbank),
        timer0(rbank, 24),
        timer1(rbank, 21),
        timer3(rbank, 36)
{
    reset();
}

void AVRIO::tick(unsigned int cycles) {
    timer0.tick(cycles);
    timer1.tick(cycles);
    timer3.tick(cycles);
}

void AVRIO::clearIRQ(unsigned int irq) {
    switch (irq) {
        case 23:
            // TIM0_OVF
            timer0.irq &= ~1;
            break;

        case 20:
            // TIM1_OVF
            timer1.irq &= ~1;
            break;

        case 35:
            // TIM3_OVF
            timer3.irq &= ~1;
            break;
    }
}

