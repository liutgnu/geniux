!assume kernel le 100k, music le 500k
!disk image:
! 0.5k  100k           500k
!|---|--------|--------------------|--.... 
!
!memory image:
!  7c00(31k)                          a0000(640k)
!|---|------------------------------------|
!
!                                9dc00(631k)  
!|------------------------------------|---|
!
!  7c00    20c00                 9dc00     
!|---|--100k--|------------500k-------|---|
!
!      19000                 96000
!|--100k--|--------500k---------|---------|
!

.text
begtext:
.data
begdata:
.bss
begbss:
.text

!rely on memory image geometry
!note! when DESTSEG changed, so should gdt_opcode!!!
BOOTSEG = 0x07c0
DESTSEG = 0x9dc0
KERNELSEG = 0x07c0

entry start
start:
mov ax,#BOOTSEG
mov ds,ax
mov ax,#DESTSEG
mov es,ax
xor ax,ax
mov si,ax
mov di,ax
mov cx,#256
rep
movw
jmpi go,#DESTSEG

go:
mov ax,cs
mov ds,ax
mov ss,ax
mov sp,#0x600
mov bp,#msgl

!extend_mem_size
mov ah,#0x88
int 0x15
mov [2],ax	!0x9dc02 

!load kernel
call load

!load music
mov ax,#4
mov sector,ax
mov ax,#1
mov head,ax
mov ax,#5
mov track,ax
mov ax,#1000
mov f_len,ax
mov ax,#0x2000	!music 2000:0c00
mov des_seg,ax
mov ax,#0xc00
mov offset,ax
call load

!source,ds:si des,es:di
mov ax,#0x07c0
mov ds,ax
xor ax,ax
mov es,ax

unfin1:
mov cx,#0x8000
unfin2:
xor ax,ax
mov si,ax
mov di,ax
rep
movw

mov ax,es
add ax,#0x1000
mov es,ax
mov ax,ds
add ax,#0x1000
mov ds,ax
cmp ax,#0x97c0
jl finish
jg unfin1
mov cx,#0x3000
jmp unfin2

finish:
mov ax,cs
mov ds,ax
mov es,ax

on_A20:
in al,#0xee

cli
lidt idt_opcode
lgdt gdt_opcode

mov ax,#0x0001
lmsw ax

jmpi 0,8

!rely on disk image geometry
sector:		.word 2
head:		.word 0
track:		.word 0
f_len:		.word 200
des_seg:	.word 0
offset:		.word 0x7c00

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

ok5:
ret

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

idt_opcode:
	.word 0
	.word 0,0
gdt_opcode:
	.word 0x7ff
	.long 0x9dc00+gdt

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
