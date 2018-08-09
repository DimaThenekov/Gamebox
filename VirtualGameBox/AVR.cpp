#include "AVR.h"
#include <cstdio>
#include <unistd.h>

#define SKIP() do { cycles++; new_pc++; if (isLongInstruction(getProgram(pc + 1))) { new_pc++; cycles++; } } while (0);

#define ILOG(fmt, ...) do { \
    if (do_log) fprintf(stderr, "0x%04x: " fmt "\n", pc << 1, ##__VA_ARGS__); \
} while (0);
//#define ILOG(fmt, ...) do {} while (0);

#define FREQ 16000000
#define BURSTS_PER_SECOND 100

uint8_t AVR::getData(uint16_t addr) {
    if (addr < 0x20) {
        // register
        return rbank.get(static_cast<AVRRegister>(addr));
    }

    if (addr < 0x200) {
        // io
        return io.get(addr);
    }

    if (addr < 0x200 + SRAM_SIZE) {
        // sram
        return sram[addr - 0x200];
    }

    // unmapped
    return 0x00;
}

void AVR::setData(uint16_t addr, uint8_t value) {
    if (addr < 0x20) {
        // register
        rbank.set(static_cast<AVRRegister>(addr), value);
        return;
    }

    if (addr < 0x200) {
        // io
        io.set(addr, value);
        return;
    }

    if (addr < 0x200 + SRAM_SIZE) {
        // sram
        sram[addr - 0x200] = value;
        return;
    }

    // unmapped. do nothing
}

uint16_t AVR::getProgram(unsigned int addr) {
    return flash[addr % FLASH_SIZE];
}

void AVR::setProgram(unsigned int addr, uint16_t value) {
    flash[addr % FLASH_SIZE] = value;
}

void AVR::updateAddFlags(uint8_t res, uint8_t Rd, uint8_t Rr) {
    uint8_t carry = (Rd & Rr) | (Rr & ~res) | (~res & Rd);
    rbank.setSREG(SREG_H, (carry >> 3) & 1);
    rbank.setSREG(SREG_C, (carry >> 7) & 1);
    rbank.setSREG(SREG_V, (((Rd & Rr & ~res) | (~Rd & ~Rr & res)) >> 7) & 1);
    updateZNSFlags(res);
}

void AVR::updateSubFlags(uint8_t res, uint8_t Rd, uint8_t Rr) {
    uint8_t carry = (~Rd & Rr) | (Rr & res) | (res & ~Rd);
    rbank.setSREG(SREG_H, (carry >> 3) & 1);
    rbank.setSREG(SREG_C, (carry >> 7) & 1);
    rbank.setSREG(SREG_V, (((Rd & ~Rr & ~res) | (~Rd & Rr & res)) >> 7) & 1);
    updateZNSFlags(res);
}

void AVR::updateZSubFlags(uint8_t res, uint8_t Rd, uint8_t Rr) {
    uint8_t carry = (~Rd & Rr) | (Rr & res) | (res & ~Rd);
    rbank.setSREG(SREG_H, (carry >> 3) & 1);
    rbank.setSREG(SREG_C, (carry >> 7) & 1);
    rbank.setSREG(SREG_V, (((Rd & ~Rr & ~res) | (~Rd & Rr & res)) >> 7) & 1);
    updatezNSFlags(res);
}

void AVR::updateZNSFlags(uint8_t res) {
    rbank.setSREG(SREG_Z, res == 0);
    rbank.setSREG(SREG_N, (res >> 7) & 1);
    rbank.setSREG(SREG_S, rbank.getSREG(SREG_N) ^ rbank.getSREG(SREG_V));
}

void AVR::updatezNSFlags(uint8_t res) {
    if (res)
        rbank.setSREG(SREG_Z, false);
    rbank.setSREG(SREG_N, (res >> 7) & 1);
    rbank.setSREG(SREG_S, rbank.getSREG(SREG_N) ^ rbank.getSREG(SREG_V));
}

void AVR::updateZNSFlags16(uint16_t res) {
    rbank.setSREG(SREG_Z, res == 0);
    rbank.setSREG(SREG_N, (res >> 15) & 1);
    rbank.setSREG(SREG_S, rbank.getSREG(SREG_N) ^ rbank.getSREG(SREG_V));
}

void AVR::updateZNV0SFlags(uint8_t res) {
    rbank.setSREG(SREG_V, 0);
    updateZNSFlags(res);
}

void AVR::updateZCVSFlags(uint8_t res, uint8_t Rd) {
    rbank.setSREG(SREG_Z, res == 0);
    rbank.setSREG(SREG_C, Rd & 1);
    rbank.setSREG(SREG_V, rbank.getSREG(SREG_N) ^ rbank.getSREG(SREG_C));
    rbank.setSREG(SREG_S, rbank.getSREG(SREG_N) ^ rbank.getSREG(SREG_V));
}

void AVR::updateZCNVSFlags(uint8_t res, uint8_t Rd) {
    rbank.setSREG(SREG_Z, res == 0);
    rbank.setSREG(SREG_C, Rd & 1);
    rbank.setSREG(SREG_N, Rd >> 7);
    rbank.setSREG(SREG_V, rbank.getSREG(SREG_N) ^ rbank.getSREG(SREG_C));
    rbank.setSREG(SREG_S, rbank.getSREG(SREG_N) ^ rbank.getSREG(SREG_V));
}

void AVR::decode6K2d(uint16_t opcode, unsigned int &K, AVRRegister &Rd) {
    K = (opcode & 0x0f) | ((opcode >> 2) & 0x30);
    Rd = static_cast<AVRRegister>((((opcode >> 4) & 0x03) + 12) << 1);
}

void AVR::decode8K4d(uint16_t opcode, unsigned int &K, AVRRegister &Rd) {
    K = (opcode & 0x0f) | ((opcode >> 4) & 0xf0);
    Rd = static_cast<AVRRegister>(((opcode >> 4) & 0x0f) + 16);
}

