
#ifndef __TINYOS__HARDWARECOMMUNICATION__PORT_H
#define __TINYOS__HARDWARECOMMUNICATION__PORT_H

#include <common/types.h>


namespace tinyos
{
    namespace hardwarecommunication
    {
        class Port{
        protected:
                tinyos::common::uint16_t portnumber;
                Port(tinyos::common::uint16_t portnumber);
                ~Port();
        };

        class Port8Bit : public Port{
        public:
                Port8Bit(tinyos::common::uint16_t portnumber);
                ~Port8Bit();
                virtual void Write(tinyos::common::uint8_t data);
                virtual tinyos::common::uint8_t Read();
        protected:
                static inline tinyos::common::uint8_t Read8(tinyos::common::uint16_t _port)
                {
                        tinyos::common::uint8_t result;
                        __asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (_port));
                        return result;
                }
                static inline void Write8(tinyos::common::uint16_t _port, tinyos::common::uint8_t _data)
                {
                        __asm__ volatile("outb %0, %1" : : "a" (_data), "Nd" (_port));
                }
        };

        class Port8BitSlow : public Port8Bit{
        public:
                Port8BitSlow(tinyos::common::uint16_t portnumber);
                ~Port8BitSlow();
                virtual void Write(tinyos::common::uint8_t data);
        protected:
                static inline void Write8Slow(tinyos::common::uint16_t _port, tinyos::common::uint8_t _data)
                {
                        __asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (_data), "Nd" (_port));
                }
        };

        class Port16Bit : public Port{
        public:
                Port16Bit(tinyos::common::uint16_t portnumber);
                ~Port16Bit();
                virtual void Write(tinyos::common::uint16_t data);
                virtual tinyos::common::uint16_t Read();
        protected:
                static inline tinyos::common::uint16_t Read16(tinyos::common::uint16_t _port)
                {
                        tinyos::common::uint16_t result;
                        __asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (_port));
                        return result;
                }
                static inline void Write16(tinyos::common::uint16_t _port, tinyos::common::uint16_t _data)
                {
                        __asm__ volatile("outw %0, %1" : : "a" (_data), "Nd" (_port));
                }
        };

        class Port32Bit : public Port{
        public:
                Port32Bit(tinyos::common::uint16_t portnumber);
                ~Port32Bit();
                virtual void Write(tinyos::common::uint32_t data);
                virtual tinyos::common::uint32_t Read();
        protected:
                static inline tinyos::common::uint32_t Read32(tinyos::common::uint16_t _port)
                {
                        tinyos::common::uint32_t result;
                        __asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (_port));
                        return result;
                }

                static inline void Write32(tinyos::common::uint16_t _port, tinyos::common::uint32_t _data)
                {
                __asm__ volatile("outl %0, %1" : : "a" (_data), "Nd" (_port));
                }
        };

    }
}

#endif
