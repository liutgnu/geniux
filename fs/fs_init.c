#include <fs/fs.h>
#include <kernel/kernel.h>
extern int read_d2m_super(unsigned int dev_t);

void fs_init()
{
  int tmp;
  if ((tmp=read_d2m_super(ROOT_DEV_T))==-1)
  {  
    printk("error!fs_init failed!\n");
    return;
  }
}
