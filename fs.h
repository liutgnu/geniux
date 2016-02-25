/*
 *I use MINIX filesystem version 1 as geniux's "startup" filesystem. I have to say minix fs v1 is not a good fs, but it's simple enough to learn. Most of all, it's the only one with full references at my hand.
 *
 *To make a minix v1 fs:
 *mkfs.minix -n 14 -1 /dev/fd0  #short file name
*/
unsigned int dev_t;  //32 bit
#define MAJOR(dev_t) (dev_t>>20)
#define MINOR(dev_t) (dev_t & 0xfffff)
#define MAKDEV(major,minor) ((major<<20) | minor)
#define LENGTH_FILENAME 14
#ifndef NULL
#define NULL ((void *)0)
#endif

//blk_buffer.c
#define BUFFER_SIZE 1024  //should be same as zone_size
#define BUFFER_QUANTITY 20  //number of buffers
struct buffer_head{
  void * p_data;
  unsigned char bool_lock;  //0-unlocked, 1-locked
  unsigned char bool_dirt;  //0-clean, 1-dirt
  unsigned int dev_t;
  unsigned int nth_zone;
};
struct buffer_head_sequence{
  int num;
  struct buffer_head_sequence * p_prev;
  struct buffer_head_sequence * p_next;
};

//super.c
#define MAX_FS 10  //max quantity of fs in memory

//inode.c
#define MAX_INODES 20  //max quantity of inode(root dir,dir,file) in memory

//iname.c
#define ZONE_SIZE 1024
#define LENGTH_FILENAME 14

//file.c
#define MAX_SYS_FILE 64

#define ROOT_DEV_T 0x100001

struct d_super{
  unsigned short num_inodes;
  unsigned short num_zones;
  unsigned short num_imap_blocks;
  unsigned short num_zmap_blocks;
  unsigned short nth_zone_firstdatazone;
  unsigned short log_zone_size;
  unsigned long  maxsize_file;
  unsigned short magic;
};

struct m_super{
  unsigned short num_inodes;
  unsigned short num_zones;
  unsigned short num_imap_blocks;
  unsigned short num_zmap_blocks;
  unsigned short nth_zone_firstdatazone;
  unsigned short log_zone_size;
  unsigned long  maxsize_file;
  unsigned short magic;
  /************************************/
  /*
  struct bh * p_imap[8];  //max zone quatity of inode map is 8
  struct bh * p_zmap[8];  //max zone quatity of zone map is 8
  */
  unsigned int s_dev_t;
  struct m_inode * p_s_root;
  struct m_inode * p_sr_mount_on;
};

struct d_inode{
  unsigned short mode;
  unsigned short uid;
  unsigned long  size_file;
  unsigned long  sec_mtime;
  unsigned char  gid;
  unsigned char  num_links;
  unsigned short zone[9];
};

struct m_inode{
  unsigned short mode;
  unsigned short uid;
  unsigned long  size_file;
  unsigned long  sec_mtime;
  unsigned char  gid;
  unsigned char  num_links;
  unsigned short zone[9];
  /***************************/
  struct task_struct * p_tsk_wait;
  unsigned int i_dev_t;  //locate device
  unsigned short i_nth;
  unsigned short i_occupy_num;
  unsigned char i_bool_lock;
  unsigned char i_bool_dirt;
};

struct directory{
  unsigned short inode;
  char f_name[LENGTH_FILENAME];
};

struct file{
  int f_count;
  struct m_inode * f_m_inode;
  int f_pos;
};
