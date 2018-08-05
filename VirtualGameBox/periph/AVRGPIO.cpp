//
// Created by tsarn on 7/10/18.
//

#include <cstdio>
#include <cstdlib>
#include "AVRGPIO.h"

uint8_t AVRGPIO::pinRead() {
    uint8_t ans = 0;
    for (int i = 0; i < 8; ++i) {
        if ((direction >> i) & 1) {
            // OUTPUT pin
            ans |= (data & (1 << i));
        } else {
            // INPUT pin
            ans |= (getPinValue(i) << i);
        }
    }
    return ans;
}

void AVRGPIO::pinWrite(uint8_t value) {
    for (int i = 0; i < 8; ++i) {
        if (((value >> i) & 1) && ((direction >> i) & 1)) {
            // toggle OUTPUT pin
            if ((data >> i) & 1) {
                data &= ~(1 << i);
                setPinValue(i, false);
            } else {
                data |= (1 << i);
                setPinValue(i, true);
            };
        }
    }
}

uint8_t AVRGPIO::ddrRead() {
    return direction;
}

void AVRGPIO::ddrWrite(uint8_t value) {
    direction = value;
}

uint8_t AVRGPIO::portRead() {
    return data;
}

void AVRGPIO::portWrite(uint8_t value) {
    for (int i = 0; i < 8; ++i) {
        if ((direction >> i) & 1) {
            // OUTPUT pin
            if ((value >> i) & 1) {
                data |= (1 << i);
                setPinValue(i, true);
            } else {
                data &= ~(1 << i);
                setPinValue(i, false);
            }
        }
    }
}

bool AVRGPIO::getPinValue(int pin) {
    (void)pin;
    return false;
}

void AVRGPIO::setPinValue(int pin, bool value) {
    (void)pin;
    (void)value;
}
