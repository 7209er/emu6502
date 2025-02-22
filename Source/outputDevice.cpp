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

void OutputDevice::clock(){
    if(first != read(0x400) || second != read(0x401)){
        first = read(0x400);
        second = read(0x401);
        printf("First: %d, Second: %d\n", first, second);
    }
}