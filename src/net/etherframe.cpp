 
#include <net/etherframe.h>
using namespace tinyos;
using namespace tinyos::common;
using namespace tinyos::net;
using namespace tinyos::drivers;



EtherFrameHandler::EtherFrameHandler(EtherFrameProvider* backend, uint16_t etherType)
{
    this->etherType_BE = ((etherType & 0x00FF) << 8)
                       | ((etherType & 0xFF00) >> 8);
    this->backend = backend;
    backend->handlers[etherType_BE] = this;
}

EtherFrameHandler::~EtherFrameHandler()
{
    if(backend->handlers[etherType_BE] == this)
        backend->handlers[etherType_BE] = 0;
}
            
bool EtherFrameHandler::OnEtherFrameReceived(common::uint8_t* etherframePayload, common::uint32_t size)
{
    return false;
}
// 发送
void EtherFrameHandler::Send(common::uint64_t dstMAC_BE, common::uint8_t* data, common::uint32_t size)
{
    backend->Send(dstMAC_BE, etherType_BE, data, size);
}

uint32_t EtherFrameHandler::GetIPAddress()
{
    return backend->GetIPAddress();
}



            
EtherFrameProvider::EtherFrameProvider(amd_am79c973* backend)
: RawDataHandler(backend)
{
    for(uint32_t i = 0; i < 65535; i++)
        handlers[i] = 0;
}

EtherFrameProvider::~EtherFrameProvider()
{
}

bool EtherFrameProvider::OnRawDataReceived(common::uint8_t* buffer, common::uint32_t size)
{
    if(size < sizeof(EtherFrameHeader))
        return false;
    
    EtherFrameHeader* frame = (EtherFrameHeader*)buffer;
    bool sendBack = false;
    
    if(frame->dstMAC_BE == 0xFFFFFFFFFFFF //
    || frame->dstMAC_BE == backend->GetMACAddress()){
        if(handlers[frame->etherType_BE] != 0)
            sendBack = handlers[frame->etherType_BE]->OnEtherFrameReceived(
                buffer + sizeof(EtherFrameHeader), size - sizeof(EtherFrameHeader));
    }
    
    if(sendBack){
        frame->dstMAC_BE = frame->srcMAC_BE;
        frame->srcMAC_BE = backend->GetMACAddress();
    }
    
    return sendBack;
}

void EtherFrameProvider::Send(common::uint64_t dstMAC_BE, common::uint16_t etherType_BE, common::uint8_t* buffer, common::uint32_t size)
{
    uint8_t* buffer2 = (uint8_t*)MemoryManager::activeMemoryManager->malloc(sizeof(EtherFrameHeader) + size);
    EtherFrameHeader* frame = (EtherFrameHeader*)buffer2;
    // 以太帧头
    frame->dstMAC_BE = dstMAC_BE;
    frame->srcMAC_BE = backend->GetMACAddress();
    frame->etherType_BE = etherType_BE;
    // 写以太帧数据部分
    uint8_t* src = buffer;
    uint8_t* dst = buffer2 + sizeof(EtherFrameHeader);
    for(uint32_t i = 0; i < size; i++)
        dst[i] = src[i];
    // 发送以太帧
    backend->Send(buffer2, size + sizeof(EtherFrameHeader));
    
    MemoryManager::activeMemoryManager->free(buffer2);
}

uint32_t EtherFrameProvider::GetIPAddress()
{
    return backend->GetIPAddress();
}

uint64_t EtherFrameProvider::GetMACAddress()
{
    return backend->GetMACAddress();
}