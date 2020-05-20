
#ifndef __TINYOS__DRIVERS__ATA_H
#define __TINYOS__DRIVERS__ATA_H

#include <hardwarecommunication/port.h>
#include <common/types.h>

namespace tinyos
{
    namespace drivers
    {

        class AdvancedTechnologyAttachment
        {
        protected:
            hardwarecommunication::Port16Bit dataPort; //发送数据
            hardwarecommunication::Port8Bit errorPort; // 读取错误
            hardwarecommunication::Port8Bit sectorCountPort; // 有多少要读
            hardwarecommunication::Port8Bit lbaLowPort; //想读的sector的逻辑地址
            hardwarecommunication::Port8Bit lbaMidPort;
            hardwarecommunication::Port8Bit lbaHiPort;
            hardwarecommunication::Port8Bit devicePort; // 是否能读
            hardwarecommunication::Port8Bit commandPort; // 读还是写
            hardwarecommunication::Port8Bit controlPort; // 状态
            
            bool master; // 描述两个分区
            common::uint16_t btyesPerSector;
        public:
            AdvancedTechnologyAttachment(common::uint16_t portBase, bool master);
            ~AdvancedTechnologyAttachment();

            void Identify();
            void Read28(common::uint32_t sector, common::uint8_t* data, int count);
            void Write28(common::uint32_t sector, common::uint8_t* data, int count);
            void Flush();  // 冲洗缓冲
        };
    }
}

#endif