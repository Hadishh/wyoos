#ifndef __MYOS__DRIVERS__KEYBOARD_H
#define __MYOS__DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
namespace myos
{
    namespace drivers
    {
        class KeyboardEventHandler
        {
        public:
            KeyboardEventHandler();
            ~KeyboardEventHandler();
            virtual void OnKeyDown(char);
            virtual void OnKeyUp(char);
        };

        class KeyboardDriver : public myos::hardwarecommunication::InterruptHandler, public myos::drivers:: Driver
        {
            myos::hardwarecommunication:: Port8Bit dataPort;
            myos::hardwarecommunication:: Port8Bit commandPort;

            KeyboardEventHandler *handler;

        public:
            KeyboardDriver(myos::hardwarecommunication::InterruptManager *manager, KeyboardEventHandler *handler);
            ~KeyboardDriver();
            virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
            virtual void Activate();
        };
    } // namespace drivers
} // namespace myos

#endif