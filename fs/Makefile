CFLAGS = -g -fno-stack-protector
CPP = -c -I../include
LDFLAGS = -r
objects = blk_buffer.o fs_init.o iname.o ramdisk.o zmap.o file.o imap.o inode.o super.o

fs.o:$(objects)
	ld $(LDFLAGS) -o fs.o $(objects)

.c.o:
	gcc $(CPP) $(CFLAGS) -o $*.o $<
clean:
	rm -f $(objects) fs.o

##dependencies
blk_buffer.o:blk_buffer.c ../include/fs/fs.h
fs_init.o:fs_init.c ../include/fs/fs.h ../include/kernel/kernel.h
iname.o:iname.c ../include/fs/fs.h ../include/kernel/kernel.h
super.o:super.c ../include/fs/fs.h ../include/kernel/kernel.h
file.o:file.c ../include/fs/fs.h ../include/kernel/kernel.h
imap.o:imap.c ../include/fs/fs.h ../include/kernel/kernel.h
inode.o:inode.c ../include/fs/fs.h ../include/kernel/kernel.h
ramdisk.o:ramdisk.c
zmap.o:zmap.c ../include/fs/fs.h ../include/kernel/kernel.h
