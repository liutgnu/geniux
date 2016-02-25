/*
 *panic(): when kernel errors, print useful information and die.
*/
extern int printk();

void panic(char * str)
{
  printk("Kernel panic:%s\n",str);
  for (;;);
}
