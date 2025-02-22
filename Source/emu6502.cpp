#include "emu6502.h"
#include "bus.h"

// Constructor
emu6502::emu6502(){
	// Does nothing
}

emu6502::~emu6502(){
	// The destructor does nothing.
}

// Interacting with the bus
BYTE emu6502::read(WORD addr){
	return bus->read(addr);
};

void emu6502::write(WORD addr, BYTE data){
	bus->write(addr, data);
}

// Reporting if an operation has finished
bool emu6502::completed(){
	return cycles == 0;
}

// Setting flags
void emu6502::setFlag(FLAGS flag, bool val){
	switch(flag){
		case C : Cf = val; break;
		case Z : Zf = val; break;
		case I : If = val; break;
		case D : Df = val; break;
		case B : Bf = val; break;
		case V : Vf = val; break;
		case N : Nf = val; break;
		case U : Uf = val; break;
	}
}

BYTE emu6502::getFlag(FLAGS flag){
	switch(flag){
		case C : return Cf; break;
		case Z : return Zf; break;
		case I : return If; break;
		case D : return Df; break;
		case B : return Bf; break;
		case V : return Vf; break;
		case N : return Nf; break;
		case U : return Uf; break;
	}
}

void emu6502::reset(){
	// Setting PC to start location
	PC = 0xFFFC;
	// Setting SP to start location
	SP = 0xFD;

	// Resetting registers
	X = 0x00;
	Y = 0x00;
	A = 0x00;
	// Resetting flags
	Cf = 0;
	Zf = 0;
	If = 0;
	Df = 0;
	Bf = 0;
	Vf = 0;
	Nf = 0;
	Uf = 0;

	// Clearing helpers
	fetched     = 0x00;
 	addr_abs    = 0x0000;
	addr_rel    = 0x0000;

	cycles = 8;
}

// The clock function works atomicly. So, instead of executing a tiny bit of code per cycle,
// it will execute the whole operation at one go. To still have predictable length of operations
// the cycles are decremented accordingly 
void emu6502::clock(){
	if(cycles == 0){
		// If cycles equals 0, the last execution has finished and a new opcode is read
		opcode = read(PC);
		PC++;

		// Setting the corresponding cycles
		cycles = lookup[opcode].cycles;

		// If the address mode und operation require an extra cycle, they return 1, else 0
		// Here the operation will be executed
		BYTE additional_cycle1 = (this->*lookup[opcode].addrmode)();
		BYTE additional_cycle2 = (this->*lookup[opcode].operate)();

		cycles += (additional_cycle1 & additional_cycle2);
	}

	// Decrement the current number of cycles
	cycles--;
}


// Address modes
// The porpose of these address mode is, to set the absolute address to the right address.
// In the opcode functions, the data from this address will be fetched.
// Mode: Implied
BYTE emu6502::IMP(){
	fetched = A;
	return 0;
} 

// Mode: Immediate
BYTE emu6502::IMM(){
	addr_abs = PC++;
	return 0;
}

// Mode: Zero Page
BYTE emu6502::ZP0(){
	addr_abs = read(PC);
	PC++;
	addr_abs &= 0x00FF;
	return 0;
} 

// Mode: Zero Page X
BYTE emu6502::ZPX(){
	addr_abs = read(PC) + X;
	PC++;
	addr_abs &= 0x00FF;
	return 0;
}

// Mode: Zero Page Y
BYTE emu6502::ZPY(){
	addr_abs = read(PC) + Y;
	PC++;
	addr_abs &= 0x00FF;
	return 0;
}

// Mode: Relative
BYTE emu6502::REL(){
	addr_rel = read(PC);
	PC++;
	if(addr_rel & 0x80)
		addr_rel |= 0xFF00;
	return 0;
}

// Mode: Ansolute
BYTE emu6502::ABS(){
	// Little Endian
	WORD lo = read(PC);
	PC++;
	WORD hi = read(PC);
	PC++;
	addr_abs = (hi << 8) | lo;
	return 0;
} 

