	.file	"result.bc"
	.text
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp1:
	.cfi_def_cfa_offset 16
	movl	$0, 4(%rsp)
	movl	$2000, (%rsp)           # imm = 0x7D0
	movslq	(%rsp), %rdi
	cmpq	$99, %rdi
	jg	.LBB0_3
# BB#1:                                 # %bounds
	movl	$230, globalArray(,%rdi,4)
	jmp	.LBB0_2
.LBB0_3:                                # %elseblock
	movl	$100, %esi
	callq	print
.LBB0_2:                                # %endBlock
	xorl	%eax, %eax
	popq	%rdx
	ret
.Ltmp2:
	.size	main, .Ltmp2-main
	.cfi_endproc

	.globl	print
	.align	16, 0x90
	.type	print,@function
print:                                  # @print
	.cfi_startproc
# BB#0:                                 # %entry
	subq	$24, %rsp
.Ltmp4:
	.cfi_def_cfa_offset 32
	movq	%rsi, %rax
	movq	%rdi, 16(%rsp)
	movq	%rax, 8(%rsp)
	movq	16(%rsp), %rsi
	movl	$.L.str, %edi
	movq	%rax, %rdx
	xorb	%al, %al
	callq	printf
	addq	$24, %rsp
	ret
.Ltmp5:
	.size	print, .Ltmp5-print
	.cfi_endproc

	.type	globalArray,@object     # @globalArray
	.comm	globalArray,400,16
	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	 "The first number is %ld and the second one is %ld\n"
	.size	.L.str, 51


	.section	".note.GNU-stack","",@progbits
