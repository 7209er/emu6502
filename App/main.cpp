#include <stdio.h>
#include "emu6502.h"
#include "bus.h"

int main(){
    Bus bus;
    bus.cpu.reset();

    bus.write(0xFFFC, 0x4C);  // JMP #2000 
    bus.write(0xFFFD, 0x00);  //
    bus.write(0xFFFE, 0x20);  //

    // This program is inserted with down bellow
/*
    bus.ram[0x1414 - 0x1000] = 0xA2;    // LDX #21
    bus.ram[0x1415 - 0x1000] = 0x21;    // 
    bus.ram[0x1416 - 0x1000] = 0x8E;    // STX #0400 (triggers output)
    bus.ram[0x1417 - 0x1000] = 0x00;    //
    bus.ram[0x1418 - 0x1000] = 0x04;    //
    bus.ram[0x1419 - 0x1000] = 0xA0;    // LDY #23
    bus.ram[0x141A - 0x1000] = 0x23;    // 
    bus.ram[0x141B - 0x1000] = 0x8C;    // STY #0401 (triggers output)
    bus.ram[0x141C - 0x1000] = 0x01;    //
    bus.ram[0x141D - 0x1000] = 0x04;    //
*/

    bus.loadProgram("0xA2 21 0x8E 0x00 0x04 0xA0 0x23 0x8C 0x01 0x04");

    int counter = 20; // Random number, simply to halt the program
    while(counter){
        bus.clock();
        
        counter--;
    }

    return 0;
}