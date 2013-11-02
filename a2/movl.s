#Test for rmmovl & mrmovl
.pos 0x100
	irmovl $100, %eax
	irmovl $2, %edx
	rmmovl %eax, 0x3000(%edx, 4)
	mrmovl 0x3000(%edx, 4), %esp
	
	halt



.pos 0x3000
replace: 
	.long 5
	.long 10
	.long 15