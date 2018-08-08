//
// Created by tsarn on 7/10/18.
//

#include <cstdio>
#include <thread>
#include "Display.h"
#include <QtWidgets>
#include <cstring>


Display::Display() {
    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            screen[y * WIDTH + x] = QColor(0, 0, 0);
        }
    }
    reset();
}

void Display::reset() {
    rgb1 = rgb2 = 0;
    clock = latch = oe = false;
    bufptr = 0;
    line = 0;
    cclock = latchb = latchj = false;
    btnptr = 0;
    curbtn = false;
    pass = 0;
    options = 0;
    for (int i = 0; i < WIDTH; ++i) {
        buf[0][i] = 0;
        buf[1][i] = 0;
        for (int j = 0; j < HEIGHT; ++j) {
            for (int pass = 0; pass < MAX_PASSES; ++pass) {
                frame[pass][i][j] = 0;
            }
        }
    }
    for (int i = 0; i < 16; ++i) {
        btn[i] = latched_btn[i] = false;
        for (int j = 0; j < MAX_PASSES; ++j) {
            oe_begin[j][i] = oe_end[j][i] = 0;
        }
    }
}

void Display::button_update(int b, bool pressed)
{
    btn[b] = pressed;
}

void Display::update() {
    for (int i = 0; i < WIDTH; ++i) {
        for (int j = 0; j < HEIGHT; ++j) {
            QColor color;
            if ((options & 0x7) == 0) {
                // a single pass

                uint8_t value = frame[0][i][j];
                color = QColor(((value >> 0) & 1) * 255,
                               ((value >> 1) & 1) * 255,
                               ((value >> 2) & 1) * 255, 255);
            } else {
                int line = j & 0xf;
                long long totalTime = 0;
                for (int p = 0; p <= (options & 0x7); ++p) {
                    totalTime += oe_end[p][line] - oe_begin[p][line];
                }
                color = QColor(0, 0, 0, 255);
                if (totalTime > 0) {
                    for (int p = 0; p <= (options & 0x7); ++p) {
                        long long dur = oe_end[p][line] - oe_begin[p][line];
                        double frac = 1.0 * dur / totalTime;
                        uint8_t value = frame[p][i][j];
                        QColor c(((value >> 0) & 1) * 255,
                                 ((value >> 1) & 1) * 255,
                                 ((value >> 2) & 1) * 255);
                        color.setRed((int) round(c.red() * frac) + color.red());
                        color.setGreen((int) round(c.green() * frac) + color.green());
                        color.setBlue((int) round(c.blue() * frac) + color.blue());
                    }
                }
            }

            screen[j * WIDTH + i] = color;
        }
    }
}

AVRGPIOA::AVRGPIOA(Display &d, AVRRegisterBank &rbank)
    : display(d), rbank(rbank)
{
}

void AVRGPIOA::setPinValue(int pin, bool value) {
    if (pin < 3) {
        if (value) {
            display.rgb2 |= (1 << pin);
        } else {
            display.rgb2 &= ~(1 << pin);
        }
    }
}

AVRGPIOB::AVRGPIOB(Display &display, AVRRegisterBank &rbank) : display(display), rbank(rbank) {

}

void AVRGPIOB::setPinValue(int pin, bool value) {
    if (pin < 3) {
        if (value) {
            display.rgb1 |= (1 << pin);
        } else {
            display.rgb1 &= ~(1 << pin);
        }
    }
}

AVRGPIOE::AVRGPIOE(Display &display, AVRRegisterBank &rbank) : display(display), rbank(rbank) {

}

void AVRGPIOE::setPinValue(int pin, bool value) {
    bool old_clock = display.clock;
    bool old_latch = display.latch;
    bool old_oe = display.oe;

    if (pin == 3) display.clock = value;
    if (pin == 4) display.latch = value;
    if (pin == 5) display.oe = value;

    if (old_oe && !display.oe) {
        // OE ENABLED

        display.oe_begin[display.pass][display.line] = rbank.getCycles();
    }

    if (!old_oe && display.oe) {
        // OE DISABLED

        display.oe_end[display.pass][display.line] = rbank.getCycles();
    }

    if (old_clock && !display.clock) {
        // CLOCK FALLING

        display.buf[0][display.bufptr] = display.rgb1;
        display.buf[1][display.bufptr] = display.rgb2;
        display.bufptr++;
        if (display.bufptr >= 128) display.bufptr = 0;
    }

    if (!old_latch && display.latch) {
        // LATCH RISING
        for (int i = 0; i < 128; ++i) {
            int x = i % 64;
            int y = display.line;
            if (i < 64)
                y += 32;
            display.frame[display.pass][x][y] = display.buf[0][i];
            display.frame[display.pass][x][y + 16] = display.buf[1][i];
        }
        display.bufptr = 0;
        int limit = 15;
        if (display.options & 0x80) {
            limit = 0;
        }
        if (display.line == limit) {
            display.pass++;
            if (display.pass > (display.options & 0x7)) {
                display.update();
                display.pass = 0;
            }
        }
    }
}

AVRGPIOF::AVRGPIOF(Display &display, AVRRegisterBank &rbank) : display(display), rbank(rbank) {

}

void AVRGPIOF::setPinValue(int pin, bool value) {
    if (pin < 4) {
        if (value) {
            display.line |= (1 << pin);
        } else {
            display.line &= ~(1 << pin);
        }
    }
}

AVRGPIOK::AVRGPIOK(Display &display, AVRRegisterBank &rbank) : display(display), rbank(rbank) {

}

void AVRGPIOK::setPinValue(int pin, bool value) {
    bool old_cclock = display.cclock;
    //bool old_latchb = display.latchb;
    bool old_latchj = display.latchj;

    if (pin == 0) display.cclock = value;
    if (pin == 1) display.latchb = value;
    if (pin == 2) display.latchj = value;

    if (display.latchj && !old_latchj) {
        // LATCHJ RISING
        display.btnptr = 0;
        memcpy(display.latched_btn, display.btn, sizeof(display.latched_btn));
    }

    if (!display.cclock && old_cclock) {
        // CLOCK FALLING
        display.curbtn = display.latched_btn[display.btnptr];
        display.btnptr++;
    }
}

bool AVRGPIOK::getPinValue(int pin) {
    if (pin == 3) {
        return !display.curbtn;
    }
    return false;
}
