/*
*we need some syscall here, so user applications could get kernel services.
*/

//int sys_fork();	//0
//int sys_print();	//1
//int sys_utime();	//2
//int sys_open();       //3
//int sys_close();      //4
//int sys_read();       //5
//int sys_exec();	//6

extern int sys_exec();
extern int sys_fork();
extern int jiffies;
extern int open_file(char * dir_file);
extern int close_file(int fd);
extern int read_file(int fd,unsigned char * buf,int b_size);
typedef int (*f_int)();

int sys_print(char ch,int z)  //start from 0
{
int position=z*2;
__asm__ volatile ("movl $0x18,%%edx\n\t"
		"mov  %%dx,%%gs\n\t"
		"movb %%al,%%gs:(%%ebx)"
		::"a" (ch),"b" (position));
return 0;
}

int sys_utime()
{
	return (jiffies);  //no need to change 0.01s into s
}

int sys_open(char * dir_file)
{
        return open_file(dir_file);
}

int sys_close(int fd)
{
        return close_file(fd);
}

int sys_read(int fd,unsigned char * buf,int b_size)
{
        return read_file(fd,buf,b_size);
}

////////////////////////c system call interface
int fork()
{
	int int_num=0;
	int tmp;
	__asm__ ("int $0x80"
		:"=a" (tmp)
		:"a" (int_num));
	return tmp;
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

int open(char * dir_file)
{
  int int_num=3;
  int tmp;
  __asm__ ("int $0x80"
	   :"=a" (tmp)
	   :"a" (int_num),"b" (dir_file));
  return tmp;
}

int close(int fd)
{
  int int_num=4;
  int tmp;
  __asm__ ("int $0x80"
	   :"=a" (tmp)
	   :"a" (int_num),"b" (fd));
  return tmp;
}

int read(int fd,unsigned char * buf,int b_size)
{
  int int_num=5;
  int tmp;
  __asm__ ("int $0x80"
	   :"=a" (tmp)
	   :"a" (int_num),"b" (fd),"c" (buf),"d" (b_size));
  return tmp;
}

int exe(char * filename)
{
  int int_num=6;
  int tmp;
  __asm__ ("int $0x80"
	   :"=a" (tmp)
	   :"a" (int_num),"b" (filename));
  return tmp;
}

f_int syscall_table[]={ sys_fork, sys_print, sys_utime ,sys_open ,sys_close, sys_read, sys_exec };
