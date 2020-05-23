
#include <net/tcp.h>

using namespace tinyos;
using namespace tinyos::common;
using namespace tinyos::net;



TransmissionControlProtocolHandler::TransmissionControlProtocolHandler()
{
}

TransmissionControlProtocolHandler::~TransmissionControlProtocolHandler()
{
}

bool TransmissionControlProtocolHandler::HandleTransmissionControlProtocolMessage(TransmissionControlProtocolSocket* socket, uint8_t* data, uint16_t size)
{
    return true;
}





TransmissionControlProtocolSocket::TransmissionControlProtocolSocket(TransmissionControlProtocolProvider* backend)
{
    this->backend = backend;
    handler = 0;
    state = CLOSED;
}

TransmissionControlProtocolSocket::~TransmissionControlProtocolSocket()
{
}

bool TransmissionControlProtocolSocket::HandleTransmissionControlProtocolMessage(uint8_t* data, uint16_t size)
{
    if(handler != 0)
        return handler->HandleTransmissionControlProtocolMessage(this, data, size);
    return false;
}

void TransmissionControlProtocolSocket::Send(uint8_t* data, uint16_t size)
{
    while(state != ESTABLISHED){
    }
    backend->Send(this, data, size, PSH|ACK);
}

void TransmissionControlProtocolSocket::Disconnect()
{
    backend->Disconnect(this);
}





TransmissionControlProtocolProvider::TransmissionControlProtocolProvider(InternetProtocolProvider* backend)
: InternetProtocolHandler(backend, 0x06)
{
    for(int i = 0; i < 65535; i++)
        sockets[i] = 0;
    numSockets = 0;
    freePort = 1024;
}

TransmissionControlProtocolProvider::~TransmissionControlProtocolProvider()
{
}







uint32_t bigEndian32(uint32_t x)
{
    return ((x & 0xFF000000) >> 24)
         | ((x & 0x00FF0000) >> 8)
         | ((x & 0x0000FF00) << 8)
         | ((x & 0x000000FF) << 24);
}


