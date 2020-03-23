###  -m32是32位i386机型  
###  -nostdlib是指链接时不使用标准的system startup files or libaries 例如跳过了libgcc.a
###  -fno-builtin禁用内置函数，函数不能使用__builtin_开头
###  -fno-rtti Disable generation of information about every class with virtual functions for use by the C ++ runtime type identification features (dynamic_cast and typeid). If you don't use those parts of the language, you can save some space by using this flag.禁止生成带有虚拟函数的类信息，可以节省一些空间
###  -fno-leading-underscore 强制更改目标文件中c符号的表达方式，一种用途是帮助链接旧的汇编代码。同时使gcc生成代码不与没有此设定的二进制代码兼容，并不是所有目标都完整支持
###  -fno-use-cxa-atexit Register destructors for objects with static storage duration提供析构函数
###  -fno-exceptions 启用内核开发模式
GPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
###           ###
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = loader.o gdt.o port.o interrupts.o kernel.o

%.o: %.cpp
		g++ $(GPPPARAMS) -o $@ -c $<

%.o: %.s
		as $(ASPARAMS) -o $@ $<

mykernel.bin: linker.ld $(objects)
		ld $(LDPARAMS) -T $< -o $@ $(objects)

install: mykernel.bin
		sudo cp $< /boot/mykernel.bin

mykernel.iso: mykernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot/
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "My Operating System"{' >> iso/boot/grub/grub.cfg
	echo 'multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
	echo 'boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso
	
run: mykernel.iso
	(killall VirtualBox && sleep 1) || true
	VirtualBox --startvm "My Operating System" &

.PHONY: clean
clean:
	rm -f $(objects) mykernel.bin mykernel.iso

	
