/*
 *This file deals with files
 *NOTE! since our file system is very weak, i try to make fs as small as possible. some features of minix fs are not implemented!!!
*/
#include "fs.h"
#include "kernel.h"
#define mem_copyb(src_addr,dst_addr,byte_count)  \
	__asm__ ("push %%es\n\t"  \
		"push %%ds\n\t"  \
		"mov $0x10,%%ax\n\t"  \
		"mov %%ax,%%es\n\t"  \
		"mov %%ax,%%ds\n\t"  \
		"cld\n\t"  \
		"rep\n\t"  \
		"movsb\n\t"  \
		"pop %%ds\n\t"  \
		"pop %%es\n\t"  \
		::"c" (byte_count),"D" (dst_addr),"S" (src_addr))

extern short int get_absolute_dir_file(char * p_absolute_dir);
extern int get_inode(unsigned int dev_t,int i_nth);
extern struct m_inode inodes_table[MAX_INODES];
extern struct buffer_head buffer_head_table[BUFFER_QUANTITY];
extern int get_buffer_serial_num(unsigned int dev_t,int nth_zone);
unsigned char * f_pos_2_mem_addr(int fd,int f_pos);
extern unsigned char blk_buffer[BUFFER_QUANTITY][BUFFER_SIZE];

//contains every opened file, independent from task_struct
struct file files_table[MAX_SYS_FILE]={{0,},};

int get_empty_file_table()
{
  int n_tmp;
  for (n_tmp=0;n_tmp<MAX_SYS_FILE;n_tmp++)
  {
    if (files_table[n_tmp].f_count==0)
    {
      files_table[n_tmp].f_m_inode=NULL;
      files_table[n_tmp].f_pos=0;
      return n_tmp;
    }
  }
  if (n_tmp==MAX_SYS_FILE)
  {
    printk("error!get_empty_file_table:file table is full!\n");
    return -1;
  }
}

//useage: open("/usr/bin/vi")
//WARNING:file share is not implemented!!!
int open_file(char * dir_file)
{
  short int inode_tmp;
  int serial_num_inodes_table;
  int fd;
  if ((inode_tmp=get_absolute_dir_file(dir_file))==-1)
    return -1;
  if ((serial_num_inodes_table=get_inode(ROOT_DEV_T,inode_tmp))==-1)
    return -1;
  if ((fd=get_empty_file_table())==-1)
  {
    inodes_table[serial_num_inodes_table].i_occupy_num--;
    return -1;
  }
  files_table[fd].f_count=1;
  files_table[fd].f_m_inode=&inodes_table[serial_num_inodes_table];
  files_table[fd].f_pos=0;
  return fd;
}

int close_file(int fd)
{
  if (fd<0 || fd>MAX_SYS_FILE)
    return -1;
  files_table[fd].f_count--;
  if (files_table[fd].f_count<0)
  {
    printk("error!close_file:fd %d less than 0!\n",fd);
    return -1;
  }
  files_table[fd].f_m_inode->i_occupy_num--;
  return 0;
}

//NOTE:here buffer_size must NOT be greater than BUFFER_SIZE
int read_file(int fd,unsigned char * buf,int b_size)
{
  int file_max_size,half1,half2;
  unsigned char * p_start;

  //once opened f_count >0
  if (files_table[fd].f_count==0)
  {
    printk("error!read_file:invalid fd:%d\n",fd);
    return -1;
  }
  file_max_size=files_table[fd].f_m_inode->size_file;
  b_size=b_size<(file_max_size-files_table[fd].f_pos)?b_size:(file_max_size-files_table[fd].f_pos);

  if ((p_start=f_pos_2_mem_addr(fd,files_table[fd].f_pos))==NULL)
    return -1;
  if ((p_start+b_size-&blk_buffer[0][0])/BUFFER_SIZE==(p_start-&blk_buffer[0][0])/BUFFER_SIZE)
  {
    mem_copyb(p_start,buf,b_size);
    buffer_head_table[(p_start-&blk_buffer[0][0])/BUFFER_SIZE].bool_lock=(unsigned char)0;
    files_table[fd].f_pos+=b_size;
    return b_size;
  }
  else
  {
    half1=((p_start-&blk_buffer[0][0])/BUFFER_SIZE+1)*BUFFER_SIZE+&blk_buffer[0][0]-p_start;
    mem_copyb(p_start,buf,half1);
    buffer_head_table[(p_start-&blk_buffer[0][0])/BUFFER_SIZE].bool_lock=(unsigned char)0;
    files_table[fd].f_pos+=half1;
    half2=b_size-half1;
    if ((p_start=f_pos_2_mem_addr(fd,files_table[fd].f_pos))==NULL)
      return -1;
    mem_copyb(p_start,buf+half1,half2);
    buffer_head_table[(p_start-&blk_buffer[0][0])/BUFFER_SIZE].bool_lock=(unsigned char)0;
    files_table[fd].f_pos+=half2;
    return b_size;
  }
}

unsigned char * f_pos_2_mem_addr(int fd,int f_pos)
{
  int i,serial_num_buffer_table;
  i=f_pos/BUFFER_SIZE;
  if (i>6)
  {
    printk("error!f_pos_2_mem_addr:big file unsupported!\n");
    return NULL;
  }
  serial_num_buffer_table=get_buffer_serial_num(files_table[fd].f_m_inode->i_dev_t,files_table[fd].f_m_inode->zone[i]);
  if (serial_num_buffer_table==-1)
    return NULL;
  return (buffer_head_table[serial_num_buffer_table].p_data+f_pos%BUFFER_SIZE);
}
