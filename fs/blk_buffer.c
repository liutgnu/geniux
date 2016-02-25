#include <fs/fs.h>
unsigned char blk_buffer[BUFFER_QUANTITY][BUFFER_SIZE]={{0,},};
struct buffer_head buffer_head_table[BUFFER_QUANTITY]={{0,},};

struct buffer_head_sequence buffer_head_sequence_table[BUFFER_QUANTITY]={{0,},};

struct buffer_head_sequence * p_buffer_using;

extern inline read_ramdisk(int nth_zone,int zone_size,unsigned char * p_dest);
extern inline write_ramdisk(int nth_zone,int zone_size,unsigned char * p_src);
int read_blk_device(unsigned int dev_t,int nth_zone,int zone_size,unsigned char * p_dest);
int write_blk_device(unsigned int dev_t,int nth_zone,int zone_size,unsigned char * p_src);

void buffer_init()
{
  int i=0;
  for (;i<BUFFER_QUANTITY;i++)
  {
    (buffer_head_table[i]).p_data=&(blk_buffer[i][0]);
    buffer_head_table[i].bool_lock=(unsigned char)0;
    buffer_head_table[i].bool_dirt=(unsigned char)0;
    buffer_head_table[i].dev_t=0;
    buffer_head_table[i].nth_zone=0;    
  }
  for (i=0;i<BUFFER_QUANTITY;i++)
  {
    buffer_head_sequence_table[i].num=i;
    buffer_head_sequence_table[i].p_prev=&buffer_head_sequence_table[(i-1+BUFFER_QUANTITY)%BUFFER_QUANTITY];
    buffer_head_sequence_table[i].p_next=&buffer_head_sequence_table[(i+1)%BUFFER_QUANTITY];
  }
  p_buffer_using=(struct buffer_head_sequence *)&buffer_head_sequence_table[0];
}

struct buffer_head_sequence * get_empty_buffer_head()
{
  struct buffer_head_sequence * p_tmp=p_buffer_using->p_prev;
  struct buffer_head_sequence * p_tail=p_buffer_using->p_prev;
  int tmp;
  while (buffer_head_table[p_tmp->num].bool_lock==(unsigned char)1)
  {
    p_tmp=p_tmp->p_prev;
    if (p_tmp==p_tail)
      return NULL;
  }
  if (buffer_head_table[p_tmp->num].bool_dirt==(unsigned char)1)
  {
    buffer_head_table[p_tmp->num].bool_lock=(unsigned char)1;
    tmp=write_blk_device(buffer_head_table[p_tmp->num].dev_t,buffer_head_table[p_tmp->num].nth_zone,BUFFER_SIZE,buffer_head_table[p_tmp->num].p_data);
    if (tmp!=0)
    {
      printk("error:write_blk_device error!dev_t %d,nth_zone %d\n",buffer_head_table[p_tmp->num].dev_t,buffer_head_table[p_tmp->num].nth_zone);
      buffer_head_table[p_tmp->num].bool_lock=(unsigned char)0;
      return NULL;
    }
    else
    {
      buffer_head_table[p_tmp->num].bool_dirt=(unsigned char)0;
      buffer_head_table[p_tmp->num].bool_lock=(unsigned char)0;
    }
  }
  return p_tmp; 
}

int get_buffer_serial_num(unsigned int dev_t,int nth_zone)
{
  int i=0;
  struct buffer_head_sequence * p_tmp=p_buffer_using;
  for (;i<BUFFER_QUANTITY;i++)
  {
    if (buffer_head_table[p_tmp->num].dev_t==dev_t && buffer_head_table[p_tmp->num].nth_zone==nth_zone)
    {
      if (p_tmp!=p_buffer_using)
      {
	/*delete p_tmp node*/
	p_tmp->p_prev->p_next=p_tmp->p_next;
	p_tmp->p_next->p_prev=p_tmp->p_prev;

	/*modify p_tmp to be head*/
	p_tmp->p_prev=p_buffer_using->p_prev;
	p_tmp->p_next=p_buffer_using;

	/*modify p_tmp's neighbour*/
	p_buffer_using->p_prev->p_next=p_tmp;
	p_buffer_using->p_prev=p_tmp;

	/*change head pointer*/
	p_buffer_using=p_tmp;
      }
      return (p_tmp->num);
    }
    else
      p_tmp=p_tmp->p_next;
  }
  if ((p_tmp=get_empty_buffer_head())==NULL)
    return -1;
  if (p_tmp!=p_buffer_using)
  {
    /*delete p_tmp node*/
    p_tmp->p_prev->p_next=p_tmp->p_next;
    p_tmp->p_next->p_prev=p_tmp->p_prev;
    
    /*modify p_tmp to be head*/
    p_tmp->p_prev=p_buffer_using->p_prev;
    p_tmp->p_next=p_buffer_using;
    
    /*modify p_tmp's neighbour*/
    p_buffer_using->p_prev->p_next=p_tmp;
    p_buffer_using->p_prev=p_tmp;
    
    /*change head pointer*/
    p_buffer_using=p_tmp;
  }

  /*need future improve!!! the parameter "BUFFER_SIZE" of function read_blk_device can be updated as N*BUFFER_SIZE for further extension*/
  i=read_blk_device(dev_t,nth_zone,BUFFER_SIZE,buffer_head_table[p_tmp->num].p_data);
  if (i<0)
    return -1;
  else
  {
    buffer_head_table[p_tmp->num].dev_t=dev_t;
    buffer_head_table[p_tmp->num].nth_zone=nth_zone;
    /*lock this new buffer, REMEMBER to UNLOCK it after using!!!*/
    buffer_head_table[p_tmp->num].bool_lock=(unsigned char)1;
    return (p_tmp->num);
  }
}

/********************************************************************************
 *                     device-->>interface-->>buffer                             *
 *******************************************************************************/
/*
 *in compatible with modern Linux, to get a MAJOR(dev_t) by cmdline: cat /proc/devices.
 *for further devices, add I/O control HERE!!!
*/
int read_blk_device(unsigned int dev_t,int nth_zone,int zone_size,unsigned char * p_dest)
{
  switch (MAJOR(dev_t))
  {
  case 1:  //ramdisk
    return read_ramdisk(nth_zone,zone_size,p_dest);
  default:  //unsupported devices
    return -1;
  }
}

int write_blk_device(unsigned int dev_t,int nth_zone,int zone_size,unsigned char * p_src)
{
  switch (MAJOR(dev_t))
  {
  case 1:  //ramdisk
    return write_ramdisk(nth_zone,zone_size,p_src);
  default:  //unsupported devices
    return -1;
  }
}
