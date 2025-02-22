// Reccently I got interested in CPU emulation. Some friends suggested that I should look 
// into the 6502 because of its simple design. So I tried a few approches, guided by the
// docs. Then I came accross Javidx9's video series of NES emulation.
// I found his approach quite nice, and thus decided to implement it myself.
// Here is a link to his channel: https://www.youtube.com/javidx9 
//
// I made some minor changes, like using Bitfields and an array instead of vectors.
// Additionally, I created a mock device that outputs a text if its memory address
// gets new data. I plan on integrating a line drawing device using OpenGL next.
// ~7209er

#pragma once

#include "datatypes.h"

// http://www.6502.org/users/obelisk/6502/architecturew.html


// Forward declaration of the bus class to avoid circular inclusions
class Bus;


class emu6502{
public:
    emu6502();
    ~emu6502();

    // CPU registers
    WORD PC = 0x0000;   // Program counter
    BYTE SP = 0x00;     // Stack pointer (points to 0x0100 - 0x01FF)
    BYTE X  = 0x00;     // X register
    BYTE Y  = 0x00;     // Y register
    BYTE A  = 0x00;     // Accumulator

    // Flags
    BYTE Cf : 1 = 0;     // Carry bit
    BYTE Zf : 1 = 0;     // Zero
    BYTE If : 1 = 0;     // Disable interupts
    BYTE Df : 1 = 0;     // Decimal mode !!not implemented!!
    BYTE Bf : 1 = 0;     // Break
    BYTE Vf : 1 = 0;     // Overflow
    BYTE Nf : 1 = 0;     // Negative
    BYTE Uf : 1 = 0;     // Unused bit

    // Enums for accessing flags
    enum FLAGS{
        C = 0,
        Z = 1,
        I = 2,
        D = 3,
        B = 4,
        V = 5,
        N = 6,
        U = 7
    };

    // Flags
    void setFlag(FLAGS flag, bool val);
    BYTE getFlag(FLAGS flag);

    void reset();
    void clock();
    // irq, nmi

    bool completed();

    // Connecting the CPU with the bus
    void ConnectBus(Bus* t) { bus = t; }
    
    
private:
    // Components for the bus
    Bus* bus = nullptr;
    BYTE read(WORD addr);
    void write(WORD addr, BYTE data);

    // Auxiliary variables
    BYTE fetched     = 0x00;     // Holds the data fetched inside the address mode functions
    BYTE opcode      = 0x00;     // Holds the current instruction
    WORD tempVal     = 0x00;
    WORD addr_abs    = 0x0000;   // Holds the absolute address 
    WORD addr_rel    = 0x0000;   // Holds the relative address 
    BYTE cycles      = 0;        // Counts the remaining cycles

    BYTE fetch();

    struct INSTRUCTION{
        BYTE (emu6502::*operate ) (void) = nullptr; // Function pointer to the current operation
        BYTE (emu6502::*addrmode) (void) = nullptr; // Function pointer to the current addressing mode
        BYTE cycles = 0;
    };

