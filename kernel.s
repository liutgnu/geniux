##this is the main kernel,so some varibles need to be shared.
.text
page_dir:
## note! _start must be globl, otherwise ld can't find entry.
.globl _start

_start:
movl $0x10,%eax
mov  %ax,%ds

##check A20
xorl %eax,%eax
incl %eax
movl %eax,0x000000
cmpl %eax,0x100000
je die
jmp setup_gdt
die:jmp die

##setup_gdt
setup_gdt:
lss  init_stack,%esp
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
mov  $system_int,%ax
xorl %edx,%edx
movl $0xef00,%edx
movl $0x080,%ecx
lea  idt(,%ecx,8),%edi
movl %eax,(%edi)
movl %edx,4(%edi)

xorl %edx,%edx
movl $0xef00,%edx
movl $page_fault,%eax
andl $0xffff0000,%eax
addl %eax,%edx
movl $page_fault,%eax
andl $0xffff,%eax
addl $0x00080000,%eax
movl $0x0e,%ecx
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

jmp  set_page

##kernel only uses 8MB, far enough
.org 0x1000
page0:
.org 0x2000
page1:
.org 0x3000

set_page:
movl $1024*3,%ecx
xorl %eax,%eax
xorl %edi,%edi
cld
rep
stosl

movl $page0+7,page_dir
movl $page1+7,page_dir+4

movl $page0,%edi
movl $0x00000007,%eax
cld
1:
stosl
addl $0x1000,%eax
cmpl $0x7ff007,%eax	##bug here!!! assume mem=8MB
jle  1b

xorl %eax,%eax
movl %eax,%cr3

movl %cr0,%eax
orl  $0x80000000,%eax
movl %eax,%cr0
jmp after_paging

##goto TSS_0
after_paging:
call init_mem
call ramdisk_init
call buffer_init
call fs_init

pushfl
andl $0xffffbfff,(%esp)
popfl
movl $0x20,%eax
ltr  %ax
movl $0x28,%eax
lldt %ax
sti
movl %esp,%eax
pushl $0x17
pushl %eax
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

.globl sys_fork
sys_fork:
push  %gs
pushl %esi
pushl %edi
pushl %ebp
call  copy_process
addl  $16,%esp
ret

##task0 start
#user_print_time
##task0 end

##task1 start
#user_gus_num
##task1 end

.globl jiffies
##varibles
jiffies:.long 0
.globl current
current:.long TASK0
.globl gdt_addr
gdt_addr:.long ggdt

##lgdt_opcode_start:
lgdt_opcode:
	.word (end_gdt-ggdt)-1
	.long ggdt
##lgdt_opcode_end

##gdt_start:
ggdt:	
	.quad 0x0000000000000000
	.quad 0x00c09a00000007ff ##code seg,level 0,read and exec,0x08
	.quad 0x00c09200000007ff ##data seg,level 0,read and write,0x10
	.quad 0x00c0920b80000002 ##display seg,level 0,read and write,0x18
	.word 0x68,tss0,0xe900,0x0 ##tss0 
	.word 0x40,ldt0,0xe200,0x0 ##ldt0
.org ggdt+(64*2+4)*8	##leave 64 tasks space
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

.globl TASK0
TASK0:
	.long 0,0,0,0			##state,pid,p_parent,p_child
ldt0:	
	.value 0,0,0,0			##ldt[0]
	.value 0x3ff,0,0xfa00,0x00c0	##ldt[1],code
	.value 0x3ff,0,0xf200,0x00c0	##ldt[2],data
tss0:	
	.long 0					##back link
	.long stack0,0x10			##esp0,ss0
	.long 0,0,0,0,0				##esp1,ss1,esp2,ss2,cr3
	.long 0,0,0,0,0				##eip,eflags,eax,ecx,edx
	.long 0,usr_stack0,0,0,0		##ebx,esp,ebp,esi,edi
	.long 0x17,0x0f,0x17,0x17,0x17,0x17	##es,cs,ss,ds,fs,gs
	.long 0x28,0x8000000			##ldt,trace bitmap

##usr_task0 stack
	.fill 1024,1,0
usr_stack0:
##usr_stack0 end

##stack0:
	.org TASK0+4096
stack0:
##stack0 end
