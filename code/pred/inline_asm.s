	.file	"inline_asm.c"
	.version	"01.01"
gcc2_compiled.:
.text
	.align 4
.globl higher_level_partition
	.type	 higher_level_partition,@function
higher_level_partition:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$36, %esp
	movl	12(%ebp), %eax
	incl	%eax
	movl	%eax, -4(%ebp)
	movl	16(%ebp), %eax
	decl	%eax
	movl	%eax, -8(%ebp)
	movl	16(%ebp), %eax
	imull	$4, %eax, %edx
	movl	8(%ebp), %eax
	movl	(%eax,%edx), %eax
	movl	%eax, -12(%ebp)
	movl	12(%ebp), %eax
	imull	$4, %eax, %edx
	movl	8(%ebp), %eax
	movl	(%eax,%edx), %eax
	movl	%eax, -16(%ebp)
	.p2align 2
.L3:
	movl	-16(%ebp), %edx
	movl	$0, %eax
	cmpl	-12(%ebp), %edx
	setb	%al
	movl	%eax, -20(%ebp)
	cmpl	$0, -20(%ebp)
	je	.L6
	movl	-4(%ebp), %eax
	movl	%eax, -36(%ebp)
	jmp	.L7
	.p2align 2
.L6:
	movl	-8(%ebp), %eax
	movl	%eax, -36(%ebp)
.L7:
	movl	-36(%ebp), %eax
	movl	%eax, -24(%ebp)
	movl	$0, %eax
	cmpl	$0, -20(%ebp)
	setne	%al
	movl	%eax, -28(%ebp)
	movl	-28(%ebp), %edx
	leal	-4(%ebp), %eax
	addl	%edx, (%eax)
	movl	-28(%ebp), %eax
	addl	-8(%ebp), %eax
	decl	%eax
	movl	%eax, -8(%ebp)
	movl	-16(%ebp), %eax
	movl	%eax, -32(%ebp)
	movl	-24(%ebp), %eax
	imull	$4, %eax, %edx
	movl	8(%ebp), %eax
	movl	(%eax,%edx), %eax
	movl	%eax, -16(%ebp)
	movl	-24(%ebp), %eax
	imull	$4, %eax, %ecx
	movl	8(%ebp), %edx
	movl	-32(%ebp), %eax
	movl	%eax, (%edx,%ecx)
	movl	-4(%ebp), %eax
	cmpl	-8(%ebp), %eax
	jl	.L3
	nop
	movl	-8(%ebp), %eax
	imull	$4, %eax, %eax
	addl	8(%ebp), %eax
	addl	$4, %eax
	movl	(%eax), %eax
	movl	%eax, -32(%ebp)
	movl	-8(%ebp), %eax
	imull	$4, %eax, %eax
	addl	8(%ebp), %eax
	leal	4(%eax), %ecx
	movl	16(%ebp), %eax
	imull	$4, %eax, %edx
	movl	8(%ebp), %eax
	movl	(%eax,%edx), %eax
	movl	%eax, (%ecx)
	movl	16(%ebp), %eax
	imull	$4, %eax, %ecx
	movl	8(%ebp), %edx
	movl	-32(%ebp), %eax
	movl	%eax, (%edx,%ecx)
	movl	-8(%ebp), %eax
	incl	%eax
	movl	%eax, %eax
	leave
	ret
.Lfe1:
	.size	 higher_level_partition,.Lfe1-higher_level_partition
	.align 4
.globl partition
	.type	 partition,@function
partition:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%edi
	pushl	%esi
	pushl	%ebx
	subl	$12, %esp
	movl	12(%ebp), %ecx
	movl	16(%ebp), %edx
	movl	8(%ebp), %eax
#APP
	movl	(%eax, %edx, 4), %esi
	movl	(%eax, %ecx, 4), %edi
	incl	%ecx
	decl	%edx
	
.LOOP_LABEL132:
	movl 	%edx, %ebx
	cmpl 	%esi, %edi
	cmovb 	%ecx, %ebx
	xchgl	(%eax, %ebx, 4), %edi
	movl	$0, %ebx
	setb	%bl
	addl	%ebx, %ecx
	decl	%edx
	addl	%ebx, %edx
	cmpl	%ecx, %edx
	jae	.LOOP_LABEL132
	incl	%edx
	xchgl	%esi, (%eax, %edx, 4)
	
#NO_APP
	movl	%ecx, %eax
	movl	%eax, -20(%ebp)
	movl	%edx, %eax
	movl	%eax, -24(%ebp)
	movl	%esi, %eax
	movl	%eax, -16(%ebp)
	movl	16(%ebp), %ebx
	movl	8(%ebp), %eax
	movl	-16(%ebp), %esi
#APP
	movl	%esi,(%eax, %ebx, 4)
	
#NO_APP
	movl	-24(%ebp), %eax
	movl	%eax, %eax
	addl	$12, %esp
	popl	%ebx
	popl	%esi
	popl	%edi
	popl	%ebp
	ret
.Lfe2:
	.size	 partition,.Lfe2-partition
		.section	.rodata
.LC0:
	.string	"a[%d] = %d\n"
.LC1:
	.string	"pivot is %d at position %d\n"
.LC2:
	.string	"b:\n"
