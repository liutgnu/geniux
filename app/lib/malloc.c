#define BLOCK_SIZE 128
#define BLOCK_MAP_SIZE 64  /*64 ints*/
#define MAX_BLOCK_NUM BLOCK_MAP_SIZE*32  /*so the maximum malloc memory size is 64*8*128=64K*/
#define HEAP_START 0x9000000  /*ok,ok it's ugly*/
#ifndef NULL
#define NULL 0
#endif
extern int printf();

struct zone_struct{
  void * start_address;
  int length;  //how many blocks this zone contains?
};

unsigned int block_map[BLOCK_MAP_SIZE]={0,};
struct zone_struct zone[MAX_BLOCK_NUM]={{0,},};

int get_block_map_bit_status(int offset){  //start from 0
  int x=offset/32;
  int y=offset%32;
  return((block_map[x]>>y)&1);
}

void set_block_map_bit_status(int offset,int status){  //start from 0
  int x=offset/32;
  int y=offset%32;
  int tmp=1;
  if (status==0){
    tmp=~(tmp<<y);
    block_map[x]&=tmp;
  }
  else if (status==1){
    tmp=(tmp<<y);
    block_map[x]|=tmp;
  }
  else{
    printf("status error!status:%d\n",status); 
  }
}

int get_free_blocks(int size){  //size in bytes
  if (size<=0){
    printf("size illegal!size:%d\n",size);
    return -1;
  }
  int expected_block_num=(size-1)/BLOCK_SIZE+1;
  int offset=0,tmp=0;
  while (offset<MAX_BLOCK_NUM){
    if (get_block_map_bit_status(offset)==0){
      for (tmp=0;tmp<expected_block_num;tmp++){
	if (get_block_map_bit_status(offset+tmp)==1)
	  break;
      }
      if (tmp==expected_block_num)
	break;
      else
	offset+=tmp;
    }else
      offset++;
  }
  if (offset>=MAX_BLOCK_NUM){
    printf("not enough memory!\n");
    return -1;
  }
  
  //now mark this zone
  for (tmp=0;tmp<expected_block_num;tmp++)
    set_block_map_bit_status(offset+tmp,1);
  return offset;
}

void * malloc(int size){
  int offset,tmp;
  if ((offset=get_free_blocks(size))<0)
    return NULL;
  
  for(tmp=0;tmp<MAX_BLOCK_NUM;tmp++){
    if (zone[tmp].start_address==NULL)
      break;
  }
  if (tmp==MAX_BLOCK_NUM){  //it's never gonna happen. just in case for debug use
    printf("zone full!\n");
    return NULL;
  }
  zone[tmp].start_address=HEAP_START+offset*BLOCK_SIZE;
  zone[tmp].length=(size-1)/BLOCK_SIZE+1;
  bzero(zone[tmp].start_address,(zone[tmp].length)*BLOCK_SIZE);
  return zone[tmp].start_address;
}

void free(void * address){
  int tmp,i;
  for (tmp=0;tmp<MAX_BLOCK_NUM;tmp++){
    if (zone[tmp].start_address==address)
      break;
  }
  if (tmp==MAX_BLOCK_NUM){
    printf("address %x is not malloced!\n",address);
    return;
  }
  
  //unmark this zone
  for (i=0;i<zone[tmp].length;i++){
    set_block_map_bit_status(((int)address-HEAP_START)/BLOCK_SIZE+i,0);
  }
  zone[tmp].start_address=NULL;
  zone[tmp].length=0;
}
