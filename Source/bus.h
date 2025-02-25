#pragma once

#include <string>
#include <sstream>

#include "datatypes.h"
#include "emu6502.h"
#include "outputDevice.h"
#include "drawingDevice.h"

class Bus{
public:
    Bus();
    ~Bus();

    void clock();

    // Devices
    emu6502 cpu;
    OutputDevice od;
    DrawingDevice dd;

private:
    BYTE ram[60 * 1024]; // 60kB of RAM from 0x1000 - 0xFFFF
    BYTE stack[256];     // 0x0100 - 0x01FF
    BYTE zeropage[256];  // 0x0000 - 0x0000
    BYTE odRAM[5];       // 0x0400 - 0x0404
    BYTE ddRAM[3];       // 0x0500 - 0x0502

public:
    BYTE read(WORD addr);
    void write(WORD addr, BYTE data);

    void loadProgram(std::string program);

    bool shouldTerminate();
    void setTermination();
private:
    bool terminationFlag = false;
};