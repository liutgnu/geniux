# Overview #
---


## 1 Introduction ##

Geniux is a 32bit, multi-tasking operate system kernel, based on but not limited to the design of linux-0.12.

The purpose of the project is to learn: 
- mechanism of OS
- programming and debugging skills of C and assembly language
- how linux works
- fun of kernel hacking

## 2 Building and running ##

### Build system

Make sure in 32bit host Linux, open a terminal and go to geniux directory, type the following command if you want to build the system:

	$ make geniux

If something goes wrong, make sure you have installed all necessary building tools. There should be messages, such as “make: as86: Command not found” if you haven't installed bin86. If everything goes OK, you should have a geniux.img generated.

### Build app

Once the system finished booting, it will load a user space program called elf_child from it's file system called fs/minix.img. The minix.img already contains an excutable file, you don't have to do this step, but if you'd like to build your own program, type the following command to build one:

	$ sudo mount fs/minix /mnt
	$ cd app && make
	$ sudo cp elf_child /mnt/binary
	$ sudo umount /mnt

### Run

If you don't want to build the system, you can try the previously built geniux.img located in the directory of geniux. Let's try to boot the new system by typing the following command:

    $ qemu -m 8 -fda geniux.img

or:

	$ qemu-system-i386 -m 8 -fda geniux.img

depending on the version of qemu installed on your computer. Of course you have to install qemu at first.

## 3 Screen shot ##


Once you have booted into geniux, you should see a window like this, which indicates geniux is running successfully.

![screenshot](https://github.com/liutgnu/geniux/blob/master/screenshot.png)

Figure 1: Screen shot of geniux

## 4 How geniux works ##


### 4.1 Geniux structure overview

Figure 1: Geniux system structure

Geniux kernel is a monolithic kernel, I mainly referred to [fake-linux-0.00](http://oldlinux.org/Linux.old/kernel/0.00/linux-0.00-041217.tar.gz) and [linux-0.12](https://www.kernel.org/pub/linux/kernel/Historic/old-versions/linux-0.12.tar.gz) to build. I think monolithic kernel is easier to learn and understand comparing to microkernel such as minix3. User tasks run in user mode, cpu privilege ring3, getting all services needed through system call, interrupt 0x80, same as linux; kernel runs in kernel mode, cpu privilege ring0, providing system calls and error handling routine.

### 4.2 Boot overview

As for IBM compatible computers, there is POST(Power-on self-test) right after powered on. I don't plan to go too far on this, in short, there are a series of testing and initial sequences. After these, BIOS(Basic Input Output System) load the boot sector(512 Bytes) into memory started at address 0x7c00 and jump to that address, this is the very place our system starts. As for geniux, boot sector is the first 512 bytes of geniux.img. Figure 1 shows the map of geniux.img.

![Map_of_geniux.img.png](https://github.com/liutgnu/geniux/blob/master/Map_of_geniux.img.png)

Figure 1: Map of geniux.img

The function of boot sector is to load 
- 1) kernel, 
- 2) minix file system and
- 3) switch CPU mode from Real Mode to Protected Mode.

### Why the size of minix file system is 500k?

In real mode of CPU, and in purpose of no damage on BIOS area, the size of available memory is (640k-31k=609k), kernel takes 100k, the maximum size of minix file system is 509k. So I leave it to be 500k, see File system.

### Boot step 1

BIOS loads boot sector to memory address 0x7c00:

