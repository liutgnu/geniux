/*
 *This file deals with inode map
*/
#include <fs/fs.h>
#include <kernel/kernel.h>

extern int get_buffer_serial_num(unsigned int dev_t,int nth_zone);
extern struct buffer_head buffer_head_table[BUFFER_QUANTITY];
extern int get_m_super(unsigned int dev_t);
extern struct m_super super_table[MAX_FS];

//returns nth inode's imap status(0 or 1)
int read_imap_status(unsigned int dev_t,int i_nth)
{
  int serial_num_tmp,status;
  unsigned char * tmp;
  serial_num_tmp=get_buffer_serial_num(dev_t,2+(i_nth-1)/8192);
  i_nth=(i_nth-1)%8192;
  if (serial_num_tmp==-1)
    return -1;
  tmp=(unsigned char *)(buffer_head_table[serial_num_tmp].p_data)+i_nth/8;
  i_nth=i_nth%8;
  status=(int)(((*tmp)>>i_nth)&1);
  buffer_head_table[serial_num_tmp].bool_lock=(unsigned char)0;
  return status;
}

int write_imap_status(unsigned int dev_t,unsigned char bool_status,int i_nth)
{
  int serial_num_tmp;
  unsigned char * tmp;
  serial_num_tmp=get_buffer_serial_num(dev_t,2+(i_nth-1)/8192);
  i_nth=(i_nth-1)%8192;
  if (serial_num_tmp==-1)
    return -1;
  tmp=(unsigned char *)(buffer_head_table[serial_num_tmp].p_data)+i_nth/8;
  i_nth=i_nth%8;
  if (bool_status=(bool_status & 1))  //no problem here
    (*tmp)=(*tmp)|(bool_status<<i_nth);
  else
    (*tmp)=(*tmp)&((unsigned char)(~(1<<i_nth)));
  buffer_head_table[serial_num_tmp].bool_dirt=(unsigned char)1;
  buffer_head_table[serial_num_tmp].bool_lock=(unsigned char)0;
  return 0;
}

int get_free_imap(unsigned int dev_t)
{
  int serial_num_tmp,n_tmp,i,j,k;
  unsigned char * tmp;
  if ((n_tmp=get_m_super(dev_t))==-1)
    return -1;
  n_tmp=super_table[n_tmp].num_imap_blocks;
  for (i=1;i<=n_tmp;i++)
  {
    serial_num_tmp=get_buffer_serial_num(dev_t,1+i);
    if (serial_num_tmp==-1)
      return -1;
    tmp=(unsigned char *)(buffer_head_table[serial_num_tmp].p_data);
    for (j=0;j<1024;j++)
    {
      if ((*(tmp+j))!=0xff)
      {
	for (k=0;(((*(tmp+j))>>k) & 1)==1;k++);  //void loop
	buffer_head_table[serial_num_tmp].bool_lock=(unsigned char)0;
	return ((i-1)*8192+j*8+k);
      }      
    }
    buffer_head_table[serial_num_tmp].bool_lock=(unsigned char)0;
  }
  printk("error!get_free_imap:imap is full!\n");
  return -1;
}

/****************************************************************************
 *                        interface for higher function                     *
 ***************************************************************************/
//NEW:find an empty imap bit
//int get_free_imap(unsigned int dev_t)

//READ:get the status of a certain imap bit
//int read_imap_status(unsigned int dev_t,int i_nth)

//WRITE:write the status to a certain imap bit
//int write_imap_status(unsigned int dev_t,unsigned char bool_status,int i_nth
