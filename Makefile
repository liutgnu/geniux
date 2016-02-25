objects1 = syscall.o user_task.o fork.o panic.o schedule.o memory.o printk.o

objects2 = syscall.c user_task.c fork.c panic.c schedule.c memory.c printk.c

kernel.o: kernel.s
	as -o kernel.o kernel.s

page.o: page.s
	as -o page.o page.s

final.bin: kernel.o page.o $(objects1) $(objects2)
	gcc -c $(objects2)
	ld -Ttext 0x00 --oformat binary -o final.bin kernel.o page.o $(objects1) -M>system.map

system: kernel.o page.o $(objects1)
	ld -Ttext 0x00 -o system kernel.o page.o $(objects1)

boot.o:boot.s
	as86 -0 -o boot.o boot.s
boot:boot.o
	ld86 -0 -d -o boot boot.o

fakedisk:final.bin boot
	dd if=boot of=fakedisk
	dd if=final.bin of=fakedisk bs=512 seek=1 
clean: 
	rm -f $(objects1) Image boot boot.o kernel.o page.o final.bin system.map