// 握手挥手实现
bool TransmissionControlProtocolProvider::OnInternetProtocolReceived(uint32_t srcIP_BE, uint32_t dstIP_BE,
                                        uint8_t* internetprotocolPayload, uint32_t size)
{
    
    if(size < 20)
        return false;
    TransmissionControlProtocolHeader* msg = (TransmissionControlProtocolHeader*)internetprotocolPayload;

    uint16_t localPort = msg->dstPort;
    uint16_t remotePort = msg->srcPort;
    
    TransmissionControlProtocolSocket* socket = 0;
    for(uint16_t i = 0; i < numSockets && socket == 0; i++){
        if( sockets[i]->localPort == msg->dstPort
        &&  sockets[i]->localIP == dstIP_BE
        &&  sockets[i]->state == LISTEN
        && (((msg -> flags) & (SYN | ACK)) == SYN)) //请求
            socket = sockets[i];
        else if( sockets[i]->localPort == msg->dstPort
        &&  sockets[i]->localIP == dstIP_BE
        &&  sockets[i]->remotePort == msg->srcPort
        &&  sockets[i]->remoteIP == srcIP_BE) //已连接
            socket = sockets[i];
    }
        
    bool reset = false;
    
    if(socket != 0 && msg->flags & RST) // 严重错误
        socket->state = CLOSED;

    
    if(socket != 0 && socket->state != CLOSED){
        switch((msg -> flags) & (SYN | ACK | FIN)) // 判断处理
        {
            case SYN: // 别人请求连接
                if(socket -> state == LISTEN){ // 别人请求同时我们在监听
                    socket->state = SYN_RECEIVED; // 服务器已经收到并发送同步（SYNC）信号之后等待确认（ACK）请求
                    socket->remotePort = msg->srcPort;
                    socket->remoteIP = srcIP_BE;
                    socket->acknowledgementNumber = bigEndian32( msg->sequenceNumber ) + 1;
                    socket->sequenceNumber = 0xbeefcafe;
                    Send(socket, 0,0, SYN|ACK);
                    socket->sequenceNumber++;
                }
                else
                    reset = true;
                break;

                
            case SYN | ACK: // 别人确认我们的请求
                if(socket->state == SYN_SENT){ //等待确认状态
                    socket->state = ESTABLISHED;  // 连接打开，收到的数据可以发送给用户
                    socket->acknowledgementNumber = bigEndian32( msg->sequenceNumber ) + 1;
                    socket->sequenceNumber++;
                    Send(socket, 0,0, ACK);
                }
                else
                    reset = true;
                break;
                
                
            case SYN | FIN:
            case SYN | FIN | ACK:
                reset = true;
                break;

                
            case FIN:
            case FIN|ACK: // 其实写的简单些
                if(socket->state == ESTABLISHED){//连接状态
                    socket->state = CLOSE_WAIT; // 等待连接终止请求
                    socket->acknowledgementNumber++;
                    Send(socket, 0,0, ACK);
                    Send(socket, 0,0, FIN|ACK);
                }
                else if(socket->state == CLOSE_WAIT){//直接关闭
                    socket->state = CLOSED;
                }
                else if(socket->state == FIN_WAIT1
                    || socket->state == FIN_WAIT2){ //等待终止请求
                    socket->state = CLOSED;
                    socket->acknowledgementNumber++;
                    Send(socket, 0,0, ACK);
                }
                else
                    reset = true;
                break;
                
                
            case ACK: // 收到确认
                if(socket->state == SYN_RECEIVED){ // 收到确认，开始连接收数据
                    socket->state = ESTABLISHED;
                    return false;
                }//主动关闭端在FIN-WAIT-1状态下收到ACK确认包，进入等待远程TCP的连接终止请求的半关闭状态。这时可以接收数据，但不再发送数据。
                else if(socket->state == FIN_WAIT1){//收到确认进入wait2
                    socket->state = FIN_WAIT2;
                    return false;
                }
                else if(socket->state == CLOSE_WAIT){
                    socket->state = CLOSED; // 完全断开
                    break;
                }
                
                if(msg->flags == ACK)
                    break;
                
                // no break, because of piggybacking
                
            default:
                
                if(bigEndian32(msg->sequenceNumber) == socket->acknowledgementNumber){
                    reset = !(socket->HandleTransmissionControlProtocolMessage(internetprotocolPayload + msg->headerSize32*4,
                                                                              size - msg->headerSize32*4));
                    if(!reset){
                        int x = 0;
                        for(int i = msg->headerSize32*4; i < size; i++) // 确认收到数据的位置
                            if(internetprotocolPayload[i] != 0)
                                x = i;
                        socket->acknowledgementNumber += x - msg->headerSize32*4 + 1;
                        Send(socket, 0,0, ACK);
                    }
                }
                else{
                    // data in wrong order
                    reset = true;
                }
                
        }
    }
    
    
    if(reset){ // 不对需要重连
        if(socket != 0){
            Send(socket, 0,0, RST);
        }
        else{
            TransmissionControlProtocolSocket socket(this);
            socket.remotePort = msg->srcPort;
            socket.remoteIP = srcIP_BE;
            socket.localPort = msg->dstPort;
            socket.localIP = dstIP_BE;
            socket.sequenceNumber = bigEndian32(msg->acknowledgementNumber);
            socket.acknowledgementNumber = bigEndian32(msg->sequenceNumber) + 1;
            Send(&socket, 0,0, RST);
        }
    }
    

    if(socket != 0 && socket->state == CLOSED) // 断开连接释放
        for(uint16_t i = 0; i < numSockets && socket == 0; i++)
            if(sockets[i] == socket){
                sockets[i] = sockets[--numSockets];
                MemoryManager::activeMemoryManager->free(socket);
                break;
            }
    
    
    return false;
}


// ------------------------------------------------------------------------------------------


