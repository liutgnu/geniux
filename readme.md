# This is geniux operate system kernel.

---

## To compile:
**1)**  make sure in 32-bit linux

**2)**  type the following command to compile:

	make geniux                                                                                                    
                                                                                                                         
## To run:                                                                                                               
	qemu-system-i386 -m 8 -fda geniux.img                                                                              
                                                                                                                         
#FAQ                                                                                                                     
                                                                                                                          
## After booting, what's printed on my screen?
![image](https://github.com/geniux/screenshot.png)
Actually the kernel powers an interpreter, which runs in the user space and interprets an assembly-language-like script. The interpreter is kind of like a shell, which you will see in the first place after linux finishs booting.

The source code of the interpreter is located at /app and the assembly-language-like script is stored in /fs/minix.img, a MINIX file system image.If you want to modify the script, you may type the following command:

	sudo mount minix.img /mnt
	vi /mnt/source/source.s

The script now prints the Fibonacci sequence. Details about interpreter and assembly-language-like script can be found in [https://github.com/liutgnu/asm-interpreter](https://github.com/liutgnu/asm-interpreter)
