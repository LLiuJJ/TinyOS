###  -m32是32位i386机型  
###  -nostdlib是指链接时不使用标准的system startup files or libaries 例如跳过了libgcc.a
###  -fno-builtin禁用内置函数，函数不能使用__builtin_开头
###  -fno-rtti Disable generation of information about every class with virtual functions for use by the C ++ runtime type identification features (dynamic_cast and typeid). If you don't use those parts of the language, you can save some space by using this flag.禁止生成带有虚拟函数的类信息，可以节省一些空间
###  -fno-leading-underscore 强制更改目标文件中c符号的表达方式，一种用途是帮助链接旧的汇编代码。同时使gcc生成代码不与没有此设定的二进制代码兼容，并不是所有目标都完整支持
###  -fno-use-cxa-atexit Register destructors for objects with static storage duration提供析构函数
###  -fno-exceptions 启用内核开发模式
GCCPARAMS = -m32 -Iinclude -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings
###           ###
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/loader.o \
		obj/gdt.o \
		obj/memorymanagement.o \
		obj/drivers/driver.o \
		obj/hardwarecommunication/port.o \
		obj/hardwarecommunication/interruptstubs.o \
		obj/hardwarecommunication/interrupts.o \
		obj/syscalls.o \
		obj/multitasking.o \
		obj/drivers/amd_am79c973.o \
		obj/hardwarecommunication/pci.o \
		obj/drivers/keyboard.o \
		obj/drivers/mouse.o \
		obj/drivers/vga.o \
		obj/drivers/ata.o \
		obj/filesystem/msdospart.o \
		obj/filesystem/fat.o \
		obj/gui/widget.o \
		obj/gui/window.o \
		obj/gui/desktop.o \
		obj/net/etherframe.o \
		obj/net/arp.o \
		obj/net/ipv4.o \
		obj/net/icmp.o \
		obj/net/udp.o \
		obj/net/tcp.o \
		obj/kernel.o

obj/%.o: src/%.cpp
		mkdir -p $(@D)
		gcc $(GCCPARAMS) -o $@ -c $<

obj/%.o: src/%.s
		mkdir -p $(@D)
		as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
		ld $(LDPARAMS) -T $< -o $@ $(objects)

install: mykernel.bin
		sudo cp $< /boot/mykernel.bin

## make mykernel.iso 
mykernel.iso: mykernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp mykernel.bin iso/boot/mykernel.bin
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "My Operating System"{' >> iso/boot/grub/grub.cfg
	echo 'multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
	echo 'boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=mykernel.iso iso
	rm -rf iso

# run on VirtualBox	
run: mykernel.iso
	(killall VirtualBox && sleep 1) || true
	VirtualBox --startvm "My Operating System" &

#clean .o .bin .iso
.PHONY: clean
clean:
	rm -rf obj mykernel.bin mykernel.iso

	
