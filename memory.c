/*
 *This file contains low functions of memory control, basicly operate on one page(4k), higher functions are not contained.
 *Main memory start from 1MB, in which ramdisk(1MB~3MB) and applications avaliable memory(3MB above)
*/
#include "kernel.h"

//all bound to 4k
#define RAMDISK 2*1024*1024
#define MAIN_MEM_START 0x100000
#define EXT_MEM_SIZE (*(unsigned short int *)0x9dc02)*1024
#define APP_MEM_START MAIN_MEM_START+RAMDISK
#define APP_MEM_END MAIN_MEM_START+EXT_MEM_SIZE
#define MMAP_SIZE 2304 //1MB~10MB
extern unsigned long TASK0;
extern struct task_struct *p_tasks[];

/*
 *0xff means page(4k) unavaliable, 0x00 means page free, 0xN means page occupied by N linear addresses
*/
unsigned char mmap[MMAP_SIZE]={0,};  //8 bit

void init_mem()
{
  int i;
  for (i=0;i<MMAP_SIZE;i++)
    mmap[i]=(unsigned char)0xff;
  for (i=RAMDISK/4096;i<EXT_MEM_SIZE/4096;i++)
    mmap[i]=(unsigned char)0;
  //task0 for p_tasks
  p_tasks[0]=(struct task_struct *)TASK0;
  printk("Total Memory:%d KB ok!\n",EXT_MEM_SIZE/1024+1024);
}

//get physical address of free page
unsigned char * phy_get_free_page()
{
  int i=RAMDISK/4096;
  while (i<MMAP_SIZE)
  {
    if (mmap[i]==0)
      break;
    else
      i++;
  }
  if (i==MMAP_SIZE)
  {
    printk("Not enough memory!");
    return((unsigned char *)0);
  }
  mmap[i]=(unsigned char)1;
  __asm__ volatile ("push %%es\n\t"
		    "movl %%eax,%%edi\n\t"
		    "movl $0x10,%%eax\n\t"
		    "mov  %%ax,%%es\n\t"
		    "movl $1024,%%ecx\n\t"
		    "cld\n\t"
		    "xorl %%eax,%%eax\n\t"
		    "rep\n\t"
		    "stosl\n\t"
		    "pop %%es"
		    ::"a" (MAIN_MEM_START+(i<<12)));
  return ((unsigned char *)(MAIN_MEM_START+(i<<12)));
}

/*
 *free a page by giving its physical address.
 *addr equal to 4k*N is recommended, but addr equal to 4k*N+[0,4k) can also work, both mean to free the Nth page.  
*/
void phy_free_page(unsigned char * phy_addr)
{
  int i;
  if (phy_addr < (unsigned char *)MAIN_MEM_START)
    panic("Try to free kernel");
  if (phy_addr >= (unsigned char *)APP_MEM_END)
    panic("Try to free memory higher than hightest");
  i=((unsigned int)phy_addr-MAIN_MEM_START)/4096;
  if (mmap[i]==0)
    panic("Try to free a free page");
  if (mmap[i]==0xff)
    panic("Try to free an unavaliable page");
  mmap[i]--;
}

/*

void put_page(unsigned char *phy_addr,unsigned char *line_addr)
{
  unsigned int * tmp;
  if (phy_addr & 0xfff !=0 || line_addr & 0xfff !=0)
    printk("align warn: Try to put physical addr %x to linear addr %x\n",phy_addr,line_addr);
  if (phy_addr < MAIN_MEM_START)
    panic("Try to put kernel addr %x",phy_addr);
  if (phy_addr >= APP_MEM_END)
    panic("Try to put nonexistent addr %x",phy_addr);
  tmp=(unsigned int *)((line_addr & 0xffc00000)>>20+PAGE_DIR);
  if ((*tmp)&1)
    tmp=(unsigned int *)(*tmp & 0xfffff000);
  else
  {
    *tmp=(unsigned int)(phy_get_free_page() | 7);
    tmp=(unsigned int *)(*tmp & 0xfffff000);
  }
  *((line_addr & 0x3ff000)>>10+tmp)=(unsigned int)(phy_addr | 7);
}

*/