// Mode: Absolute X
BYTE emu6502::ABX(){
	WORD lo = read(PC);
	PC++;
	WORD hi = read(PC);
	PC++;
	addr_abs = (hi << 8) | lo;
	addr_abs += X;

	if((addr_abs & 0xFF00) != (hi << 8)) // If the first byte has changed, the page is turned
		return 1;
	else
		return 0;
}

// Mode: Absolute Y
BYTE emu6502::ABY(){
	WORD lo = read(PC);
	PC++;
	WORD hi = read(PC);
	PC++;
	addr_abs = (hi << 8) | lo;
	addr_abs += Y;

	if((addr_abs & 0xFF00) != (hi << 8)) // If the first byte has changed, the page is turned
		return 1;
	else
		return 0;
} 

// Mode: Indirect
BYTE emu6502::IND(){
	WORD lo = read(PC);
	PC++;
	WORD hi = read(PC);
	PC++;
	WORD ptr = (hi << 8) | lo;

	if(lo == 0x00FF){ // Simulate bug in the hardware
		addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr);
	}
	else{
		addr_abs = (read(ptr + 1) << 8) | read(ptr);
	}

	return 0;
}

// Mode: Indirect X
BYTE emu6502::IZX(){
	WORD temp = read(PC);
	PC++;

	WORD lo = read((temp + X) & 0x00FF);
	WORD hi = read((temp + X + 1) & 0x00FF);
	addr_abs = (hi << 8) | lo;

	return 0;
} 

// Mode: Indirect Y
BYTE emu6502::IZY(){
	WORD temp = read(PC);
	PC++;

	WORD lo = read(temp & 0x00FF);
	WORD hi = read((temp + 1) & 0x00FF);

	addr_abs = (hi << 8) | lo;
	addr_abs += Y;
	
	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1;
	else
		return 0;
}

BYTE emu6502::fetch(){
	if(!(lookup[opcode].addrmode == &emu6502::IMP)){
		fetched = read(addr_abs);
	}
	return fetched;
}

// Operations
// Visit http://www.6502.org/users/obelisk/6502/reference.html
// for more info

// Addition with Carry
BYTE emu6502::ADC(){
	fetch();

	tempVal = (WORD) A + (WORD) fetched + (WORD) getFlag(C); 
	setFlag(C, tempVal > 0xFF);
	setFlag(Z, (tempVal & 0x00FF) == 0);
	setFlag(V, (~((WORD) A ^ (WORD) fetched) & ((WORD) A ^ (WORD) tempVal)) & 0x0080);
	setFlag(N, tempVal & 0x80);
	A = tempVal & 0x00FF;

	// Can require an additional cycle
	return 1;
}

// Subtraction with Carry
BYTE emu6502::SBC(){
	fetch();

	WORD value = ((WORD) fetched) ^ 0x00FF;

	tempVal = (WORD) A + value + (WORD) getFlag(C); 
	setFlag(C, tempVal & 0xFF00);
	setFlag(Z, (tempVal & 0x00FF) == 0);
	setFlag(V, (tempVal ^ (WORD) A) & (tempVal ^ value) & 0x0080);
	setFlag(N, tempVal & 0x80);
	A = tempVal & 0x00FF;

	// Can require an additional cycle
	return 1;
}

// Bitwise Logic AND
BYTE emu6502::AND(){
	fetch();
	A = A & fetched;
	setFlag(Z, A == 0x00);
	setFlag(N, A & 0x80);
	return 1;
}

// Arithmetic Shift Left
BYTE emu6502::ASL(){
	fetch();
	tempVal = (WORD) fetched < 2;
	setFlag(C, (tempVal & 0x0100));
	setFlag(Z, (tempVal & 0x00FF) == 0x0000);
	setFlag(N, tempVal & 0x0080);
	if(lookup[opcode].addrmode == & emu6502::IMP)
		A = tempVal & 0x00FF;
	else
		write(addr_abs, tempVal & 0x00FF);
	return 0;
}

