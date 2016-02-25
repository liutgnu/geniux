#include <kernel/stdarg.h>
//a long string should be less than 500, end with '\0'
#define CONSOLE_BUF_LENGTH 500
//display size:80*25
#define DISPLAY_SIZE 2000
char console_buf[CONSOLE_BUF_LENGTH];
char * console_buf_head=console_buf;
char * console_buf_tail=console_buf;
int console_buf_full_carry=0;
int console_buf_crisis=0;
//since the first 3 lines used by user_pnt_time and user_gus_num, printk takes over the 4th line and below
int cursor_x=0;
int cursor_y=13;
extern int sys_print();

/*
 *the following 2 functions only process on console_buf, directly insert or delete without check in/out chars.
 *in function number(),we'll deal with formatting number.
 *in function printk(),formatting string.
*/
unsigned char console_buf_insert(char char_in)
{
  if (console_buf_full_carry==1 && console_buf_head == console_buf_tail)
  {
    return 0xfe;
  }

  if (console_buf_crisis==1)
  {
    return 0xff;
  }

  console_buf_crisis=1;
  * console_buf_tail=char_in;
  console_buf_tail++;
  if (console_buf_tail==console_buf+CONSOLE_BUF_LENGTH)
  {
    console_buf_tail=console_buf_tail-CONSOLE_BUF_LENGTH;
    console_buf_full_carry=1;
  }
  console_buf_crisis=0;
  return 0;
}

unsigned console_buf_del()
{
  char char_out;
  if (console_buf_full_carry==0 && console_buf_head==console_buf_tail)
  {
    return 0xfe;
  }

  if (console_buf_crisis==1)
  {
    return 0xff;
  }

  console_buf_crisis=1;
  char_out=*console_buf_head++;
  if (console_buf_head==console_buf+CONSOLE_BUF_LENGTH)
  {
    console_buf_head=console_buf_head-CONSOLE_BUF_LENGTH;
    console_buf_full_carry=0;    
  }  
  console_buf_crisis=0;
  return char_out;
} 

/*
 *put formatted number into console_buf, no return value.
*/
void number(int num,int base)
{
  char tmp[20];
  char reverse_tmp[20];
  char char_tmp;
  int i=0;
  int j=0;

  if (num==0)
    tmp[i++]='0';
  else
  {
    while (num!=0)
    {
      char_tmp=num%base+48;
      if (char_tmp >=58)
        tmp[i++]=char_tmp+7;
      else
        tmp[i++]=char_tmp;
      num=num/base;
    }
  }
  i--;
  while (i>=0)
  {
    reverse_tmp[j++]=tmp[i--];
  }
  reverse_tmp[j]='\0';
  i=0;
  while (reverse_tmp[i]!='\0')
  {
    console_buf_insert(reverse_tmp[i++]);
  }
}

int printk(char *fmt, ...)
{
  char * ap;
  char * tmp_fmt;
  char * str;
  char char_out;
  int out_cont=0;
  int position;

  va_start(ap,fmt);
  for (tmp_fmt=fmt;*tmp_fmt!='\0';tmp_fmt++)
  {
    if (*tmp_fmt!='%')
    {
      console_buf_insert(*tmp_fmt);
      continue;
    }
    switch (*++tmp_fmt)
    {
    case 'd':
      number(va_arg(ap,int),10);
      break;
    case 'x':
      number(va_arg(ap,int),16);
      break;
    case 'o':
      number(va_arg(ap,int),8);
      break;
    case 'b':
      number(va_arg(ap,int),2);
      break;
    case 's':
      str=va_arg(ap,char *);
      while (* str!='\0')
      {
	console_buf_insert(*str++);
      }
      break;
    default:
      console_buf_insert(*--tmp_fmt);
      break;
    }
  }
  console_buf_insert('\0');

  //process finished, now output
  while ((char_out=console_buf_del())!='\0' && ((unsigned char)char_out!=0xff || (unsigned char)char_out!=0xfe))
  {
    //clean a new line
    if (cursor_x==0)
    {
      position=cursor_y*80*2;
      __asm__ volatile ("push %%es\n\t"
			"movl %%eax,%%edi\n\t"
		        "movl $0x18,%%eax\n\t"
			"mov  %%ax,%%es\n\t"
		        "movl $80,%%ecx\n\t"
			"cld\n\t"
		        "movl $0x0720,%%eax\n\t"  //light gray, space
			"rep\n\t"
		        "stosw\n\t"
			"pop %%es"
		        ::"a" (position));
    }
    
    if (char_out=='\n')
    {
      cursor_x=0;
      cursor_y++;
    }
    else
      sys_print(char_out,cursor_y*80+(cursor_x++));

    if (cursor_x==80)
    {
      cursor_x=0;
      cursor_y++;
    }
    if (cursor_y==25)
      cursor_y=13;
    out_cont++;
  }
  return out_cont;   //number of chars output
}

void clear_screen(){
  int i;
  for (i=0;i<2000;i++){
    sys_print((char)0,i);
  }
}
