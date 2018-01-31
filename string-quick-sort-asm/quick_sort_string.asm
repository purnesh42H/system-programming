.globl main
.data
    initial: 		.asciiz "Initial array:\n" 	# char * initial 		= "Initial array:\n"
	sorted: 		.asciiz "Sorted array:\n" 	# char * sorted 		= "Initial array:\n"
	next_line: 		.asciiz "\n" 				# char * next_line 		= "\n"
	lbracket: 		.asciiz "[ "				# char * lbracket 		= "[ "
	rbracket: 		.asciiz " ]"				# char * rbracket 		= " ]"
	single_space: 	.asciiz " "					# char * single_space 	= " "
	
    # const char * data[] = {"Joe", "Jenny", "Jill", "John",
    #                      	"Jeff", "Joyce", "Jerry", "Janice",
    #                      	"Jake", "Jonna", "Jack", "Jocelyn",
    #                       "Jessie", "Jess", "Janet", "Jane"};
	
				.align 5
    dataName: 	.asciiz "Joe"
        		.align 5
        		.asciiz "Jenny"
        		.align 5
        		.asciiz "Jill"
        		.align 5
				.asciiz "John"
        		.align 5
				.asciiz "Jeff"
        		.align 5
				.asciiz "Joyce"
        		.align 5
				.asciiz "Jerry"
        		.align 5
				.asciiz "Janice"
        		.align 5
				.asciiz "Jake"
        		.align 5
				.asciiz "Jonna"
        		.align 5
				.asciiz "Jack"
        		.align 5
				.asciiz "Jocelyn"
        		.align 5
				.asciiz "Jessie"
        		.align 5
				.asciiz "Jess"
        		.align 5
				.asciiz "Janet"
        		.align 5
				.asciiz "Jane"
		
	# int * dataAddr[64]
				.align 2
	dataAddr: 	.space 64 # 16 pointers to strings: 16*4 = 64