.LC3:
	.string	"bad b: k = %d, b[k] = %d\n"
.text
	.align 4
.globl main
	.type	 main,@function
main:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$500, %esp
	subl	$4, %esp
	pushl	$400
	pushl	$0
	leal	-472(%ebp), %eax
	pushl	%eax
	call	memset
	addl	$16, %esp
	movl	$0, -12(%ebp)
	.p2align 2
.L19:
	cmpl	$9, -12(%ebp)
	jle	.L22
	jmp	.L20
	.p2align 2
.L22:
	movl	-12(%ebp), %eax
	movl	%eax, %eax
	leal	0(,%eax,4), %ebx
	leal	-72(%ebp), %eax
	movl	%eax, %ecx
	movl	-12(%ebp), %edx
	movl	%edx, %eax
	sall	$1, %eax
	addl	%edx, %eax
	leal	0(,%eax,4), %edx
	movl	$10, -492(%ebp)
	movl	%edx, %eax
	cltd
	idivl	-492(%ebp)
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	sall	$1, %eax
	movl	%eax, (%ebx,%ecx)
	movl	-12(%ebp), %eax
	movl	%eax, %eax
	leal	0(,%eax,4), %edx
	leal	-72(%ebp), %eax
	movl	%eax, %eax
	movl	(%edx,%eax), %eax
	movl	%eax, %eax
	leal	0(,%eax,4), %edx
	leal	-472(%ebp), %eax
	movl	%eax, %eax
	incl	(%edx,%eax)
	leal	-12(%ebp), %eax
	incl	(%eax)
	jmp	.L19
	.p2align 2
.L20:
	movl	$25, -36(%ebp)
	movl	$0, -12(%ebp)
	.p2align 2
.L23:
	cmpl	$9, -12(%ebp)
	jle	.L26
	jmp	.L24
	.p2align 2
.L26:
	subl	$4, %esp
	movl	-12(%ebp), %eax
	movl	%eax, %eax
	leal	0(,%eax,4), %edx
	leal	-72(%ebp), %eax
	movl	%eax, %eax
	pushl	(%edx,%eax)
	pushl	-12(%ebp)
	pushl	$.LC0
	call	printf
	addl	$16, %esp
	leal	-12(%ebp), %eax
	incl	(%eax)
	jmp	.L23
	.p2align 2
.L24:
	subl	$4, %esp
	pushl	$9
	pushl	$0
	leal	-72(%ebp), %eax
	pushl	%eax
	call	higher_level_partition
	addl	$16, %esp
	movl	%eax, %eax
	movl	%eax, -12(%ebp)
	subl	$4, %esp
	pushl	-12(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, %eax
	leal	0(,%eax,4), %edx
	leal	-72(%ebp), %eax
	movl	%eax, %eax
	pushl	(%edx,%eax)
	pushl	$.LC1
	call	printf
	addl	$16, %esp
	movl	$0, -12(%ebp)
	.p2align 2
.L27:
	cmpl	$9, -12(%ebp)
	jle	.L30
	jmp	.L28
	.p2align 2
.L30:
	subl	$4, %esp
	movl	-12(%ebp), %eax
	movl	%eax, %eax
	leal	0(,%eax,4), %edx
	leal	-72(%ebp), %eax
	movl	%eax, %eax
	pushl	(%edx,%eax)
	pushl	-12(%ebp)
	pushl	$.LC0
	call	printf
	addl	$16, %esp
	movl	-12(%ebp), %eax
	movl	%eax, %eax
	leal	0(,%eax,4), %edx
	leal	-72(%ebp), %eax
	movl	%eax, %eax
	movl	(%edx,%eax), %eax
	movl	%eax, %eax
	leal	0(,%eax,4), %edx
	leal	-472(%ebp), %eax
	movl	%eax, %eax
	decl	(%edx,%eax)
	leal	-12(%ebp), %eax
	incl	(%eax)
	jmp	.L27
	.p2align 2
.L28:
	subl	$12, %esp
	pushl	$.LC2
	call	printf
	addl	$16, %esp
	movl	$0, -12(%ebp)
	.p2align 2
.L31:
	cmpl	$99, -12(%ebp)
	jle	.L34
	jmp	.L32
	.p2align 2
.L34:
	movl	-12(%ebp), %eax
	movl	%eax, %eax
	leal	0(,%eax,4), %edx
	leal	-472(%ebp), %eax
	movl	%eax, %eax
	cmpl	$0, (%edx,%eax)
	je	.L33
	subl	$4, %esp
	movl	-12(%ebp), %eax
	movl	%eax, %eax
	leal	0(,%eax,4), %edx
	leal	-472(%ebp), %eax
	movl	%eax, %eax
	pushl	(%edx,%eax)
	pushl	-12(%ebp)
	pushl	$.LC3
	call	printf
	addl	$16, %esp
.L33:
	leal	-12(%ebp), %eax
	incl	(%eax)
	jmp	.L31
	.p2align 2
.L32:
	movl	$0, %eax
	movl	-4(%ebp), %ebx
	leave
	ret
.Lfe3:
	.size	 main,.Lfe3-main
	.ident	"GCC: (GNU) 2.96 20000731 (Red Hat Linux 7.1 2.96-98)"
