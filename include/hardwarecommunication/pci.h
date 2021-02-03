#ifndef __MYOS__HARDWARECOMMUNICATIO__PCI_H
#define __MYOS__HARDWARECOMMUNICATIO__PCI_H
#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <memorymanagment.h>
namespace myos
{
    namespace hardwarecommunication
    {
        enum BaseAddressRegisterType
        {
            MemoryMapping = 0,
            InputOutput = 1
        };
        class BaseAddressRegister
        {
        public:
            bool prefetchable;
            myos::common::uint8_t *address;
            myos::common::uint32_t size;
            BaseAddressRegisterType type;
        };
        class PeripheralComponentInterconnectDescriptor
        {
        public:
            myos::common::uint32_t portBase;
            myos::common::uint32_t interrupt;

            myos::common::uint16_t bus;
            myos::common::uint16_t device;
            myos::common::uint16_t function;

            myos::common::uint16_t vendor_id;
            myos::common::uint16_t device_id;

            myos::common::uint8_t class_id;
            myos::common::uint8_t subclass_id;
            myos::common::uint8_t interface_id;

            myos::common::uint8_t revision;
            PeripheralComponentInterconnectDescriptor();
            ~PeripheralComponentInterconnectDescriptor();
        };
        class PeripheralComponentInterconnectController
        {
            myos::hardwarecommunication::Port32Bit commandPort;
            myos::hardwarecommunication::Port32Bit dataPort;

        public:
            PeripheralComponentInterconnectController();
            ~PeripheralComponentInterconnectController();

            myos::common::uint32_t Read(
                myos::common::uint16_t bus,
                myos::common::uint16_t device,
                myos::common::uint16_t function,
                myos::common::uint32_t registerOffset);
            void Write(
                myos::common::uint16_t bus,
                myos::common::uint16_t device,
                myos::common::uint16_t function,
                myos::common::uint32_t registerOffset,
                myos::common::uint32_t data);
            bool DeviceHasFunction(myos::common::uint16_t bus, myos::common::uint16_t device);
            void SelectDrivers(myos::drivers::DriverManager *driverManager, myos::hardwarecommunication::InterruptManager *interruptManager);
            myos::drivers::Driver *GetDriver(PeripheralComponentInterconnectDescriptor device, myos::hardwarecommunication::InterruptManager *interruptManager);
            PeripheralComponentInterconnectDescriptor GetDeviceDescriptor(myos::common::uint16_t bus, myos::common::uint16_t device, myos::common::uint16_t function);
            BaseAddressRegister GetBaseAddressRegister(myos::common::uint16_t bus, myos::common::uint16_t device, myos::common::uint16_t function, myos::common::uint16_t bar);
        };
    } // namespace hardwarecommunication
} // namespace myos
#endif