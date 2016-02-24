.text
begtext:
.data
begdata:
.bss
begbss:
.text

BOOTSEG = 0x07c0
KERNELSEG = 0x0900
SYSLEN = 6

entry start
start:
jmpi go,#BOOTSEG
go:
mov ax,cs
mov ds,ax
mov ss,ax
mov sp,#0x500

load_kernel:
mov dx,#0x0000
mov cx,#0x0002
mov ax,#KERNELSEG
mov es,ax
mov bx,#0x0000
mov ax,#0x0200+SYSLEN
int 0x13
jnc print_msg
error: jmp error

print_msg:
mov ax,cs
mov es,ax
mov cx,#20
mov dx,#0x1004
mov bx,#0x000c
mov bp,#msgl
mov ax,#0x1301
int 0x10

;;setup stack
mov ax,KERNELSEG
mov ss,ax
mov sp,#0xff00
mov ds,ax
mov es,ax
jmpi 0,KERNELSEG

msgl: .ascii "Loading system ..."
      .byte 13,10
.org 510
.word 0xaa55
.text
endtext:
.data
enddata:
.bss
endbss:
