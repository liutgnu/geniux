#include "../app/include/sys_interface.h"

void task0()
{
  __asm__ ("movl $0x17,%%eax\n\t"
	   "mov  %%ax,%%ds\n\t"
	   "mov  %%ax,%%es\n\t"
	   "mov  %%ax,%%fs\n\t"
	   "mov  %%ax,%%gs"::);


 int int_num = 0;
 int tmp;
 __asm__ ("int $0x80":"=a" (tmp):"a" (int_num));  //fork a process
 if (tmp==0) {
  exe("/binary/elf_child");
  print_XY('C',0,0);
  for(;;);
 } else if (tmp==-1) {
  print_XY('E',0,1);
  for(;;);
 } else {
  for(;;);
 }

}
