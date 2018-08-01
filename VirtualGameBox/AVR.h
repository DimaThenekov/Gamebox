#ifndef AVR_H
#define AVR_H

#include "AVRRegisterBank.h"
#include "AVRIO.h"
#include <array>
#include "atmega2560.h"
#include "periph/Display.h"
#include <cstdint>

class AVR {
public:
    std::array<uint16_t, FLASH_SIZE> flash;
    std::array<uint8_t, SRAM_SIZE> sram;
    std::array<uint8_t, EEPROM_SIZE> eeprom;

private:
    AVRRegisterBank rbank;
    AVRIO io;
    Display &display;
    bool do_log;

    uint8_t getData(uint16_t addr);

    void setData(uint16_t addr, uint8_t value);

    uint16_t getProgram(unsigned int addr);

    void setProgram(unsigned int addr, uint16_t value);

private:
    void updateAddFlags(uint8_t res, uint8_t Rd, uint8_t Rr);

    void updateSubFlags(uint8_t res, uint8_t Rd, uint8_t Rr);

    void updateZSubFlags(uint8_t res, uint8_t Rd, uint8_t Rr);

    void updateZNSFlags(uint8_t res);

    void updatezNSFlags(uint8_t res);

    void updateZNSFlags16(uint16_t res);

    void updateZNV0SFlags(uint8_t res);

    void updateZCVSFlags(uint8_t res, uint8_t Rd);

    void updateZCNVSFlags(uint8_t res, uint8_t Rd);

    static void decode5d5r(uint16_t opcode, AVRRegister &Rd, AVRRegister &Rr);

    static void decode4d4r(uint16_t opcode, AVRRegister &Rd, AVRRegister &Rr);

    static void decode3d3r(uint16_t opcode, AVRRegister &Rd, AVRRegister &Rr);

    static void decodew4d4r(uint16_t opcode, AVRRegister &Rd, AVRRegister &Rr);

    static void decode6K2d(uint16_t opcode, unsigned int &K, AVRRegister &Rd);

    static void decode8K4d(uint16_t opcode, unsigned int &K, AVRRegister &Rd);

    static void decode12k(uint16_t opcode, int &k);

    static void decode22K(uint16_t opcode1, uint16_t opcode2, unsigned int &K);

    static void decode5r3b(uint16_t opcode, AVRRegister &Rr, unsigned int &b);

    static void decode5A3b(uint16_t opcode, unsigned int &A, unsigned int &b);

    static void decode5d(uint16_t opcode, AVRRegister &Rd);

    static void decode5d6A(uint16_t opcode, AVRRegister &Rd, unsigned int &A);

    static void decode7k3b(uint16_t opcode, int &k, unsigned int &b);

    static bool isLongInstruction(uint16_t opcode);

    void pushStack(uint8_t value);

    void pushStack24(unsigned int value);

    uint8_t popStack();

    unsigned int popStack24();

public:
    AVR(Display &display);

    void reset();

    unsigned int tick();

    void run();
};

#endif // AVR_H
