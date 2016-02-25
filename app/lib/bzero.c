//clear size bytes from address
void bzero(void * address,int size){
  int i;
  char * start=(char *)address;
  for (i=0;i<size;i++)
    *(start+i)=0;
}
