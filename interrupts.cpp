#include "interrupts.h"
void printf(char* str);
void printfHex(uint8_t);
InterruptHandler::InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager){
    this->interruptManager = interruptManager;
    this->interruptNumber = interruptNumber;
    interruptManager->handlers[interruptNumber] = this;
}

InterruptHandler :: ~InterruptHandler() { 
    if(interruptManager->handlers[interruptNumber] == this)
        interruptManager->handlers[interruptNumber]= 0;
 }

uint32_t InterruptHandler:: HandleInterrupt(uint32_t esp){
    return esp;
}








InterruptManager:: GateDescriptor InterruptManager:: interruptDescriptorTable[256];
InterruptManager* InterruptManager::ActiveInterruptManager = 0;

void InterruptManager ::SetInterruptDescriptorTableEntry(
                uint8_t interruptnumber,
                uint16_t codeSegmentSelectorOffset,
                void (*handler)(),
                uint8_t DescriptorPrivilageLevel,
                uint8_t DescriptorType
            )
{
    
    const uint8_t IDT_DESC_PRESENT = 0x80;
    interruptDescriptorTable[interruptnumber].handlerAddressLowBits = ((uint32_t) handler) & 0xffff;
    interruptDescriptorTable[interruptnumber].handlerAddressHighBits = (((uint32_t)handler) >> 16) & 0xfff;
    interruptDescriptorTable[interruptnumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    interruptDescriptorTable[interruptnumber].access = IDT_DESC_PRESENT | DescriptorType | ((DescriptorPrivilageLevel & 0x3) << 0x5);
    interruptDescriptorTable[interruptnumber].reserved = 0x00;
}

InterruptManager:: InterruptManager(GlobalDescriptorTable* gdt)
: picMasterCommand(0x20),
  picMasterData(0x21),
  picSlaveCommand(0xA0),
  picSlaveData(0xA1)
{
    uint16_t codeSegment = gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xe;

    for(uint8_t i = 255; i > 0; --i)
    {
        handlers[i] = 0;
        SetInterruptDescriptorTableEntry(i, codeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
    }

    SetInterruptDescriptorTableEntry(0x20, codeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x21, codeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x2C, codeSegment, &HandleInterruptRequest0x0C, 0, IDT_INTERRUPT_GATE);

    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);

    picMasterData.Write(0x20);
    picSlaveData.Write(0x28);

    picMasterData.Write(0x04);
    picSlaveData.Write(0x02);

    picMasterData.Write(0x01);
    picSlaveData.Write(0x01);

    picMasterData.Write(0x00);
    picSlaveData.Write(0x00);

    InterruptDescriptorTablePointer idt;
    idt.base = (uint32_t)interruptDescriptorTable;
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    asm volatile("lidt %0" : : "m" (idt));
    printf("[OK] Initilized Interrupt Descriptor Table.\n");
}

InterruptManager::~InterruptManager(){   }

uint32_t InterruptManager::handleInterrupt(uint8_t interruptnumber, uint32_t esp){
    if(ActiveInterruptManager != 0)
        return ActiveInterruptManager->DoHandleInterrupt(interruptnumber, esp);
    printf("[ERR] No active interrupt manager to handle interrrupts.\n");
    return esp;
}

uint32_t InterruptManager::DoHandleInterrupt(uint8_t interrupt, uint32_t esp){
    
    if(handlers[interrupt] != 0){
        esp = handlers[interrupt]->HandleInterrupt(esp);
    }
    else if(interrupt != 0x20){
        char* hex = "0123456789ABCDEF";
        printf("Unhandled Interrupt 0x");
        printfHex(interrupt);
        printf("\n");
    }
    if(interrupt >= 0x20 && interrupt < 0x30){
        picMasterCommand.Write(0x20);
        // if it's in slave interrupts
        if(interrupt > 0x28)
            picSlaveCommand.Write(0x20);
    }
    return esp;
}

void InterruptManager ::Activate()
{
    if(ActiveInterruptManager != 0)
        ActiveInterruptManager->Deactivate();
    ActiveInterruptManager = this;
    asm("sti");
    printf("[OK] Activated Interrupt Descriptor Table.\n");
}

void InterruptManager::Deactivate(){
    if(ActiveInterruptManager == this){
        ActiveInterruptManager = 0;
        asm("cli");
    }
}