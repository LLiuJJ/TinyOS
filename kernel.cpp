#include "types.h"
#include "gdt.h"
#include "port.h"
#include "keyboard.h"
#include "mouse.h"
#include "interrupts.h"

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
		if(y >= 25){ //超过屏幕，清屏
			for(y = 0; y < 25; y++)
				for(x = 0; x < 80; x++) // | ‘  ’ 清屏
					VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
			x = 0;
			y = 0;
		}
	}
}




extern "C" void kernelMain(void* multiboot_structure, unsigned int /*multiboot_magic*/)
{
	printf("Hello world!\n");

	GlobalDescriptorTable gdt;

    InterruptManager interrupts(&gdt);

	KeyboardDriver keyboard(&interrupts);
	MouseDriver mouse(&interrupts);

	interrupts.Activate();

	printf("----111---");
	while(1);
}
