/*
*this file deals with inode directory and file name, sounds fun!
*/
#include "fs.h"
#include "kernel.h"

extern inline int get_m_super(unsigned int dev_t);
extern int get_buffer_serial_num(unsigned int dev_t,int nth_zone);
extern struct buffer_head buffer_head_table[BUFFER_QUANTITY];
extern struct m_super super_table[MAX_FS];
extern int get_inode(unsigned int dev_t,int i_nth);
extern struct m_inode inodes_table[MAX_INODES];

/**************************************************************************
 *                   function on directoy:inode and f_name                *
 *************************************************************************/
//we need a function on string here, same return 0, different return -1
int cmp_string(char * str_src, char * str_dest)
{
  int n_tmp=0;
  while (n_tmp<LENGTH_FILENAME)
  {
    if (*str_src=='\0' || *str_dest=='\0')
      break;
    if (*str_src++ != *str_dest++)
      return -1;
    n_tmp++;
  }
  if (n_tmp==LENGTH_FILENAME)
    return 0;
  else if (*str_src == *str_dest)
    return 0;
  else
    return -1;
}

//here left a premise: a directory can only contain zone_size/sizeof(struct dir) items at most!!!
short int fname_2_inode(char * p_fname, struct directory * p_dir_start)
{
  int n_tmp;
  struct directory * p_dir_tmp;
  for (n_tmp=0;n_tmp<(ZONE_SIZE/16);n_tmp++)  //zone_size/sizeof(struct directory)
  {
    p_dir_tmp=p_dir_start+n_tmp;
    if ((cmp_string(p_fname,p_dir_tmp->f_name))==0)
      return (p_dir_tmp->inode);
  }
  return -1;
}

//here left a premise: a directory can only contain zone_size/sizeof(struct dir) items at most!!!
char * inode_2_fname(short int inode, struct directory * p_dir_start)
{
  int n_tmp;
  struct directory * p_dir_tmp;
  for (n_tmp=0;n_tmp<ZONE_SIZE/16;n_tmp++)
  {
    p_dir_tmp=p_dir_start+n_tmp;
    if ((p_dir_tmp->inode)==inode)
      return (p_dir_tmp->f_name);
  }
  return NULL;
}

/*************************************************************************/

short int inode_prev2next(unsigned int dev_t,short int inode_prev,char * next_name)
{
  int serial_num_buffer_table1,serial_num_buffer_table2,serial_num_super_table,i,j;
  struct d_inode * p_d_inode;
  struct directory * p_dir;
  if ((serial_num_super_table=get_m_super(dev_t))==-1)
    return -1;
  serial_num_buffer_table1=get_buffer_serial_num(dev_t,2+super_table[serial_num_super_table].num_imap_blocks+super_table[serial_num_super_table].num_zmap_blocks+(inode_prev-1)/32);
  if (serial_num_buffer_table1==-1)
    return -1;
  p_d_inode=(struct d_inode *)(buffer_head_table[serial_num_buffer_table1].p_data)+(inode_prev-1)%32;
  for (i=0;i<7;i++)  //need future improve: we expect dir's are small enough
  {
    if ((p_d_inode->zone[i])==0)
      continue;
    serial_num_buffer_table2=get_buffer_serial_num(dev_t,p_d_inode->zone[i]);
    if (serial_num_buffer_table2==-1)
      return -1; 
    p_dir=(struct directory *)(buffer_head_table[serial_num_buffer_table2].p_data);
    for (j=0;j<64;j++)
    {
      if (cmp_string((p_dir+j)->f_name,next_name)==0)
      {
	buffer_head_table[serial_num_buffer_table2].bool_lock=(unsigned char)0;
        buffer_head_table[serial_num_buffer_table1].bool_lock=(unsigned char)0;
	return ((p_dir+j)->inode);
      }
    }
  }
  buffer_head_table[serial_num_buffer_table1].bool_lock=(unsigned char)0;
  return -1;
}

//need furture improve:we expect all directories within a same dev_t
//acrossing dev_t access is not supported!!! 
//useage:get_inode(dev_t,get_absolute_dir_file("/usr/bin/vi"));
short int get_absolute_dir_file(char * p_absolute_dir)
{
  char name[15]={'\0',};
  char * p_name;
  int dir_length=0,i,j,n;
  unsigned int dev_t;
  short int inode_tmp;

  if (*p_absolute_dir!='/')
    return -1;
  while (*(p_absolute_dir+dir_length++)!='\0');
  i=1;

  if ((n=get_inode(ROOT_DEV_T,1))==-1)
    return -1;
  inode_tmp=inodes_table[n].i_nth;
  dev_t=inodes_table[n].i_dev_t;

  while (i<dir_length)
  { 
    j=0;
    p_name=name;
    while (*(p_absolute_dir+i)!='/' && *(p_absolute_dir+i)!='\0')
    {
      *(p_name+j++)=*(p_absolute_dir+i++);
    }
    *(p_name+j)='\0';
    i++;
    inode_tmp=inode_prev2next(dev_t,inode_tmp,name);
  }
  inodes_table[n].i_occupy_num--;
  return inode_tmp;
}
