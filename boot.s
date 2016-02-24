.text
begtext:
.data
begdata:
.bss
begbss:
.text

BOOTSEG = 0x07c0
KERNELSEG = 0x0900
SYSLEN = 30

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

!source,ds:si dis,es:di 
cli
mov ax,#KERNELSEG
mov ds,ax
xor ax,ax
mov es,ax
mov si,ax
mov di,ax
mov cx,#0x3000
rep
movw

mov ax,#BOOTSEG
mov ds,ax
lidt idt_opcode
lgdt gdt_opcode

mov ax,#0x0001
lmsw ax

jmpi 0,8

gdt:
	.word 0,0,0,0

	.word 0x07ff
	.word 0x0000
	.word 0x9a00
	.word 0x00c0

	.word 0x07ff
	.word 0x0000
	.word 0x9200
	.word 0x00c0

	.word 0x0002
	.word 0x8000
	.word 0x920b
	.word 0x00c0

idt_opcode:
	.word 0
	.word 0,0
gdt_opcode:
	.word 0x7ff
	.word 0x7c00+gdt,0

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
