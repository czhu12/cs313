	.file	"heapsort.c"
	.text
	.p2align 4,,15
	.globl	heapsort
	.type	heapsort, @function
heapsort:
.LFB11:
	.cfi_startproc
	testl	%edi, %edi #sets condition codes for value of last
	pushq	%rbx #save previous base register
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	movl	%edi, %ebx #move value of last to ebx, I think ebx now tracks value of i
	js	.L1 #jumping to if statment if last < 0 ( sign flag = 1 )
	call	heapify_array #call heapify_array
	.p2align 4,,10
	.p2align 3
.L3:
	movl	%ebx, %edi #move ebx ( i ) to edi, parameter i passed to extract_max through edi?
	call	extract_max #call extract_max
	movslq	%ebx, %rdx #move from 32 bit number to 64 bit, rdx holds a 64 bit value of i
	subl	$1, %ebx #subtract 1
	cmpl	$-1, %ebx 
	movl	%eax, heap(,%rdx,4) #store return value of extract_max to heap at index dx ( i )
	jne	.L3 #jump if not equal to 0 ( if zero flag = 0)
.L1:
	popq	%rbx #restore value of base pointer from previous function stack frame
	.cfi_def_cfa_offset 8
	ret #return
	.cfi_endproc
.LFE11:
	.size	heapsort, .-heapsort
	.ident	"GCC: (SUSE Linux) 4.7.2 20130108 [gcc-4_7-branch revision 195012]"
	.section	.comment.SUSE.OPTs,"MS",@progbits,1
	.string	"Ospwg"
	.section	.note.GNU-stack,"",@progbits
