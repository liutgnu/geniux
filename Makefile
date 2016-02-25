objects1 = initial.o kernel.o keyboard.o keyboard_mid.o schedule.o syscall.o user_gus_num.o user_pnt_time.o

objects2 = initial.c keyboard.c keyboard_mid.c schedule.c syscall.c user_gus_num.c user_pnt_time.c

final.bin: $(objects1) $(objects2)
	gcc -c $(objects2)
	ld -Ttext 0x00 --oformat binary -o final.bin $(objects1) -M>system.map

boot.o:boot.s
	as86 -0 -o boot.o boot.s
boot:boot.o
	ld86 -0 -d -o boot boot.o

fakedisk:final.bin boot
	dd if=boot of=fakedisk
	dd if=final.bin of=fakedisk bs=512 seek=1 
clean: 
	rm -f $(objects1) Image boot boot.o final.bin system.map
