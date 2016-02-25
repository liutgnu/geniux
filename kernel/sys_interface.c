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
//int sys_fsize();      //7

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

int fsize(int fd)
{
  int int_num=7;
  int tmp;
  __asm__ ("int $0x80"
	   :"=a" (tmp)
	   :"a" (int_num),"b" (fd));
  return tmp;
}
