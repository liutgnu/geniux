/*
 *This file deals with super zone
*/
#include <fs/fs.h>
#include <kernel/kernel.h>
extern int get_buffer_serial_num(unsigned int dev_t,int nth_zone);
extern struct buffer_head buffer_head_table[BUFFER_QUANTITY];

struct m_super super_table[MAX_FS]={{0,},};

//returns serial_num in buffer_head_table
int get_d_super(unsigned int dev_t)
{
  int n_tmp;
  if ((n_tmp=get_buffer_serial_num(dev_t,1))==-1)
    return -1;
  return n_tmp;
}

//returns serial_num in super_table
int get_m_super(unsigned int dev_t)
{
  int n_tmp;
  for (n_tmp=0;n_tmp<MAX_FS;n_tmp++)
  {
    if (super_table[n_tmp].s_dev_t==dev_t)
      return n_tmp;
  }
  printk("error!get_m_super:invalid dev_t %d\n",dev_t);
  return -1;
}

//return serial_num in super_table
int read_d2m_super(unsigned int dev_t)
{
  int n_tmp,serial_num_tmp;
  struct d_super * p_d_super_tmp;
  for (n_tmp=0;n_tmp<MAX_FS;n_tmp++)
  {
    if (super_table[n_tmp].magic==(unsigned short)0)
    {
      if ((serial_num_tmp=get_d_super(dev_t))==-1)
	return -1;
      p_d_super_tmp=(struct d_super *)(buffer_head_table[serial_num_tmp].p_data);
      super_table[n_tmp].num_inodes=p_d_super_tmp->num_inodes;
      super_table[n_tmp].num_zones=p_d_super_tmp->num_zones;
      super_table[n_tmp].num_imap_blocks=p_d_super_tmp->num_imap_blocks;
      super_table[n_tmp].num_zmap_blocks=p_d_super_tmp->num_zmap_blocks;
      super_table[n_tmp].nth_zone_firstdatazone=p_d_super_tmp->nth_zone_firstdatazone;
      super_table[n_tmp].log_zone_size=p_d_super_tmp->log_zone_size;
      super_table[n_tmp].maxsize_file=p_d_super_tmp->maxsize_file;
      super_table[n_tmp].magic=p_d_super_tmp->magic;
      //temporarily filled out 
      super_table[n_tmp].s_dev_t=dev_t;
      super_table[n_tmp].p_s_root=NULL;
      super_table[n_tmp].p_sr_mount_on=NULL;
      /*************************************************************/
      buffer_head_table[serial_num_tmp].bool_lock=(unsigned char)0;
      return n_tmp;
    }
  }
  printk("error!read_d2m_super:super_table is full!\n");
  return -1;
}

//return -1(fail), 0(success)
int write_m2d_super(unsigned int dev_t)
{
  int n_tmp,serial_num_tmp;
  struct d_super * p_d_super_tmp;
  for (n_tmp=0;n_tmp<MAX_FS;n_tmp++)
  {
    if (super_table[n_tmp].s_dev_t==dev_t)
    {
      if ((serial_num_tmp=get_d_super(dev_t))==-1)
	return -1;
      p_d_super_tmp=(struct d_super *)(buffer_head_table[serial_num_tmp].p_data);
      p_d_super_tmp->num_inodes=super_table[n_tmp].num_inodes;
      p_d_super_tmp->num_zones=super_table[n_tmp].num_zones;
      p_d_super_tmp->num_imap_blocks=super_table[n_tmp].num_imap_blocks;
      p_d_super_tmp->num_zmap_blocks=super_table[n_tmp].num_zmap_blocks;
      p_d_super_tmp->nth_zone_firstdatazone=super_table[n_tmp].nth_zone_firstdatazone;
      p_d_super_tmp->log_zone_size=super_table[n_tmp].log_zone_size;
      p_d_super_tmp->maxsize_file=super_table[n_tmp].maxsize_file;
      p_d_super_tmp->magic=super_table[n_tmp].magic;
      /*************************************************************/
      buffer_head_table[serial_num_tmp].bool_dirt=(unsigned char)1;
      buffer_head_table[serial_num_tmp].bool_lock=(unsigned char)0;
      return 0;
    }
  }
  printk("error!write_m2d_super:invalid dev_t %d\n",dev_t);
  return -1;
}

/********************************************************************
 *                   interface for higher functions                 *
 *******************************************************************/
//NEW:read super from disk to memory:
//int read_d2m_super(dev_t);

//DELETE:remove super from memory:
int del_m_super(unsigned int dev_t)
{
  int n_tmp;
  if ((n_tmp=get_m_super(dev_t))==-1)
    return -1;
  super_table[n_tmp].magic=(unsigned short)0;
  return 0;
}

//CREAT:mkfs on m_super and write m_super to disk:
//int write_m2d_super(dev_t);
