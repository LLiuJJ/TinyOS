# TinyOS

TinyOS 是一些简版的 OS 实现，下面列出的是主要要实现的功能。

- 1     "Hello-World" Operating System, Boot-Process and Text-Output
- 2	    [Running the OS in a virtual machine (VirtualBox)](./doc/1.md)
- 3	    Memory-Segmentation, Global Descriptor Table
- 4	    Hardware-Communication / Ports
- 5	    Interrupts
- 6	    Keyboard
- 7	    Mouse
- 8	    Abstractions for Drivers
- 9	    Tidying Up
- 10    Peripheral Component Interconnect (PCI)
- 11	Base Address Registers
- 12	Graphics Mode (VGA)
- 13	GUI Framework Basics
- 14	Desktop, Windows
- 15	Multitasking
- 16	Memory-Management / Heap
- 17	Network 1
- 18	Network 2
- 19	Hard Discs (IDE/ATA)
- 20	System Calls, POSIX compliance
- 21	Network: Ethernet Frames
- 22	Network: Address Resolution Protocol (ARP)
- 23	Network: Internet Protocol (IPv4)
- 24	Network: Internet Control Message Protocol (ICMP)
- 25	Network: User Datagram Protocol (UDP)
- 26	Network: Transmission Control Protocol (TCP)
- 27	Network: Transmission Control Protocol (TCP)
- 28	Network: TCP and Hyper Text Terminal Protocol (HTTP)
- 29    [Partition Table](github.com/fenglin-Zhou/TinyOS)
- 30    [File Allocation Table(FAT32)](github.com/fenglin-Zhou/TinyOS)
-------------------------
make命令
- `make clean`  删除已经编译的*.o mykernel.bin mykernel.iso。
- `make run`    这个包括编译运行代码，并且会启动virtualbox运行刚编译出来的系统。
- `make mykernel.iso`  创建iso文件，首先你需要下载xorriso软件，不然不能使用该命令。

##### 参考资料

Orange'S:一个操作系统的实现 https://book.douban.com/subject/3735649/
现代操作系统 https://book.douban.com/subject/3852290/
TCP/IP详解 卷1：协议 https://book.douban.com/subject/1088054/
TCP/IP详解 卷2：实现 https://book.douban.com/subject/1087767/
