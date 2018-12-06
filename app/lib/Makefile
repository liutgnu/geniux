CFLAGS = -m32
LDFLAGS = -r
CPP = -c -I../include
objects = bzero.o malloc.o printf.o sys_interface.o

lib.o:$(objects)
	ld $(LDFLAGS) -o lib.o $(objects)
.c.o:
	gcc $(CPP) $(CFLAGS) -o $*.o $<

clean:
	rm -f $(objects) lib.o

##dependencies
printf.o:printf.c ../include/stdarg.h