    // Lookup table for the instructions
    // Each entry consists of a pointer to the corresponding function, a pointer to the mode
    // and the associated number of cycles. The positon in the table corresponds to the opcode
    // table.
    // For more info visit page 10 of https://web.archive.org/web/20221112231348if_/http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf
    INSTRUCTION lookup[256] = {
        { &emu6502::BRK, &emu6502::IMM, 7 },{ &emu6502::ORA, &emu6502::IZX, 6 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 8 },{ &emu6502::NOP, &emu6502::IMP, 3 },{ &emu6502::ORA, &emu6502::ZP0, 3 },{ &emu6502::ASL, &emu6502::ZP0, 5 },{ &emu6502::XXX, &emu6502::IMP, 5 },{ &emu6502::PHP, &emu6502::IMP, 3 },{ &emu6502::ORA, &emu6502::IMM, 2 },{ &emu6502::ASL, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::NOP, &emu6502::IMP, 4 },{ &emu6502::ORA, &emu6502::ABS, 4 },{ &emu6502::ASL, &emu6502::ABS, 6 },{ &emu6502::XXX, &emu6502::IMP, 6 },
        { &emu6502::BPL, &emu6502::REL, 2 },{ &emu6502::ORA, &emu6502::IZY, 5 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 8 },{ &emu6502::NOP, &emu6502::IMP, 4 },{ &emu6502::ORA, &emu6502::ZPX, 4 },{ &emu6502::ASL, &emu6502::ZPX, 6 },{ &emu6502::XXX, &emu6502::IMP, 6 },{ &emu6502::CLC, &emu6502::IMP, 2 },{ &emu6502::ORA, &emu6502::ABY, 4 },{ &emu6502::NOP, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 7 },{ &emu6502::NOP, &emu6502::IMP, 4 },{ &emu6502::ORA, &emu6502::ABX, 4 },{ &emu6502::ASL, &emu6502::ABX, 7 },{ &emu6502::XXX, &emu6502::IMP, 7 },
		{ &emu6502::JSR, &emu6502::ABS, 6 },{ &emu6502::AND, &emu6502::IZX, 6 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 8 },{ &emu6502::BIT, &emu6502::ZP0, 3 },{ &emu6502::AND, &emu6502::ZP0, 3 },{ &emu6502::ROL, &emu6502::ZP0, 5 },{ &emu6502::XXX, &emu6502::IMP, 5 },{ &emu6502::PLP, &emu6502::IMP, 4 },{ &emu6502::AND, &emu6502::IMM, 2 },{ &emu6502::ROL, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::BIT, &emu6502::ABS, 4 },{ &emu6502::AND, &emu6502::ABS, 4 },{ &emu6502::ROL, &emu6502::ABS, 6 },{ &emu6502::XXX, &emu6502::IMP, 6 },
		{ &emu6502::BMI, &emu6502::REL, 2 },{ &emu6502::AND, &emu6502::IZY, 5 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 8 },{ &emu6502::NOP, &emu6502::IMP, 4 },{ &emu6502::AND, &emu6502::ZPX, 4 },{ &emu6502::ROL, &emu6502::ZPX, 6 },{ &emu6502::XXX, &emu6502::IMP, 6 },{ &emu6502::SEC, &emu6502::IMP, 2 },{ &emu6502::AND, &emu6502::ABY, 4 },{ &emu6502::NOP, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 7 },{ &emu6502::NOP, &emu6502::IMP, 4 },{ &emu6502::AND, &emu6502::ABX, 4 },{ &emu6502::ROL, &emu6502::ABX, 7 },{ &emu6502::XXX, &emu6502::IMP, 7 },
		{ &emu6502::RTI, &emu6502::IMP, 6 },{ &emu6502::EOR, &emu6502::IZX, 6 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 8 },{ &emu6502::NOP, &emu6502::IMP, 3 },{ &emu6502::EOR, &emu6502::ZP0, 3 },{ &emu6502::LSR, &emu6502::ZP0, 5 },{ &emu6502::XXX, &emu6502::IMP, 5 },{ &emu6502::PHA, &emu6502::IMP, 3 },{ &emu6502::EOR, &emu6502::IMM, 2 },{ &emu6502::LSR, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::JMP, &emu6502::ABS, 3 },{ &emu6502::EOR, &emu6502::ABS, 4 },{ &emu6502::LSR, &emu6502::ABS, 6 },{ &emu6502::XXX, &emu6502::IMP, 6 },
		{ &emu6502::BVC, &emu6502::REL, 2 },{ &emu6502::EOR, &emu6502::IZY, 5 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 8 },{ &emu6502::NOP, &emu6502::IMP, 4 },{ &emu6502::EOR, &emu6502::ZPX, 4 },{ &emu6502::LSR, &emu6502::ZPX, 6 },{ &emu6502::XXX, &emu6502::IMP, 6 },{ &emu6502::CLI, &emu6502::IMP, 2 },{ &emu6502::EOR, &emu6502::ABY, 4 },{ &emu6502::NOP, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 7 },{ &emu6502::NOP, &emu6502::IMP, 4 },{ &emu6502::EOR, &emu6502::ABX, 4 },{ &emu6502::LSR, &emu6502::ABX, 7 },{ &emu6502::XXX, &emu6502::IMP, 7 },
		{ &emu6502::RTS, &emu6502::IMP, 6 },{ &emu6502::ADC, &emu6502::IZX, 6 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 8 },{ &emu6502::NOP, &emu6502::IMP, 3 },{ &emu6502::ADC, &emu6502::ZP0, 3 },{ &emu6502::ROR, &emu6502::ZP0, 5 },{ &emu6502::XXX, &emu6502::IMP, 5 },{ &emu6502::PLA, &emu6502::IMP, 4 },{ &emu6502::ADC, &emu6502::IMM, 2 },{ &emu6502::ROR, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::JMP, &emu6502::IND, 5 },{ &emu6502::ADC, &emu6502::ABS, 4 },{ &emu6502::ROR, &emu6502::ABS, 6 },{ &emu6502::XXX, &emu6502::IMP, 6 },
		{ &emu6502::BVS, &emu6502::REL, 2 },{ &emu6502::ADC, &emu6502::IZY, 5 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 8 },{ &emu6502::NOP, &emu6502::IMP, 4 },{ &emu6502::ADC, &emu6502::ZPX, 4 },{ &emu6502::ROR, &emu6502::ZPX, 6 },{ &emu6502::XXX, &emu6502::IMP, 6 },{ &emu6502::SEI, &emu6502::IMP, 2 },{ &emu6502::ADC, &emu6502::ABY, 4 },{ &emu6502::NOP, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 7 },{ &emu6502::NOP, &emu6502::IMP, 4 },{ &emu6502::ADC, &emu6502::ABX, 4 },{ &emu6502::ROR, &emu6502::ABX, 7 },{ &emu6502::XXX, &emu6502::IMP, 7 },
		{ &emu6502::NOP, &emu6502::IMP, 2 },{ &emu6502::STA, &emu6502::IZX, 6 },{ &emu6502::NOP, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 6 },{ &emu6502::STY, &emu6502::ZP0, 3 },{ &emu6502::STA, &emu6502::ZP0, 3 },{ &emu6502::STX, &emu6502::ZP0, 3 },{ &emu6502::XXX, &emu6502::IMP, 3 },{ &emu6502::DEY, &emu6502::IMP, 2 },{ &emu6502::NOP, &emu6502::IMP, 2 },{ &emu6502::TXA, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::STY, &emu6502::ABS, 4 },{ &emu6502::STA, &emu6502::ABS, 4 },{ &emu6502::STX, &emu6502::ABS, 4 },{ &emu6502::XXX, &emu6502::IMP, 4 },
		{ &emu6502::BCC, &emu6502::REL, 2 },{ &emu6502::STA, &emu6502::IZY, 6 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 6 },{ &emu6502::STY, &emu6502::ZPX, 4 },{ &emu6502::STA, &emu6502::ZPX, 4 },{ &emu6502::STX, &emu6502::ZPY, 4 },{ &emu6502::XXX, &emu6502::IMP, 4 },{ &emu6502::TYA, &emu6502::IMP, 2 },{ &emu6502::STA, &emu6502::ABY, 5 },{ &emu6502::TXS, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 5 },{ &emu6502::NOP, &emu6502::IMP, 5 },{ &emu6502::STA, &emu6502::ABX, 5 },{ &emu6502::XXX, &emu6502::IMP, 5 },{ &emu6502::XXX, &emu6502::IMP, 5 },
		{ &emu6502::LDY, &emu6502::IMM, 2 },{ &emu6502::LDA, &emu6502::IZX, 6 },{ &emu6502::LDX, &emu6502::IMM, 2 },{ &emu6502::XXX, &emu6502::IMP, 6 },{ &emu6502::LDY, &emu6502::ZP0, 3 },{ &emu6502::LDA, &emu6502::ZP0, 3 },{ &emu6502::LDX, &emu6502::ZP0, 3 },{ &emu6502::XXX, &emu6502::IMP, 3 },{ &emu6502::TAY, &emu6502::IMP, 2 },{ &emu6502::LDA, &emu6502::IMM, 2 },{ &emu6502::TAX, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::LDY, &emu6502::ABS, 4 },{ &emu6502::LDA, &emu6502::ABS, 4 },{ &emu6502::LDX, &emu6502::ABS, 4 },{ &emu6502::XXX, &emu6502::IMP, 4 },
		{ &emu6502::BCS, &emu6502::REL, 2 },{ &emu6502::LDA, &emu6502::IZY, 5 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 5 },{ &emu6502::LDY, &emu6502::ZPX, 4 },{ &emu6502::LDA, &emu6502::ZPX, 4 },{ &emu6502::LDX, &emu6502::ZPY, 4 },{ &emu6502::XXX, &emu6502::IMP, 4 },{ &emu6502::CLV, &emu6502::IMP, 2 },{ &emu6502::LDA, &emu6502::ABY, 4 },{ &emu6502::TSX, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 4 },{ &emu6502::LDY, &emu6502::ABX, 4 },{ &emu6502::LDA, &emu6502::ABX, 4 },{ &emu6502::LDX, &emu6502::ABY, 4 },{ &emu6502::XXX, &emu6502::IMP, 4 },
		{ &emu6502::CPY, &emu6502::IMM, 2 },{ &emu6502::CMP, &emu6502::IZX, 6 },{ &emu6502::NOP, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 8 },{ &emu6502::CPY, &emu6502::ZP0, 3 },{ &emu6502::CMP, &emu6502::ZP0, 3 },{ &emu6502::DEC, &emu6502::ZP0, 5 },{ &emu6502::XXX, &emu6502::IMP, 5 },{ &emu6502::INY, &emu6502::IMP, 2 },{ &emu6502::CMP, &emu6502::IMM, 2 },{ &emu6502::DEX, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::CPY, &emu6502::ABS, 4 },{ &emu6502::CMP, &emu6502::ABS, 4 },{ &emu6502::DEC, &emu6502::ABS, 6 },{ &emu6502::XXX, &emu6502::IMP, 6 },
		{ &emu6502::BNE, &emu6502::REL, 2 },{ &emu6502::CMP, &emu6502::IZY, 5 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 8 },{ &emu6502::NOP, &emu6502::IMP, 4 },{ &emu6502::CMP, &emu6502::ZPX, 4 },{ &emu6502::DEC, &emu6502::ZPX, 6 },{ &emu6502::XXX, &emu6502::IMP, 6 },{ &emu6502::CLD, &emu6502::IMP, 2 },{ &emu6502::CMP, &emu6502::ABY, 4 },{ &emu6502::NOP, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 7 },{ &emu6502::NOP, &emu6502::IMP, 4 },{ &emu6502::CMP, &emu6502::ABX, 4 },{ &emu6502::DEC, &emu6502::ABX, 7 },{ &emu6502::XXX, &emu6502::IMP, 7 },
		{ &emu6502::CPX, &emu6502::IMM, 2 },{ &emu6502::SBC, &emu6502::IZX, 6 },{ &emu6502::NOP, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 8 },{ &emu6502::CPX, &emu6502::ZP0, 3 },{ &emu6502::SBC, &emu6502::ZP0, 3 },{ &emu6502::INC, &emu6502::ZP0, 5 },{ &emu6502::XXX, &emu6502::IMP, 5 },{ &emu6502::INX, &emu6502::IMP, 2 },{ &emu6502::SBC, &emu6502::IMM, 2 },{ &emu6502::NOP, &emu6502::IMP, 2 },{ &emu6502::SBC, &emu6502::IMP, 2 },{ &emu6502::CPX, &emu6502::ABS, 4 },{ &emu6502::SBC, &emu6502::ABS, 4 },{ &emu6502::INC, &emu6502::ABS, 6 },{ &emu6502::XXX, &emu6502::IMP, 6 },
		{ &emu6502::BEQ, &emu6502::REL, 2 },{ &emu6502::SBC, &emu6502::IZY, 5 },{ &emu6502::XXX, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 8 },{ &emu6502::NOP, &emu6502::IMP, 4 },{ &emu6502::SBC, &emu6502::ZPX, 4 },{ &emu6502::INC, &emu6502::ZPX, 6 },{ &emu6502::XXX, &emu6502::IMP, 6 },{ &emu6502::SED, &emu6502::IMP, 2 },{ &emu6502::SBC, &emu6502::ABY, 4 },{ &emu6502::NOP, &emu6502::IMP, 2 },{ &emu6502::XXX, &emu6502::IMP, 7 },{ &emu6502::NOP, &emu6502::IMP, 4 },{ &emu6502::SBC, &emu6502::ABX, 4 },{ &emu6502::INC, &emu6502::ABX, 7 },{ &emu6502::XXX, &emu6502::IMP, 7 }
    };

