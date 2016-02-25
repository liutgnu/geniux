objects1 = initial.o kernel.o keyboard.o keyboard_mid.o schedule.o syscall.o user_gus_num.o user_pnt_time.o sound.o

objects2 = initial.c keyboard.c keyboard_mid.c schedule.c syscall.c user_gus_num.c user_pnt_time.c sound.c

final.bin: $(objects1) $(objects2)
	gcc -c $(objects2)
	ld -Ttext 0x00 --oformat binary -o final.bin $(objects1) -M>system.map

boot.o:boot.s
	as86 -0 -o boot.o boot.s
boot:boot.o
	ld86 -0 -d -o boot boot.o

kernel.s:kernel.S
	gcc -E kernel.S -o kernel.s

Image:final.bin boot
	dd if=boot of=Image
	dd if=final.bin of=Image bs=512 seek=1 
disk:Image
	dd if=Image of=/dev/fd0
	dd if=god.wav of=/dev/fd0 bs=512 seek=128
	sync

fakedisk:Image
	dd if=Image of=fakedisk
	dd if=god.wav of=fakedisk bs=512 seek=128
	sync

clean: 
	rm -f $(objects1) Image boot boot.o kernel.s final.bin system.map
