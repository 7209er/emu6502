#include "drawingDevice.h"
#include "bus.h"
DrawingDevice::DrawingDevice(){
    // Clearing Data
    for(BYTE_S &i : vertexData){
        i = 0x00;
    }

    vertexData[0] = -0x5F;
    vertexData[1] = -0x5F;
    vertexData[2] = -0x5F;
    vertexData[3] =  0x7F;
    vertexData[4] =  0x7F;
    vertexData[5] =  0x5F;
    vertexData[6] =  0x5F;
    vertexData[7] = -0x5F;
}

DrawingDevice::~DrawingDevice(){
    // Does nothing
}

void DrawingDevice::clock(){
    // Process events first
    throwTermination();
    // Updates a vertex if 0x0502 is 0x01, uploads it into OpenGLDevice if it is 0x02
    updateVerticies();
    // Render
    glDev.render();

}

BYTE DrawingDevice::read(WORD addr){
    return bus->read(addr);
}

void DrawingDevice::write(WORD addr, BYTE data){
    bus->write(addr, data);
}

void DrawingDevice::reset(){
    // Clearing Data
    for(BYTE_S &i : vertexData){
        i = 0x00;
    }
}

void DrawingDevice::throwTermination(){
    if(glDev.shouldTerminate)
        bus->setTermination();
}

void DrawingDevice::updateVerticies(){
    BYTE currentValue = bus->read(0x0502);
    // Updates a vertex
    if(currentValue == 0x0001){
        vertexData[counter] = bus->read(0x0500);
        counter++;
        vertexData[counter] = bus->read(0x0501);
        counter++;
        counter %= 8;
    }
    // Uploads the current buffer into OpenGLDevice
    else if(currentValue == 0x0002){
        // Normalizing the coords to a range from -1.0 to 1.0 and converting them into floats
        float tempData[8] = {
            vertexData[0] / 128.0f, vertexData[1] / 128.0f,
            vertexData[2] / 128.0f, vertexData[3] / 128.0f,
            vertexData[4] / 128.0f, vertexData[5] / 128.0f,
            vertexData[6] / 128.0f, vertexData[7] / 128.0f
        };
        glDev.update(tempData);
        counter = 0;
    }
    bus->write(0x0502, 0x0000);
}