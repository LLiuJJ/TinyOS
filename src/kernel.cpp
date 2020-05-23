#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <hardwarecommunication/interrupts.h>
#include <syscalls.h>
#include <hardwarecommunication/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <drivers/ata.h>
#include <filesystem/msdospart.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>

#include <drivers/amd_am79c973.h>
#include <net/etherframe.h>
#include <net/arp.h>
#include <net/ipv4.h>
#include <net/icmp.h>
#include <net/udp.h>
#include <net/tcp.h>


using namespace tinyos;
using namespace tinyos::common;
using namespace tinyos::hardwarecommunication;
using namespace tinyos::drivers;
using namespace tinyos::filesystem;
using namespace tinyos::gui;
using namespace tinyos::net;

void printf(char *str) //printf函数，在屏幕输出字符串，通过屏幕地址逐一显示，因为地址是固定从头显示
{
	static uint16_t* VideoMemory = (uint16_t*)0xb8000;

	static uint8_t x = 0, y = 0;
	//在屏幕上字符高25 80宽
	for(int i = 0; str[i] != '\0'; ++i){

		switch(str[i]){
		case '\n':
			y++;
			x = 0;
			break;

		default:
        	VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
			x++;
			break;
		}
		if(x >= 80){ //换行
			y++;
			x = 0;
		}
		if(y >= 25){//超过屏幕，内容上移，强迫症清屏看不见了
			for(y = 0; y < 24; y++)
				for(x = 0; x < 80; x++)
					VideoMemory[80*y+x] = VideoMemory[80*(y+1)+x];
			for(x = 0; x < 80; x++)
				VideoMemory[80*24+x] = (VideoMemory[80*24+x] & 0xFF00) | ' ';
			y = 24;
			x = 0;
		}
		// if(y >= 25){ //超过屏幕，清屏
		// 	for(y = 0; y < 25; y++)
		// 		for(x = 0; x < 80; x++) // | ‘  ’ 清屏
		// 			VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
		// 	x = 0;
		// 	y = 0;
		// }
	}
}

void printfHex(uint8_t key)
{
    char* foo = "00";
    char* hex = "0123456789ABCDEF";
    foo[0] = hex[(key >> 4) & 0xF];
    foo[1] = hex[key & 0xF];
    printf(foo);
}

void printfHex16(uint16_t key)
{
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}
void printfHex32(uint32_t key)
{
    printfHex((key >> 24) & 0xFF);
    printfHex((key >> 16) & 0xFF);
    printfHex((key >> 8) & 0xFF);
    printfHex( key & 0xFF);
}

class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:
	void OnKeyDown(char c)
	{
		char *foo = " ";
		foo[0] = c;
		printf(foo);
	}
};

class MouseToConsole : public MouseEventHandler
{
    int8_t x, y;
public:

	MouseToConsole()
	{
		static uint16_t* VideoMemory = (uint16_t*)0xb8000;
		x = 40;
		y = 12;
		VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
						| ((VideoMemory[80*y+x] & 0x0F00) << 4)
						| (VideoMemory[80*y+x] & 0x00FF);
	
	}
	void OnMouseMove(int xoffset, int yoffset)
	{
        static uint16_t* VideoMemory = (uint16_t*)0xb8000;
    // 翻转颜色
        VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
                        | ((VideoMemory[80*y+x] & 0x0F00) << 4)
                        | ((VideoMemory[80*y+x] & 0x00FF));

        x += xoffset;
        if(x < 0) x = 0;
        if(x >= 80) x = 79;

        y -= yoffset;
        if(y < 0) y = 0;
        if(y >= 25) y = 24;

        VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4)
                        | ((VideoMemory[80*y+x] & 0x0F00) << 4)
                        | ((VideoMemory[80*y+x] & 0x00FF));
	}
};

class PrintfUDPHandler : public UserDatagramProtocolHandler
{
public:
	void HandleUserDatagramProtocolMessage(UserDatagramProtocolSocket* socket, common::uint8_t* data, common::uint16_t size)
	{
		char *foo = " ";
		for(int i = 0; i < size; i++){
			foo[0] = data[i];
			printf(foo);
		}
	}
};

