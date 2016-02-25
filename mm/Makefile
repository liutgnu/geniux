CFLAGS = -g -fno-stack-protector
CPP = -c -I../include
LDFLAGS = -r
objects = memory.o

mm.o:$(objects) page.o
	ld $(LDFLAGS) -o mm.o $(objects) page.o
.c.o:
	gcc $(CPP) $(CFLAGS) -o $*.o $<

page.o:page.s
	as -g -o page.o page.s

clean:
	rm -f $(objects) page.o mm.o

##dependencies
memory.o:memory.c ../include/kernel/kernel.h ../include/kernel/tsk.h
