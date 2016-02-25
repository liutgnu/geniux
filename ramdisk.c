/*
 *NOTE!!!
 *in boot.s, we defined 100k kernel and 500k data, also we defined
 *ramdisk space in memory.c, so we need to previously mkfs in 500k data, 
 *set a fs of ramdisk size. then copy this "fs" to ramdisk. now we
 *get a normal ramdisk into use. A little tricky!:)
 */
#define RAMDISK 2*1024*1024
#define MAIN_MEM_START 0x100000  //also start of ramdisk
#define F_DATA 500*1024
#define F_DATA_START 0x19000
/*byte_count is 2^n*/
#define mem_copy(src_addr,dst_addr,byte_count)  \
	__asm__ ("mov %%ds,%%bx\n\t"  \
		"mov %%es,%%dx\n\t"  \
		"mov $0x10,%%ax\n\t"  \
		"mov %%ax,%%es\n\t"  \
		"mov %%ax,%%ds\n\t"  \
		"cld\n\t"  \
		"rep\n\t"  \
		"movsl\n\t"  \
		"mov %%dx,%%es\n\t"  \
		"mov %%bx,%%ds\n\t"  \
		::"c" (byte_count/4),"D" (dst_addr),"S" (src_addr))

void init_ramdisk()
{
  mem_copy((unsigned char *)F_DATA_START,(unsigned char *)MAIN_MEM_START,F_DATA);
}

/********************************************************************************
 *                               ramdisk I/O                                    *
 *******************************************************************************/
inline int read_ramdisk(int nth_zone,int zone_size,unsigned char * p_dest)
{
  if (zone_size*(nth_zone+1)>RAMDISK)
    return -1;
  else
  {
    mem_copy((unsigned char *)(MAIN_MEM_START+zone_size*nth_zone),p_dest,zone_size);
    return 0;
  }
}

inline int write_ramdisk(int nth_zone,int zone_size,unsigned char * p_src)
{
  if (zone_size*(nth_zone+1)>RAMDISK)
    return -1;
  else
  {
    mem_copy(p_src,(unsigned char *)(MAIN_MEM_START+zone_size*nth_zone),zone_size);
    return 0;
  }
}

void ramdisk_init()
{
  mem_copy(0x19000,MAIN_MEM_START,512000);  //500k
}
