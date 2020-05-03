
#include <gdt.h>

using namespace tinyos;
using namespace tinyos::common;

GlobalDescriptorTable::GlobalDescriptorTable()
:nullSegmentSelector(0, 0, 0),
unusedSegmentSelector(0, 0, 0),
codeSegmentSelector(0, 64*1024*1024, 0x9A),
dataSegmentSelector(0, 64*1024*1024, 0x92)
{
    uint32_t i[2];
    i[1] = (uint32_t)this;
    i[0] = sizeof(GlobalDescriptorTable) << 16;  //左移16位，因为是高字节


    asm volatile("lgdt (%0)": :"p" (((uint8_t *)i)+2));

}

GlobalDescriptorTable::~GlobalDescriptorTable() {

}

uint16_t GlobalDescriptorTable::DataSegmentSelector() {
    //选择器地址 - 表的地址
    return (uint8_t*)&dataSegmentSelector - (uint8_t*)this;
}


uint16_t GlobalDescriptorTable::CodeSegmentSelector() {  //代码段
    return (uint8_t*)&codeSegmentSelector - (uint8_t*)this;
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t flags) {

    uint8_t *target = (uint8_t*)this;
    if(limit <= 65536){ //小于16位
        target[6] = 0x40;  //告诉process这是16位,够用了
    }else{ // 如果不够用，limit超过了内存给定的大小   --我的理解，不知道对不对待考证
        if((limit & 0xFFF) != 0xFFF)  //改变limit的大小，成为可用的值
            limit = (limit >> 12) - 1;
        else    //
            limit = limit >> 12;
        target[6] = 0xC0;
    }
    // limit  0，1，6是由表的结构所决定
    target[0] = limit & 0xFF;  //下八位
    target[1] = (limit >> 8) & 0xFF;   //上八位
    target[6] |= (limit >> 16) & 0xF;  //四位 右移&后｜取或
    // ptr
    target[2] = base & 0xFF;
    target[3] = (base >> 8) & 0xFF;
    target[4] = (base >> 16) & 0xFF;
    target[7] = (base >> 24) & 0xFF;
    // flags
    target[5] = flags;

}


uint32_t GlobalDescriptorTable::SegmentDescriptor::Base() {  //ptr
    uint8_t *target = (uint8_t*)this;
    uint32_t result = target[7];
    result = (result << 8) + target[4];  //前面进行了右移，现在左移回去
    result = (result << 8) + target[3];
    result = (result << 8) + target[2];
    return result;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit() {
    uint8_t *target = (uint8_t*)this;
    uint32_t result = target[6] & 0xF;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];

    if((target[6] & 0xC0) == 0xC0) //limit超限
        result = (result << 12) | 0xFFF;

    return result;
}
