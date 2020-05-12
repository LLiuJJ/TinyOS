.set MAGIC, 0x1badb002  #对于bootloader来说，他不知道什么是kernel,它只是从设定好的位置开始运行程序。所以我们需要将kernel程序写入这个神奇的位置: 0x1badb002
.set FLAGS, (1<<0 | 1<<1)
.set CHECKSUM, -(MAGIC + FLAGS)


.section .multiboot
	.long MAGIC
	.long FLAGS
	.long CHECKSUM

.section .text
.extern kernelMain
.extern callConstructors
.global loader

loader:
	mov $kernel_stack, %esp
	call callConstructors
	push %eax
	push %ebx
	call kernelMain



_stop:
	cli
	hlt
	jmp _stop
	


.section .bss
.space 4*1024*1024; # 4 MiB 为了避免kernel_stack被填满，留一些内存在kernelMain和kernel stack之间。
kernel_stack:

.space 18*102; #18 kib
headp: