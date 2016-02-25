/*
*kernel.h contains useful functions available for whole kernel.
*/
extern int printk(char * fmt, ...);	//printk.c
extern void panic(char * str);  	//panic.c

