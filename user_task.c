#include"sys_interface.h"
extern void print_logo();

void task0()
{
  __asm__ ("movl $0x17,%%eax\n\t"
	   "mov  %%ax,%%ds\n\t"
	   "mov  %%ax,%%es\n\t"
	   "mov  %%ax,%%fs\n\t"
	   "mov  %%ax,%%gs"::);
  int tmp;
  int tmp1;
  int int_num=0;  //syscall number for fork
  __asm__ ("int $0x80":"=a" (tmp):"a" (int_num));  //fork!
  if (tmp==0)  //child
  {
    print_XY('A',0,2);
    exe("/binary/elf_child");
    for(;;);
  }
  else if (tmp==-1)
  {
    print_XY('B',1,2);
    for(;;);
  }
  else  //parent
  {
    print_logo();
    __asm__ ("int $0x80":"=a" (tmp1):"a" (int_num));  //fork again!
    if (tmp1==0)
    {
      print_XY('C',2,2);
      exe("/binary/elf_parent");
      for(;;);
    }
    else if (tmp1==-1)
    {
      print_XY('D',3,2);
      for(;;);
    }
    else
    {
      print_XY('E',4,2);
      for (;;);
    }
  }
}
