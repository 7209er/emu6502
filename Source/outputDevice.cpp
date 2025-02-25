#include "outputDevice.h"
#include "bus.h"

OutputDevice::OutputDevice(){
    first = 0x00;
    second = 0x00;
}

OutputDevice::~OutputDevice(){
    // does nothing
}

void OutputDevice::ConnectBus(Bus* ptr){
    bus = ptr;
}

BYTE OutputDevice::read(WORD addr){
    return bus->read(addr);
}

// An output occurs each time that a value has changed
// Both values can change at the same "transaction" (Reset)
// so no if else.
void OutputDevice::clock(){
    if(first != read(0x400)){
        first = read(0x400);
        printf("First: %d\n", first);
    }
    if(second != read(0x401)){
        second = read(0x401);
        printf("Second: %d\n", second);
    }
}