![Map_of_memory_step_1.png](https://github.com/liutgnu/geniux/blob/master/Map_of_memory_step_1.png)

Figure 2: Map of memory, step 1

### Boot step 2

Boot sector program copies itself to memory address 0x9dc00(631k), then continues itself from the new address:

![Map_of_memory_step_2.png](https://github.com/liutgnu/geniux/blob/master/Map_of_memory_step_2.png)

Figure 3: Map of memory, step 2

### Boot step 3

Use BIOS services to get extended memory size, load kernel from floppy disk to memory address 0x7c00 and minix file system to memory address 0x20c00:

![Map_of_memory_step_3.png](https://github.com/liutgnu/geniux/blob/master/Map_of_memory_step_3.png)

Figure 4: Map of memory, step 3

### Boot step 4

BIOS services are no longer needed, so we move kernel and file system to memory address 0:

![Map_of_memory_step_4.png](https://github.com/liutgnu/geniux/blob/master/Map_of_memory_step_4.png)

Figure 5: Map of memory, step 4

### Boot step 5

Set up gdt and ldt, switch CPU mode to protected mode, jump to kernel entry and execute from there.

### 4.3 Kernel overview

Kernel.s is the main kernel, I mainly referred to a [fake-linux-0.00's](http://oldlinux.org/Linux.old/kernel/0.00/linux-0.00-041217.tar.gz) head.s to build. Its structure is very simple:

![Structure_of_kernel.s.png](https://github.com/liutgnu/geniux/blob/master/Structure_of_kernel.s.png)

Figure 6: Structure of kernel.s

Like any other operate systems, geniux contains 4 parts: 
- process management
- memory management
- I/O management
- filesystem

The following 4 parts will discuss those parts in detail.

## 5 Process management ##

### 5.1 Process scheduling

The maximum process quantity is 64, timer interrupts in every 10ms, which invokes the process scheduling routing. Since the system is very simple, there is no task priorities, so scheduling is implemented by judging whether a task status is ready, then switch to the ready-status task.

![Process_scheduling.png](https://github.com/liutgnu/geniux/blob/master/Process_scheduling.png)

Figure 7: Process scheduling

### 5.2 Process creation

Task 0 is created by hand (tss, ldt, gdt are done manually, see kernel.s), then task n is created by syscall fork(). Also, there is exec() syscall to replace the current process image with a new process image.

6 Memory management
-------------------

In geniux, physical addresses 1MB\~3MB are reserved for ramdisk, 3MB above are dynamically allocated memory area for applications.

![Physical_memory_map.png](https://github.com/liutgnu/geniux/blob/master/Physical_memory_map.png)

Figure 8: Physical memory map

Meanwhile, the maximum supported virtual address for each task is 4GB.

![Virtual_memory_map.png](https://github.com/liutgnu/geniux/blob/master/Virtual_memory_map.png)

Figure 9: Virtual memory map

7 I/O management
----------------

Geniux now only supports read from ramdisk.

When invoking syscall int read(int fd, unsigned char \* buf, int b\_size), the position of specific data block is calculated by b\_size and f\_pos, the latter is known as the *current file offset*. Then geniux copies the data in that block to address \*buf, without the standard routine *copy\_to\_user()* in linux.

Actually it's a known bug in geniux, the user space and kernel space isn't separated well, so in kernel space you can directly read or write user space, that's why *copy\_to\_user()* is not needed. This bug should be fixed later.

## 8 File system ##


Geniux now only supports MINIX filesystem version 1.

File fs/minix.img is a normal 1.44MB minix filesystem image, but been tampered with during geniux.img making. Since floppy disk driver is not implemented, in order to load disk data into memory, I have to load disk data at boot time by BIOS services. The size of available memory space at boot time is 640KB-31.5KB-100KB=508.5KB, so there is almost only 500KB available, that is, 500KB data of minix.img is really loaded into memory, 500KB above is NOT loaded in.

You can rewrite files located in minix.img by mounting minix.img to your system:

	$ sudo mount fs/minix.img /mnt

Minix.img now contains an executable file “binary/elf\_child”, which is a userspace program interprets an assembly-language script. The script is locate in “source/source.s”, which prints the Fibonacci sequence. See screen shot.

Details about interpreter and assembly-language script can be found in [*https://github.com/liutgnu/asm-interpreter*](https://github.com/liutgnu/asm-interpreter)
