#include "tsk.h"
#define TASK_QUATITY 64  //pid ranges from 0~63
#define MAIN_MEM_START 0x100000
/*byte_count is 2^n*/
#define mem_copy(src_addr,dst_addr,byte_count)  \
	__asm__ ("push %%es\n\t"  \
		"push %%ds\n\t"  \
		"mov $0x10,%%ax\n\t"  \
		"mov %%ax,%%es\n\t"  \
		"mov %%ax,%%ds\n\t"  \
		"cld\n\t"  \
		"rep\n\t"  \
		"movsl\n\t"  \
		"pop %%ds\n\t"  \
		"pop %%es\n\t"  \
		::"c" (byte_count>>2),"D" (dst_addr),"S" (src_addr))

//task_map contains 64 bits,0 means no task, 1 means task occuping.
struct task_map{
  unsigned long m_low;
  unsigned long m_high;
} t_map ={1,0};

struct task_struct *p_tasks[TASK_QUATITY]={(struct task_struct *)0,};
extern struct task_struct * current;
extern struct gldt_struct * gdt_addr;
unsigned long copy_tables_pages();
unsigned char * phy_get_free_page();
void share_pages_dommap();
void share_pages_nommap();
extern unsigned char mmap[];

int find_empty_task()
{
  long tmp=t_map.m_low;
  int i=0;
  while (i<32)
  {
    if ((tmp&1)==1)
    {
      tmp>>=1;
      i++;
    }
    else
      break;
  }
  if (i==32)
  {
    tmp=t_map.m_high;
    while (i<64)
    {
      if ((tmp&1)==1)
      {
	tmp>>=1;
	i++;
      }
      else
	break;
    }
  }
  if (i==64)
    printk("warning:task map is full!\n");
  return (i);
}

int copy_process(unsigned long ebp,unsigned long edi,unsigned long esi,
		 unsigned long gs,unsigned long none,unsigned long ebx,
		 unsigned long ecx,unsigned long edx,unsigned long eax,
		 unsigned long fs,unsigned long es,unsigned long ds,
		 unsigned long eip,unsigned long cs,unsigned long eflags,
		 unsigned long esp,unsigned long ss)
{
  struct task_struct *p;
  struct gldt_struct *gdt_tmp_new;
  struct gldt_struct *gdt_tmp_old;
  unsigned long p_tmp;
  int tmp;

  p=(struct task_struct *)phy_get_free_page();
  if (p==(struct task_struct *)0)
  {
    printk("warning:copy_process error!\n");
    return -1;
  }

  p->state=-1;
  if ((tmp=find_empty_task())==64)
  {
    printk("warning:copy_process error!\n");
    return -1;
  }
  p_tasks[tmp]=p;
  p->pid=tmp;
  p->p_parent=current;
  current->p_child=p;
  p->p_child=(struct task_struct *)0;
  p->tss.back_link=0;
  p->tss.esp0=(unsigned long)p+4*1024;  //top of this page. though in app mem area, managed by kernel.
  p->tss.ss0=0x10;  //kernel, data seg

  //esp1,ss1,esp2,ss2 are not supported by now.
  //cr3 is left behind

  p->tss.eip=eip;
  p->tss.eflags=eflags;
  p->tss.eax=0;  //fork return value
  p->tss.ecx=ecx;
  p->tss.edx=edx;
  p->tss.ebx=ebx;
  p->tss.esp=esp;  //no problem here!!!
  p->tss.ebp=ebp;
  p->tss.esi=esi;
  p->tss.edi=edi;
  p->tss.es=es & 0xffff;
  p->tss.cs=cs & 0xffff;
  p->tss.ss=ss & 0xffff;
  p->tss.ds=ds & 0xffff;
  p->tss.fs=fs & 0xffff;
  p->tss.gs=gs & 0xffff;
  //ldt left behind
  p->tss.trace_bitmap=0x80000000;
  p->tss.cr3=copy_tables_pages();
  p->ldt[0]=current->ldt[0];
  p->ldt[1]=current->ldt[1];
  p->ldt[2]=current->ldt[2];
  
  //gdt.tss(n)
  gdt_tmp_new=gdt_addr+(p->pid)*2+4;
  gdt_tmp_old=gdt_addr+(current->pid)*2+4;
  gdt_tmp_new->seg_limit=gdt_tmp_old->seg_limit;
  p_tmp=(unsigned long)(&(p->tss));
  gdt_tmp_new->B_0_15=(unsigned short int)(p_tmp & 0xffff);  //low 16bit
  gdt_tmp_new->B_16_23=((unsigned short int)(p_tmp >> 16) & 0xff) | 0xe900;
  gdt_tmp_new->B_24_31=((unsigned short int)(p_tmp >> 16) & 0xff00) | 0x80;

  //gdt.ldt(n)
  gdt_tmp_new++;
  gdt_tmp_old++;
  gdt_tmp_new->seg_limit=gdt_tmp_old->seg_limit;
  p_tmp=(unsigned long)(&(p->ldt[0]));
  gdt_tmp_new->B_0_15=(unsigned short int)(p_tmp & 0xffff);  //low 16bit
  gdt_tmp_new->B_16_23=((unsigned short int)(p_tmp>>16) & 0xff) | 0xe200;
  gdt_tmp_new->B_24_31=((unsigned short int)(p_tmp>>16) & 0xff00) | 0x80;

  p->tss.ldt=40+(p->pid)*16;
  p->state=0;
  printk("Greetings from fork()!\n");
  return (int)(p->pid);
}

