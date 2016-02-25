CFLAGS = -g -fno-stack-protector
CPP = -c -I../include
LDFLAGS = -r
objects = elf.o exec.o fork.o panic.o printk.o schedule.o syscall.o sys_interface.o user_task.o

geniux.o:$(objects) kernel.o
	ld $(LDFLAGS) -o geniux.o kernel.o $(objects)

.c.o:
	gcc $(CPP) $(CFLAGS) -o $*.o $<

kernel.o:kernel.s
	as -g -o kernel.o kernel.s

clean:
	rm -f $(objects) kernel.o geniux.o

##dependencies
elf.o:elf.c ../include/kernel/elf.h ../include/kernel/tsk.h
fork.o:fork.c ../include/kernel/tsk.h
printk.o:printk.c ../include/kernel/stdarg.h
syscall.o:syscall.c
exec.o:exec.c ../include/kernel/elf.h ../include/kernel/tsk.h
schedule.o:schedule.c ../include/kernel/tsk.h