// Branch if Carry Clear
BYTE emu6502::BCC(){
	if(getFlag(C) == 0){
		cycles++;
		addr_abs = PC + addr_rel;

		if((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		
		PC = addr_abs;
	}
	return 0;
}

// Branch if Carry Set
BYTE emu6502::BCS(){
	if(getFlag(C) == 1){
		cycles++;
		addr_abs = PC + addr_rel;

		if((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		
		PC = addr_abs;
	}
	return 0;
}

// Branch if Equal
BYTE emu6502::BEQ(){
	if(getFlag(Z) == 1){
		cycles++;
		addr_abs = PC + addr_rel;

		if((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		
		PC = addr_abs;
	}
	return 0;
}

// Bit Test
BYTE emu6502::BIT(){
	fetch();

	tempVal = A & fetched;
	setFlag(Z, tempVal == 0x0000);
	setFlag(N, fetched & 0x80);
	setFlag(V, fetched & 0x40);

	return 0;
}

// Branch if Minus
BYTE emu6502::BMI(){
	if(getFlag(N) == 1){
		cycles++;
		addr_abs = PC + addr_rel;

		if((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		
		PC = addr_abs;
	}
	return 0;
}

// Branch if Not Equal
BYTE emu6502::BNE(){
	if(getFlag(Z) == 0){
		cycles++;
		addr_abs = PC + addr_rel;

		if((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		
		PC = addr_abs;
	}
	return 0;
}

// Branch if Positive
BYTE emu6502::BPL(){
	if(getFlag(N) == 0){
		cycles++;
		addr_abs = PC + addr_rel;

		if((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		
		PC = addr_abs;
	}
	return 0;
}

// Break
BYTE emu6502::BRK(){
	PC++;

	setFlag(I, 1);
	write(0x0100 + SP, (PC >> 8) & 0x00FF);
	SP--;
	write(0x0100 + SP, PC & 0x00FF);
	SP--;

	setFlag(B, 1);
	write(0x0100 + SP, (((((((((((((Uf < 1) | Nf) < 1) | Vf) < 1) | Bf) < 1) | Df) < 1) | If) < 1) | Zf) < 1) | Cf);
	SP--;
	setFlag(B, 0);
	PC = ((WORD) read(0xFFFF) << 8) | ((WORD) read(0xFFFE));

	return 0;
}

// Branch if Overflow Clear
BYTE emu6502::BVC(){
	if(getFlag(V) == 0){
		cycles++;
		addr_abs = PC + addr_rel;

		if((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		
		PC = addr_abs;
	}
	return 0;
}

// Branch if Overflow Set
BYTE emu6502::BVS(){
	if(getFlag(V) == 1){
		cycles++;
		addr_abs = PC + addr_rel;

		if((addr_abs & 0xFF00) != (PC & 0xFF00))
			cycles++;
		
		PC = addr_abs;
	}

	return 0;
}

// Clear Carry Flag
BYTE emu6502::CLC(){
	setFlag(C, 0);
	return 0;
}

// Clear Decimal Flag
BYTE emu6502::CLD(){
	setFlag(D, 0);
	return 0;
}

// Clear Interrupt Flag / Disable Interrupts
BYTE emu6502::CLI(){
	setFlag(I, 0);
	return 0;
}

// Clear Overflow Flag
BYTE emu6502::CLV(){
	setFlag(V, 0);
	return 0;
}

// Compare Accumulator Flag
BYTE emu6502::CMP(){
	fetch();
	tempVal = (WORD) A - (WORD) fetched;
	setFlag(C, A >= fetched);
	setFlag(Z, (tempVal & 0x00FF) == 0x0000);
	setFlag(N, tempVal & 0x0080);
	return 0;
}

// Compare X Register
BYTE emu6502::CPX(){
	fetch();
	tempVal = (WORD) X - (WORD) fetched;
	setFlag(C, X >= fetched);
	setFlag(Z, (tempVal & 0x00FF) == 0x0000);
	setFlag(N, tempVal & 0x0080);
	return 0;
}

// Compare Y Register
BYTE emu6502::CPY(){
	fetch();
	tempVal = (WORD) Y - (WORD) fetched;
	setFlag(C, Y >= fetched);
	setFlag(Z, (tempVal & 0x00FF) == 0x0000);
	setFlag(N, tempVal & 0x0080);
	return 0;
}

// Decrement Value at Memory Location
BYTE emu6502::DEC(){
	fetch();
	tempVal = fetched - 1;
	write(addr_abs, tempVal & 0x00FF);
	setFlag(Z, (tempVal & 0x00FF) == 0);
	setFlag(N, tempVal & 0x0080);
	return 0;
}

// Decrement X Register
BYTE emu6502::DEX(){
	X--;
	setFlag(Z, X == 0x00);
	setFlag(N, X & 0x80);
	return 0;
}

// Decrement Y Register
BYTE emu6502::DEY(){
	Y--;
	setFlag(Z, Y == 0x00);
	setFlag(N, Y & 0x80);
	return 0;
}

// Bitwise Logic OR
BYTE emu6502::EOR(){
	fetch();
	A = A ^ fetched;
	setFlag(Z, A == 0x00);
	setFlag(N, A & 0x80);
	return 0;
}

// Increment Value at Memory Location
BYTE emu6502::INC(){
	fetch();
	tempVal = fetched + 1;
	write(addr_abs, tempVal & 0x00FF);
	setFlag(Z, (tempVal & 0x00FF) == 0x0000);
	setFlag(N, tempVal & 0x0080);
	return 0;
}

// Increment X Register
BYTE emu6502::INX(){
	X++;
	setFlag(Z, X == 0x00);
	setFlag(N, X & 0x80);
	return 0;
}

// Increment Y Register
BYTE emu6502::INY(){
	Y++;
	setFlag(Z, Y == 0x00);
	setFlag(N, Y & 0x80);
	return 0;
}

// Jump to Location
BYTE emu6502::JMP(){
	PC = addr_abs;
	return 0;
}

// Jump to Sub-Routine
BYTE emu6502::JSR(){
	PC--;

	write(0x0100 + SP, (PC >> 8) & 0x00FF);
	SP--;
	write(0x0100 + SP, PC & 0x00FF);
	SP--;

	PC = addr_abs;
	return 0;
}

// Load the Accumulator
BYTE emu6502::LDA(){
	fetch();
	A = fetched;
	setFlag(Z, A == 0x00);
	setFlag(N, A & 0x80);
	return 1;
}

// Load X Register
BYTE emu6502::LDX(){
	fetch();
	X = fetched;
	setFlag(Z, X == 0x00);
	setFlag(N, X & 0x80);
	return 1;
}

// Load Y Register
BYTE emu6502::LDY(){
	fetch();
	Y = fetched;
	setFlag(Z, Y == 0x00);
	setFlag(N, Y & 0x80);
	return 1;
}


// Logical Shift Right
BYTE emu6502::LSR(){
	fetch();
	setFlag(C, fetched & 0x0001);
	tempVal = fetched >> 1;
	setFlag(Z, tempVal == 0x0000);
	setFlag(N, tempVal & 0x0080);
	if(lookup[opcode].addrmode == &emu6502::IMP)
		A = tempVal & 0x00FF;
	else
		write(addr_abs, tempVal & 0x00FF);
	return 0;
}

// No Operation
BYTE emu6502::NOP(){
	// Does nothing
	return 0;
}

// Bitwise Logic OR
BYTE emu6502::ORA(){
	fetch();
	A = A | fetched;
	setFlag(Z, A == 0x00);
	setFlag(N, A & 0x80);
	return 1;
}

// Push Accumulator to stack
BYTE emu6502::PHA(){
	write(0x0100 + SP, A);
	SP--;
	return 0;
}

// Push Status Register to stack
BYTE emu6502::PHP(){
	write(0x0100 + SP, (((((((((((((Uf < 1) | Nf) < 1) | Vf) < 1) | Bf) < 1) | Df) < 1) | If) < 1) | Zf) < 1) | Cf);
	SP--;
	return 0;
}

// Pop Accumulator of stack
BYTE emu6502::PLA(){
	SP++;
	A = read(0x0100 + SP);
	setFlag(Z, A = 0x00);
	setFlag(N, A & 0x80);
	return 0;
}

// Pop Status of the stack
BYTE emu6502::PLP(){
	SP++;
	tempVal = read(0x0100 + SP);
	Cf = (tempVal & 0x01) > 0;
	Zf = (tempVal & 0x02) > 0;
	If = (tempVal & 0x04) > 0;
	Df = (tempVal & 0x08) > 0;
	Bf = (tempVal & 0x10) > 0;
	Vf = (tempVal & 0x20) > 0;
	Nf = (tempVal & 0x40) > 0;
	Uf = (tempVal & 0x80) > 0;
	return 0;
}

// Rotate Left
BYTE emu6502::ROL(){
	fetch();
	tempVal = (fetched << 1) | getFlag(C);
	setFlag(C, tempVal & 0x0100);
	setFlag(Z, (tempVal & 0x00FF) == 0x0000);
	setFlag(N, tempVal & 0x0080);
	if(lookup[opcode].addrmode == &emu6502::IMP)
		A = tempVal & 0x00FF;
	else
		write(addr_abs, tempVal & 0x00FF);
	return 0;
}

// Rotate Right
BYTE emu6502::ROR(){
	fetch();
	tempVal = (fetched >> 1) | ((WORD) getFlag(C) << 7);
	setFlag(C, tempVal & 0x0001);
	setFlag(Z, (tempVal & 0x00FF) == 0x0000);
	setFlag(N, tempVal & 0x0080);
	if(lookup[opcode].addrmode == &emu6502::IMP)
		A = tempVal & 0x00FF;
	else
		write(addr_abs, tempVal & 0x00FF);
	return 0;
}

// Return from Interrupt
BYTE emu6502::RTI(){
	SP++;
	tempVal = read(0x0100 + SP);
	Cf = (tempVal & 0x01) > 0;
	Zf = (tempVal & 0x02) > 0;
	If = (tempVal & 0x04) > 0;
	Df = (tempVal & 0x08) > 0;
	Bf = (tempVal & 0x10) > 0;
	Vf = (tempVal & 0x20) > 0;
	Nf = (tempVal & 0x40) > 0;
	Uf = (tempVal & 0x80) > 0;
	
	SP++;
	PC = read(0x0100 + SP);
	SP++;
	PC |= (read(0x0100 + SP) << 8);
	return 0;
}

// Return from Subroutine
BYTE emu6502::RTS(){
	SP++;
	PC = read(0x0100 + SP);
	SP++;
	PC |= (read(0x0100 + SP) << 8);
	return 0;
}

// Set Carry Flag
BYTE emu6502::SEC(){
	setFlag(C, 1);
	return 0;
}

// Set Decimal Flag
BYTE emu6502::SED(){
	setFlag(D, 1);
	return 0;
}

// Set Interrupt Flag
BYTE emu6502::SEI(){
	setFlag(I, 1);
	return 0;
}

// Store Accumulator at address
BYTE emu6502::STA(){
	write(addr_abs, A);
	return 0;
}

// Store X at address
BYTE emu6502::STX(){
	write(addr_abs, X);
	return 0;
}

// Store Y at address
BYTE emu6502::STY(){
	write(addr_abs, Y);
	return 0;
}

// Transfer Accumulator to X
BYTE emu6502::TAX(){
	X = A;
	setFlag(Z, X == 0x00);
	setFlag(N, X & 0x80);
	return 0;
}

// Transfer Accumulator to Y
BYTE emu6502::TAY(){
	Y = A;
	setFlag(Z, Y == 0x00);
	setFlag(N, Y & 0x80);
	return 0;
}

// Transfer SP to X
BYTE emu6502::TSX(){
	X = SP;
	setFlag(Z, X == 0x00);
	setFlag(N, X & 0x80);
	return 0;
}

// Transfer X to Accumulator
BYTE emu6502::TXA(){
	A = X;
	setFlag(Z, A == 0x00);
	setFlag(N, A & 0x80);
	return 0;
}

// Transfer X to SP
BYTE emu6502::TXS(){
	SP = X;
	return 0;
}

// Transfer Y to Accumulator
BYTE emu6502::TYA(){
	A = Y;
	setFlag(Z, Y == 0x00);
	setFlag(N, Y & 0x80);
	return 0;
}

// Illegal opcodes
BYTE emu6502::XXX(){
	return 0;
}