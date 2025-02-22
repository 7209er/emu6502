#include <stdio.h>
#include "emu6502.h"
#include "bus.h"

int main(){
    Bus bus;
    bus.cpu.reset();

    bus.ram[0xEFFC] = 0x4C;             // JMP #1414
    bus.ram[0xEFFD] = 0x14;             //
    bus.ram[0xEFFE] = 0x14;             //
    bus.ram[0x1414 - 0x1000] = 0xA2;    // LDX #21
    bus.ram[0x1415 - 0x1000] = 0x21;    // STX #0400 (triggers output)
    bus.ram[0x1416 - 0x1000] = 0x8E;    //
    bus.ram[0x1417 - 0x1000] = 0x00;    //
    bus.ram[0x1418 - 0x1000] = 0x04;    //

    int counter = 20;
    while(counter){
        bus.clock();
        
        counter--;
    }

    return 0;
}