.pos 0x100
	irmovl bottom, %esp #initalize test
	irmovl $0, %ecx
	call *functions(%ecx), %eax
	irmovl $4, %edx
	call *functions(%edx), %eax
	halt

.pos 0x1000
jumpToMe:
	pushl %eax
	irmovl $3, %eax
	ret
	
.pos 0x2000
jumpTooMe:
	pushl %eax
	irmovl $4, %eax
	ret



.pos 0x3000
functions:	.long 0x1000
			.long 0x2000
top:	            .long 0x00000000,0x20     # top of stack.
bottom:             .long 0x00000000          # bottom of stack.

