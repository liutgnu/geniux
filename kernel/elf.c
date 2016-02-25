#include <kernel/elf.h>
#include <kernel/tsk.h>
#define NULL (void *)0
#define mem_copyb(src_addr,dst_addr,byte_count)  \
	__asm__ ("mov %%ds,%%bx\n\t"  \
		"mov %%es,%%dx\n\t"  \
		"mov $0x10,%%ax\n\t"  \
		"mov %%ax,%%es\n\t"  \
		"mov %%ax,%%ds\n\t"  \
		"cld\n\t"  \
		"rep\n\t"  \
		"movsb\n\t"  \
		"mov %%dx,%%es\n\t"  \
		"mov %%bx,%%ds\n\t"  \
		::"c" (byte_count),"D" (dst_addr),"S" (src_addr))

int load_segment(unsigned long virtaddr,int fd,unsigned long cr3)
{
  struct elf_head_struct * file_tmp;
  unsigned char * p_magic;
  int program_segment_num;
  int phoff;
  struct program_segment_head_struct * file_tmp1;
  unsigned char * file_tmp2;
  int i;
  unsigned long load_virtaddr_start;
  unsigned long load_fileoffset_start;
  unsigned long load_fileoffset_end;
  int load_num;
  int j;
  unsigned char * p_tmp;

  file_tmp=(struct elf_head_struct *)read_file_pos_1k(fd,0);
  if (file_tmp==NULL)
  {
    printk("error!load_segment:read elf head error!\n");
    return -1;
  }
  p_magic=file_tmp->e_ident;
  if (*(unsigned long *)p_magic!=0x464c457f || file_tmp->e_type!=2 || file_tmp->e_machine!=3)
  {
    printk("error!load_segment:invalid elf format!\n");
    phy_free_page((unsigned char *)file_tmp);
    return -1;
  }
  program_segment_num=(int)(file_tmp->e_phnum);
  phoff=file_tmp->e_phoff;
  phy_free_page((unsigned char *)file_tmp);  //OK, we have finished read elf head
  
  file_tmp1=(struct program_segment_head_struct *)read_file_pos_1k(fd,phoff);
  if (file_tmp1==NULL)
  {
    printk("error!load_segment:read program segment error!\n");
    return -1;
  }
  for (i=0;i<program_segment_num;i++)
  {
    if ((file_tmp1+i)->p_type!=1)
      continue;
    if (virtaddr>=(file_tmp1+i)->p_vaddr && virtaddr<((file_tmp1+i)->p_vaddr+(file_tmp1+i)->p_memsz))
    {
      //OK, virtaddr is in THIS segment
      //there is something can be loaded from elf file
      if (virtaddr<((file_tmp1+i)->p_vaddr+(file_tmp1+i)->p_filesz))
      {
	load_virtaddr_start=virtaddr-virtaddr%4096;
	load_fileoffset_start=load_virtaddr_start-(file_tmp1+i)->p_vaddr+(file_tmp1+i)->p_offset;

	//now we load ONLY 1 page(4k) from elf file
	load_fileoffset_end=((file_tmp1+i)->p_offset+(file_tmp1+i)->p_filesz)<(load_fileoffset_start+4096)?((file_tmp1+i)->p_offset+(file_tmp1+i)->p_filesz):(load_fileoffset_start+4096);
	if (load_fileoffset_end-load_fileoffset_start==0)
	  load_num=0;
	else
	  load_num=(load_fileoffset_end-load_fileoffset_start)/1024+1;

	file_tmp2=phy_get_free_page();
	if (file_tmp2==NULL)
	{
	  printk("error!load_segment:not enough memory for dest page1!\n");
	  phy_free_page((unsigned char *)file_tmp1);
	  return -1;
	}
	for (j=0;j<load_num;j++)  //load_num must be 1~4
	{
	  p_tmp=read_file_pos_1k(fd,load_fileoffset_start+j*1024);
	  if (p_tmp==NULL)
	  {
	    printk("error!load_segment:read within elf error!\n");
	    phy_free_page((unsigned char *)file_tmp1);
	    phy_free_page(file_tmp2);
	  }
	  mem_copyb(p_tmp,file_tmp2+1024*j,1024);
	  phy_free_page(p_tmp);
	}
	put_page(file_tmp2,(unsigned char *)load_virtaddr_start,cr3);
	phy_free_page((unsigned char *)file_tmp1);
	return 0;
      }
      else  //nothing to be loaded from elf file
      {
	file_tmp2=phy_get_free_page();
	if (file_tmp2==NULL)
	{
	  printk("error!load_segment:not enough memory for dest page2!\n");
	  phy_free_page((unsigned char *)file_tmp1);
	  return -1;
	}
	put_page(file_tmp2,(unsigned char *)(virtaddr-virtaddr%4096),cr3);
	phy_free_page((unsigned char *)file_tmp1);
	return 0;
      }
    }
    else
      continue;
  }
  if (i==program_segment_num)
  {
    phy_free_page((unsigned char *)file_tmp1);
    return -1;
  }
}

void do_no_page(unsigned long address)
{
  int tmp;
  extern struct task_struct * current;
  unsigned char * p_tmp;

  if ((tmp=load_segment(address,current->exe_fd,current->tss.cr3))==0)
    return;
  else
  {
    p_tmp=phy_get_free_page();
    if (p_tmp==NULL)
    {
      printk("error!do_no_page: not enough memory, happens at virtual address %x\n",address);
      return;
    }
    put_page(p_tmp,address-address%4096,current->tss.cr3);
  }
}
