
#ifndef __TINYOS__HARDWARECOMMUNICATION__PCI_H
#define __TINYOS__HARDWARECOMMUNICATION__PCI_H

#include <drivers/driver.h>
#include <common/types.h>
#include <hardwarecommunication/port.h>
#include <hardwarecommunication/interrupts.h>

namespace tinyos
{
    namespace hardwarecommunication
    {
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
            void SelectDrivers(tinyos::drivers::DriverManager * driverManager);
            PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(tinyos::common::uint16_t bus, tinyos::common::uint16_t device, tinyos::common::uint16_t function);
            
        };

    }
}

#endif