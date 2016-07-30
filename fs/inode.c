/*
 *This file deals with inode
*/
#include <fs/fs.h>
#include <kernel/kernel.h>

extern struct m_super super_table[MAX_FS];
extern int get_buffer_serial_num(unsigned int dev_t,int nth_zone);
extern struct buffer_head buffer_head_table[BUFFER_QUANTITY];
extern struct m_inode inodes_table[MAX_INODES];
extern int write_m2d_inode(int serial_num_inodes_table);
extern int get_m_super(unsigned int dev_t);

struct m_inode inodes_table[MAX_INODES]={{0,},};

//write m_inode to disk
int write_m2d_inode(int serial_num_inodes_table)
{
  int serial_num_tmp,i;
  struct d_inode * p_d_inode_tmp;
  if (serial_num_inodes_table>=MAX_INODES)
    return -1;
  if ((i=get_m_super(inodes_table[serial_num_inodes_table].i_dev_t))==-1)
    return -1;
  //lock this inode
  inodes_table[serial_num_inodes_table].i_bool_lock=(unsigned char)1;
  serial_num_tmp=get_buffer_serial_num(inodes_table[serial_num_inodes_table].i_dev_t,2+super_table[i].num_imap_blocks+super_table[i].num_zmap_blocks+(inodes_table[serial_num_inodes_table].i_nth-1)/32);
  if (serial_num_tmp==-1)
    return -1;
  p_d_inode_tmp=(struct d_inode *)(buffer_head_table[serial_num_tmp].p_data)+(inodes_table[serial_num_inodes_table].i_nth-1)%32;

  p_d_inode_tmp->mode=inodes_table[serial_num_inodes_table].mode;
  p_d_inode_tmp->uid=inodes_table[serial_num_inodes_table].uid;
  p_d_inode_tmp->size_file=inodes_table[serial_num_inodes_table].size_file;
  p_d_inode_tmp->sec_mtime=inodes_table[serial_num_inodes_table].sec_mtime;
  p_d_inode_tmp->gid=inodes_table[serial_num_inodes_table].gid;
  p_d_inode_tmp->num_links=inodes_table[serial_num_inodes_table].num_links;
  for (i=0;i<9;i++)
    p_d_inode_tmp->zone[i]=inodes_table[serial_num_inodes_table].zone[i];
  
  buffer_head_table[serial_num_tmp].bool_lock=(unsigned char)0;
  buffer_head_table[serial_num_tmp].bool_dirt=(unsigned char)1;
  inodes_table[serial_num_inodes_table].i_bool_dirt=(unsigned char)0;
  inodes_table[serial_num_inodes_table].i_bool_lock=(unsigned char)0;
  return 0;
}

//returns serial_num in inodes_table
//NOTE:remember to decrease i_occupy_num!!!
int get_inode(unsigned int dev_t,int i_nth)
{
  int n_tmp,serial_num_tmp,i;
  struct d_inode * p_d_inode_tmp;
  for (n_tmp=0;n_tmp<MAX_INODES;n_tmp++)
  {
    if (inodes_table[n_tmp].i_dev_t==dev_t && inodes_table[n_tmp].i_nth==i_nth)
    {
      (inodes_table[n_tmp].i_occupy_num)++;
      return n_tmp;
    }
  }
  //is super_table[X].this_dev_t exist?
  if ((i=get_m_super(dev_t))==-1)
    return -1;
  //get an empty m_inode
  for (n_tmp=0;n_tmp<MAX_INODES;n_tmp++)
  {
    if (inodes_table[n_tmp].i_bool_lock==(unsigned char)0 && inodes_table[n_tmp].i_occupy_num==(unsigned short)0)
      break;
  }
  if (n_tmp==MAX_INODES)
  {
    printk("error!get_inode:inodes_table is full!\n");
    return -1;
  }

  //ok we have an empty m_inode
  inodes_table[n_tmp].i_bool_lock==(unsigned char)1;
  
  //is this empty m_inode clean?
  if (inodes_table[n_tmp].i_bool_dirt==(unsigned char)1)
  {
    if (write_m2d_inode(n_tmp)==-1)
      return -1;
  }
  //ok this m_inode is both clean and empty
  serial_num_tmp=get_buffer_serial_num(dev_t,2+super_table[i].num_imap_blocks+super_table[i].num_zmap_blocks+(i_nth-1)/32);
  if (serial_num_tmp==-1)
    return -1;
  p_d_inode_tmp=(struct d_inode *)(buffer_head_table[serial_num_tmp].p_data)+(i_nth-1)%32;
  inodes_table[n_tmp].mode=p_d_inode_tmp->mode;
  inodes_table[n_tmp].uid=p_d_inode_tmp->uid;
  inodes_table[n_tmp].size_file=p_d_inode_tmp->size_file;
  inodes_table[n_tmp].sec_mtime=p_d_inode_tmp->sec_mtime;
  inodes_table[n_tmp].gid=p_d_inode_tmp->gid;
  inodes_table[n_tmp].num_links=p_d_inode_tmp->num_links;
  for (i=0;i<9;i++)
    inodes_table[n_tmp].zone[i]=p_d_inode_tmp->zone[i];
  /*****************************************************************/
  inodes_table[n_tmp].p_tsk_wait=NULL;
  inodes_table[n_tmp].i_dev_t=dev_t;
  inodes_table[n_tmp].i_nth=i_nth;
  inodes_table[n_tmp].i_occupy_num=(unsigned short)1;
  inodes_table[n_tmp].i_bool_dirt=(unsigned char)0;

  buffer_head_table[serial_num_tmp].bool_lock=(unsigned char)0;
  return n_tmp;
}

//function missing!!! for dirs, except for files, which implemented in file.c
//int del_m_inode(int serial_num_inodes_table)
