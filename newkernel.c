typedef unsigned char Bit8u;
typedef unsigned short Bit16u;

unsigned char mem[10];
void main()
{
  void print_num(num);
  void print_char(a);
  void input(address);
  void get_random_num();
  
  unsigned char inp[10];
  int loop_num; 
  int i,j;
  int count_a,count_b;
  
  get_random_num(mem); //mem[6],mem[4],mem[2],mem[0]
  for (loop_num=0;loop_num<15;loop_num++)
  {
    count_a=0;
    count_b=0;
    print_char('G');print_char('O');print_char('|');
    input(inp); //inp[6],inp[4],inp[2],inp[0]

    //check A
    for (i=0;i<=6;i=i+2)
    {if (mem[i]==inp[i]) count_a++;}

    //check B
    for (i=0;i<=6;i=i+2)
    {
      for (j=0;j<=6;j=j+2)
      {if (inp[j]==mem[i]) count_b++;}
    }
    count_b=count_b-count_a;
    print_num(count_a);print_char('A');print_num(count_b);print_char('B');print_char('|');

    if (count_a==4 && count_b==0) 
    {
      print_char('O');print_char('K');print_char('|');
      print_num(mem[6]);print_num(mem[4]);print_num(mem[2]);print_num(mem[0]);
    }
    else continue;
  }

  if (loop_num >14) 
  {
    print_char('N');print_char('O');print_char('|');
    print_num(mem[6]);print_num(mem[4]);print_num(mem[2]);print_num(mem[0]);
  }
  for(;;);  //program ends here
}


void print_char(a)
Bit8u a;
{
#asm
push bp
mov  bp,sp
  push ax
  push bx
  push cx
  push dx

  mov  al,4[bp] ;; word
  mov  ah,#0x0a
  mov  bx,#0
  mov  cx,#1
  int  #0x10
  
  mov  ah,#0x03 ;; X-Y
  mov  bx,#0
  int  #0x10

  mov  ax,#0x0200 ;; move to next row
  add  dx,#1
  int  #0x10
  
  pop  dx
  pop  cx
  pop  bx
  pop  ax
pop bp
#endasm
}

void print_num(num)
int num;
{
#asm
push bp
mov  bp,sp
  push ax
  push bx
  push cx
  push dx

  mov  al,4[bp] ;; word
  add  al,#48
  mov  ah,#0x0a
  mov  bx,#0
  mov  cx,#1
  int  #0x10
  
  mov  ah,#0x03 ;; X-Y
  mov  bx,#0
  int  #0x10

  mov  ax,#0x0200 ;; move to next row
  add  dx,#1
  int  #0x10
  
  pop  dx
  pop  cx
  pop  bx
  pop  ax
pop bp
#endasm
}

void input(address) // a[10] a[6],a[4],a[2],a[0]:high->low
Bit16u address;
{
#asm
push bp
mov  bp,sp

  push ax
  push bx
  push cx

  mov cx,#4
iput:
  in  al,#0x60
  cmp al,#0x82
  jb  iput
  cmp al,#0x8b
  ja  iput
  sub al,#0x81
  mov ah,#0
  mov bl,#10
  div bl
  mov al,ah ;;true value

  push ax

;;reset keyboard

  mov  bx,#0xffff
test1:
  in   al,#0x64
  and  al,#1
  cmp  al,#0
  jne  ok
  sub  bx,#1
  cmp  bx,#0
  ja   not_ok
  je   ok
not_ok:
  mov  al,#0x00
  out  #0x80,al
  jmp  test1
;;ok

ok: dec cx
    cmp cx,#0
    jne  iput

  mov  bx,4[bp]

  pop  ax
  mov  [bx],ax
  pop  ax
  mov  2[bx],ax
  pop  ax
  mov  4[bx],ax
  pop  ax
  mov  6[bx],ax

  pop cx
  pop bx
  pop ax
pop bp
#endasm
}


void get_time(address)
Bit16u address;
{
#asm
push bp
mov  bp,sp

  push ax
  push bx
  
  mov  bx,4[bp]

  mov  ah,#2
  int  #0x1a

  mov  [bx],dx

  pop bx
  pop ax
pop bp
#endasm
}

unsigned char left(num)
unsigned char num;
{
#asm
push bp
mov  bp,sp
  mov al,4[bp]
  mov ah,#0
  mov bl,#10
  div bl
  mov al,ah
  mov ah,#0
pop bp

#endasm
}


void get_random_num() //b[10] b[6],b[4],b[2],b[0]
{
  unsigned char a[5];
  int i;
  get_time(a);mem[0]=left(a[1]*a[1]*a[1]);
  do {get_time(a);mem[2]=left(a[1]*a[1]*a[1]);} while(mem[2]==mem[0]);
  do {get_time(a);mem[4]=left(a[1]*a[1]*a[1]);} while(mem[4]==mem[0] || mem[4]==mem[2]);
  do {get_time(a);mem[6]=left(a[1]*a[1]*a[1]);} while(mem[6]==mem[0] || mem[6]==mem[2] || mem[6]==mem[4]);
  for (i=1;i<10;i=i+2) {mem[i]=3;}
}


