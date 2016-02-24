##this is the main kernel,so some varibles need to be shared.
##we would better keep full .text begtext..., otherwise get error or not :)
.globl begtext,begdata,begbss,endtext,enddata,endbss,current,jiffies,gdt
.text
begtext:
.data
begdata:
.bss
begbss:
.text

## note! _start must be globl, otherwise ld can't find entry.
.globl _start

_start:
movl $0x10,%eax
mov  %ax,%ds
lss  init_stack,%esp

##setup_gdt
lgdt lgdt_opcode

##setup_idt
movl $0x00080000,%eax
mov  $default_int,%ax
xorl %edx,%edx
movl $0x8e00,%edx
movl $256,%ecx
movl $idt,%edi
rep:
movl %eax,(%edi)
movl %edx,4(%edi)
addl $8,%edi
decl %ecx
cmpl $0,%ecx
jne  rep

movl $0x00080000,%eax
mov  $timer_int,%ax
xorl %edx,%edx
movl $0x8e00,%edx
movl $0x08,%ecx
lea  idt(,%ecx,8),%edi
movl %eax,(%edi)
movl %edx,4(%edi)

movl $0x00080000,%eax
mov  $keyboard_interrupt,%ax
xorl %edx,%edx
movl $0x8e00,%edx
movl $0x09,%ecx
lea  idt(,%ecx,8),%edi
movl %eax,(%edi)
movl %edx,4(%edi)

movl $0x00080000,%eax
mov  $system_int,%ax
xorl %edx,%edx
movl $0xef00,%edx
movl $0x080,%ecx
lea  idt(,%ecx,8),%edi
movl %eax,(%edi)
movl %edx,4(%edi)

lidt lidt_opcode

##reset register
movl $0x10,%eax
mov  %ax,%ds
mov  %ax,%es
mov  %ax,%fs
mov  %ax,%gs

##setup_8253
movb $0x36,%al
mov  $0x43,%dx
outb %al,%dx
mov  $11930,%ax
mov  $0x40,%dx
outb %al,%dx
movb %ah,%al
outb %al,%dx

##initialize system
call init_varibles

##goto TSS_0
pushfl 
andl $0xffffbfff,(%esp)
popfl 
movl $0x20,%eax
ltr  %ax
movl $0x28,%eax
lldt %ax
movl $0,current
sti
pushl $0x17
pushl $init_stack
pushfl
pushl $0x0f
pushl $task0
iret
##finish

##default_int start
##do nothing
default_int:
pushl %eax
push  %ds
movl  $0x10,%eax
mov   %ax,%ds
movb  $0,%al
.word 0x00eb,0x00eb
pop   %ds
popl  %eax
iret
##default_int end

##timer_int start  
timer_int:
pushl %eax	#0x14(%esp)
pushl %ebx	#0x10(%esp)
pushl %ecx	#0x0c(%esp)
pushl %edx	#0x08(%esp)
push  %ds	#0x04(%esp)
push  %es	#0x00(%esp)
movl  $0x10,%eax
mov   %ax,%ds
mov   %ax,%es

movl  jiffies,%eax
addl  $1,%eax
movl  %eax,jiffies

movb  $0x20,%al	#EOI
outb  %al,$0x20

movl  0x1c(%esp),%eax	#eip 0x18,cs 0x1c
andl  $3,%eax	#cpl
pushl %eax
call  schedule
popl  %eax	#drop former parameter

pop   %es
pop   %ds
popl  %edx
popl  %ecx
popl  %ebx
popl  %eax
iret
##timer_int end

##keyboard_interrupt start
keyboard_interrupt:
pushl %eax
pushl %ebx
pushl %ecx
pushl %edx
push  %ds
push  %es
movl  $0x10,%eax
mov   %ax,%ds
mov   %ax,%es

xorl  %eax,%eax
inb   $0x60,%al
cmpb  $0xe0,%al
je    do_e0
cmpb  $0xe1,%al
je    do_e1
pushl %eax
call  *key_table(,%eax,4)	## f**k ld! you must add * before key_table. otherwise you get a warning and error run
popl  %eax  #here is a little tricky, we must drop the former parameter

