#include "elf.h"
#include "tsk.h"
#define NULL (void *)0
extern struct task_struct * current;
int doo_execve(unsigned long * eip,unsigned long none,unsigned long * filename)
{
  int exe_fd;
  unsigned char * p_stack;
  struct elf_head_struct * file_tmp;
  unsigned char * p_magic;
  unsigned long entry;
  int program_segment_num;
  int phoff;
  unsigned long * tmp;
  int i;
  unsigned long tmp1;

  if ((p_stack=phy_get_free_page())==NULL)
  {
    printk("error!do_execve:no memory for stack!\n");
    return -1;
  }
  if ((exe_fd=open(filename))==-1)
  {
    printk("error!do_execve:can't open elf file!\n");
    return -1;
  }

  //get necessary info of elf, such as code entry, code seg length, data seg length
  current->exe_fd=exe_fd;
  file_tmp=(struct elf_head_struct *)read_file_pos_1k(exe_fd,0);
  if (file_tmp==NULL)
  {
    printk("error!do_execve:read elf head error!\n");
    return -1;
  }
  p_magic=file_tmp->e_ident;
  if (*(unsigned long *)p_magic!=0x464c457f || file_tmp->e_type!=2 || file_tmp->e_machine!=3)
  {
    printk("error!do_execve:invalid elf format!\n");
    phy_free_page((unsigned char *)file_tmp);
    return -1;
  }
  entry=file_tmp->e_entry;
  program_segment_num=(int)(file_tmp->e_phnum);
  phoff=file_tmp->e_phoff;
  phy_free_page((unsigned char *)file_tmp);

  /*
    NOTE:There seems to be a problem.since EIP=0x8048000,code segment base must be 0x00!?
    If code segment base=0x8048000,1)program cannot access address lower than 0x8048000, kind of wasted isn't it? 2)since eip represents code segment offset, if code segment base settled this way, EIP must not be as big as 0x8048000.
    So I have to live segment base to 0x00, let EIP ESP etc do their own bussiness.
    That leaves another problem, what about segment privileges? segment limit privilege provided by 80x86 will be unfunctional, isn't it serious? I don't know how modern OS handles this.
  */ 
  /*
  struct program_segment_head_struct * file_tmp1=(struct program_segment_head_struct *)read_file_pos_1k(exe_fd,phoff);
  if (file_tmp1==NULL)
  {
    printk("error!do_execve:read program segment error!\n");
    return -1;
  }
  unsigned long code_seg_limit=0;
  unsigned long data_seg_limit=0;
  int i;
  for (i=0;i<program_segment_num;i++)
  {
    if ((file_tmp1+i)->p_type!=1)  //type:LOAD
      continue;
    if ((file_tmp1+i)->p_flags==5)  //CODE:R_E
    {
      code_seg_limit=(file_tmp1+i)->memsz;
    }
    if ((file_tmp1+i)->p_flags==6)  //DATA:RW_
    {
      data_seg_limit=(file_tmp1+i)->memsz; 
    }
  }
  phy_free_page((unsigned char *)file_tmp1);
  */

  //1)free all program memory  ##bug here!!! NEED IMPROVED!
  //2)free all memory page  ##NOTE here!!!  you cannot modify the pages mapped to kernel(0~8MB), because current cr3 is pointing at child's virtual memory, if any modification happens, instructions such as *tmp=0 would recieve a page fault!!!

  tmp=(unsigned long *)(current->tss.cr3);

  for (i=2;i<1024;i++)  //8MB above!!!
  {
    if ((*(tmp+i)) & 1)  //1
    {
      tmp1=(*(tmp+i)) & 0xfffff000;
      phy_free_page((unsigned char *)tmp1);
      *(tmp+i)=0;
    }
  }

  //3)setup ldt
  //code segment:
  current->ldt[1].seg_limit=0xffff;
  current->ldt[1].B_0_15=0;
  current->ldt[1].B_16_23=0xfa00;
  current->ldt[1].B_24_31=0x00cf;
  //data segment:
  current->ldt[2].seg_limit=0xffff;
  current->ldt[2].B_0_15=0;
  current->ldt[2].B_16_23=0xf200;
  current->ldt[2].B_24_31=0x00cf;  

  //4)setup tss
  //NOTE:there is no need setting up cs and eip, unlike "ljmp $TSS0,$0". the former case, task switch is done by iret, cs and eip are provided by stack, so you don't have to set up cs and eip in tss; the later case, task switch is done by ljmp, cs and eip are provided by tss, so you have to set up cs and eip in tss. Same as ss and esp, looks like we only need to set up ss0 and esp0.
/*  unsigned char * stack_base=phy_get_free_page();
  if (stack_base==NULL)
  {
    printk("error!do_execve: no memory for stack0!\n");
    return -1;
  }
  current->tss.esp0=(unsigned long)stack_base+4092;
*/  //5)modify return EIP and ESP
  *eip=entry;
  put_page(p_stack,0xbf801000,current->tss.cr3);

  *(eip+3)=0xbf801ffc;  //0xbf802000-4, top of a 4K page. OKOK, modern OS stack varies, I just leave 0xbf802000 to be the stack top.
  return 0;
} 
