
#ifndef __TINYOS__DRIVERS__VGA_H
#define __TINYOS__DRIVERS__VGA_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>

namespace tinyos
{
    namespace drivers
    {
        class VideoGraphicsArray
        {
        protected:
            tinyos::hardwarecommunication::Port8Bit miscPort; //阴极射线管
            tinyos::hardwarecommunication::Port8Bit crtcIndexPort; //控制器索引
            tinyos::hardwarecommunication::Port8Bit crtcDataPort; //数据
            tinyos::hardwarecommunication::Port8Bit sequencerIndexPort;
            tinyos::hardwarecommunication::Port8Bit sequencerDataPort;
            tinyos::hardwarecommunication::Port8Bit graphicsControllerIndexPort;
            tinyos::hardwarecommunication::Port8Bit graphicsControllerDataPort;
            tinyos::hardwarecommunication::Port8Bit attributeControllerIndexPort;
            tinyos::hardwarecommunication::Port8Bit attributeControllerReadPort;  
            tinyos::hardwarecommunication::Port8Bit attributeCOntrollerWritePort;
            tinyos::hardwarecommunication::Port8Bit attributeControllerResetPort;
        
            void WriteRegisters(tinyos::common::uint8_t* registers);
            tinyos::common::uint8_t* GetFrameBufferSegment();

            virtual tinyos::common::uint8_t GetColorIndex(tinyos::common::uint8_t r, tinyos::common::uint8_t g, tinyos::common::uint8_t b);

        public:
            VideoGraphicsArray();
            ~VideoGraphicsArray();

            virtual bool SupportsMode(tinyos::common::uint32_t width, tinyos::common::uint32_t height, tinyos::common::uint32_t colordepth);
            virtual bool SetMode(tinyos::common::uint32_t width, tinyos::common::uint32_t height, tinyos::common::uint32_t colordepth);
            virtual void PutPixel(tinyos::common::int32_t x, tinyos::common::int32_t y, tinyos::common::uint8_t r, tinyos::common::uint8_t g, tinyos::common::uint8_t b);
            virtual void PutPixel(tinyos::common::int32_t x, tinyos::common::int32_t y, tinyos::common::uint8_t colorIndex);

            virtual void FillRectangle(tinyos::common::uint32_t x, tinyos::common::uint32_t y, tinyos::common::uint32_t w, tinyos::common::uint32_t h, tinyos::common::uint8_t r, tinyos::common::uint8_t g, tinyos::common::uint8_t b);

        };

    }
}

#endif