    // Addressing modes
    // Each type of addressing has a number of cycles associated with it.
    // Some modes can require an additional cycle under certain conditions,
    // they will signal this by returning 1.
    // For more info visit the docs: http://www.6502.org/users/obelisk/6502/reference.html
    BYTE IMP(); BYTE IMM();
    BYTE ZP0(); BYTE ZPX();
    BYTE ZPY(); BYTE REL();
    BYTE ABS(); BYTE ABX();
    BYTE ABY(); BYTE IND();
    BYTE IZX(); BYTE IZY();

    // Opcodes
    // These are the 56 official opcodes of the 6502.
    // By splitting up opcodes from addressing modes, only 56 + 12 = 68 cenarios have
    // to be implemented. Implementing each operation + addressing mode manually
    // would require 128+ cases.
    // Some opcodes can require an additional cycle under certain conditions,
    // they will signal this by returning 1.
    // For more info, again, visit the docs: http://www.6502.org/users/obelisk/6502/reference.html
    BYTE ADC();	BYTE AND();	BYTE ASL();	BYTE BCC();
	BYTE BCS();	BYTE BEQ();	BYTE BIT();	BYTE BMI();
	BYTE BNE();	BYTE BPL();	BYTE BRK();	BYTE BVC();
	BYTE BVS();	BYTE CLC();	BYTE CLD();	BYTE CLI();
	BYTE CLV();	BYTE CMP();	BYTE CPX();	BYTE CPY();
	BYTE DEC();	BYTE DEX();	BYTE DEY();	BYTE EOR();
	BYTE INC();	BYTE INX();	BYTE INY();	BYTE JMP();
	BYTE JSR();	BYTE LDA();	BYTE LDX();	BYTE LDY();
	BYTE LSR();	BYTE NOP();	BYTE ORA();	BYTE PHA();
	BYTE PHP();	BYTE PLA();	BYTE PLP();	BYTE ROL();
	BYTE ROR();	BYTE RTI();	BYTE RTS();	BYTE SBC();
	BYTE SEC();	BYTE SED();	BYTE SEI();	BYTE STA();
	BYTE STX();	BYTE STY();	BYTE TAX();	BYTE TAY();
	BYTE TSX();	BYTE TXA();	BYTE TXS();	BYTE TYA();

    // This function captures all unofficial opcodes.
    // It does nothing and is implemented identical to the NOP
    BYTE XXX();

};