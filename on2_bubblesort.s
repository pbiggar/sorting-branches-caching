	.file	"on2_bubblesort.c"
	.section	".text"
	.align 4
	.global on2_bubblesort
	.type	on2_bubblesort,#function
	.proc	020
on2_bubblesort:
	!#PROLOGUE# 0
	save	%sp, -112, %sp
	!#PROLOGUE# 1
	add	%i1, -1, %i1
	mov	0, %g1
	cmp	%g1, %i1
	bge	.LL1
	mov	%i1, %o0
.LL14:
	cmp	%i1, %g1
	ble	.LL17
	mov	1, %o7
	sll	%i1, 2, %i2
.LL18:
	add	%i0, %i2, %i5
	ld	[%i0+%i2], %i3
	ld	[%i5-4], %i4
	cmp	%i3, %i4
	bgeu	.LL8
	add	%i1, -1, %i1
	st	%i3, [%i5-4]
	mov	0, %o7
	st	%i4, [%i0+%i2]
.LL8:
	cmp	%i1, %g1
	bg	.LL18
	sll	%i1, 2, %i2
.LL17:
	cmp	%o7, 0
	bne	.LL1
	add	%g1, 1, %g1
	cmp	%g1, %o0
	bl	.LL14
	mov	%o0, %i1
.LL1:
	nop
	ret
	restore
.LLfe1:
	.size	on2_bubblesort,.LLfe1-on2_bubblesort
	.align 4
	.global on2_bubblesort2
	.type	on2_bubblesort2,#function
	.proc	020
on2_bubblesort2:
	!#PROLOGUE# 0
	save	%sp, -112, %sp
	!#PROLOGUE# 1
	add	%i1, -1, %i1
	mov	0, %o7
	cmp	%o7, %i1
	bge	.LL34
	mov	%i1, %o0
.LL32:
	cmp	%i1, %o7
	ble	.LL36
	mov	%i1, %i3
	sll	%i3, 2, %i2
.LL38:
	add	%i0, %i2, %i5
	ld	[%i0+%i2], %i4
	ld	[%i5-4], %g1
	cmp	%i4, %g1
	bgeu,a	.LL37
	add	%i3, -1, %i3
	st	%i4, [%i5-4]
	mov	%i3, %i1
	st	%g1, [%i0+%i2]
	add	%i3, -1, %i3
.LL37:
	cmp	%i3, %o7
	bg	.LL38
	sll	%i3, 2, %i2
.LL36:
	mov	%i1, %o7
	cmp	%o7, %o0
	bl	.LL32
	mov	%o0, %i1
.LL34:
	nop
	ret
	restore
.LLfe2:
	.size	on2_bubblesort2,.LLfe2-on2_bubblesort2
	.ident	"GCC: (GNU) 3.2"
