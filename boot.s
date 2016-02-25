.text
begtext:
.data
begdata:
.bss
begbss:
.text

BOOTSEG = 0x07c0
INITSEG = 0x9000
KERNELSEG = 0x0800
SYSLEN = 64

entry start
start:
mov ax,#BOOTSEG
mov ds,ax
mov ax,#INITSEG
mov es,ax
xor ax,ax
mov si,ax
mov di,ax
mov cx,#256
rep
movw
jmpi go,#INITSEG

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
jnc load
die_one:jmp die_one

sector:		.word 3
head:		.word 1
track:		.word 3
f_len:		.word 915
des_seg:	.word 0x1000
offset:		.word 0

load:
mov ax,f_len
cmp ax,#0
je  midf

mov ax,#0x0201
mov bx,sector
mov cl,bl
mov bx,track
mov ch,bl
mov bx,head
mov dh,bl
mov dl,#0
mov bx,des_seg
mov es,bx
mov bx,offset
int 0x13
jnc ok1
die_two:jmp die_two

ok1:
mov bx,sector
inc bx
mov sector,bx
cmp bx,#19
jne ok2
mov bx,#1
mov sector,bx
mov bx,head
inc bx
mov head,bx
jmp ok2

midf:jmp ok5
midb:jmp load

ok2:
mov bx,head
cmp bx,#2
jne ok3
mov bx,#0
mov head,bx
mov bx,track
inc bx
mov track,bx

ok3:
mov bx,offset
add bx,#0x200
mov offset,bx
cmp bx,#0
jne ok4
mov offset,bx
mov bx,des_seg
add bx,#0x1000
mov des_seg,bx

ok4:
mov bx,f_len
dec bx
mov f_len,bx
jmp midb

!move kernel to 0x0000
ok5:
mov ax,cs
mov es,ax
mov bp,#msgl	!keep those

!source,ds:si des,es:di
cli
mov ax,#KERNELSEG
mov ds,ax
xor ax,ax
mov es,ax
mov si,ax
mov di,ax
mov cx,#0x2280	!value keep samll
rep
movsd

mov ax,#INITSEG
mov ds,ax
mov es,ax
lidt idt_opcode
lgdt gdt_opcode

mov ax,#0x0001
lmsw ax

jmpi 0x38,8

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
	.long 0x90000+gdt

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