// 发送
void TransmissionControlProtocolProvider::Send(TransmissionControlProtocolSocket* socket, uint8_t* data, uint16_t size, uint16_t flags)
{
    uint16_t totalLength = size + sizeof(TransmissionControlProtocolHeader);
    uint16_t lengthInclPHdr = totalLength + sizeof(TransmissionControlProtocolPseudoHeader);
    // 指向加了phdr的整个报文
    uint8_t* buffer = (uint8_t*)MemoryManager::activeMemoryManager->malloc(lengthInclPHdr); 
    // phdr指向开头  msg指向tcp报文开头  buffer2指向数据部分
    TransmissionControlProtocolPseudoHeader* phdr = (TransmissionControlProtocolPseudoHeader*)buffer;
    TransmissionControlProtocolHeader* msg = (TransmissionControlProtocolHeader*)(buffer + sizeof(TransmissionControlProtocolPseudoHeader));
    uint8_t* buffer2 = buffer + sizeof(TransmissionControlProtocolHeader)
                              + sizeof(TransmissionControlProtocolPseudoHeader);
    
    msg->headerSize32 = sizeof(TransmissionControlProtocolHeader)/4;
    msg->srcPort = socket->localPort;
    msg->dstPort = socket->remotePort;
    
    msg->acknowledgementNumber = bigEndian32( socket->acknowledgementNumber );
    msg->sequenceNumber = bigEndian32( socket->sequenceNumber );
    msg->reserved = 0;
    msg->flags = flags; //状态
    msg->windowSize = 0xFFFF;
    msg->urgentPtr = 0;
    
    msg->options = ((flags & SYN) != 0) ? 0xB4050402 : 0;
    
    socket->sequenceNumber += size; // 
        
    for(int i = 0; i < size; i++)
        buffer2[i] = data[i];
    
    phdr->srcIP = socket->localIP;
    phdr->dstIP = socket->remoteIP;
    phdr->protocol = 0x0600;
    phdr->totalLength = ((totalLength & 0x00FF) << 8) | ((totalLength & 0xFF00) >> 8);    
    
    msg -> checksum = 0;
    msg -> checksum = InternetProtocolProvider::Checksum((uint16_t*)buffer, lengthInclPHdr);
    
    InternetProtocolHandler::Send(socket->remoteIP, (uint8_t*)msg, totalLength);
    MemoryManager::activeMemoryManager->free(buffer);
}


// 连接特定
TransmissionControlProtocolSocket* TransmissionControlProtocolProvider::Connect(uint32_t ip, uint16_t port)
{
    TransmissionControlProtocolSocket* socket = (TransmissionControlProtocolSocket*)MemoryManager::activeMemoryManager->malloc(sizeof(TransmissionControlProtocolSocket));
    
    if(socket != 0){
        new (socket) TransmissionControlProtocolSocket(this);
        
        socket -> remotePort = port;
        socket -> remoteIP = ip;
        socket -> localPort = freePort++;
        socket -> localIP = backend->GetIPAddress();
        
        socket -> remotePort = ((socket -> remotePort & 0xFF00)>>8) | ((socket -> remotePort & 0x00FF) << 8);
        socket -> localPort = ((socket -> localPort & 0xFF00)>>8) | ((socket -> localPort & 0x00FF) << 8);
        
        sockets[numSockets++] = socket;
        socket -> state = SYN_SENT;
        
        socket -> sequenceNumber = 0xbeefcafe;
        
        Send(socket, 0,0, SYN); //第一步发送连接请求
    }
    
    return socket;
}

// 断开连接
void TransmissionControlProtocolProvider::Disconnect(TransmissionControlProtocolSocket* socket)
{
    socket->state = FIN_WAIT1;
    Send(socket, 0,0, FIN + ACK);
    socket->sequenceNumber++;
}

// 监听
TransmissionControlProtocolSocket* TransmissionControlProtocolProvider::Listen(uint16_t port)
{
    TransmissionControlProtocolSocket* socket = (TransmissionControlProtocolSocket*)MemoryManager::activeMemoryManager->malloc(sizeof(TransmissionControlProtocolSocket));
    
    if(socket != 0){
        new (socket) TransmissionControlProtocolSocket(this);
        
        socket -> state = LISTEN;
        socket -> localIP = backend->GetIPAddress();
        socket -> localPort = ((port & 0xFF00)>>8) | ((port & 0x00FF) << 8);
        
        sockets[numSockets++] = socket;
    }
    
    return socket;
}
void TransmissionControlProtocolProvider::Bind(TransmissionControlProtocolSocket* socket, TransmissionControlProtocolHandler* handler)
{
    socket->handler = handler;
}