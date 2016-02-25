/*
*we need some syscall here, so user applications could get kernel services.
*/

//int sys_input();	//0 
//int sys_print();	//1
//int sys_utime();	//2
//int sys_sound();	//3

#include "tsk.h"
#include "varible.h"
extern char keyboard_mid();
extern void play_sound();

int sys_input()
{
	return (int)keyboard_mid();
}

int sys_print(char ch,int z)  //start from 0
{
int position=z*2;
__asm__ volatile ("movl $0x18,%%edx\n\t"
		"mov  %%dx,%%gs\n\t"
		"movb %%al,%%gs:(%%ebx)"
		::"a" (ch),"b" (position));
return 0;
}

int sys_sound()
{
	play_sound();
	return 0;
}

int sys_utime()
{
	return (jiffies);  //no need to change 0.01s into s
}

//c system call interface
char input()
{
	int int_num=0;
	int tmp;
	__asm__ ("int $0x80"
		:"=a" (tmp)
		:"a" (int_num));
	return (char)tmp;
}

//print on screen in X-Y form
void print_XY(char ch,int x,int y)
{
	int int_num=1;
	__asm__ ("int $0x80"
		::"a" (int_num),"b" (ch),"c" (y*80+x));
}

//print on screen in disaplay memory form
void print_Z(char ch,int z)
{
	int int_num=1;
	__asm__ ("int $0x80"
		::"a" (int_num),"b" (ch),"c" (z));
}

//return time in 0.01s
int time_ms()
{
	int int_num=2;
	int tmp;
	__asm__ ("int $0x80"
		:"=a" (tmp)
		:"a" (int_num));
	return tmp;
}

//return time in 1s
int time_s()
{
	int int_num=2;
	int tmp;
	__asm__ ("int $0x80"
		:"=a" (tmp)
		:"a" (int_num));
	return (tmp/100);
}

void sound()
{
	int int_num=3;
	__asm__ ("int $0x80"
		::"a" (int_num));
}