class PrintfTCPHandler : public TransmissionControlProtocolHandler
{
public:
	bool HandleTransmissionControlProtocolMessage(TransmissionControlProtocolSocket* socket, common::uint8_t* data, common::uint16_t size)
	{
		char *foo = " ";
		for(int i = 0; i < size; i++){
			foo[0] = data[i];
			printf(foo);
		}
		if(size > 4
			&& data[0] == 'G'
			&& data[1] == 'E'
			&& data[2] == 'T'
			&& data[3] == ' '
			&& data[4] == '/'
			&& data[5] == ' '
			&& data[6] == 'H'
			&& data[7] == 'T'
			&& data[8] == 'T'
			&& data[9] == 'P'){
			socket->Send((uint8_t*)"HTTP/1.1 200 OK\r\nServer:Myos\r\nContent-Type: text/html\r\n\r\n<html><head><titile>My Operating System</title></head><body><b>My Operating System</b> http://learnbycoding.cn/#/</body></html>\r\n", 196);
			socket->Disconnect();
		
		}
		return true;
	}
};


void sysprintf(char* str)
{
	asm("int $0x80" : : "a" (4), "b" (str)); // ax 4 bx str
}
void taskA()
{
	while(true){
		sysprintf("task A");
	}
}
void taskB()
{
	while(true){
		sysprintf("task B");
	}
}



typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors(){
	for(constructor* i = &start_ctors; i != &end_ctors; i++){
		(*i)();
	}
}


