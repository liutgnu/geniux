CFLAGS = -m32
CPP = -c -Iinclude
objects = calc_num.o compiler.o

elf_child:$(objects) lib/lib.o
	ld -e main -o elf_child $(objects) lib/lib.o
.c.o:
	gcc $(CPP) $(CFLAGS) -o $*.o $<
lib/lib.o:
	(cd lib;make)

clean:
	rm -f $(objects) elf_child
	(cd lib;make clean)

##dependencies
calc_num.o:calc_num.c ./include/sys_interface.h
compiler.o:compiler.c ./include/sys_interface.h
