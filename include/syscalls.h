
#ifndef __TINYOS__SYSCALLS_H
#define __TINYOS__SYSCALLS_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <multitasking.h>

namespace tinyos
{
    class SyscallHandler : public hardwarecommunication::InterruptHandler
    {
        
    public:
        SyscallHandler(hardwarecommunication::InterruptManager* interruptManager, tinyos::common::uint8_t InterruptNumber);
        ~SyscallHandler();

        virtual tinyos::common::uint32_t HandleInterrupt(tinyos::common::uint32_t esp);

    };
}


#endif