extern "C" void kernelMain(const void* multiboot_structure,  uint32_t /*multiboot_magic*/)
{
	printf("Hello world!\n");

	GlobalDescriptorTable gdt;
    
	uint32_t* menupper = (uint32_t*)(((size_t)multiboot_structure) + 8); // 8字节用于引导
	size_t heap = 10 * 1024  * 1024;
	MemoryManager memoryManager(heap, (*menupper) * 1024 - heap - 10*1024);

	printf("heap: 0x");
	printfHex((heap >> 24) & 0xFF);
	printfHex((heap >> 16) & 0xFF);
	printfHex((heap >> 8) & 0xFF);
	printfHex((heap) & 0xFF);

	void *allocated = memoryManager.malloc(1024);
	printf("\nmallocated: 0x");
	printfHex(((size_t)allocated >> 24) & 0xFF);
	printfHex(((size_t)allocated >> 16) & 0xFF);
	printfHex(((size_t)allocated >> 8) & 0xFF);
	printfHex((size_t)allocated & 0xFF);
	printf("\n");

	TaskManager taskManager;
	// Task task1(&gdt, taskA);
	// Task task2(&gdt, taskB);
	// taskManager.AddTask(&task1);
	// taskManager.AddTask(&task2);

	InterruptManager interrupts(0x20, &gdt, &taskManager);
	SyscallHandler syscalls(&interrupts, 0x80);

	printf("Initializing Hareware, Stage 1\n");

	#ifdef GRAPHICSMODE
		Desktop desktop(320, 200, 0x00, 0x00, 0xA8);
	#endif

	DriverManager drvManager;

		#ifdef GRAPHICSMODE
			KeyboardDriver keyboard(&interrupts, &desktop);
		#else
			PrintfKeyboardEventHandler kbhandler;
			KeyboardDriver keyboard(&interrupts, &kbhandler);
		#endif
		drvManager.AddDriver(&keyboard);

		#ifdef GRAPHICSMODE
			MouseDriver mouse(&interrupts, &desktop);
		#else
			MouseToConsole mousehandler;
			MouseDriver mouse(&interrupts, &mousehandler);
		#endif	
		drvManager.AddDriver(&mouse);

		PeripheralComponentInterconnectController PCIController;
		PCIController.SelectDrivers(&drvManager, &interrupts);

		VideoGraphicsArray vga;


	printf("Initializing Hareware, Stage 2\n");
	drvManager.ActivateAll();  //启动所有设备，包括键盘鼠标

	printf("Initializing Hareware, Stage 3\n");
	
	#ifdef GRAPHICSMODE
		vga.SetMode(320, 200, 8);
		Window win1(&desktop, 10, 10, 20, 20, 0xA8, 0x00, 0x00);
		desktop.AddChild(&win1);
		Window win2(&desktop, 40, 30, 10, 10, 0x00, 0xA8, 0x00);
		desktop.AddChild(&win2);
	#endif

	// interrupt 14
	AdvancedTechnologyAttachment ata0m(0x1F0, true);
	printf("ATA Primary Master: ");
	ata0m.Identify();

	AdvancedTechnologyAttachment ata0s(0x1F0, false);
	printf("ATA Primary Slave: ");
	ata0s.Identify();
	
	printf("\n\n\n\n\n");
    MSDOSPartitionTable::ReadPartitions(&ata0s);

	char* atabuffer = "https://LearnBycoding.cn";
	// ata0s.Write28(0, (uint8_t*)atabuffer, 25);
	// ata0s.Flush();

	// ata0s.Read28(0, (uint8_t*)atabuffer, 25);

	// interrupt 15
	AdvancedTechnologyAttachment ata1m(0x170, true);
	AdvancedTechnologyAttachment ata1s(0x170, false);

	// third: 0x1E8
	// fourth: 0x168

	// amd_am79c973* eth0 = (amd_am79c973*)(drvManager.drivers[2]);
	
	// uint8_t ip1 = 10, ip2 = 0, ip3 = 2, ip4 = 15;// vritualbox提供的ip
	// uint32_t ip_be = ((uint32_t)ip4 << 24)
	// 				|((uint32_t)ip3 << 16)
	// 				|((uint32_t)ip2 << 8)
	// 				|(uint32_t)ip1;
	
	// eth0->SetIPAddress(ip_be);
	// EtherFrameProvider etherframe(eth0);
	// AddressResolutionProtocol arp(&etherframe);

	// uint8_t gip1 = 10, gip2 = 0, gip3 = 2, gip4 = 2; // virtualbox默认的网关地址
	// uint32_t gip_be = ((uint32_t)gip4 << 24)
	// 				|((uint32_t)gip3 << 16)
	// 				|((uint32_t)gip2 << 8)
	// 				|(uint32_t)gip1;

	// uint8_t subnet1 = 255, subnet2 = 255, subnet3 = 255, subnet4 = 0;//子网掩码
	// uint32_t subnet_be = ((uint32_t)subnet4 << 24)
	// 				|((uint32_t)subnet3 << 16)
	// 				|((uint32_t)subnet2 << 8)
	// 				|(uint32_t)subnet1;

	// InternetProtocolProvider ipv4(&etherframe, &arp, gip_be, subnet_be);	
	// InternetControlMessageProtocol icmp(&ipv4);
	// UserDatagramProtocolProvider udp(&ipv4);
	// TransmissionControlProtocolProvider tcp(&ipv4);
	// // etherframe.Send(0xFFFFFFFFFFFF, 0X0608, (uint8_t*)"FOO", 3);
	// //eth0->Send((uint8_t*)"Hello Network", 13);

	interrupts.Activate();
	
	// printf("\n\n\n\n\n");
	// // arp.Resolve(gip_be);
	// // ipv4.Send(gip_be, 0x0000, (uint8_t*)"foobar", 6);
	// arp.BroadcastMACAddress(gip_be);

	// PrintfTCPHandler tcphandler;
	// TransmissionControlProtocolSocket* tcpsocket = tcp.Listen(1234);
	// tcp.Bind(tcpsocket, &tcphandler);
	// // tcpsocket->Send((uint8_t*)"Hello TCP!", 10);

	// // icmp.RequestEchoReply(gip_be);

	// // PrintfUDPHandler udphandler;
	// // UserDatagramProtocolSocket* udpsocket = udp.Connect(gip_be, 1234);
	// // udp.Bind(udpsocket, &udphandler);
	// // udpsocket->Send((uint8_t*)"Hello UDP!", 10);

	// // UserDatagramProtocolSocket* udpsocket = udp.Listen(1234);
	// // udp.Bind(udpsocket, &udphandler);

	while(1){
		#ifdef GRAPHICSMODE
			desktop.Draw(&vga);
		#endif
	}
}