void AVR::decode12k(uint16_t opcode, int &k) {
    k = opcode & 0xfff;
    if (opcode & 0x800)
        k |= (-1 ^ 0xfff);
}

void AVR::decode22K(uint16_t opcode1, uint16_t opcode2, unsigned int &K) {
    K = ((opcode1 >> 3) & 0x1e) | (opcode1 & 0x01);
    K = (K << 16) | opcode2;
}

void AVR::decode5r3b(uint16_t opcode, AVRRegister &Rr, unsigned int &b) {
    b = opcode & 0x07;
    Rr = static_cast<AVRRegister>((opcode >> 4) & 0x1f);
}

void AVR::decode5d(uint16_t opcode, AVRRegister &Rd) {
    Rd = static_cast<AVRRegister>((opcode >> 4) & 0x1f);
}

void AVR::decode5d6A(uint16_t opcode, AVRRegister &Rd, unsigned int &A) {
    Rd = static_cast<AVRRegister>((opcode >> 4) & 0x1f);
    A = (opcode & 0x0f) | ((opcode >> 5) & 0x30);
}

void AVR::decode7k3b(uint16_t opcode, int &k, unsigned int &b) {
    k = (opcode >> 3) & 0x7f;
    if (opcode & 0x200)
        k |= (-1 ^ 0x7f);
    b = opcode & 0x07;
}

void AVR::decode5d5r(uint16_t opcode, AVRRegister &Rd, AVRRegister &Rr) {
    Rr = static_cast<AVRRegister>((((opcode >> 9) & 1) << 4) | (opcode & 0x0f));
    Rd = static_cast<AVRRegister>((opcode >> 4) & 0x1f);
}

void AVR::decode4d4r(uint16_t opcode, AVRRegister &Rd, AVRRegister &Rr) {
    Rd = static_cast<AVRRegister>(0x10 | ((opcode >> 4) & 0x0f));
    Rr = static_cast<AVRRegister>(0x10 | (opcode & 0x0f));
}

void AVR::decode3d3r(uint16_t opcode, AVRRegister &Rd, AVRRegister &Rr) {
    Rd = static_cast<AVRRegister>(0x10 | ((opcode >> 4) & 0x07));
    Rr = static_cast<AVRRegister>(0x10 | (opcode & 0x07));
}

void AVR::decodew4d4r(uint16_t opcode, AVRRegister &Rd, AVRRegister &Rr) {
    Rd = static_cast<AVRRegister>(((opcode >> 4) & 0x0f) << 1);
    Rr = static_cast<AVRRegister>((opcode & 0x0f) << 1);
}

void AVR::decode5A3b(uint16_t opcode, unsigned int &A, unsigned int &b) {
    A = (opcode >> 3) & 0x1f;
    b = opcode & 0x07;
}

void AVR::pushStack(uint8_t value) {
    uint16_t sp = rbank.get(AVRRegister::SP);
    setData(sp, value);
    rbank.set(AVRRegister::SP, sp - 1);
}

void AVR::pushStack24(unsigned int value) {
    pushStack((value >> 0) & 0xff);
    pushStack((value >> 8) & 0xff);
    pushStack((value >> 16) & 0xff);
}

uint8_t AVR::popStack() {
    uint16_t sp = rbank.get(AVRRegister::SP);
    rbank.set(AVRRegister::SP, sp + 1);
    return getData(sp + 1);
}

unsigned int AVR::popStack24() {
    unsigned int res = 0;
    res |= (popStack() << 16);
    res |= (popStack() << 8);
    res |= (popStack() << 0);
    return res;
}

AVR::AVR(Display &display) : rbank(), io(rbank, display), display(display) {
    do_log = false;
    reset();
}

void AVR::reset() {
    rbank.reset();
    io.reset();
    sram.fill(0x00);
}

bool AVR::isLongInstruction(uint16_t opcode) {
    if ((opcode & 0xfc0f) == 0x9000) return true; // LDS / STS
    if ((opcode & 0xfe0c) == 0x940c) return true; // JMP / CALL
    return false;
}

