//
// Created by tsarn on 7/10/18.
//

#include <cstdio>
#include <thread>
#include "Display.h"
#include <SFML/Graphics.hpp>
#include <cstring>

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


Display::Display() {
    screen = new sf::VertexArray(sf::Quads, WIDTH * HEIGHT * 4);
    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            int pos = 4 * (x * HEIGHT + y);
            int left = 1 + x * (SCALE + 1);
            int top = 1 + y * (SCALE + 1);
            int right = left + SCALE;
            int bottom = top + SCALE;
            (*screen)[pos + 0].position = sf::Vector2f(left, top);
            (*screen)[pos + 1].position = sf::Vector2f(right, top);
            (*screen)[pos + 2].position = sf::Vector2f(right, bottom);
            (*screen)[pos + 3].position = sf::Vector2f(left, bottom);
            (*screen)[pos + 0].color = sf::Color::Black;
            (*screen)[pos + 1].color = sf::Color::Black;
            (*screen)[pos + 2].color = sf::Color::Black;
            (*screen)[pos + 3].color = sf::Color::Black;
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

void Display::start() {
    sf::RenderWindow window(sf::VideoMode(W_WIDTH, W_HEIGHT), "VirtualGameBox", sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
                bool pressed = (event.type == sf::Event::KeyPressed);

                switch (event.key.code) {
                    case sf::Keyboard::Down:
                        btn[BUTTON_DOWN] = pressed;
                        break;
                    case sf::Keyboard::Up:
                        btn[BUTTON_UP] = pressed;
                        break;
                    case sf::Keyboard::Left:
                        btn[BUTTON_LEFT] = pressed;
                        break;
                    case sf::Keyboard::Right:
                        btn[BUTTON_RIGHT] = pressed;
                        break;
                    case sf::Keyboard::Q:
                        btn[BUTTON_NW] = pressed;
                        break;
                    case sf::Keyboard::A:
                        btn[BUTTON_SW] = pressed;
                        break;
                    case sf::Keyboard::P:
                        btn[BUTTON_NE] = pressed;
                        break;
                    case sf::Keyboard::L:
                        btn[BUTTON_SE] = pressed;
                        break;
                    case sf::Keyboard::X:
                        btn[BUTTON_A] = pressed;
                        break;
                    case sf::Keyboard::Z:
                        btn[BUTTON_B] = pressed;
                        break;
                    case sf::Keyboard::Space:
                        btn[BUTTON_SELECT] = pressed;
                        break;
                    case sf::Keyboard::Return:
                        btn[BUTTON_START] = pressed;
                        break;
                    default:;
                }

                if (pressed && event.key.code == sf::Keyboard::Escape) window.close();
            }
        }

        window.clear(sf::Color::Black);
        window.draw(*screen);
        window.display();
    }
}

void Display::update() {
    for (int i = 0; i < WIDTH; ++i) {
        for (int j = 0; j < HEIGHT; ++j) {
            sf::Color color;
            if ((options & 0x7) == 0) {
                // a single pass

                uint8_t value = frame[0][i][j];
                color = sf::Color(((value >> 0) & 1) * 255, ((value >> 1) & 1) * 255, ((value >> 2) & 1) * 255);
            } else {
                int line = j & 0xf;
                long long totalTime = 0;
                for (int p = 0; p <= (options & 0x7); ++p) {
                    totalTime += oe_end[p][line] - oe_begin[p][line];
                }
                color = sf::Color::Black;
                if (totalTime > 0) {
                    for (int p = 0; p <= (options & 0x7); ++p) {
                        long long dur = oe_end[p][line] - oe_begin[p][line];
                        double frac = 1.0 * dur / totalTime;
                        uint8_t value = frame[p][i][j];
                        sf::Color c(((value >> 0) & 1) * 255, ((value >> 1) & 1) * 255, ((value >> 2) & 1) * 255);
                        color.r += (int) round(c.r * frac);
                        color.g += (int) round(c.g * frac);
                        color.b += (int) round(c.b * frac);
                    }
                }
            }

            int pos = 4 * (i * HEIGHT + j);
            (*screen)[pos + 0].color = color;
            (*screen)[pos + 1].color = color;
            (*screen)[pos + 2].color = color;
            (*screen)[pos + 3].color = color;
        }
    }
}

AVRGPIOA::AVRGPIOA(Display &display, AVRRegisterBank &rbank) : display(display), rbank(rbank) {

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
    bool old_latchb = display.latchb;
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
