#include "mouse.h"
#include "kbd.h"
#define SCREEEN_HEIGHT 25
#define SCREEEN_WITDH 80 

void printf(char*);


MouseDriver:: MouseDriver(InterruptManager* manager)
: InterruptHandler(0x2c, manager),
dataPort(0x60),
commandPort(0x64)
{
    offset = 0;
    buttons = 0;

    uint16_t* VideoMemory = (uint16_t*)0xb8000;
        // flip colors
    int16_t middle = SCREEEN_WITDH * (SCREEEN_HEIGHT / 2) + SCREEEN_WITDH / 2;
    VideoMemory[middle] = ((VideoMemory[middle] & 0xf000) >> 4)
                                            | ((VideoMemory[middle] & 0x0f00) << 4)
                                            | (VideoMemory[middle] & 0x00ff);

    
    commandPort.Write(0xa8); // activate
    commandPort.Write(0x20); // give us your current state
    uint8_t state = dataPort.Read() | 2;
    commandPort.Write(0x60); // set state
    dataPort.Write(state);

    commandPort.Write(0xd4);
    dataPort.Write(0xf4); // activate keyboard

    dataPort.Read();
}

MouseDriver:: ~MouseDriver() { }

uint32_t MouseDriver::HandleInterrupt(uint32_t esp){
    uint8_t status = commandPort.Read();
    if(!(status & 0x20))
        return esp;
    uint8_t key = dataPort.Read();    
    buffer[offset] = key;
    offset = (offset + 1) % 3;
    static int8_t x = SCREEEN_WITDH / 2, y = SCREEEN_HEIGHT / 2;
    //movement interrupt
    if(offset == 0){

        static uint16_t* VideoMemory = (uint16_t*)0xb8000;
        // flip colors
        VideoMemory[SCREEEN_WITDH * y + x] = ((VideoMemory[SCREEEN_WITDH * y + x] & 0xf000) >> 4)
                                            | ((VideoMemory[SCREEEN_WITDH * y + x] & 0x0f00) << 4)
                                            | (VideoMemory[SCREEEN_WITDH * y + x] & 0x00ff); 
        x += buffer[1];
        if(x < 0) x = 0;
        if(x >= SCREEEN_WITDH) x = SCREEEN_WITDH - 1;

        y -= buffer[2];
        if(y < 0) y = 0;
        if(y >= SCREEEN_HEIGHT) y = SCREEEN_HEIGHT - 1;

        // flip colors
        VideoMemory[SCREEEN_WITDH * y + x] = ((VideoMemory[SCREEEN_WITDH * y + x] & 0xf000) >> 4)
                                            | ((VideoMemory[SCREEEN_WITDH * y + x] & 0x0f00) << 4)
                                            | (VideoMemory[SCREEEN_WITDH * y + x] & 0x00ff);

        for(uint8_t i = 0; i < 3; ++i){
            if((buffer[0] & (0x01 << i)) != (buttons & (0x01 << i)))
            {
                //button pressed
                VideoMemory[SCREEEN_WITDH * y + x] = ((VideoMemory[SCREEEN_WITDH * y + x] & 0xf000) >> 4)
                                            | ((VideoMemory[SCREEEN_WITDH * y + x] & 0x0f00) << 4)
                                            | (VideoMemory[SCREEEN_WITDH * y + x] & 0x00ff);
            }
        }
        buttons = buffer[0];
    }
    return esp;
}