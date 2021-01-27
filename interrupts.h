#ifndef __INTERRUPTMANAGER_H
#define __INTERRUPTMANAGER_H
#include "types.h"
#include "port.h"
#include "gdt.h"
    class InterruptManager;
    class InterruptHandler{
        protected:
            uint8_t interruptNumber;
            InterruptManager* interruptManager;
            InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager);
            ~InterruptHandler();
        public:
            virtual uint32_t HandleInterrupt(uint32_t esp); 
    };
    class InterruptManager{
        friend class InterruptHandler;
        protected:
            static InterruptManager* ActiveInterruptManager;
            InterruptHandler* handlers[256];
            struct GateDescriptor
            {
                uint16_t handlerAddressLowBits;
                uint16_t gdt_codeSegmentSelector;
                uint8_t reserved;
                uint8_t access;
                uint16_t handlerAddressHighBits;
            }__attribute__((packed));

            struct InterruptDescriptorTablePointer{
                uint16_t size;
                uint32_t base;
            }__attribute__((packed));

            static GateDescriptor interruptDescriptorTable[256];
            static void SetInterruptDescriptorTableEntry(
                uint8_t interruptnumber,
                uint16_t codeSegmentSelectorOffset,
                void (*handler)(),
                uint8_t DescriptorPrivilageLevel,
                uint8_t DescriptorType
            );
            
            static void IgnoreInterruptRequest();
            static void HandleInterruptRequest0x00();
            static void HandleInterruptRequest0x01();
            static void HandleInterruptRequest0x0C();

            Port8BitSlow picMasterCommand;
            Port8BitSlow picMasterData;
            Port8BitSlow picSlaveCommand;
            Port8BitSlow picSlaveData;
        public:
            static uint32_t handleInterrupt(uint8_t interruptnumber, uint32_t esp);
            uint32_t DoHandleInterrupt(uint8_t interrupt, uint32_t esp);
            InterruptManager(GlobalDescriptorTable* gdt);
            ~InterruptManager();
            void Activate();
            void Deactivate();
            
    };
#endif