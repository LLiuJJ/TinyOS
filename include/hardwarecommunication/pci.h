
#ifndef __TINYOS__HARDWARECOMMUNICATION__PCI_H
#define __TINYOS__HARDWARECOMMUNICATION__PCI_H

#include <drivers/driver.h>
#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <hardwarecommunication/interrupts.h>

// #include <drivers/amd_am79c973.h>
#include <memorymanagement.h>

namespace tinyos
{
    namespace hardwarecommunication
    {

        enum BaseAddressRegisterType
        {
            MemoryMapping = 0,
            InputOutput = 1
        };

        class BaseAddressRegister //内存映射
        {
        public:
            bool prefetchable;
            tinyos::common::uint8_t* address;
            tinyos::common::uint32_t size;
            BaseAddressRegisterType type;
        };

        class PeripheralComponentInterconnectDeviceDescriptor
        {
        public:
            tinyos::common::uint32_t portBase;
            tinyos::common::uint32_t interrupt;
            
            tinyos::common::uint16_t bus;
            tinyos::common::uint16_t device;
            tinyos::common::uint16_t function;

            tinyos::common::uint16_t vendor_id;
            tinyos::common::uint16_t device_id;
            
            tinyos::common::uint8_t class_id; 
            tinyos::common::uint8_t subclass_id;
            tinyos::common::uint8_t interface_id;
            
            tinyos::common::uint8_t revision;

            PeripheralComponentInterconnectDeviceDescriptor();
            ~PeripheralComponentInterconnectDeviceDescriptor();
        };

        class PeripheralComponentInterconnectController
        {
            Port32Bit dataPort;
            Port32Bit commandPort;
        public:
            PeripheralComponentInterconnectController();
            ~PeripheralComponentInterconnectController();

            tinyos::common::uint32_t Read(tinyos::common::uint16_t bus, tinyos::common::uint16_t device, tinyos::common::uint16_t function, tinyos::common::uint32_t registeroffset);
            void Write(tinyos::common::uint16_t bus, tinyos::common::uint16_t device, tinyos::common::uint16_t function, tinyos::common::uint32_t registeroffset, tinyos::common::uint32_t value);
            //判断是否存在
            bool DeviceHasFunctions(tinyos::common::uint16_t bus, tinyos::common::uint16_t device);
            
            //
            void SelectDrivers(tinyos::drivers::DriverManager * driverManager, tinyos::hardwarecommunication::InterruptManager*  interrupts);
            tinyos::drivers::Driver* GetDriver(PeripheralComponentInterconnectDeviceDescriptor dev, tinyos::hardwarecommunication::InterruptManager*  interrupts);
            PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(tinyos::common::uint16_t bus, tinyos::common::uint16_t device, tinyos::common::uint16_t function);
            
            BaseAddressRegister GetBaseAddressRegister(tinyos::common::uint16_t bus, tinyos::common::uint16_t device, tinyos::common::uint16_t function, tinyos::common::uint16_t bar);
        };

    }
}

#endif