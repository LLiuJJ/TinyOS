
#include <drivers/ata.h>


using namespace tinyos;
using namespace tinyos::common;
using namespace tinyos::drivers;
using namespace tinyos::hardwarecommunication;

void printf(char*);

AdvancedTechnologyAttachment::AdvancedTechnologyAttachment(common::uint16_t portBase, bool master)
:   dataPort(portBase),
    errorPort(portBase + 1),
    sectorCountPort(portBase + 2),
    lbaLowPort(portBase + 3),
    lbaMidPort(portBase + 4),
    lbaHiPort(portBase + 5),
    devicePort(portBase + 6),
    commandPort(portBase + 7),
    controlPort(portBase + 0x206)
{
    btyesPerSector = 512; // 定义一块就是512
    this->master = master;
}

AdvancedTechnologyAttachment::~AdvancedTechnologyAttachment()
{

}
// 确认设备
void AdvancedTechnologyAttachment::Identify()
{
    devicePort.Write(master ? 0xA0 : 0xB0);
    controlPort.Write(0);

    devicePort.Write(0xA0);
    uint8_t status = commandPort.Read();
    if(status == 0xFF)
        return;
    
    devicePort.Write(master ? 0xA0 : 0xB0);
    sectorCountPort.Write(0);
    lbaLowPort.Write(0);
    lbaMidPort.Write(0);
    lbaHiPort.Write(0);
    commandPort.Write(0xEC);

    status = commandPort.Read();
    if(status == 0x00)
        return; // no device
    // 等设备准备好
    while(((status & 0x80) == 0x80)
            && ((status & 0x01) != 0x01))
        status = commandPort.Read();
    
    if(status & 0x01){
        printf("ERROR");
        return;
    }

    for(uint16_t i = 0; i < 256; i++){
        uint16_t data = dataPort.Read();
        char* foo = "   \0";
        foo[1] = (data >> 8) & 0x00FF;
        foo[0] = data & 0x00FF;
        printf(foo);
    }
}
void AdvancedTechnologyAttachment::Read28(common::uint32_t sector, common::uint8_t* data, int count)
{
    if(sector & 0xF0000000)
        return;
    if(count > btyesPerSector)
        return;

    devicePort.Write((master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
    errorPort.Write(0);
    sectorCountPort.Write(1);
    // lba是8位
    lbaLowPort.Write( sector & 0x000000FF );
    lbaMidPort.Write((sector & 0x0000FF00) >> 8);
    lbaHiPort.Write( (sector & 0x00FF0000) >> 16);
    commandPort.Write(0x20);

    uint8_t status = commandPort.Read();
    while(((status & 0x80) == 0x80)
            && ((status & 0x01) != 0x01))
        status = commandPort.Read();
    
    if(status & 0x01){
        printf("ERROR");
        return;
    }

    printf("Reading from ATA: ");
    // 一次加两个因为dataport是一次读16位，所以对位操作就可以将数据正确读出
    for(uint16_t i = 0; i < count; i+= 2){
        uint16_t wdata = dataPort.Read();
        
        char* foo = "   \0";
        foo[1] = (wdata >> 8) & 0x00FF;
        foo[0] = wdata & 0x00FF;
        printf(foo);

        data[i] = wdata & 0x00FF;
        if(i+1 < count)
            data[i+1] = (wdata >> 8) & 0x00FF;
    }
    // 不够的补空
    for(uint16_t i = count + (count % 2); i < btyesPerSector; i+= 2){
        dataPort.Read();
    }
}
void AdvancedTechnologyAttachment::Write28(common::uint32_t sector, common::uint8_t* data, int count)
{
    if(sector & 0xF0000000)
        return;
    if(count > btyesPerSector)
        return;

    devicePort.Write((master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
    errorPort.Write(0);
    sectorCountPort.Write(1);

    lbaLowPort.Write( sector & 0x000000FF );
    lbaMidPort.Write((sector & 0x0000FF00) >> 8);
    lbaHiPort.Write( (sector & 0x00FF0000) >> 16);
    commandPort.Write(0x30);

    printf("Writing to ATA: ");
    // 开始写入数据
    for(uint16_t i = 0; i < count; i+= 2){
        uint16_t wdata = data[i];
        if(i+1 < count)
            wdata |= ((uint16_t)data[i+1]) << 8;
        
        char* foo = "   \0";
        foo[1] = (wdata >> 8) & 0x00FF;
        foo[0] = wdata & 0x00FF;
        printf(foo);

        dataPort.Write(wdata);
    }
    // 写入少于的，后面补0
    for(uint16_t i = count + (count % 2); i < btyesPerSector; i+= 2){
        dataPort.Write(0x0000);
    }
}

void AdvancedTechnologyAttachment::Flush()
{
    devicePort.Write(master ? 0xA0 : 0xB0);
    commandPort.Write(0xE7);

    uint8_t status = commandPort.Read();
    while(((status & 0x80) == 0x80)
            && ((status & 0x01) != 0x01))
        status = commandPort.Read();
    
    if(status & 0x01){
        printf("ERROR");
        return;
    }
}