reset_kb:
inb   $0x61,%al
call  delay
orb   $0x80,%al
call  delay
outb  %al,$0x61
call  delay
andb  $0x7f,%al
call  delay
outb  %al,$0x61

movb  $0x20,%al
outb  %al,$0x20  #EOI
pop   %es
pop   %ds
popl  %edx
popl  %ecx
popl  %ebx
popl  %eax
iret

##when we need distinguish LEFT and RIGHT
do_e0:
jmp reset_kb

do_e1:
jmp reset_kb

delay:
	jmp D1
D1:     jmp D2
D2:	ret 
##keyboard_interrupt end

##system_int
##eax sys_call num, ebx first parameter, ecx second, edx third
system_int:
push  %ds
push  %es
push  %fs
pushl %eax
pushl %edx
pushl %ecx
pushl %ebx

movl  $0x10,%edx  ##kernel data seg
mov   %dx,%ds
mov   %dx,%es
movl  $0x17,%edx  ##user data seg
mov   %dx,%fs

call  *syscall_table(,%eax,4)
pushl %eax  ##return value

popl  %eax
popl  %ebx
popl  %ecx
popl  %edx
addl  $4,%esp
pop   %fs
pop   %es
pop   %ds
iret
##system_int end 

##task0 start
#user_print_time
##task0 end

##task1 start
#user_gus_num
##task1 end

##varibles
jiffies:.long 0
current:.long 0

##lgdt_opcode_start:
lgdt_opcode:
	.word (end_gdt-gdt)-1
	.long gdt
##lgdt_opcode_end

##gdt_start:
gdt:	
	.quad 0x0000000000000000
	.quad 0x00c09a00000007ff ##code seg,level 0,read and exec,0x08
	.quad 0x00c09200000007ff ##data seg,level 0,read and write,0x10
	.quad 0x00c0920b80000002 ##display seg,level 0,read and write,0x18
	.word 0x68,tss0,0xe900,0x0 ##tss0 
	.word 0x40,ldt0,0xe200,0x0 ##ldt0
	.word 0x68,tss1,0xe900,0x0 ##tss1
	.word 0x40,ldt1,0xe200,0x0 ##ldt1
end_gdt:
##gdt_end

##lidt_opcode_start:
lidt_opcode:
	.word 256*8-1
	.long idt
##lid_opcode end

##idt_start:
idt:	.fill 256,8,0
##idt_end

##kernel_seg_start:
	.fill 1024,4,0
init_stack:
	.long init_stack ##%esp
	.word 0x10       ##%ss
##kernel_seg_end

##task0 ldt:
ldt0:
	.quad 0x0000000000000000
	.quad 0x00c0fa00000003ff ##code seg,level 3,read and exec,0x0f
	.quad 0x00c0f200000003ff ##data seg,level 3,read and write,0x17
##task0 ldt end

##task0 tss:
tss0:
	.long 0
	.long stack0,0x10
	.long 0,0,0,0,0
	.long 0,0,0,0,0
	.long 0,usr_stack0,0,0,0
	.long 0,0,0,0,0,0
	.long 0x28,0x8000000
##task0 tss end

##task0 stack
	.fill 1024,4,0
stack0:
##stack0 end

##usr_stack0:
	.fill 1024,4,0
usr_stack0:
##usr_stack1 end

##task1 ldt:
ldt1:
	.quad 0x0000000000000000
	.quad 0x00c0fa00000003ff ##code seg,level 3,read and exec,0x0f
	.quad 0x00c0f200000003ff ##data seg,level 3,read and write,0x17
##task1 ldt end

##task1 tss:
tss1:
	.long 0
	.long stack1,0x10
	.long 0,0,0,0,0
	.long task1,0x200,0,0,0
	.long 0,usr_stack1,0,0,0
	.long 0x17,0x0f,0x17,0x17,0x17,0x17
	.long 0x38,0x8000000
##task1 tss end

##task1 stack
	.fill 1024,4,0
stack1:
##stack1 end

##usr_stack1:
	.fill 1024,4,0
usr_stack1:
##usr_stack1 end

.text
endtext:
.data
enddata:
.bss
endbss:
