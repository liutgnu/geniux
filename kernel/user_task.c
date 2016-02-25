extern int exe();

void task0()
{
  __asm__ ("movl $0x17,%%eax\n\t"
	   "mov  %%ax,%%ds\n\t"
	   "mov  %%ax,%%es\n\t"
	   "mov  %%ax,%%fs\n\t"
	   "mov  %%ax,%%gs"::);
  exe("/binary/elf_child");
  for(;;);
}
