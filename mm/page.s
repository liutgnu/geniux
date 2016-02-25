##int 14 page_fault

.globl page_fault
page_fault:
	xchgl %eax,(%esp)
	pushl %ebx
	pushl %ecx
	pushl %edx
	push  %ds
	push  %es
	push  %fs
	mov   $0x10,%dx
	mov   %dx,%ds
	mov   %dx,%es
	mov   %dx,%fs
	movl  %cr2,%edx
	pushl %edx
	testl $1,%eax
	je    1f
	call  do_page_wprotected
	jmp   2f
1:	call  do_no_page
2:	addl  $4,%esp
	pop   %fs
	pop   %es
	pop   %ds
	popl  %edx
	popl  %ecx
	popl  %ebx
	popl  %eax
	iret
