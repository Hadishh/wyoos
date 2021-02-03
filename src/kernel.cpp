#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>
#include <memorymanagment.h>
#define SCREEEN_HEIGHT 25
#define SCREEEN_WITDH 80 

#define GRAPHICS_MODE

using namespace myos;
using namespace myos::common;
using namespace myos::hardwarecommunication;
using namespace myos::drivers;
using namespace myos::gui;
Desktop* mainDesktop;
VideoGraphicsArray* mainVGA;
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
            for (y = 0; y < SCREEEN_HEIGHT; ++y)
                for (x = 0; x < SCREEEN_WITDH; ++x)
                    graphicPointer[y * SCREEEN_WITDH + x] = (graphicPointer[y * SCREEEN_WITDH + x] & 0xFF00) | ' ';
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


void taskA(){
    while(true)
        printf("A");
}

void taskB(){
    while(true)
        printf("B");
}

void DrawStuff(){
    while (1){
        #ifdef GRAPHICS_MODE
            if(mainDesktop != 0 && mainVGA != 0)
                mainDesktop->Draw(mainVGA);
        #endif
    }
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
    TaskManager taskManager;
    uint32_t* memupper = (uint32_t*)((size_t)multiboot_structure + 8); //size of the 
    common::size_t heap = 10 * 1024 * 1024;
    MemoryManager memoryManager(heap, (*memupper) * 1024 - heap - 10*1024);
    printf("heap:0x");
    printfHex((heap >> 24) & 0xff);
    printfHex((heap >> 16) & 0xff);
    printfHex((heap >> 8) & 0xff);
    printfHex((heap) & 0xff);

    void* allocated = memoryManager.malloc(1024);
    printf("\nallocated:0x");
    printfHex(((size_t)allocated >> 24) & 0xff);
    printfHex(((size_t)allocated >> 16) & 0xff);
    printfHex(((size_t)allocated >> 8) & 0xff);
    printfHex(((size_t)allocated) & 0xff);
    printf("\n");
    Task GUIDrawer(&gdt, DrawStuff);
    taskManager.AddTask(&GUIDrawer);

    InterruptManager interrupts(&gdt, &taskManager);
    #ifdef GRAPHICS_MODE
    Desktop desktop(320, 200, 0x00, 0x00, 0xa8);
    mainDesktop = &desktop;
    #endif
    DriverManager drvMgr;
    #ifndef GRAPHICS_MODE
        PrintfKeyboardEventHandler keyboardEventHandler;
        KeyboardDriver keyboard(&interrupts, &keyboardEventHandler);
    #endif
    #ifdef GRAPHICS_MODE
        KeyboardDriver keyboard(&interrupts, &desktop);
    #endif
    drvMgr.AddDriver(&keyboard);
    #ifndef GRAPHICS_MODE
        MouseToConsole mouseHandlr;
        MouseDriver mouse(&interrupts, &mouseHandlr);
    #endif
    #ifdef GRAPHICS_MODE
        MouseDriver mouse(&interrupts, &desktop);
    #endif
    drvMgr.AddDriver(&mouse);
    PeripheralComponentInterconnectController PCIController;
    PCIController.SelectDrivers(&drvMgr, &interrupts);
    #ifdef GRAPHICS_MODE
        VideoGraphicsArray vga;
        mainVGA = &vga;
    #endif
    drvMgr.ActivateAll();
    //printf("\nWell I'm done.");
    #ifdef GRAPHICS_MODE
        vga.SetMode(320, 200, 8);
        Window win1(&desktop, 10, 10, 20, 20, 0xa8, 0x00, 0x00);
        desktop.AddChild(&win1);
        Window win2(&desktop, 40, 15, 30, 30, 0x00, 0xa8, 0x00);
        desktop.AddChild(&win2);
    #endif
    interrupts.Activate();
    while (1);
}
