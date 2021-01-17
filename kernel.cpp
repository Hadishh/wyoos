#include "types.h"
#include "gdt.h"
#define SCREEEN_HEIGHT 25
#define SCREEEN_WITDH 80 
void printf(int8_t *str)
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
                for (int j = 0; j < SCREEEN_WITDH; ++i)
                    graphicPointer[i * SCREEEN_WITDH + j] = (graphicPointer[i * SCREEEN_WITDH + j] & 0xFF00) | ' ';
            y = 0;
            x = 0;
        }
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

extern "C" void kernelMain(const void *multiboot_structure, uint32_t /*magicnumber*/)
{
    printf("Booting up the system. But while I'm being ready you can solve the problem that I have! Tell me how much bannana can be in a basket full of apples containing 18 apples?\nThink about it and I'll do my job!");
    GlobalDescriptorTable gdt;
    printf("\nWell I'm done. The answer was 0 :). gl hf!");
    while (1);
}