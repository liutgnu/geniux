#include"sys_interface.h"

void task0()
{
  __asm__ ("movl $0x17,%%eax\n\t"
	   "mov  %%ax,%%ds\n\t"
	   "mov  %%ax,%%es\n\t"
	   "mov  %%ax,%%fs\n\t"
	   "mov  %%ax,%%gs"::);
  unsigned char char_tmp[100]={0,};
  int fd;
  int int_num=0;
  int tmp;
  int i=0,j=160;
  __asm__ ("int $0x80":"=a" (tmp):"a" (int_num));
  if (tmp==0)  //child
  {
    print_XY('C',0,0);
    for(;;);
  }
  else if (tmp==-1)
  {
    print_XY('E',0,1);
  }
  else  //parent
  {
    if ((fd=open("/hello.c"))==-1)
      {print_XY('B',1,1);for(;;);}
    if (read(fd,char_tmp,90)==-1)
      {print_XY('B',2,1);for(;;);}
    while (char_tmp[i]!='\0')
      {print_Z(char_tmp[i++],j++);}
    if (close(fd)==-1)
      {print_XY('B',3,1);for(;;);}
    print_XY('A',4,1);
    for (;;);
  }
}


