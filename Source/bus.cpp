#include "bus.h"

Bus::Bus(){
    // Connecting the devices with the bus
    cpu.ConnectBus(this);
    od.ConnectBus(this);
    dd.ConnectBus(this);
    
    // Clearing 
    for(BYTE &i : ram){
        i = 0x00;
    }

    for(BYTE &i : stack){
        i = 0x00;
    }

    for(BYTE &i : zeropage){
        i = 0x00;
    }

    for(BYTE &i : odRAM){
        i = 0x00;
    }

    for(BYTE &i : ddRAM){
        i = 0x00;
    }
};

Bus::~Bus(){
    // Does nothing
};

// Starting point
void Bus::clock(){
    cpu.clock();
    od.clock();
    dd.clock();
}

BYTE Bus::read(WORD addr){
    if(addr >= 0x1000 && addr <= 0xFFFF) // Checks to which storage the address corresponds
        return ram[addr - 0x1000];
    
    else if(addr >= 0x0100 && addr <= 0x01FF)
        return stack[addr - 0x0100];
    
    else if(addr >= 0x0000 && addr <= 0x0100)
        return zeropage[addr];

    else if(addr >= 0x0400 && addr <= 0x0404)
        return odRAM[addr - 0x0400];

    else if(addr >= 0x0500 && addr <= 0x0502)
        return ddRAM[addr - 0x0500];

    return 0;
}

void Bus::write(WORD addr, BYTE data){
    if(addr >= 0x1000 && addr <= 0xFFFF) // Checks to which storage the address corresponds
        ram[addr - 0x1000] = data;

    else if(addr >= 0x0100 && addr <= 0x01FF)
        stack[addr - 0x0100] = data;
    
    else if(addr >= 0x0000 && addr <= 0x0100)
        zeropage[addr] = data;

    else if(addr >= 0x0400 && addr <= 0x0404)
        odRAM[addr - 0x0400] = data;
    
    else if(addr >= 0x0500 && addr <= 0x0502)
        ddRAM[addr - 0x0500] = data;
}

void Bus::loadProgram(std::string program){
    std::stringstream stream(program);
    WORD offset = 0x2000;   // Program starts at 0x2000
    while(!stream.eof()){
        std::string ins;
        stream >> ins;
        write(offset, std::stoul(ins, nullptr, 16));
        offset++;
    }
}

bool Bus::shouldTerminate(){
    return terminationFlag;
}

void Bus::setTermination(){
    terminationFlag = true;
}