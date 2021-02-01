#ifndef __MYOS__DRIVERS_VGA_H
#define __MYOS__DRIVERS_VGA_H

#include <common/types.h>
#include <hardwarecommunication/port.h>

namespace myos{
    namespace drivers{
        class VideoGraphicsArray
        {
            protected:
            hardwarecommunication::Port8Bit miscPort;
            hardwarecommunication::Port8Bit crtcIndexPort;
            hardwarecommunication::Port8Bit crtcDataPort;
            hardwarecommunication::Port8Bit sequencerIndexPort;
            hardwarecommunication::Port8Bit sequencerDataPort;
            hardwarecommunication::Port8Bit graphicsControllerIndexPort;
            hardwarecommunication::Port8Bit graphicsControllerDataPort;
            hardwarecommunication::Port8Bit attributeControllerIndexPort;
            hardwarecommunication::Port8Bit attributeControllerReadPort;
            hardwarecommunication::Port8Bit attributeControllerWritePort;
            hardwarecommunication::Port8Bit attributeControllerResetPort;
            void WriteRegisters(myos::common::uint8_t* registers);
            myos::common::uint8_t* GetFrameBufferSegment();
            
            virtual myos::common::uint8_t GetColorIndex(myos::common::uint8_t r, myos::common::uint8_t g, myos::common::uint8_t b);

            public:
            VideoGraphicsArray();
            ~VideoGraphicsArray();
            virtual bool SupportsMode(myos::common::uint32_t width, myos::common::uint32_t height, myos::common::uint32_t colorDepth);
            virtual bool SetMode(myos::common::uint32_t width, myos::common::uint32_t height, myos::common::uint32_t colorDepth);
            virtual void PutPixel(myos::common::int32_t x, myos::common::int32_t y, myos::common::uint8_t r, myos::common::uint8_t g, myos::common::uint8_t b);
            virtual void PutPixel(myos::common::int32_t x, myos::common::int32_t y, myos::common::uint8_t colorIndex);
            virtual void FillRectangle(myos::common::int32_t x, myos::common::int32_t y, myos::common::int32_t w, myos::common::int32_t h, myos::common::uint8_t r, myos::common::uint8_t g, myos::common::uint8_t b);
        };
    }
}
#endif