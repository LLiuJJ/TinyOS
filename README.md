# TinyOS

TinyOS 是一些简版的 OS 实现，下面列出的是主要要实现的功能。

- 1     ["Hello-World" Operating System, Boot-Process and Text-Output](./doc/1/1.md)
- 2	    Running the OS in a virtual machine (VirtualBox)
- 3	    [Memory-Segmentation, Global Descriptor Table](./doc/3/3.md)
- 4	    [Hardware-Communication / Ports](./doc/4/4.md)
- 5	    [Interrupts](./doc/5/5.md)
- 6	    [Keyboard](./doc/6.6.md)
- 7	    Mouse
- 8	    Abstractions for Drivers
- 9	    Tidying Up
- 10    [Peripheral Component Interconnect (PCI)](./doc/10/10.md)
- 11	Base Address Registers
- 12	[Graphics Mode (VGA)](./doc/12/12.md)
- 13	GUI Framework Basics
- 14	Desktop, Windows
- 15	[Multitasking](./doc/15/15.md)
- 16	[Memory-Management / Heap](./doc/16/16.md)
- 17	[AMD amd_am79c973](./doc/17/17.md)
- 18	AMD continued
- 19	[Hard Discs (IDE/ATA)](./doc/19/19.md)
- 20	[System Calls, POSIX compliance](./doc/20/20.md)
- 21	[Network: Ethernet Frames](./doc/21/21.md)
- 22	[Network: Address Resolution Protocol (ARP)](./doc/22/22.md)
- 23	[Network: Internet Protocol (IPv4)](./doc/23/23.md)
- 24	[Network: Internet Control Message Protocol (ICMP)](./doc/23/23.md)
- 25	Network: User Datagram Protocol (UDP)
- 26	Network: Transmission Control Protocol (TCP)
- 27	[Network: Transmission Control Protocol (TCP)](./doc/24/24.md)
- 28	Network: TCP and Hyper Text Terminal Protocol (HTTP)
- 29    Partition Table
- 30    [File Allocation Table(FAT32)](./doc/30/30.md)
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
