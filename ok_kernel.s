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

##goto TSS_1
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
push %ds
pushl %eax
movl $0x10,%eax
mov  %ax,%ds
call print_time

movb $0x20,%al
outb %al,$0x20
movl $1,%eax
cmpl %eax,current
je   a1
movl $1,current
ljmp $0x30,$0
jmp  a2
a1:
movl $0,current
ljmp $0x20,$0
a2:
popl %eax
pop  %ds
iret
##timer_int end

##system_int
##do_nothing
system_int:
pushl %eax
push  %ds
movl  $0x10,%eax
mov   %ax,%ds
movb  $0,%al
.word 0x00eb,0x00eb
pop   %ds
popl  %eax
iret
##system_int end 

##print_time_start:
#print_char format:al,1 ah,10 bl,100
#print_time format:cl,1 ch,10 dl,100

print_time:
pushl %eax
pushl %ebx
pushl %ecx
pushl %edx

movb bit_100,%dl
movb bit_10,%ch
movb bit_1,%cl

incb %cl
cmpb $10,%cl
je   l1  ##carry
jmp  l4  ##return

l1:
xorb %cl,%cl
incb %ch
cmpb $10,%ch
je   l2  ##carry
jmp  l4  ##return

l2:
xorb %ch,%ch
incb %dl
cmpb $10,%dl
je   l3  #carry
jmp  l4  #return

l3:
xorb %dl,%dl ##start over

l4:
##save each bit
movb %cl,bit_1
movb %ch,bit_10
movb %dl,bit_100

movb $0,%bl
movb $2,%bh
movb %cl,%al
addb $48,%al
call print_char

movb $0,%bl
movb $1,%bh
movb %ch,%al
addb $48,%al
call print_char

movb $0,%bl
movb $0,%bh
movb %dl,%al
addb $48,%al
call print_char

popl %edx
popl %ecx
popl %ebx
popl %eax
ret
##print_time end

##print_char start
##al,character bh,x bl,y
##gs point to display
print_char:
push %gs
pushl %eax
pushl %ebx
pushl %ecx
pushl %edx

push  %ax
movb %bl,%al
movb $80,%cl
mulb %cl
addb %bh,%al
adcb $0,%ah
inc  %ax
mov  %ax,%bx

cmp  $2000,%bx
jb   l5   
xorl %eax,%eax
xorl %edx,%edx
movl %ebx,%eax
movl $2000,%ecx
divl %ecx
movl %edx,%ebx

l5:
pop  %ax
mov  $0x18,%dx
mov  %dx,%gs
shl  $1,%ebx
movb %al,%gs:(%ebx)
shr  $1,%ebx

popl %edx
popl %ecx
popl %ebx
popl %eax
pop %gs
ret
##print_char end


##task0 start
task0:
movb  $0,%al
.word 0x00eb,0x00eb
jmp task0
##task0 end


##task1 start
task1:
movb  $0,%al
.word 0x00eb,0x00eb
jmp task1
##task1 end

##varibles
current:.long 0
bit_100:.long 0
bit_10: .long 0
bit_1:  .long 0

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
	.fill 128,4,0
init_stack:
	.long init_stack ##%esp
	.word 0x10       ##%ss
##kernel_seg_end

##task0 ldt:
ldt0:
	.quad 0x0000000000000000
	.quad 0x00c0fa00000003ff ##code seg,level 3,read and exec,0x08
	.quad 0x00c0f200000003ff ##data seg,level 3,read and write,0x10
##task0 ldt end

##task0 tss:
tss0:
	.long 0
	.long stack0,0x10
	.long 0,0,0,0,0
	.long 0,0,0,0,0
	.long 0,0,0,0,0
	.long 0,0,0,0,0,0
	.long 0x28,0x8000000
##task0 tss end

##task0 stack
	.fill 128,4,0
stack0:
##stack0 end

##task1 ldt:
ldt1:
	.quad 0x0000000000000000
	.quad 0x00c0fa00000003ff ##code seg,level 3,read and exec,0x08
	.quad 0x00c0f200000003ff ##data seg,level 3,read and write,0x10
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
	.fill 128,4,0
stack1:
##stack1 end

##usr_stack1:
	.fill 128,4,0
usr_stack1:
##usr_stack1 end





