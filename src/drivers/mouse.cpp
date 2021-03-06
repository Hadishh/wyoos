#include  <drivers/mouse.h>
#include "drivers/kbd.h"
#define SCREEEN_HEIGHT 25
#define SCREEEN_WITDH 80 
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;
void printf(char*);

MouseEventHandler::MouseEventHandler() { }
void MouseEventHandler::OnMouseActivate() { }
void MouseEventHandler::OnMouseDown(uint8_t) { }
void MouseEventHandler:: OnMouseUp(uint8_t) { }
void MouseEventHandler:: OnMouseMove(int x, int y) { }


MouseDriver:: MouseDriver(InterruptManager* manager, MouseEventHandler* handler)
: InterruptHandler(0x2c, manager),
dataPort(0x60),
commandPort(0x64)
{
    this->handler = handler;
}

MouseDriver:: ~MouseDriver() { }

void MouseDriver :: Activate(){
    offset = 0;
    buttons = 0;
    handler->OnMouseActivate();
    commandPort.Write(0xa8); // activate
    commandPort.Write(0x20); // give us your current state
    uint8_t state = dataPort.Read() | 2;
    commandPort.Write(0x60); // set state
    dataPort.Write(state);
    commandPort.Write(0xd4);
    dataPort.Write(0xf4); // activate mouse
    dataPort.Read();
    printf("[OK] Mouse Attached.\n");
}


uint32_t MouseDriver::HandleInterrupt(uint32_t esp){
    uint8_t status = commandPort.Read();
    if((!(status & 0x20)))
        return esp;
    uint8_t key = dataPort.Read();    
    buffer[offset] = key;
    if(this->handler == 0)
        return esp;
    offset = (offset + 1) % 3;
    //movement interrupt
    if(offset == 0){
        if(buffer[1] != 0 || buffer[2] != 0)
            handler->OnMouseMove((int8_t)buffer[1], -((int8_t)buffer[2]));
        
        for(uint8_t i = 0; i < 3; ++i){
            if((buffer[0] & (0x01 << i)) != (buttons & (0x01 << i)))
            {
                if(buttons & (0x1<<i))
                    handler->OnMouseUp(i + 1);
                else 
                    handler->OnMouseDown(i + 1);
            }
        }
        buttons = buffer[0];
    }
    return esp;
}