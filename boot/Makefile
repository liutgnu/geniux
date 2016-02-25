default:boot

boot.o:boot.s
	as86 -0 -o boot.o boot.s
boot:boot.o
	ld86 -0 -d -o boot boot.o
clean:
	rm -f boot.o boot