unsigned int AVR::tick() {
    unsigned int pc = rbank.get(AVRRegister::PC);
    // FETCH INSTRUCTION
    uint16_t opcode = getProgram(pc);

    // DECODE INSTRUCTION
    AVRRegister Rd, Rr;
    unsigned int K, A;
    int k;

    unsigned int cycles = 1;
    unsigned int new_pc = pc + 1;
    unsigned int op4 = (opcode >> 12);
    unsigned int op6 = (opcode >> 10);
    unsigned int op7 = (opcode >> 9);
    unsigned int op8 = (opcode >> 8);
    bool ok = false;
    bool processIRQ = true;

    switch (op4) {
        case 0b0000: {
            switch (op6 & 0b0011) {
                case 0b00: {
                    switch (op8 & 0b0011) {
                        case 0b01: {
                            // MOVW - copy register word (0000 0001 dddd rrrr)

                            decodew4d4r(opcode, Rd, Rr);
                            rbank.set(Rd, rbank.get(Rr));
                            rbank.set(static_cast<AVRRegister>(Rd + 1), rbank.get(static_cast<AVRRegister>(Rr + 1)));
                            ok = true;
                            ILOG("MOVW R%d:R%d, R%d:R%d", Rd + 1, Rd, Rr + 1, Rr);
                        }
                            break;

                        case 0b10: {
                            // MULS - multiply signed (0000 0010 dddd rrrr)

                            decode4d4r(opcode, Rd, Rr);
                            int16_t vRd = static_cast<int8_t>(rbank.get(Rd));
                            int16_t vRr = static_cast<int8_t>(rbank.get(Rr));
                            uint16_t res = vRd * vRr;
                            rbank.set(AVRRegister::R0, res & 0xff);
                            rbank.set(AVRRegister::R1, res >> 8);
                            rbank.setSREG(SREG_C, (res >> 15) & 1);
                            rbank.setSREG(SREG_Z, res == 0);
                            ok = true;
                            cycles = 2;
                            ILOG("MULS R%d, R%d", Rd, Rr);
                        }
                            break;

                        case 0b11: {
                            switch (((opcode >> 3) & 1) | ((opcode >> 6) & 2)) {
                                case 0b00: {
                                    // MULSU - multiply signed with unsigned (0000 0011 0ddd 0rrr)

                                    decode3d3r(opcode, Rd, Rr);
                                    int16_t vRd = static_cast<int8_t>(rbank.get(Rd));
                                    int16_t vRr = rbank.get(Rr);
                                    uint16_t res = vRd * vRr;
                                    rbank.set(AVRRegister::R0, res & 0xff);
                                    rbank.set(AVRRegister::R1, res >> 8);
                                    rbank.setSREG(SREG_C, (res >> 15) & 1);
                                    rbank.setSREG(SREG_Z, res == 0);
                                    ok = true;
                                    cycles = 2;
                                    ILOG("MULSU R%d, R%d", Rd, Rr);
                                }
                                    break;

                                case 0b01: {
                                    // FMUL - fractional multiply (0000 0011 0ddd 1rrr)

                                    decode3d3r(opcode, Rd, Rr);
                                    uint16_t vRd = rbank.get(Rd);
                                    uint16_t vRr = rbank.get(Rr);
                                    uint16_t res = (vRd * vRr) << 1;
                                    rbank.set(AVRRegister::R0, res & 0xff);
                                    rbank.set(AVRRegister::R1, res >> 8);
                                    rbank.setSREG(SREG_C, (res >> 15) & 1);
                                    rbank.setSREG(SREG_Z, res == 0);
                                    ok = true;
                                    cycles = 2;
                                    ILOG("FMUL R%d, R%d", Rd, Rr);
                                }
                                    break;

                                case 0b10: {
                                    // FMULS - fractional multiply signed (0000 0011 1ddd 0rrr)

                                    decode3d3r(opcode, Rd, Rr);
                                    int16_t vRd = static_cast<int8_t>(rbank.get(Rd));
                                    int16_t vRr = static_cast<int8_t>(rbank.get(Rr));
                                    uint16_t res = (vRd * vRr) << 1;
                                    rbank.set(AVRRegister::R0, res & 0xff);
                                    rbank.set(AVRRegister::R1, res >> 8);
                                    rbank.setSREG(SREG_C, (res >> 15) & 1);
                                    rbank.setSREG(SREG_Z, res == 0);
                                    ok = true;
                                    cycles = 2;
                                    ILOG("FMULS R%d, R%d", Rd, Rr);
                                }
                                    break;

                                case 0b11: {
                                    // FMULSU - fractional multiply signed with unsigned (0000 0011 1ddd 1rrr)

                                    decode3d3r(opcode, Rd, Rr);
                                    int16_t vRd = static_cast<int8_t>(rbank.get(Rd));
                                    uint16_t vRr = rbank.get(Rr);
                                    uint16_t res = (vRd * vRr) << 1;
                                    rbank.set(AVRRegister::R0, res & 0xff);
                                    rbank.set(AVRRegister::R1, res >> 8);
                                    rbank.setSREG(SREG_C, (res >> 15) & 1);
                                    rbank.setSREG(SREG_Z, res == 0);
                                    ok = true;
                                    cycles = 2;
                                    ILOG("FMULSU R%d, R%d", Rd, Rr);
                                }
                                    break;
                            }
                        }
                            break;
                    }
                }
                    break;
                case 0b01: {
                    // CPC - compare with carry (0000 01rd dddd rrrr)

                    decode5d5r(opcode, Rd, Rr);
                    uint8_t vRd = rbank.get(Rd);
                    uint8_t vRr = rbank.get(Rr);
                    uint8_t res = vRd - vRr - rbank.getSREG(SREG_C);
                    updateZSubFlags(res, vRd, vRr);
                    ok = true;
                    ILOG("CPC R%d, R%d", Rd, Rr);
                }
                    break;

                case 0b10: {
                    // SBC - subtract with carry (0000 10rd dddd rrrr)

                    decode5d5r(opcode, Rd, Rr);
                    uint8_t vRd = rbank.get(Rd);
                    uint8_t vRr = rbank.get(Rr);
                    uint8_t res = vRd - vRr - rbank.getSREG(SREG_C);
                    rbank.set(Rd, res);
                    updateZSubFlags(res, vRd, vRr);
                    ok = true;
                    ILOG("SBC R%d, R%d", Rd, Rr);
                }
                    break;

                case 0b11: {
                    // ADD - add without carry (0000 11rd dddd rrrr)

                    decode5d5r(opcode, Rd, Rr);
                    uint8_t vRd = rbank.get(Rd);
                    uint8_t vRr = rbank.get(Rr);
                    uint8_t res = vRd + vRr;
                    rbank.set(Rd, res);
                    updateAddFlags(res, vRd, vRr);
                    ok = true;
                    ILOG("ADD R%d, R%d", Rd, Rr);
                }
                    break;
            }
        }
            break;

        case 0b0001: {
            switch (op6 & 0b0011) {
                case 0b00: {
                    // CPSE - compare skip if equal (0001 00rd dddd rrrr)

                    decode5d5r(opcode, Rd, Rr);
                    uint8_t vRd = rbank.get(Rd);
                    uint8_t vRr = rbank.get(Rr);
                    if (vRd == vRr) {
                        SKIP();
                    }
                    ok = true;
                    ILOG("CPSE R%d, R%d", Rd, Rr);
                }
                    break;

                case 0b01: {
                    // CP - compare (0001 01rd dddd rrrr)

                    decode5d5r(opcode, Rd, Rr);
                    uint8_t vRd = rbank.get(Rd);
                    uint8_t vRr = rbank.get(Rr);
                    uint8_t res = vRd - vRr;
                    updateSubFlags(res, vRd, vRr);
                    ok = true;
                    ILOG("CP R%d, R%d", Rd, Rr);
                }
                    break;

                case 0b10: {
                    // SUB - subtract without carry (0001 10rd dddd rrrr)

                    decode5d5r(opcode, Rd, Rr);
                    uint8_t vRd = rbank.get(Rd);
                    uint8_t vRr = rbank.get(Rr);
                    uint8_t res = vRd - vRr;
                    rbank.set(Rd, res);
                    updateSubFlags(res, vRd, vRr);
                    ok = true;
                    ILOG("SUB R%d, R%d", Rd, Rr);
                }
                    break;

                case 0b11: {
                    // ADC - add with carry (0001 11rd dddd rrrr)

                    decode5d5r(opcode, Rd, Rr);
                    uint8_t vRd = rbank.get(Rd);
                    uint8_t vRr = rbank.get(Rr);
                    uint8_t res = vRd + vRr + rbank.getSREG(SREG_C);
                    rbank.set(Rd, res);
                    updateAddFlags(res, vRd, vRr);
                    ok = true;
                    ILOG("ADC R%d, R%d", Rd, Rr);
                }
                    break;
            }
        }
            break;

        case 0b0010: {
            switch (op6 & 0b0011) {
                case 0b00: {
                    // AND - logical AND (0010 00rd dddd rrrr)

                    decode5d5r(opcode, Rd, Rr);
                    uint8_t vRd = rbank.get(Rd);
                    uint8_t vRr = rbank.get(Rr);
                    uint8_t res = vRd & vRr;
                    rbank.set(Rd, res);
                    updateZNV0SFlags(res);
                    ok = true;
                    ILOG("AND R%d, R%d", Rd, Rr);
                }
                    break;

                case 0b01: {
                    // EOR - Exclusive OR (0010 01rd dddd rrrr)

                    decode5d5r(opcode, Rd, Rr);
                    uint8_t vRd = rbank.get(Rd);
                    uint8_t vRr = rbank.get(Rr);
                    uint8_t res = vRd ^vRr;
                    rbank.set(Rd, res);
                    updateZNV0SFlags(res);
                    ok = true;
                    ILOG("EOR R%d, R%d", Rd, Rr);
                }
                    break;

                case 0b10: {
                    // OR - logical OR (0010 10rd dddd rrrr)

                    decode5d5r(opcode, Rd, Rr);
                    uint8_t vRd = rbank.get(Rd);
                    uint8_t vRr = rbank.get(Rr);
                    uint8_t res = vRd | vRr;
                    rbank.set(Rd, res);
                    updateZNV0SFlags(res);
                    ok = true;
                    ILOG("OR R%d, R%d", Rd, Rr);
                }
                    break;

                case 0b11: {
                    // MOV - copy register (0010 11rd dddd rrrr)

                    decode5d5r(opcode, Rd, Rr);
                    rbank.set(Rd, rbank.get(Rr));
                    ok = true;
                    ILOG("MOV R%d, R%d", Rd, Rr);
                }
                    break;
            }
        }
            break;

        case 0b0011: {
            // CPI - compare with immediate (0011 KKKK dddd KKKK)

            decode8K4d(opcode, K, Rd);
            uint8_t vRd = rbank.get(Rd);
            uint8_t res = vRd - K;
            updateSubFlags(res, vRd, K);
            ok = true;
            ILOG("CPI R%d, %d", Rd, K);
        }
            break;

        case 0b0100: {
            // SBCI - subtract immediate with carry (0100 KKKK dddd KKKK)

            decode8K4d(opcode, K, Rd);
            uint8_t vRd = rbank.get(Rd);
            uint8_t res = vRd - K - rbank.getSREG(SREG_C);
            rbank.set(Rd, res);
            updateZSubFlags(res, vRd, K);
            ok = true;
            ILOG("SBCI R%d, %d", Rd, K);
        }
            break;

        case 0b0101: {
            // SUBI - subtract immediate without carry (0101 KKKK dddd KKKK)

            decode8K4d(opcode, K, Rd);
            uint8_t vRd = rbank.get(Rd);
            uint8_t res = vRd - K;
            rbank.set(Rd, res);
            updateSubFlags(res, vRd, K);
            ok = true;
            ILOG("SUBI R%d, %d", Rd, K);
        }
            break;

        case 0b0110: {
            // ORI - logical OR with immediate (0110 KKKK dddd KKKK)

            decode8K4d(opcode, K, Rd);
            uint8_t vRd = rbank.get(Rd);
            uint8_t res = vRd | K;
            rbank.set(Rd, res);
            updateZNV0SFlags(res);
            ok = true;
            ILOG("ORI R%d, %d", Rd, K);
        }
            break;

        case 0b0111: {
            // ANDI - logical AND with immediate (0111 KKKK dddd KKKK)

            decode8K4d(opcode, K, Rd);
            uint8_t vRd = rbank.get(Rd);
            uint8_t res = vRd & K;
            rbank.set(Rd, res);
            updateZNV0SFlags(res);
            ok = true;
            ILOG("ANDI R%d, %d", Rd, K);
        }
            break;

        case 0b1001: {
            switch (op8 & 0b1111) {
                case 0b0000:
                case 0b0001: {
                    switch (opcode & 0b1111) {
                        case 0b0000: {
                            // LDS - load direct from data space (1001 000d dddd 0000 / kkkk kkkk kkkk kkkk)

                            decode5d(opcode, Rd);
                            unsigned int addr = getProgram(pc + 1);
                            rbank.set(Rd, getData(addr));
                            new_pc = pc + 2;
                            cycles = 2;
                            ok = true;
                            ILOG("LDS 0x%04x, R%d", addr, Rd);
                        }
                            break;

                        case 0b0100:
                        case 0b0101:
                        case 0b0110:
                        case 0b0111: {
                            // LPM - load program memory (1001 000d dddd 010x)
                            // ELPM - extended load program memory (1001 000d dddd 011x)

                            decode5d(opcode, Rd);
                            unsigned int ramp = 0;
                            char post = 0;
                            if (opcode & 1) {
                                post = '+';
                            }
                            if ((opcode >> 1) & 1) {
                                ILOG("ELPM R%d, Z%.1s", Rd, &post);
                                ramp = (rbank.get(AVRRegister::RAMPZ) << 16);
                            } else {
                                ILOG("LPM R%d, Z%.1s", Rd, &post);
                            }

                            unsigned int addr = rbank.get(AVRRegister::Z) + ramp;
                            uint16_t data = getProgram(addr >> 1);
                            uint8_t byte;
                            if (addr & 1) {
                                byte = data >> 8;
                            } else {
                                byte = data & 0xff;
                            }

                            rbank.set(Rd, byte);

                            if (opcode & 1) {
                                rbank.set(AVRRegister::Z, rbank.get(AVRRegister::Z) + 1);
                            }

                            cycles = 2;
                            ok = true;
                        }
                            break;

                        case 0b0001:
                        case 0b0010:
                        case 0b1001:
                        case 0b1010:
                        case 0b1100:
                        case 0b1101:
                        case 0b1110: {
                            // LD - load indirect from data space to register (1001 000d dddd ????)

                            char arg;
                            if ((opcode & 0b1100) == 0b1100) {
                                Rd = AVRRegister::X;
                                arg = 'X';
                            } else if ((opcode & 0b1000) == 0b1000) {
                                Rd = AVRRegister::Y;
                                arg = 'Y';
                            } else {
                                Rd = AVRRegister::Z;
                                arg = 'Z';
                            }

                            char pre = ' ';
                            char post = ' ';

                            if ((opcode & 0b0011) == 0b0010) {
                                rbank.set(Rd, rbank.get(Rd) - 1);
                                pre = '-';
                            }

                            decode5d(opcode, Rr);
                            rbank.set(Rr, getData(rbank.get(Rd)));

                            if ((opcode & 0b0011) == 0b0001) {
                                rbank.set(Rd, rbank.get(Rd) + 1);
                                post = '+';
                            }

                            cycles = 2;
                            ok = true;
                            ILOG("LD R%d, %c%c%c to 0x%04x from 0x%04x", Rr, pre, arg, post, rbank.get(Rr), rbank.get(Rd));
                        }
                            break;

                        case 0b1111: {
                            // POP - pop register from stack (1001 000d dddd 1111)

                            decode5d(opcode, Rd);
                            rbank.set(Rd, popStack());
                            cycles = 2;
                            ok = true;
                            ILOG("POP R%d", Rd);
                        }
                    }
                    break;
                }

                case 0b0010:
                case 0b0011: {
                    switch (opcode & 0b1111) {
                        case 0b0000: {
                            // STS - store direct to data space (1001 001d dddd 0000 / kkkk kkkk kkkk kkkk)

                            decode5d(opcode, Rd);
                            unsigned int addr = getProgram(pc + 1);
                            setData(addr, rbank.get(Rd));
                            new_pc = pc + 2;
                            cycles = 2;
                            ok = true;
                            ILOG("STS 0x%04x, R%d", addr, Rd);
                        }
                            break;

                        case 0b0001:
                        case 0b0010:
                        case 0b1001:
                        case 0b1010:
                        case 0b1100:
                        case 0b1101:
                        case 0b1110: {
                            // ST - store indirect to data space from register (1001 001d dddd ????)

                            char arg;
                            if ((opcode & 0b1100) == 0b1100) {
                                Rd = AVRRegister::X;
                                arg = 'X';
                            } else if ((opcode & 0b1000) == 0b1000) {
                                Rd = AVRRegister::Y;
                                arg = 'Y';
                            } else {
                                Rd = AVRRegister::Z;
                                arg = 'Z';
                            }

                            char pre = ' ';
                            char post = ' ';

                            if ((opcode & 0b0011) == 0b0010) {
                                rbank.set(Rd, rbank.get(Rd) - 1);
                                pre = '-';
                            }

                            decode5d(opcode, Rr);
                            setData(rbank.get(Rd), rbank.get(Rr));

                            if ((opcode & 0b0011) == 0b0001) {
                                rbank.set(Rd, rbank.get(Rd) + 1);
                                post = '+';
                            }

                            cycles = 2;
                            ok = true;
                            ILOG("ST R%d, %c%c%c", Rr, pre, arg, post);
                        }
                            break;

                        case 0b1111: {
                            // PUSH - push register on stack (1001 001d dddd 1111)

                            decode5d(opcode, Rd);
                            pushStack(rbank.get(Rd));
                            cycles = 2;
                            ok = true;
                            ILOG("PUSH R%d", Rd);
                        }
                            break;
                    }
                }
                    break;


                case 0b0100:
                case 0b0101: {
                    switch (opcode & 0b1111) {
                        case 0b0000: {
                            // COM - one's complement (1001 010d dddd 0000)

                            decode5d(opcode, Rd);
                            uint8_t vRd = rbank.get(Rd);
                            uint8_t res = 0xff - vRd;
                            rbank.set(Rd, res);
                            updateZNV0SFlags(res);
                            rbank.setSREG(SREG_C, true);
                            ok = true;
                            ILOG("COM R%d", Rd);
                        }
                            break;

                        case 0b0001: {
                            // NEG - two's complement (1001 010d dddd 0001)

                            decode5d(opcode, Rd);
                            uint8_t vRd = rbank.get(Rd);
                            uint8_t res = 0x00 - vRd;
                            rbank.set(Rd, res);
                            rbank.setSREG(SREG_H, ((res >> 3) | (vRd >> 3)) & 1);
                            rbank.setSREG(SREG_V, res == 0x80);
                            rbank.setSREG(SREG_C, res != 0);
                            updateZNSFlags(res);
                            ok = true;
                            ILOG("NEG R%d", Rd);
                        }
                            break;

                        case 0b0010: {
                            // SWAP - swap nibbles (1001 010d dddd 0010)

                            decode5d(opcode, Rd);
                            uint8_t vRd = rbank.get(Rd);
                            uint8_t res = (vRd >> 4) | (vRd << 4);
                            rbank.set(Rd, res);
                            ok = true;
                            ILOG("SWAP R%d", Rd);
                        }
                            break;

                        case 0b0011: {
                            // INC - increment (1001 010d dddd 0011)

                            decode5d(opcode, Rd);
                            uint8_t vRd = rbank.get(Rd);
                            uint8_t res = vRd + 1;
                            rbank.set(Rd, res);
                            rbank.setSREG(SREG_V, res == 0x80);
                            updateZNSFlags(res);
                            ok = true;
                            ILOG("INC R%d", Rd);
                        }
                            break;

                        case 0b0101: {
                            // ASR - arithmetic shift right (1001 010d dddd 0101)

                            decode5d(opcode, Rd);
                            uint8_t vRd = rbank.get(Rd);
                            uint8_t res = (vRd >> 1) | (vRd & 0x80);
                            rbank.set(Rd, res);
                            updateZCNVSFlags(res, vRd);
                            ok = true;
                            ILOG("ASR R%d", Rd);
                        }
                            break;

                        case 0b0110: {
                            // LSR - logical shift right (1001 010d dddd 0110)

                            decode5d(opcode, Rd);
                            uint8_t vRd = rbank.get(Rd);
                            uint8_t res = vRd >> 1;
                            rbank.set(Rd, res);
                            rbank.setSREG(SREG_N, 0);
                            updateZCVSFlags(res, vRd);
                            ok = true;
                            ILOG("LSR R%d", Rd);
                        }
                            break;

                        case 0b0111: {
                            // ROR - rotate right through carry (1001 010d dddd 0111)

                            decode5d(opcode, Rd);
                            uint8_t vRd = rbank.get(Rd);
                            uint8_t res = (vRd >> 1) | (static_cast<uint8_t>(rbank.getSREG(SREG_C) << 7));
                            rbank.set(Rd, res);
                            updateZCNVSFlags(res, vRd);
                            ok = true;
                            ILOG("ROR R%d", Rd);
                        }
                            break;

                        case 0b1000: {
                            if (op8 & 1) {
                            } else {
                                // BSET - set bit in SREG   (1001 0100 0sss 1000)
                                // BCLR - clear bit in SREG (1001 0100 1sss 1000)

                                uint8_t bit = (opcode >> 4) & 0x7;
                                rbank.setSREG(static_cast<AVRStatusBit>(bit), !((opcode >> 7) & 1));
                                ok = true;
                                if ((opcode >> 7) & 1) {
                                    ILOG("BCLR %d", bit);
                                } else {
                                    ILOG("BSET %d", bit);
                                }
                            }
                        }
                            break;

                        case 0b1010: {
                            // DEC - decrement (1001 010d dddd 1010)

                            decode5d(opcode, Rd);
                            uint8_t vRd = rbank.get(Rd);
                            uint8_t res = vRd - 1;
                            rbank.set(Rd, res);
                            rbank.setSREG(SREG_V, res == 0x7f);
                            updateZNSFlags(res);
                            ok = true;
                            ILOG("DEC R%d", Rd);
                        }
                            break;
                    }
                }
                    break;

                case 0b0110: {
                    // ADIW - add immediate to word (1001 0110 KKdd KKKK)

                    decode6K2d(opcode, K, Rd);
                    uint16_t vRd = rbank.get(Rd) |
                                   (static_cast<uint16_t>(rbank.get(static_cast<AVRRegister>(Rd + 1))) << 8);
                    uint16_t res = vRd + K;
                    rbank.set(Rd, res & 0xff);
                    rbank.set(static_cast<AVRRegister>(Rd + 1), (res >> 8) & 0xff);
                    cycles = 2;
                    rbank.setSREG(SREG_V, ((~vRd & res) >> 15) & 1);
                    rbank.setSREG(SREG_C, ((vRd & ~res) >> 15) & 1);
                    updateZNSFlags16(res);
                    ok = true;
                    ILOG("ADIW R%d:R%d, %d", Rd + 1, Rd, K);
                }
                    break;

                case 0b0111: {
                    // SBIW - subtract immediate from word (1001 0111 KKdd KKKK)

                    decode6K2d(opcode, K, Rd);
                    uint16_t vRd = rbank.get(Rd) |
                                   (static_cast<uint16_t>(rbank.get(static_cast<AVRRegister>(Rd + 1))) << 8);
                    uint16_t res = vRd - K;
                    rbank.set(Rd, res & 0xff);
                    rbank.set(static_cast<AVRRegister>(Rd + 1), (res >> 8) & 0xff);
                    cycles = 2;
                    rbank.setSREG(SREG_V, ((~vRd & res) >> 15) & 1);
                    rbank.setSREG(SREG_C, ((vRd & ~res) >> 15) & 1);
                    updateZNSFlags16(res);
                    ok = true;
                    ILOG("SBIW R%d:R%d, %d", Rd + 1, Rd, K);
                }
                    break;

                case 0b1000: {
                    // CBI - clear bit in i/o register (1001 1000 AAAA Abbb)

                    decode5A3b(opcode, A, K);

                    setData(A + 0x20, getData(A + 0x20) & ~(1 << K));
                    cycles = 2;
                    ok = true;
                    ILOG("CBI %d, %d", A, K);
                }
                    break;

                case 0b1001: {
                    // SBIC - skip if bit in i/o register is clear (1001 1001 AAAA Abbb)

                    decode5A3b(opcode, A, K);
                    if (!((getData(A + 0x20) >> K) & 1)) {
                        SKIP();
                    }
                    ok = true;
                    ILOG("SBIC %d, %d", A, K);
                }
                    break;

                case 0b1010: {
                    // SBI - set bit in I/O register

                    decode5A3b(opcode, A, K);
                    setData(A + 0x20, getData(A + 0x20) | (1 << K));
                    cycles = 2;
                    ok = true;
                    ILOG("SBI %d, %d", A, K);
                }
                    break;

                case 0b1011: {
                    // SBIS - skip if bit in i/o register is clear (1001 1011 AAAA Abbb)

                    decode5A3b(opcode, A, K);
                    if ((getData(A + 0x20) >> K) & 1) {
                        SKIP();
                    }
                    ok = true;
                    ILOG("SBIS %d, %d", A, K);
                }
                    break;

                case 0b1100:
                case 0b1101:
                case 0b1110:
                case 0b1111: {
                    // MUL - multiply unsigned (1001 11rd dddd rrrr)

                    decode5d5r(opcode, Rd, Rr);
                    uint16_t vRd = rbank.get(Rd);
                    uint16_t vRr = rbank.get(Rr);
                    uint16_t res = vRd * vRr;
                    rbank.set(AVRRegister::R0, res & 0xff);
                    rbank.set(AVRRegister::R1, res >> 8);
                    rbank.setSREG(SREG_C, (res >> 15) & 1);
                    rbank.setSREG(SREG_Z, res == 0);
                    ok = true;
                    cycles = 2;
                    ILOG("MUL R%d, R%d", Rd, Rr);
                }
                    break;
            }
        }
            break;

        case 0b1011: {
            // IN/OUT - load/store an I/O location to/from register (1011 oAAd dddd AAAA)

            decode5d6A(opcode, Rd, K);

            if ((opcode >> 11) & 1) {
                ILOG("OUT %d, R%d", K, Rd);
                io.set(K + 0x20, rbank.get(Rd));
            } else {
                ILOG("IN R%d, %d", Rd, K);
                rbank.set(Rd, io.get(K + 0x20));
            }
            ok = true;
        }
            break;

        case 0b1100: {
            // RJMP - relative jump (1100 kkkk kkkk kkkk)

            decode12k(opcode, k);
            cycles = 2;
            new_pc = pc + k + 1;
            ok = true;
            ILOG("RJMP %d (JMP 0x%04x)", k, new_pc << 1);
        }
            break;

        case 0b1101: {
            // RCALL - relative call (1101 kkkk kkkk kkkk)

            decode12k(opcode, k);
            cycles = 4;
            pushStack24(pc + 1);
            new_pc = pc + k + 1;
            ok = true;
            ILOG("RCALL %d (CALL 0x%04x)", k, new_pc << 1);
        }
            break;

        case 0b1110: {
            // LDI - load immediate (1110 KKKK dddd KKKK)

            decode8K4d(opcode, K, Rd);
            rbank.set(Rd, K);
            ok = true;
            ILOG("LDI R%d, %d", Rd, K);
        }
            break;

        case 0b1111: {
            switch (op7 & 0b111) {
                case 0b010:
                case 0b011: {
                    // BRBC - branch if bit in SREG is cleared (1111 01kk kkkk ksss)

                    decode7k3b(opcode, k, K);
                    if (!rbank.getSREG(static_cast<AVRStatusBit>(K))) {
                        cycles++;
                        new_pc = pc + k + 1;
                    }
                    ok = true;
                    ILOG("BRBC %d, %d", K, k);
                }
                    break;

                case 0b000:
                case 0b001: {
                    // BRBS - branch if bit in SREG is set (1111 01kk kkkk ksss)

                    decode7k3b(opcode, k, K);
                    if (rbank.getSREG(static_cast<AVRStatusBit>(K))) {
                        cycles++;
                        new_pc = pc + k + 1;
                    }
                    ok = true;
                    ILOG("BRBS %d, %d", K, k);
                }
                    break;

                case 0b100: {
                    if ((opcode >> 3) & 1) {
                    } else {
                        // BLD - bit load to bit in register from T flag in SREG (1111 100r rrrr 0bbb)

                        decode5r3b(opcode, Rr, K);
                        if (rbank.getSREG(SREG_T)) {
                            rbank.set(Rr, rbank.get(Rr) | (1 << K));
                        } else {
                            rbank.set(Rr, rbank.get(Rr) & ~(1 << K));
                        }
                        ok = true;
                        ILOG("BLD R%d, %d", Rr, K);
                    }
                }
                    break;

                case 0b101: {
                    if ((opcode >> 3) & 1) {
                    } else {
                        // BST - bit store from bit in register to T flag in SREG (1111 101r rrrr 0bbb)

                        decode5r3b(opcode, Rr, K);
                        rbank.setSREG(SREG_T, (rbank.get(Rr) >> K) & 1);
                        ok = true;
                        ILOG("BST R%d, %d", Rr, K);
                    }
                }
                    break;

                case 0b110: {
                    if ((opcode >> 3) & 1) {
                    } else {
                        // SBRC - skip if bit in register is cleared (1111 110r rrrr 0bbb)

                        decode5r3b(opcode, Rr, K);
                        uint8_t vRr = rbank.get(Rr);
                        if (!((vRr >> K) & 1)) {
                            SKIP();
                        }
                        ok = true;
                        ILOG("SBRC R%d, %d", Rr, K);
                    }
                }
                    break;

                case 0b111: {
                    if ((opcode >> 3) & 1) {
                    } else {
                        // SBRS - skip if bit in register is set (1111 111r rrrr 0bbb)

                        decode5r3b(opcode, Rr, K);
                        uint8_t vRr = rbank.get(Rr);
                        if ((vRr >> K) & 1) {
                            SKIP();
                        }
                        ok = true;
                        ILOG("SBRS R%d, %d", Rr, K);
                    }
                }
                    break;

            }
        }
            break;
    }

    if (!ok) {
        switch (opcode) {
            case 0:
                // NOP

                ILOG("NOP")
                ok = true;
                break;

            case 0b1001010000001001: {
                // IJMP - indirect jump (1001 0100 0000 1001)

                new_pc = rbank.get(AVRRegister::Z);
                cycles = 2;
                ok = true;
                ILOG("IJMP (JMP 0x%04x)", new_pc << 1);
            }
                break;

            case 0b1001010000011001: {
                // EIJMP - extended indirect jump (1001 0100 0001 1001)

                new_pc = rbank.get(AVRRegister::Z) | (rbank.get(AVRRegister::EIND) << 16);
                cycles = 2;
                ok = true;
                ILOG("EJMP (JMP 0x%04x)", new_pc << 1);
            }
                break;

            case 0b1001010100001000: {
                // RET - return from subroutine (1001 0101 0000 1000)

                new_pc = popStack24();
                ok = true;
                cycles = 5;
                ILOG("RET");
            }
                break;

            case 0b1001010100001001: {
                // ICALL - indirect call (1001 0101 0000 1001)

                new_pc = rbank.get(AVRRegister::Z);
                pushStack24(pc + 1);
                cycles = 4;
                ok = true;
                ILOG("ICALL (CALL 0x%04x)", new_pc << 1);
            }
                break;

            case 0b1001010100011000: {
                // RETI - return from interrupt (1001 0101 0001 1000)

                new_pc = popStack24();
                rbank.setSREG(SREG_I, true);
                ok = true;
                cycles = 5;
                processIRQ = false;
                ILOG("RETI");
            }
                break;

            case 0b1001010100011001: {
                // EICALL - extended indirect call (1001 0101 0001 1001)

                new_pc = rbank.get(AVRRegister::Z) | (rbank.get(AVRRegister::EIND) << 16);
                pushStack24(pc + 1);
                cycles = 4;
                ok = true;
                ILOG("EICALL (CALL 0x%04x)", new_pc << 1);
            }
                break;

            case 0b1001010111001000: {
                // LPM - load program memory (1001 0101 1100 1000)
                unsigned int addr = rbank.get(AVRRegister::Z);
                uint16_t data = getProgram(addr >> 1);
                uint8_t byte;
                if (addr & 1) {
                    byte = data >> 8;
                } else {
                    byte = data & 0xff;
                }
                rbank.set(AVRRegister::R0, byte);

                cycles = 2;
                ok = true;
                ILOG("LPM");
            }
                break;

            case 0b1001010111011000: {
                // ELPM - extended load program memory (1001 0101 1101 1000)
                unsigned int addr = rbank.get(AVRRegister::Z) +
                                    (rbank.get(AVRRegister::RAMPZ) << 16);
                uint16_t data = getProgram(addr >> 1);
                uint8_t byte;
                if (addr & 1) {
                    byte = data >> 8;
                } else {
                    byte = data & 0xff;
                }
                rbank.set(AVRRegister::R0, byte);

                cycles = 2;
                ok = true;
                ILOG("ELPM");
            }
                break;
        }
    }

    if (!ok) {
        if ((opcode & 0xd200) == 0x8000) {
            // LDD - load indirect from data space to register (10k0 kk0d dddd ykkk)

            char reg;
            if (opcode & 0x8) {
                Rd = AVRRegister::Y;
                reg = 'Y';
            } else {
                Rd = AVRRegister::Z;
                reg = 'Z';
            }
            Rr = static_cast<AVRRegister>((opcode >> 4) & 0x1f);
            K = (opcode >> 13) & 0x1;
            K = (K << 2) | ((opcode >> 10) & 0x3);
            K = (K << 3) | (opcode & 0x7);
            rbank.set(Rr, getData(rbank.get(Rd) + K));
            cycles = 2;
            ok = true;
            ILOG("LD R%d, %c+%d", Rr, reg, K);
        }

        if ((opcode & 0xd200) == 0x8200) {
            // ST - store indirect to data space from register (10k0 kk1d dddd ykkk)

            char reg;
            if (opcode & 0x8) {
                Rd = AVRRegister::Y;
                reg = 'Y';
            } else {
                Rd = AVRRegister::Z;
                reg = 'Z';
            }
            Rr = static_cast<AVRRegister>((opcode >> 4) & 0x1f);
            K = (opcode >> 13) & 0x1;
            K = (K << 2) | ((opcode >> 10) & 0x3);
            K = (K << 3) | (opcode & 0x7);
            setData(rbank.get(Rd) + K, rbank.get(Rr));
            cycles = 2;
            ok = true;
            ILOG("ST R%d, %c+%d", Rr, reg, K);
        }

    }

    if (!ok) {
        switch (opcode & 0xfe0e) {
            case 0x940c: {
                // JMP - jump (1001 010k kkkk 110k / kkkk kkkk kkkk kkkk)

                decode22K(opcode, getProgram(pc + 1), K);
                new_pc = K;
                cycles = 3;
                ok = true;
                ILOG("JMP 0x%04x", K << 1);
            }
                break;

            case 0x940e: {
                // CALL - call (1001 010k kkkk 111k / kkkk kkkk kkkk kkkk)

                decode22K(opcode, getProgram(pc + 1), K);
                new_pc = K;
                pushStack24(pc + 2);
                cycles = 5;
                ok = true;
                ILOG("CALL 0x%04x", K << 1);
            }
                break;
        }
    }

    if (!ok) {
        fprintf(stderr, "Warning: unknown instruction at 0x%04x, opcode = %04x\n", pc * 2, opcode);
    }

    io.tick(cycles);
    rbank.tick(cycles);

    if (processIRQ && rbank.hasIRQ()) {
        pushStack24(new_pc);
        unsigned int irq = rbank.getIRQ() - 1;
        io.clearIRQ(irq);
        ILOG("IRQ %d", irq);
        new_pc = (irq << 1);
        rbank.setSREG(SREG_I, false);
    }

    rbank.set(AVRRegister::PC, new_pc);

    return cycles;
}

void AVR::run() {
    clock_t start = clock();
    unsigned long long last_burst = 0;

    while (true) {
        tick();
        if (rbank.getCycles() - last_burst >= FREQ / BURSTS_PER_SECOND) {
            clock_t end = clock();
            long long elapsed = 1000000ll * (end - start) / CLOCKS_PER_SEC;
            long long required = 1000000ll / BURSTS_PER_SECOND;
            long long usec = required - elapsed;
            if (usec > 0)
                usleep(usec);
            last_burst = rbank.getCycles();
            start = clock();
        }
    }
}
