#pragma once

#include "datatypes.h"
#include "openGLDevice.h"

class Bus;

class DrawingDevice{
public:
    DrawingDevice();
    ~DrawingDevice();

    void clock();

private:
    Bus* bus;
    OpenGLDevice glDev;
    BYTE_S vertexData[8]; // Used for storing up to 8 signed coordinates from -128 to 127
    char counter = 0;   // Holds the position of vertexData

    BYTE read(WORD addr);
    void write(WORD addr, BYTE data);

    void reset();
    void updateVerticies();
public:    
    void ConnectBus(Bus *t) { bus = t; }
    void throwTermination(); 
};