//setup tables and pages, then return table addr(cr3)
unsigned long copy_tables_pages()
{
  unsigned long src_base=current->tss.cr3;
  unsigned char *dst_base=phy_get_free_page();
  unsigned long *tmp=(unsigned long *)dst_base;
  int i=0;
  if (dst_base==(unsigned char *)0)
  {
    return 0;
  }
  mem_copy(src_base,dst_base,4*1024);  //copy table
  
  while ((*(tmp+i) & 1) && i<1024)  //oops, YOU USED 4G LINEAR SPACE!!!
  {
    dst_base=phy_get_free_page();
    if (dst_base==(unsigned char *)0)  //no enough memory, release all previously allocated pages
    {
      i--;
      for (;i>=0;i--)
      {
	phy_free_page((unsigned char *)(*(tmp+i) & 0xfffff000));
      }
      phy_free_page((unsigned char *)tmp);
      return 0;
    }
    src_base=*(unsigned long *)(current->tss.cr3 + (i<<2)) & 0xfffff000;
    mem_copy(src_base,dst_base,4*1024);  //copy page
    
    *(tmp+i)=(unsigned long)dst_base | 7;  //adapt table
    i++;
  }
  if (current->pid==0 || current->pid==1);
  else
  {  
    share_pages_nommap((unsigned long)tmp);  //child
    share_pages_dommap(current->tss.cr3);  //parent
  }
  return (unsigned long)tmp;
}

//share pages with mmap[i]++
void share_pages_dommap(unsigned long cr3)
{
  unsigned long * base_table=(unsigned long *)cr3;
  unsigned long * base_page;
  int i;
  while ((*base_table & 1) && ((unsigned long)base_table<cr3+4096))
  {
    base_page=(unsigned long *)(*base_table & 0xfffff000);
    while (*base_page & 1)
    {
      *base_page &= 0xfffffffd;
      i=((*base_page & 0xfffff000)-MAIN_MEM_START)>>12;
      mmap[i]++;
      base_page++;
    }
    base_table++;
  }
}

//share pages without mmap[i]++
void share_pages_nommap(unsigned long cr3)
{
  unsigned long * base_table=(unsigned long *)cr3;
  unsigned long * base_page;
  while ((*base_table & 1) && ((unsigned long)base_table<cr3+4096))
  {
    base_page=(unsigned long *)(*base_table & 0xfffff000);
    while (*base_page & 1)
    {
      *base_page &= 0xfffffffd;
      base_page++;
    }
    base_table++;
  }
}

void do_page_wprotected(unsigned long cr2)
{
  unsigned char *dst_base;
  int i;
  unsigned long tmp=current->tss.cr3;
  tmp=tmp+(cr2>>22);
  tmp=*(unsigned long *)tmp & 0xfffff000;
  tmp=tmp+(cr2>>12 & 0x3ff);

  i=((*(unsigned long *)tmp & 0xfffff000)-MAIN_MEM_START)>>12;
  if (mmap[i]==1)
  {
    *(unsigned long *)tmp |=2;
    return;
  }

  if ((dst_base=phy_get_free_page())==(unsigned char *)0)
  {
    printk("warning:do_page_wprotected() error!\n");
    current->state=-1;  //don't run it again! NEED IMPROVED!
    return;
  }
  
  mem_copy(*(unsigned long *)tmp & 0xfffff000,dst_base,4*1024);
  mmap[i]--;
  *(unsigned long *)tmp=(unsigned long)dst_base | 7;
}
