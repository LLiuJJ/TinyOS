#ifndef __TINYOS__NET__IPV4_H
#define __TINYOS__NET__IPV4_H

#include <common/types.h>
#include <net/etherframe.h>
#include <net/arp.h>

namespace tinyos
{
    namespace net
    {
        
        struct InternetProtocolV4Message
        {
            common::uint8_t headerLength : 4; //首部长
            common::uint8_t version : 4; //版本
            common::uint8_t tos; //服务类型
            common::uint16_t totalLength; //总长度
            
            common::uint16_t ident;//标识符
            common::uint16_t flagsAndOffset;//片偏移
            
            common::uint8_t timeToLive;//存活时间
            common::uint8_t protocol;//协议
            common::uint16_t checksum;//校验和
            
            common::uint32_t srcIP;//源地址
            common::uint32_t dstIP;//目的地址
        } __attribute__((packed));
        
        class InternetProtocolProvider;
     
        class InternetProtocolHandler
        {
        protected:
            InternetProtocolProvider* backend;
            common::uint8_t ip_protocol;
             
        public:
            InternetProtocolHandler(InternetProtocolProvider* backend, common::uint8_t protocol);
            ~InternetProtocolHandler();
            
            virtual bool OnInternetProtocolReceived(common::uint32_t srcIP_BE, common::uint32_t dstIP_BE,
                                            common::uint8_t* internetprotocolPayload, common::uint32_t size);
            void Send(common::uint32_t dstIP_BE, common::uint8_t* internetprotocolPayload, common::uint32_t size);
        };
     
     
        class InternetProtocolProvider : public EtherFrameHandler
        {
        friend class InternetProtocolHandler;
        protected:
            InternetProtocolHandler* handlers[255];
            AddressResolutionProtocol* arp;
            common::uint32_t gatewayIP; //网关ip
            common::uint32_t subnetMask; //子网掩码
            
        public:
            InternetProtocolProvider(EtherFrameProvider* backend, 
                                     AddressResolutionProtocol* arp,
                                     common::uint32_t gatewayIP, common::uint32_t subnetMask);
            ~InternetProtocolProvider();
            
            bool OnEtherFrameReceived(common::uint8_t* etherframePayload, common::uint32_t size);

            void Send(common::uint32_t dstIP_BE, common::uint8_t protocol, common::uint8_t* buffer, common::uint32_t size);
            
            static common::uint16_t Checksum(common::uint16_t* data, common::uint32_t lengthInBytes);
        };
    }
}


#endif