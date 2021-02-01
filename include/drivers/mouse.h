#ifndef __MYOS__DRIVERS__MOUSE_H
#define __MYOS__DRIVERS__MOUSE_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/port.h>
#include <drivers/driver.h>
namespace myos
{
    namespace drivers
    {
        class MouseEventHandler
        {
        public:
            MouseEventHandler();
            virtual void OnMouseActivate();
            virtual void OnMouseDown(myos::common::uint8_t button);
            virtual void OnMouseUp(myos::common::uint8_t button);
            virtual void OnMouseMove( int deltax, int deltay);
        };
        class MouseDriver : public myos::hardwarecommunication::InterruptHandler, public myos::drivers::Driver
        {
            myos::hardwarecommunication::Port8Bit dataPort;
            myos::hardwarecommunication::Port8Bit commandPort;
            myos::common::uint8_t buffer[3];
            myos::common::uint8_t offset;
            myos::common::uint8_t buttons;
            MouseEventHandler *handler;

        public:
            MouseDriver(myos::hardwarecommunication::InterruptManager *manager, myos::drivers::MouseEventHandler *handler);
            ~MouseDriver();
            virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);
            virtual void Activate();
        };

    } // namespace drivers
} // namespace myos
#endif