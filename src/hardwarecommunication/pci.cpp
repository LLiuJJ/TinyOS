
#include <hardwarecommunication/pci.h>
#include <drivers/amd_am79c973.h>

using namespace tinyos::common;
using namespace tinyos::drivers;
using namespace tinyos::hardwarecommunication;


PeripheralComponentInterconnectDeviceDescriptor::PeripheralComponentInterconnectDeviceDescriptor()
{

}
PeripheralComponentInterconnectDeviceDescriptor::~PeripheralComponentInterconnectDeviceDescriptor()
{

}



PeripheralComponentInterconnectController::PeripheralComponentInterconnectController()
: dataPort(0xCFC),
    commandPort(0xCF8)
{

}

PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController()
{

}

uint32_t PeripheralComponentInterconnectController::Read(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset)
{
    // 通过这四个值构建标识符传给PCI Controller
    uint32_t id = 
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registeroffset & 0xFC); 
    commandPort.Write(id);
    //读值
    uint32_t result = dataPort.Read();
    return result >> (8 * (registeroffset % 4));
}

void PeripheralComponentInterconnectController::Write(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value)
{
    uint32_t id = 
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registeroffset & 0xFC); 
    commandPort.Write(id);
    dataPort.Write(value);
}

bool PeripheralComponentInterconnectController::DeviceHasFunctions(uint16_t bus, uint16_t device)
{
    return Read(bus, device, 0, 0x0E) & (1 << 7);
}

void printf(char* str);
void printfHex(uint8_t);
// 查看8buses和32devices下的函数，主要是将设备添加到driverManager中
void PeripheralComponentInterconnectController::SelectDrivers(tinyos::drivers::DriverManager * driverManager, tinyos::hardwarecommunication::InterruptManager*  interrupts)
{
    for(int bus = 0; bus < 8; bus++){
        for(int device = 0; device < 32; device++){
            int numFunctions = DeviceHasFunctions(bus, device)? 8 : 1;
            for(int function = 0; function < numFunctions; function++){
                PeripheralComponentInterconnectDeviceDescriptor dev = GetDeviceDescriptor(bus, device, function);

                if(dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
                    continue;
                
                for(int barNum = 0; barNum < 6; barNum++){
                    BaseAddressRegister bar = GetBaseAddressRegister(bus, device, function, barNum);
                    if(bar.address && (bar.type == InputOutput))
                        dev.portBase = (uint32_t)bar.address;

                }

                Driver* driver = GetDriver(dev, interrupts);
                if(driver != 0)
                    driverManager->AddDriver(driver);

                printf("PCI BUS");
                printfHex(bus & 0xFF);
            
                printf(", DEVICE ");
                printfHex(device & 0xFF);

                printf(", FUNCTION ");
                printfHex(function & 0xFF);
                
                printf(" = VECNDOR ");
                printfHex((dev.vendor_id & 0xFF00) >> 8);
                printfHex(dev.vendor_id & 0xFF);

                printf(", DEVICE ");
                printfHex((dev.device_id & 0xFF00) >> 8);
                printfHex(dev.device_id & 0xFF);
                printf("\n");

            }
        }
    }
}

BaseAddressRegister PeripheralComponentInterconnectController::GetBaseAddressRegister(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar)
{
    BaseAddressRegister result;

    uint32_t headertype = Read(bus, device, function, 0x0E) & 0x7F;
    int maxBARs = 6 - (4 * headertype);
    if(bar >= maxBARs)
        return result;

    uint32_t bar_value = Read(bus, device, function, 0x10 + 4 * bar);
    result.type = (bar_value & 0x1) ? InputOutput : MemoryMapping;
    uint32_t temp;

    if(result.type == MemoryMapping){
        //result.prefetchable = ((bar_value >> 3) & 0x1) == 0x1;
        switch((bar_value >> 1) & 0x3)
        {
            case 0: //32bit Mode
            case 1: //20bit
            case 2: //64bit
            break;
        }
    }else{ //InputOutput
        result.address = (uint8_t*)(bar_value & ~0x3);
        result.prefetchable = false;
    }


    return result;
}

// 获取driver，这里我们主要就是AMD的网卡
Driver* PeripheralComponentInterconnectController::GetDriver(PeripheralComponentInterconnectDeviceDescriptor dev, InterruptManager*  interrupts)
{
    Driver* driver = 0;

    switch(dev.vendor_id)
    {
    case 0x1022: //AMD
        switch(dev.device_id)
        {
        case 0x2000: //am79c973
            
            driver = (amd_am79c973*)MemoryManager::activeMemoryManager->malloc(sizeof(amd_am79c973));
            if(driver != 0)
                new (driver) amd_am79c973(&dev, interrupts);
            printf("AMD am79c973");
            return driver;
            break;
        }
        break;
    case 0x8086: //intel
        break;
    }

    switch(dev.class_id)
    {
    case 0x03:  // graphics
        switch(dev.subclass_id)
        {
        case 0x00: //VGA
            printf("VGA ");
            break;
        }
        break;
    }

    return driver;
}
// 获取device的各项信息
PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentInterconnectController::GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function)
{
    PeripheralComponentInterconnectDeviceDescriptor result;

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
