#test2.s
.pos 0x100
	irmovl $4, %eax
	iaddl $3, %eax		#should be 7	
	isubl $7, %eax		#should be 0
	irmovl $1, %eax		#should be 1
	iandl $1, %eax		#should be 1
	ixorl $0, %eax		#should be 0
	irmovl $1, %eax		#should be 1
	imull $10, %eax		#should be 10 
	idivl $10, %eax		#should be 1
	irmovl $7, %eax		#should be 7
	imodl $5, %eax		#should be 2
	
	halt
	




.pos 0x3000
replace: .long 5
		.long 10
		.long 15