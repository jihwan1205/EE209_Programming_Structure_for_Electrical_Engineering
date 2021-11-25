### 20200355 / Jihwan Shin / Assignment 4
### --------------------------------------------------------------------
### mydc.s
###
### Desk Calculator (dc)
### --------------------------------------------------------------------

	.equ   ARRAYSIZE, 20
	.equ   EOF, -1

.section ".rodata"

## string format for scanf and printf
scanfFormat:
	.asciz "%s"
printfFormat:
	.asciz "%d\n"

## stderr string format for errors
emptyError:
	.asciz "dc: stack empty\n"

divideError:
	.asciz "dc: divide by zero\n"

overflowError:
	.asciz "dc: overflow happens\n"
### --------------------------------------------------------------------

        .section ".data"

### --------------------------------------------------------------------

        .section ".bss"
buffer:
        .skip  ARRAYSIZE

### --------------------------------------------------------------------

	.section ".text"

	## -------------------------------------------------------------
	## int main(void)
	## Runs desk calculator program.  Returns 0.
	## -------------------------------------------------------------

	.globl  main
	.type   main,@function

main:

	pushl   %ebp
	movl    %esp, %ebp

input:

	## dc number stack initialized. %esp = %ebp

	## scanf("%s", buffer)
	pushl	$buffer
	pushl	$scanfFormat
	call    scanf
	addl    $8, %esp

	## check if user input EOF
	cmpl	$EOF, %eax
	je	quit


	## check if user input is digit
	movl	$0, %ebx
	movb	buffer, %bl
	pushl 	%ebx
	call	isdigit
	addl	$4, %esp
	andl	%eax, %eax
	## buffer is digit
	jne		digit

	## buffer is not digit

	cmpb	$'_', buffer
	je		negative

	cmpb	$'+', buffer
	je		addition

	cmpb	$'-', buffer
	je		subtraction

	cmpb	$'*', buffer
	je		multiply

	cmpb	$'/', buffer
	je		divide

	cmpb	$'%', buffer
	je		remainder

	cmpb	$'^', buffer
	je		exponentation

	cmpb	$'p', buffer
	je		print

	cmpb	$'q', buffer
	je		quit

	cmpb	$'f', buffer
	je		printall

	cmpb	$'c', buffer
	je		clear

	cmpb	$'d', buffer
	je		duplicate

	cmpb	$'r', buffer
	je		reverse

## push digit into stack
digit:
	##	change string to integer
	pushl	$buffer
	call 	atoi
	addl	$4, %esp 
	## push integer into stack
	pushl	%eax 
	jmp		input

## push negative number into stack
negative:
	## change digit part of string to integer
	movl	$buffer, %ebx
	incl	%ebx
	pushl	%ebx
	call	atoi
	addl	$4, %esp
	## negate and push negative number into stack
	negl	%eax
	pushl	%eax
	jmp 	input

## handle error or proceed addition
addition:
	cmpl	%esp, %ebp
	## error if stack has no numbers before
	je		stackEmpty
	popl	%ebx
	cmpl	%esp, %ebp
	## if stack has at least two numbers before
	jne		doAddition
	## error if stack has only one number before
	pushl	%ebx
	jmp 	stackEmpty

	## do addition after checking there is no error
	doAddition:
		popl	%eax
		addl	%ebx, %eax
		## error if overflow
		jo		printOverflow
		pushl	%eax
		jmp 	input

## handle error or proceed subraction
subtraction:
	cmpl	%esp, %ebp
	## error if stack has no numbers before
	je		stackEmpty
	popl	%ebx
	cmpl	%esp, %ebp
	## if stack has at least two numbers before
	jne		doSubtraction
	## error if stack has only one number before
	pushl	%ebx
	jmp		stackEmpty

	## do subtraction after checking there is no error
	doSubtraction:
		popl	%eax
		subl	%ebx, %eax
		## error if overflow
		jo		printOverflow
		pushl	%eax
		jmp		input

## handle error or proceed multiplication
multiply:
	cmpl	%esp, %ebp
	## error if stack has no numbers before
	je		stackEmpty
	popl	%ebx
	cmpl	%esp, %ebp
	## if stack has at least two numbers before
	jne		doMultiplication
	## error if stack has only one number before
	pushl	%ebx
	jmp 	stackEmpty

	## do multiplication after checking there is no error
	doMultiplication:
		popl 	%eax
		imull	%ebx
		## error if overflow
		jo		printOverflow
		pushl	%eax
		jmp 	input

