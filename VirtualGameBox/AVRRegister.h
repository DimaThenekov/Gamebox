#ifndef AVRREGISTER_H
#define AVRREGISTER_H

enum AVRRegister {
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    R16,
    R17,
    R18,
    R19,
    R20,
    R21,
    R22,
    R23,
    R24,
    R25,
    R26,
    R27,
    R28,
    R29,
    R30,
    R31,
    X,
    Y,
    Z,
    SREG,
    SP,
    RAMPZ,
    EIND,
    PC,

    UNKNOWN
};

enum AVRStatusBit {
    SREG_C = 0,
    SREG_Z,
    SREG_N,
    SREG_V,
    SREG_S,
    SREG_H,
    SREG_T,
    SREG_I,
};

#endif // AVRREGISTER_H
