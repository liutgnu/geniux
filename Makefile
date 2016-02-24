fakedisk:tmp.bin boot
	dd if=boot of=fakedisk
	dd if=tmp.bin of=fakedisk bs=512 seek=1
	sync
boot:boot.o
	ld86 -0 -d -o boot boot.o
boot.o:boot.s
	as86 -0 -o boot.o boot.s
tmp.bin:newkernel.s
	as86 -0 -b tmp.bin newkernel.s
newkernel.s:newkernel.c
	bcc -C -c -D __i86__ -0 -S newkernel.c
clean:newkernel.s tmp.bin boot.o boot
	rm newkernel.s tmp.bin boot.o boot
