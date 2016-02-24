fakedisk:ok_kernel boot
	dd if=boot of=fakedisk
	dd if=ok_kernel of=fakedisk bs=512 seek=1 skip=8
	sync
boot:boot.o
	ld86 -0 -d -o boot boot.o
boot.o:boot.s
	as86 -0 -o boot.o boot.s
ok_kernel:ok_kernel.o
	ld -Ttext 0x00 -M -s -x -o ok_kernel ok_kernel.o > ok_kernel.map
ok_kernel.o:ok_kernel.s
	as -o ok_kernel.o ok_kernel.s
clean:
	rm ok_kernel ok_kernel.o boot boot.o ok_kernel.map