## handle error or proceed division
divide:
	cmpl	%esp, %ebp
	## error if stack has no numbers before
	je		stackEmpty
	popl	%ebx
	cmpl	%esp, %ebp
	## if stack has at least two numbers before
	jne		doDivision
	## error if stack has only one number before
	pushl	%ebx
	jmp 	stackEmpty

	## do division after checking there is no error
	doDivision:
		cmpl	$0, %ebx
		## error if dividor is zero
		je		divideZero
		movl	$0, %edx
		popl	%eax
		idivl	%ebx
		pushl	%eax
		jmp 	input

## handle error or proceed remainder calculation
remainder:
	cmpl	%esp, %ebp
	## error if stack has no numbers before
	je		stackEmpty
	popl	%ebx
	cmpl	%esp, %ebp
	## if stack has at least two numbers before
	jne		doDivision2
	## error if stack has only one number before
	pushl	%ebx
	jmp 	stackEmpty

	## do division after checking there is no error
	doDivision2:
		cmpl	$0, %ebx
		## error if dividor is zero
		je		divideZero
		movl	$0, %edx
		popl	%eax
		idivl	%ebx
		pushl	%edx	## %edx stores remainder after division
		jmp 	input

## handle error or proceed exponentation
exponentation:
	cmpl	%esp, %ebp
	## error if stack has no numbers before
	je		stackEmpty
	popl	%ebx
	cmpl	%esp, %ebp
	## if stack has at least two numbers before
	jne		doExponentation
	## error if stack has only one number before
	pushl	%ebx
	jmp 	stackEmpty

	## do exponentation after checking there is no error
	doExponentation:
		popl 	%eax
		movl	$0, %ecx
		movl	%eax, %esi
		movl	$1, %eax
		expoLoop:
			imull	%esi
			## error if overflow
			jo printOverflow
			addl	$1, %ecx
			cmpl	%ecx, %ebx
			jg		expoLoop
		pushl	%eax
		jmp 	input

## print value of top of stack
print:
	cmpl	%esp, %ebp
	## error if stack is empty
	je 		stackEmpty
	pushl	$printfFormat
	call	printf 
	addl	$4, %esp
	jmp 	input

## print contents of stack in LIFO order
printall:
	movl	%esp, %esi
	printallLoop:
		cmpl	%esi, %ebp
		jle		input
		pushl	(%esi)
		pushl	$printfFormat
		call	printf
		addl	$8, %esp
		addl	$4, %esi
		jmp		printallLoop

## clear contents of the stack
clear:
	movl	%ebp, %esp
	jmp		input

## duplicates top element of stack
duplicate:
	cmpl	%esp, %ebp
	## error if stack is empty
	je 		stackEmpty

	popl	%ebx	## save top most element
	## duplicate by pushing twice
	pushl	%ebx
	pushl	%ebx
	jmp 	input

## reverses order of top two values of stack
reverse:
	cmpl	%esp, %ebp
	## error if stack is empty
	je 		stackEmpty

	popl	%ebx	## top most element
	cmpl	%esp, %ebp
	## error if stack has only one element
	jne		doReverse
	pushl	%ebx
	jmp		stackEmpty
	doReverse:
		popl	%eax	## second element
		pushl	%ebx	## copy top most element to second
		pushl	%eax	## copy second element to top most
		jmp 	input

## print 'dc: stack empty' to stderr and continue
stackEmpty:
	pushl	$emptyError
	pushl 	stderr
	call	fprintf
	addl	$8, %esp
	jmp 	input

## print 'dc: divide by zero' to stderr and terminate
divideZero:
	pushl 	$divideError
	pushl	stderr
	call	fprintf
	addl 	$8, %esp
	jmp		quit

## print 'dc: overflow happens' to stderr and terminate
printOverflow:
	pushl	$overflowError
	pushl	stderr
	call	fprintf
	addl	$8, %esp
	jmp 	quit

## quits the program
quit:
	## return 0
	movl    $0, %eax
	movl    %ebp, %esp
	popl    %ebp
	ret
