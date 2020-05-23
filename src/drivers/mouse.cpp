
#include <drivers/mouse.h>

using namespace tinyos::common;
using namespace tinyos::drivers;
using namespace tinyos::hardwarecommunication;

MouseEventHandler::MouseEventHandler()
{

}
void MouseEventHandler::OnActivate()
{

}
void MouseEventHandler::OnMouseDown(uint32_t button)
{

}
void MouseEventHandler::OnMouseUp(uint8_t button)
{

}
void MouseEventHandler::OnMouseMove(int x, int y) 
{

}

MouseDriver::MouseDriver(InterruptManager *manager, MouseEventHandler *handler)
: InterruptHandler(manager, 0x2C),
  dataport(0x60),
  commandport(0x64)
{
    this->handler = handler;
}

MouseDriver::~MouseDriver()
{

}
void MouseDriver::Activate()
{
    offset = 0;
    buttons = 0;                       

    commandport.Write(0xA8);  //发送中断
    commandport.Write(0x20);  //获取当前状态
    uint8_t status = dataport.Read() | 2;
    commandport.Write(0x60);  // 设置status
    dataport.Write(status);

    commandport.Write(0xD4);
    dataport.Write(0xF4);
    dataport.Read();    
}

void printf(char*);

uint32_t MouseDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t status = commandport.Read();
    if((!(status & 0x20)) || handler == 0){
        return esp;
    }

    buffer[offset] = dataport.Read();
    offset = (offset + 1) % 3;  //逐渐读值，确保读对

    if(offset == 0){ //
        if(buffer[1] != 0 || buffer[2] != 0){
            handler->OnMouseMove((int8_t)buffer[1], ((int8_t)buffer[2])); //有时候会存在鼠标移动反向，可以通过在buffer2前加-或不加来调
        }
        //
        for(uint8_t i = 0; i < 3; i++){
            if(buffer[0] & (0x01 << i) != (buttons & (0x01 << i))){
                if(buttons & (0x1 << 1))
                    handler->OnMouseUp(i + 1);
                else
                    handler->OnMouseDown(i + 1);    
            } 
        }

        buttons = buffer[0];
    }
    
    return esp;
}

