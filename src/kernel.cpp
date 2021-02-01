#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <gui/desktop.h>
#define SCREEEN_HEIGHT 25
#define SCREEEN_WITDH 80 

using namespace myos;
using namespace myos::common;
using namespace myos::hardwarecommunication;
using namespace myos::drivers;
using namespace myos::gui;

void printf(char *str)
{
    static uint16_t *graphicPointer = (uint16_t *)0xb8000;
    static uint8_t x = 0, y = 0;
    for (int i = 0; str[i] != '\0'; ++i)
    {
        switch (str[i])
        {
            case '\n':
                y++;
                x = 0;
                break;
            case '\r':
                x = 0;
                break;
            default:
                graphicPointer[y * SCREEEN_WITDH + x] = (graphicPointer[y * SCREEEN_WITDH + x] & 0xFF00) | str[i];
                x++;
                break;
        }
        if (x >= SCREEEN_WITDH)
        {
            y++;
            x = 0;
        }
        if (y >= SCREEEN_HEIGHT)
        {
            for (int i = 0; i < SCREEEN_HEIGHT; ++i)
                for (int j = 0; j < SCREEEN_WITDH; ++j)
                    graphicPointer[i * SCREEEN_WITDH + j] = (graphicPointer[i * SCREEEN_WITDH + j] & 0xFF00) | ' ';
            y = 0;
            x = 0;
        }
    }
}

void printfHex(uint8_t key){
    char* hex = "0123456789abcdef";
    char* text = "00";
    text[0] = hex[(key>>4) & 0x0f];
    text[1] = hex[ key & 0x0f];
    printf(text);
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void callConstructors()
{
    for (constructor *i = &start_ctors; i < &end_ctors; i++)
        (*i)();
}

class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
    public:
        void OnKeyDown(char c){
            char* text = " ";
            text[0]= c;
            printf(text);
        }
};

class MouseToConsole : public MouseEventHandler{
    private:
        int8_t x, y;
        uint16_t* VideoMemory = (uint16_t*)0xb8000;
    public:
        MouseToConsole() { }

        virtual void OnMouseActivate(){
            x = SCREEEN_WITDH / 2;
            y = SCREEEN_HEIGHT / 2;
            uint16_t* VideoMemory = (uint16_t*)0xb8000;
            int16_t middle = SCREEEN_WITDH * y + x;
            VideoMemory[middle] = ((VideoMemory[middle] & 0xf000) >> 4)
                                            | ((VideoMemory[middle] & 0x0f00) << 4)
                                            | (VideoMemory[middle] & 0x00ff);
        }
        virtual void OnMouseMove(uint8_t deltax, uint8_t deltay){
            VideoMemory[SCREEEN_WITDH * y + x] = ((VideoMemory[SCREEEN_WITDH * y + x] & 0xf000) >> 4)
                                                | ((VideoMemory[SCREEEN_WITDH * y + x] & 0x0f00) << 4)
                                                | (VideoMemory[SCREEEN_WITDH * y + x] & 0x00ff); 
            x += deltax;
            if(x < 0) x = 0;
            if(x >= SCREEEN_WITDH) x = SCREEEN_WITDH - 1;

            y += deltay;
            if(y < 0) y = 0;
            if(y >= SCREEEN_HEIGHT) y = SCREEEN_HEIGHT - 1;
            // flip colors
            VideoMemory[SCREEEN_WITDH * y + x] = ((VideoMemory[SCREEEN_WITDH * y + x] & 0xf000) >> 4)
                                                | ((VideoMemory[SCREEEN_WITDH * y + x] & 0x0f00) << 4)
                                                | (VideoMemory[SCREEEN_WITDH * y + x] & 0x00ff); 
        
        }
};

extern "C" void kernelMain(const void *multiboot_structure, uint32_t /*magicnumber*/)
{
    printf("Booting up the system.\n");
    GlobalDescriptorTable gdt;
    InterruptManager interrupts(&gdt);
    Desktop desktop(320, 200, 0x00, 0x00, 0xa8);
    DriverManager drvMgr;
    //PrintfKeyboardEventHandler keyboardEventHandler;
    // KeyboardDriver keyboard(&interrupts, &keyboardEventHandler);
    KeyboardDriver keyboard(&interrupts, &desktop);
    drvMgr.AddDriver(&keyboard);
    // MouseToConsole mouseHandlr;
    // MouseDriver mouse(&interrupts, &mouseHandlr);
    MouseDriver mouse(&interrupts, &desktop);
    drvMgr.AddDriver(&mouse);
    PeripheralComponentInterconnectController PCIController;
    PCIController.SelectDrivers(&drvMgr, &interrupts);
    VideoGraphicsArray vga;
    drvMgr.ActivateAll();
    //printf("\nWell I'm done.");
    vga.SetMode(320, 200, 8);

    interrupts.Activate();
    while (1){
        desktop.Draw(&vga);
    }
}
