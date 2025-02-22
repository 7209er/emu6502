#pragma once

#include <stdio.h>
#include "datatypes.h"

class Bus;

class OutputDevice{
public:
    OutputDevice();
    ~OutputDevice();

    Bus* bus;

    BYTE first, second; // used for checking if data is updated

    void ConnectBus(Bus* ptr);
    void clock();

private:
    BYTE read(WORD addr);
};