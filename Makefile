CFLAGS =-g -fno-stack-protector

objects = syscall.o user_task.o fork.o panic.o schedule.o memory.o printk.o ramdisk.o blk_buffer.o super.o imap.o zmap.o inode.o iname.o file.o fs_init.o elf.o exec.o logo.o

kernel.o: kernel.s
	as -g -o kernel.o kernel.s

page.o: page.s
	as -g -o page.o page.s

.c.o:
	gcc -c $(CFLAGS) -o $*.o $<

final.bin: system
	objcopy -O binary system final.bin

system: kernel.o page.o $(objects)
	ld -Ttext 0x00 -o system kernel.o page.o $(objects) -M>system.map

boot.o:boot.s
	as86 -0 -o boot.o boot.s
boot:boot.o
	ld86 -0 -d -o boot boot.o

Image:final.bin boot
	dd if=boot of=Image
	dd if=final.bin of=Image bs=512 seek=1 
fakedisk:Image minix.img
	dd if=Image of=fakedisk
	dd if=minix.img of=fakedisk bs=512 seek=201 count=100
elf_child:elf_child.c
	gcc -S elf_child.c
	as -o elf_child.o elf_child.s
	ld -e main -o elf_child elf_child.o
elf_parent:elf_parent.c
	gcc -S elf_parent.c
	as -o elf_parent.o elf_parent.s
	ld -e main -o elf_parent elf_parent.o

clean: 
	rm -f $(objects) system Image boot boot.o kernel.o page.o final.bin system.map elf_*.o elf_*.s
