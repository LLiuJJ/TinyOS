
#ifndef __TINYOS__DRIVERS__KEYBOARD_H
#define __TINYOS__DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>

namespace tinyos
{
    namespace drivers
    {
        class KeyboardEventHandler
        {
        public:
            KeyboardEventHandler();

            virtual void OnKeyDown(char);
            virtual void OnKeyUp(char);
        };

        class KeyboardDriver : public tinyos::hardwarecommunication::InterruptHandler, public Driver
        {
            tinyos::hardwarecommunication::Port8Bit dataport;
            tinyos::hardwarecommunication::Port8Bit commandport;

            KeyboardEventHandler *handler;
        public:
            KeyboardDriver(tinyos::hardwarecommunication::InterruptManager *manager, KeyboardEventHandler *handler);
            ~KeyboardDriver();
            virtual tinyos::common::uint32_t HandleInterrupt(tinyos::common::uint32_t esp);
            virtual void Activate();

        };
    }
}


#endif