.text
main:
	## Initialize array to array of pointers
	
	pointer_init:
		#t0 is counter: i=0...16
		#t1 used for beginning of the string array which we want to set the corresponding pointer
		#t2 the place where we should store the address of a string
		# After this point: i'th value in dataAddr, point to the i'th string(the address of it)
		
		# t0 = 0
		li $t0, 0 #i=0
		
		# t1 = &dataName
		la $t1, dataName
		
		# t2 = &dataAddr
		la $t2, dataAddr
	
	pointer_loop:
		# dataAddr[i] = &dataName[i]
		sw $t1, ($t2)	
		
		# t2 = t2 + 4
		add $t2, $t2, 4
		
		# t1 = t1 + 32
		addi $t1, $t1, 32
		
		# t0 = t0 + 1
		add $t0, $t0, 1
		
		# branch to pointer_loop if (t0 < 16)
		ble $t0, 16, pointer_loop
	
	# t0 = 0
	li $t0, 0
	
	# t2 = &dataAddr
	la $t2, dataAddr
	
	# a0 = &initial
	la $a0, initial
	
	## print Initial Array
	
	# printf("Initial Array:\n")
	li $v0, 4
	syscall
	
	# print_init()
	jal print_init
	
	quick_sort_init:
	    ## Intializing the pointers for quick sort
	
		# a0 = &dataAddr
		la $a0, dataAddr
	
		# a1 = 0
		li $a1, 0
	
		# a2 = 15
		li $a2, 15
	
		# quick_sort()
		jal quick_sort
		
	## print Sorted Array
	## print \n
	
	# a0 = &next_line
	la $a0, next_line
	# printf("\n")
	li $v0, 4
    syscall
	
	# printf("Sorted Array:\n")
	la $a0, sorted
	li $v0, 4
	syscall
	
	# print_init()
	jal print_init
	
	exit:
		#exit
		li $v0, 10
		syscall
	
	print_init:
		## print left bracket
	
		# a0 = &lbracket
	    la $a0, lbracket
	
		# printf("[ ")
	    li $v0, 4
	    syscall
		
		#t2 is counter:i
		#t0 is str_pointers
		#t5 is new_address = 4*i
		
		# t2 = 0
		li $t2, 0
		
		# t0 = &dataAddr
		la $t0, dataAddr
		
	print_array:
		# printf(" ")
		la $a0, single_space
		li $v0, 4
		syscall
		
		# printf ("%s", *dataAddr[t0])
		lw $a0, ($t0)
		li $v0, 4
		syscall
		
		# t2 = t2 + 1
		add $t2, $t2, 1 #i++
		
		# t0 = t0 + 4
		add $t0, $t0, 4 #4*i	
		
		# branch to print_array if (t2 < 16)
		blt $t2, 16, print_array #while(i<16)
	
	print_done:
		## print rbracket
	
		# printf("] ")
		la $a0, rbracket
		li $v0, 4
		syscall
		jr $ra
		
	quick_sort:
		## store $s and $ra
			
			# sp = sp - 24
			addi	$sp, $sp, -24	# Adjust sp
			# sp + 0 = $s0
			sw		$s0, 0($sp)		# store s0
			# sp + 4 = $s1
			sw		$s1, 4($sp)		# store s1
			# sp + 8 = s2
			sw		$s2, 8($sp)		# store s2
			# sp + 12 = a1
			sw		$a1, 12($sp)	# store a1
			# sp + 16 = a2
			sw		$a2, 16($sp)	# store a2
			# sp + 20 = ra
			sw		$ra, 20($sp)	# store ra

		## set $s
			
			# s0 = a1
			move	$s0, $a1		# l = left
			# s1 = a2
			move	$s1, $a2		# r = right
			# a2 = a1
			move	$s2, $a1		# p = left
		
	# while (l < r)
	quick_sort_loop1:
	
		# branch to quick_sort_loop1_done if (s0 >= s1)
		bge $s0, $s1, quick_sort_loop1_done
		
		quick_sort_loop1_1:
			
			# t7 = 32
			li 		$t7, 4
			
			# s0 = s0 * t7
			mult 	$s0, $t7
			# t0 = t0 * 4bit
			mflo 	$t0
			# t0 = &dataAddr[l]
			add 	$t0, $t0, $a0
			
			# s2 = s2 * t7
			mult	$s2, $t7
			# t1 = t1 * 4bit
			mflo	$t1	
			# t1 = &dataAddr[p]	
			add		$t1, $t1, $a0	
			
			# branch to quick_sort_loop1_1_done if (dataName[l] <= dataName[p])
			jal 	strcmp
			beq 	$v0, 0, quick_sort_loop1_1_done
			
			# branch to quick_sort_loop1_1_done if (l < r)
			bge 	$s0, $a2, quick_sort_loop1_1_done
			
			# l++
			addi 	$s0, $s0, 1
			
			# quick_sort_loop1_1()
			j 		quick_sort_loop1_1
			
	quick_sort_loop1_1_done:
	
		# v0 = 0
		add 		$v0,$zero,$zero
		
		# while (dataName[r] >= dataName[p] && r > l)
		quick_sort_loop1_2:
		
			# t7 = 32
			li 		$t7, 4
			
			# s1 = s1 * t7
			mult 	$s1, $t7
			# t0 = t0 * 4bit
			mflo 	$t0
			# t0 = dataAddr[r]
			add 	$t0, $t0, $a0
			
			# s2 = s2 * t7
			mult	$s2, $t7
			# t1 = t1 * 4bit
			mflo	$t1		
			# t0 = dataAddr[r]		
			add		$t1, $t1, $a0	
			
			# branch to quick_sort_loop1_2_done if (dataName[r] >= dataName[p])
			jal strcmp
			beq 	$v0, 1, quick_sort_loop1_2_done
			
			# branch to quick_sort_loop1_2_done if (r > l)
			ble		$s1, $a1, quick_sort_loop1_2_done
			
			# r--
			addi	$s1, $s1, -1
			
			# quick_sort_loop1_2()
			j		quick_sort_loop1_2
			
	quick_sort_loop1_2_done:
	
		# v0 = 0
		add $v0,$zero,$zero
		
		# branch to if_quick1_jump if (l >= r)
		blt $s0, $s1, if_quick1_jump
		
		# swap (dataAddr[p], dataAddr[r])
		swap:
			# t7 = 32
			li 		$t7, 4
			
			# s2 = s2 * t7
			mult	$s2, $t7
			# t6 = l * 4bit
			mflo	$t6	
			# t0 = dataAddr[p]			
			add		$t0, $t6, $a0
			
			# s1 = s1 * t7
			mult	$s1, $t7
			# t6 = l * 4bit
			mflo	$t6	
			# t1 = dataAddr[r]			
			add		$t1, $t6, $a0
			
			# swap
			# *t2 = *t0
			lw		$t2, 0($t0)
			# *t3 = *t1
			lw		$t3, 0($t1)
			# *t0 = *t3
			sw		$t3, 0($t0)
			# *t2 = *t1
			sw		$t2, 0($t1)
			
	# quick_sort(arr, left, p - 1)
		# set arguments
		
		# a2 = s1
		move	$a2, $s1
		# a2 = a2 - 1
		addi	$a2, $a2, -1	# a2 = r - 1
		# quick_sort()
		jal		quick_sort
		
		# pop stack
		
		# *a1 = *(sp + 12)
		lw		$a1, 12($sp)	# load a1
		# *a2 = *(sp + 16)
		lw		$a2, 16($sp)	# load a2
		# *ra = *(sp + 20)
		lw		$ra, 20($sp)	# load ra

	# quick_sort(arr, p + 1, r)
		# set arguments
	
		# a1 = s1
		move	$a1, $s1
		# a1 = a1 + 1
		addi	$a1, $a1, 1
		
		# quick_sort()
		jal		quick_sort
		
		# pop stack
		
		# *a1 = *(sp + 12)
		lw		$a1, 12($sp)	# load a1
		# *a2 = *(sp + 16)
		lw		$a2, 16($sp)	# load a2
		# *ra = *(sp + 20)
		lw		$ra, 20($sp)	# load ra

		# return
		
		# *s0 = *(sp + 0)
		lw		$s0, 0($sp)		# load s0
		# *s1 = *(sp + 4)
		lw		$s1, 4($sp)		# load s1
		# *s2 = *(sp + 8)
		lw		$s2, 8($sp)		# load s2
		# *sp = *(sp + 24)
		addi	$sp, $sp, 24	# Adjust sp
		# return
		jr		$ra
		
	if_quick1_jump:

		# swap (dataAddr[l], dataAddr[r])
		
		# t7 = 32
		li		$t7, 4
					
		# s0 = s0 * t7
		mult	$s0, $t7
		# t6 = l * 4bit
		mflo	$t6			
		# t0 = &arr[l]	
		add		$t0, $t6, $a0
		
		# s1 = s1 * t7
		mult	$s1, $t7
		# t6 = r * 4bit
		mflo	$t6		
		# t1 = &arr[r]		
		add		$t1, $t6, $a0
		
		# Swap
		# *t2 = *t0
		lw		$t2, 0($t0)
		# *t3 = *t1
		lw		$t3, 0($t1)
		# *t0 = *t3
		sw		$t3, 0($t0)
		# *t2 = *t1
		sw		$t2, 0($t1)
		# quick_sort_loop1()
		j		quick_sort_loop1

	quick_sort_loop1_done:
		# return
		
		# *s0 = *(sp + 0) 
		lw		$s0, 0($sp)		# load s0
		# *s1 = *(sp + 4)
		lw		$s1, 4($sp)		# load s1
		# *s2 = *(sp + 8)
		lw		$s2, 8($sp)		# load s2
		# *sp = *(sp + 24)
		addi	$sp, $sp, 24	# Adjust sp
		# return
		jr		$ra
		
	# compares two strings using slt
	strcmp:
		# v0 = 0
		add $v0, $zero, $zero
		# t0 = dataName[t0]
		lw 	$t0, ($t0)
		# t1 = dataName[t1] 
		lw 	$t1, ($t1) 
		
		loop:
			# t3 = t0[0]		
		    lb 		$t3, ($t0)         					# load a byte from each string
			# t3 = t1[0]  
		    lb 		$t4, ($t1)
			# branch to loop_done if (t3 == 0)  
		    beqz 	$t3, loop_done    					# str1 end  
			# branch to loop_done if (t4 == 0)  
		    beqz 	$t4, loop_done  
			# t5 = t3 < t4
		    slt 	$t5, $t3, $t4     					# compare two bytes 
			# branch to missmatch if (t5 == 0) 
		    bnez 	$t5, missmatch  
			# branch to loop_done if (t3 != t4)
			bne 	$t3, $t4, loop_done
			# t0 = t0 + 1
		    addi 	$t0, $t0, 1      					# t1 points to the next byte of string
			# t1 = t1 + 1  
		    addi 	$t1, $t1, 1 
			# loop() 
		    j 		loop
			
		missmatch:
			# v0 = 1
			addi 	$v0, $zero, 1 
			# loop_done() 
			j 		loop_done
			
		loop_done:
			# return
			jr 	$ra