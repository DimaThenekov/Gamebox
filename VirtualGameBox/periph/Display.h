//
// Created by tsarn on 7/10/18.
//

#ifndef VIRTUALGAMEBOX_DISPLAY_H
#define VIRTUALGAMEBOX_DISPLAY_H


#include "AVRGPIO.h"
#include "QtWidgets"

#define SCALE 12
#define WIDTH 64
#define HEIGHT 64
#define W_WIDTH (1 + WIDTH * (SCALE + 1))
#define W_HEIGHT (1 + HEIGHT * (SCALE + 1))
#define MAX_PASSES 8

// Physical buttons
#define BUTTON_SW 6
#define BUTTON_NW 5
#define BUTTON_SE 3
#define BUTTON_NE 4

// Joystick buttons
#define BUTTON_UP 11
#define BUTTON_LEFT 13
#define BUTTON_DOWN 12
#define BUTTON_RIGHT 14
#define BUTTON_SELECT 9
#define BUTTON_START 10
#define BUTTON_A 7
#define BUTTON_B 8


class Display {
    friend class AVRGPIOA;
    friend class AVRGPIOB;
    friend class AVRGPIOE;
    friend class AVRGPIOF;
    friend class AVRGPIOK;

public:
    uint8_t options;

private:
    uint8_t rgb1, rgb2;
    bool clock, latch, oe;
    bool cclock, latchj, latchb, curbtn;
    uint8_t buf[2][128];
    unsigned int pass;
    long long oe_begin[MAX_PASSES][16];
    long long oe_end[MAX_PASSES][16];
    bool btn[16];
    bool latched_btn[16];
    uint8_t frame[MAX_PASSES][64][64];
    uint8_t bufptr, btnptr;
    uint8_t line;
    QColor screen[HEIGHT * WIDTH];

    void update();

public:
    Display();

    Display(const Display&) = delete;

    void operator=(const Display&) = delete;

    void reset();

    void start();
    QColor *getScreen() { return screen; }
    void button_update(int b, bool pressed);
};

class AVRGPIOA : public AVRGPIO {
private:
    Display &display;
    AVRRegisterBank &rbank;

    void setPinValue(int pin, bool value) override;

public:
    AVRGPIOA(Display &d, AVRRegisterBank &rbank);
};

class AVRGPIOB : public AVRGPIO {
private:
    Display &display;
    AVRRegisterBank &rbank;

    void setPinValue(int pin, bool value) override;

public:
    AVRGPIOB(Display &display, AVRRegisterBank &rbank);
};

class AVRGPIOE : public AVRGPIO {
private:
    Display &display;
    AVRRegisterBank &rbank;

    void setPinValue(int pin, bool value) override;

public:
    AVRGPIOE(Display &display, AVRRegisterBank &rbank);
};

class AVRGPIOF : public AVRGPIO {
private:
    Display &display;
    AVRRegisterBank &rbank;

    void setPinValue(int pin, bool value) override;

public:
    AVRGPIOF(Display &display, AVRRegisterBank &rbank);
};

class AVRGPIOK : public AVRGPIO {
private:
    Display &display;
    AVRRegisterBank &rbank;

    void setPinValue(int pin, bool value) override;

    bool getPinValue(int pin);

public:
    AVRGPIOK(Display &display, AVRRegisterBank &rbank);
};

#endif //VIRTUALGAMEBOX_DISPLAY_H
