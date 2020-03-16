#include "types.h"
#include "gdt.h"


void printf(char *str) //printf函数，在屏幕输出字符串，通过屏幕地址逐一显示，因为地址是固定从头显示
{
	unsigned short* VideoMemory = (unsigned short*)0xb8000;

	for(int i = 0; str[i] != '\0'; ++i){
        	VideoMemory[i] = (VideoMemory[i] & 0xFF00) | str[i];
	}
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;

extern "C" void callConstructors()
{
	for(constructor* i = &start_ctors; i != &end_ctors; i++){
		(*i)();
	}
}



extern "C" void kernelMain(void* multiboot_structure, unsigned int /*multiboot_magic*/)
{
	printf("Hello world!");
	GlobalDescriptorTable gdt;


	while(1);
}
