/*
 *This file deals with files
 *NOTE! since our file system is very weak, i try to make fs as small as possible. some features of minix fs are not implemented!!!
*/
#include <fs/fs.h>
#include <kernel/kernel.h>
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

int size_file(int fd)
{
  if (fd<0 || fd>MAX_SYS_FILE)
    return -1;
  return((int)files_table[fd].f_m_inode->size_file);
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
  int i,serial_num_buffer_table,serial_num_buffer_table1,serial_num_buffer_table2,serial_num_buffer_table3;
  i=f_pos/BUFFER_SIZE;
  if (i<7)  //0th inode
  {
    serial_num_buffer_table=get_buffer_serial_num(files_table[fd].f_m_inode->i_dev_t,files_table[fd].f_m_inode->zone[i]);
    if (serial_num_buffer_table==-1)
      return NULL;
    return (buffer_head_table[serial_num_buffer_table].p_data+f_pos%BUFFER_SIZE);
  }
  else if (i<519)  //1th inode
  {
    serial_num_buffer_table1=get_buffer_serial_num(files_table[fd].f_m_inode->i_dev_t,files_table[fd].f_m_inode->zone[7]);
    if (serial_num_buffer_table1==-1)
      return NULL;
    serial_num_buffer_table2=get_buffer_serial_num(files_table[fd].f_m_inode->i_dev_t,*((unsigned short *)(buffer_head_table[serial_num_buffer_table1].p_data)+i-7));
    buffer_head_table[serial_num_buffer_table1].bool_lock=(unsigned char)0;
    if (serial_num_buffer_table2==-1)
      return NULL;   
    return (buffer_head_table[serial_num_buffer_table2].p_data+f_pos%BUFFER_SIZE);
  }
  else if (i<262663)
  {
    serial_num_buffer_table1=get_buffer_serial_num(files_table[fd].f_m_inode->i_dev_t,files_table[fd].f_m_inode->zone[8]);
    if (serial_num_buffer_table1==-1)
      return NULL;    
    serial_num_buffer_table2=get_buffer_serial_num(files_table[fd].f_m_inode->i_dev_t,*((unsigned short *)(buffer_head_table[serial_num_buffer_table1].p_data)+(i-7)/512));
    buffer_head_table[serial_num_buffer_table1].bool_lock=(unsigned char)0;
    if (serial_num_buffer_table2==-1)
      return NULL;   
    serial_num_buffer_table3=get_buffer_serial_num(files_table[fd].f_m_inode->i_dev_t,*((unsigned short *)(buffer_head_table[serial_num_buffer_table2].p_data)+(i-7)%512));
    buffer_head_table[serial_num_buffer_table2].bool_lock=(unsigned char)0;
    if (serial_num_buffer_table3==-1)
      return NULL;
    return (buffer_head_table[serial_num_buffer_table3].p_data+f_pos%BUFFER_SIZE);
  }
  else
  {
    printk("error!f_pos_2_mem_addr:f_pos %d is too big!\n",f_pos);
    return NULL;
  }
}

//remember to free this 4k phy_page!!!
unsigned char * read_file_pos_1k(int fd,int f_pos)
{
  unsigned char * page_tmp;
  unsigned char * file_tmp;
  int count;
  if ((page_tmp=phy_get_free_page())==NULL)
    return NULL;
  if ((file_tmp=f_pos_2_mem_addr(fd,f_pos))==NULL)
  {
    printk("error!read_file_pos_1k:read file part1 failed!\n");
    return NULL;
  }
  if ((unsigned long)file_tmp%1024==0)
  {
    mem_copyb(file_tmp,page_tmp,1024);
    buffer_head_table[((unsigned long)file_tmp-(unsigned long)blk_buffer)/1024].bool_lock=(unsigned char)0;
    return page_tmp;
  }
  else
  {
    count=(unsigned long)file_tmp%1024;
    mem_copyb(file_tmp,page_tmp,1024-count);
    buffer_head_table[((unsigned long)file_tmp-(unsigned long)blk_buffer)/1024].bool_lock=(unsigned char)0;
    file_tmp=f_pos_2_mem_addr(fd,f_pos+1024-count);
    if (file_tmp==NULL)
    {
      printk("error!read_file_pos_1k:read file part2 failed!\n");
      phy_free_page(page_tmp);
      return NULL;
    }
    mem_copyb(file_tmp,page_tmp+1024-count,count);
    buffer_head_table[((unsigned long)file_tmp-(unsigned long)blk_buffer)/1024].bool_lock=(unsigned char)0;
    return page_tmp;
  }
}
