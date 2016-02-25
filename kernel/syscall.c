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

extern int sys_exec();
extern int sys_fork();
extern int jiffies;
extern int open_file(char * dir_file);
extern int close_file(int fd);
extern int read_file(int fd,unsigned char * buf,int b_size);
extern int size_file(int fd);
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

int sys_fsize(int fd)
{
        return size_file(fd); 
}

f_int syscall_table[]={ sys_fork, sys_print, sys_utime ,sys_open ,sys_close, sys_read, sys_exec, sys_fsize };
