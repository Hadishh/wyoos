#include <hardwarecommunication/pci.h>

using namespace myos::common;
using namespace myos::hardwarecommunication;
using namespace myos::drivers;
PeripheralComponentInterconnectDescriptor::PeripheralComponentInterconnectDescriptor() { }
PeripheralComponentInterconnectDescriptor:: ~PeripheralComponentInterconnectDescriptor() { }

PeripheralComponentInterconnectController::PeripheralComponentInterconnectController()
: dataPort(0xcfc),
  commandPort(0xcf8)
{

}

PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController(){

}

uint32_t PeripheralComponentInterconnectController::Read( uint16_t bus, uint16_t device, uint16_t function, uint32_t registerOffset)
{
    uint32_t identifier = (0x1 << 31)
                        | ((bus &0xff) << 16) 
                        | ((device & 0x1f) << 11)
                        | ((function & 0x07) << 8)
                        | (registerOffset & 0xFC);
    commandPort.Write(identifier);
    uint32_t result = dataPort.Read();
    return result >> (8 * (registerOffset % 4));
}

void PeripheralComponentInterconnectController::Write(uint16_t bus, uint16_t device, uint16_t function, uint32_t registerOffset, uint32_t data){
    uint32_t identifier = (0x1 << 31)
                        | ((bus &0xff) << 16) 
                        | ((device & 0x1f) << 11)
                        | ((function & 0x07) << 8)
                        | (registerOffset & 0xFC);
    commandPort.Write(identifier);
    dataPort.Write(data);
}

bool PeripheralComponentInterconnectController::DeviceHasFunction(uint16_t bus, uint16_t device){
    // 7th bit of function 3 at offset 0x0e says we have function or no
    return Read(bus, device, 0, 0x0e) & (1 << 0x7);
}

void printf(char*);
void printfHex(uint8_t);
void PeripheralComponentInterconnectController::SelectDrivers(DriverManager* driverManager, InterruptManager* interruptManager){
    for(int bus = 0; bus < 0x8; ++bus)
        for(int device = 0; device < 0x20; ++device){
            int numFunctions = DeviceHasFunction(bus, device)? 0x8 : 0x1;
            for(int function = 0; function < numFunctions; ++function){
                PeripheralComponentInterconnectDescriptor dev = GetDeviceDescriptor(bus, device, function);
                
                //there is no function at this step!
                if(dev.vendor_id == 0x0000 || dev.vendor_id == 0xffff)
                    continue;
                
                for(int barNum = 0; barNum < 6; ++barNum){
                    BaseAddressRegister bar = GetBaseAddressRegister(bus, device, function, barNum);
                    if(bar.address && (bar.type == BaseAddressRegisterType::InputOutput))
                        dev.portBase = (uint32_t)bar.address;
                    Driver* driver = GetDriver(dev, interruptManager);
                    if(driver != 0)
                        driverManager->AddDriver(driver);
                }
                printf("PCI BUS ");
                printfHex(bus & 0xff);

                printf(", Device ");
                printfHex(device & 0xff);

                printf(", Function ");
                printfHex(function & 0xff);

                printf(" = Vendor ");
                printfHex((dev.vendor_id & 0xff00) >> 8);
                printfHex(dev.vendor_id & 0xff);

                printf(", Device ");
                printfHex((dev.device_id & 0xff00) >> 8);
                printfHex(dev.device_id & 0xff);

                printf("\n");
                
            }
        }
}

PeripheralComponentInterconnectDescriptor PeripheralComponentInterconnectController:: GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function){
    PeripheralComponentInterconnectDescriptor result;
    result.bus = bus;
    result.device = device;
    result.function = function;
    
    result.vendor_id = Read(bus, device, function, 0x00);
    result.device_id = Read(bus, device, function, 0x02);
    
    result.class_id = Read(bus, device, function, 0x0b);
    result.subclass_id = Read(bus, device, function, 0x0a);
    result.interface_id = Read(bus, device, function, 0x09);

    result.revision = Read(bus, device, function, 0x08);
    result.interrupt = Read(bus, device, function, 0x3c);

    return result;
}

BaseAddressRegister PeripheralComponentInterconnectController::GetBaseAddressRegister(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar){
    BaseAddressRegister result;
    uint32_t headerType = Read(bus, device, function, 0x0e) & 0x7f;
    int maxBARs = 6 - (4 * headerType);

    if(bar >= maxBARs)
        return result;
    uint32_t bar_value = Read(bus, device, function, 0x10 + 4 * bar);

    result.type = (bar_value & 0x1)? BaseAddressRegisterType::InputOutput: BaseAddressRegisterType::MemoryMapping;
    uint32_t temp;
    if(result.type == BaseAddressRegisterType::MemoryMapping){
        result.prefetchable = ((bar_value >> 3) & 0x1) == 0x1;
        switch ((bar_value >> 1) & 0x3)
        {
        case 0x00:
            //32 bit mode
            /* code */
            break;
        case 0x01:
            //20 bit mode
            break;
        case 0x10:
            //64 bit mode
            break;
        default:
            break;
        }
    }   
    else //InputOutput device
    {
        result.address = (uint8_t*)(bar_value & ~0x3);
        result.prefetchable = false;
    } 

}   


Driver* PeripheralComponentInterconnectController:: GetDriver(PeripheralComponentInterconnectDescriptor device, InterruptManager* interruptManager){
    switch (device.vendor_id)
    {
        case 0x1022: //AMD
            switch(device.device_id){
                case 0x2000: // am79c973
                    break;
            }
            break;
        case 0x8086:    //Intel
            break;
    }
    switch(device.class_id){
        case 0x03: //graphics
            switch(device.subclass_id){
                case 0x00: //VGA
                    break;
            }
            break;
        
    }
    return 0;
}