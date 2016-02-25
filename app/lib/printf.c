#include<stdarg.h>
#define NUM_BUF 100
int cursor_x=0;
int cursor_y=0;
extern print_XY();
char num_buf[NUM_BUF];

void number(int value,int base){
  char * num_buf_end=num_buf;
  char reverse_buf[NUM_BUF];
  char * reverse_buf_end=reverse_buf;
  int tmp;
  unsigned int inner;

  if (value==0)
    *(reverse_buf_end++)='0';
  inner=(unsigned int)value;
  while (inner!=0){
    tmp=inner%base;
    if (tmp<10)
      *(reverse_buf_end++)='0'+(char)tmp;
    else
      *(reverse_buf_end++)='A'+(char)(tmp-10);
    inner=inner/base;
  }
  if (base==10 && value<0)
    *(reverse_buf_end++)='-';

  reverse_buf_end--;
  while(reverse_buf_end>=reverse_buf){
    *(num_buf_end++)=*(reverse_buf_end--);
  }
  *(num_buf_end)='\0';

  for(tmp=0;num_buf[tmp]!='\0';tmp++){
    print_XY(num_buf[tmp],cursor_x++,cursor_y);
    if (cursor_x==80){
      cursor_x=0;
      cursor_y++;
    }
    if(cursor_y==13)
      cursor_y=0;
  }
}

void printf(char * fmt,...){
  char * ap;
  char * tmp_fmt;
  char * str;

  va_start(ap,fmt);
  for (tmp_fmt=fmt;*tmp_fmt!='\0';tmp_fmt++){
    if (*tmp_fmt != '%'){
      switch (*tmp_fmt){
      case '\n':
	cursor_x=0;
	cursor_y++;
	if (cursor_y==13)
	  cursor_y=0;
	break;
      default:
	print_XY(*tmp_fmt,cursor_x++,cursor_y);
	if (cursor_x==80){
	  cursor_y++;
	  cursor_x=0;
	}
	if (cursor_y==13)
	  cursor_y=0;
      }
    }else{
      switch (*(++tmp_fmt)){
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
	while(*str!='\0'){
	  print_XY(*(str++),cursor_x++,cursor_y);
	  if (cursor_x==80){
	    cursor_y++;
	    cursor_x=0;
	  }
	  if (cursor_y==13)
	    cursor_y=0;
	}
	break;
      default:
	print_XY(*(--tmp_fmt),cursor_x++,cursor_y);
	if (cursor_x==80){
	  cursor_y++;
	  cursor_x=0;
	}
	if (cursor_y==13)
	  cursor_y=0;
      }
    }
  }
}
