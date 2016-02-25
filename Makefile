LDFLAGS = -Ttext 0x00
ARCHIVES = kernel/geniux.o fs/fs.o mm/mm.o

fakedisk:Image fs/minix.img
	dd if=Image of=fakedisk
	dd if=fs/minix.img of=fakedisk bs=512 seek=201 count=100

Image:final.bin boot/boot
	dd if=boot/boot of=Image
	dd if=final.bin of=Image bs=512 seek=1

final.bin:system
	objcopy -O binary system final.bin

system:$(ARCHIVES)
	ld $(LDFLAGS) -o system $(ARCHIVES) -M>system.map	

kernel/geniux.o:
	(cd kernel;make)
fs/fs.o:
	(cd fs;make)
mm/mm.o:
	(cd mm;make)
boot/boot:
	(cd boot;make)

clean:
	rm -f final.bin Image system.map system
	(cd kernel;make clean)
	(cd fs;make clean)
	(cd mm;make clean)
	(cd boot;make clean)
