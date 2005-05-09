	.file	"base_quicksort.c"
	.section	".data"
	.align 4
	.type	stack,#object
	.size	stack,4
stack:
	.long	0
	.align 4
	.type	stack_index,#object
	.size	stack_index,4
stack_index:
	.long	0
	.section	".text"
	.align 4
	.global base_quicksort
	.type	base_quicksort,#function
	.proc	020
base_quicksort:
	!#PROLOGUE# 0
	save	%sp, -112, %sp
	!#PROLOGUE# 1
	sethi	%hi(-2147483648), %o1
	cmp	%o1, %i1
	bleu	.LL74
	mov	32, %o0
.LL18:
	srl	%o1, 1, %o1
	cmp	%o1, %i1
	bgu	.LL18
	add	%o0, -1, %o0
.LL74:
	sll	%o0, 4, %g1
	call	malloc, 0
	add	%g1, 32, %o0
	sethi	%hi(stack_index), %l2
	sethi	%hi(stack), %g1
	st	%o0, [%g1+%lo(stack)]
	add	%i1, -1, %l0
	mov	0, %l1
	mov	%l2, %l3
.LL42:
	sub	%l0, %l1, %o1
	cmp	%o1, 10
	ble	.LL158
	add	%l1, %l0, %o5
	srl	%o5, 31, %o0
	add	%o5, %o0, %o4
	sra	%o4, 1, %o3
	sll	%l0, 2, %o5
	sll	%o3, 2, %g1
	add	%i0, %o5, %o4
	ld	[%i0+%g1], %o2
	sll	%l1, 2, %o3
	ld	[%o4-4], %o7
	st	%o7, [%i0+%g1]
	st	%o2, [%o4-4]
	ld	[%i0+%o3], %o1
	cmp	%o2, %o1
	bgeu,a	.LL163
	ld	[%i0+%o5], %o0
	st	%o2, [%i0+%o3]
	st	%o1, [%o4-4]
	ld	[%i0+%o3], %o1
	ld	[%i0+%o5], %o0
.LL163:
	cmp	%o0, %o1
	bgeu,a	.LL164
	ld	[%o4-4], %o1
	st	%o0, [%i0+%o3]
	st	%o1, [%i0+%o5]
	mov	%o1, %o0
	ld	[%o4-4], %o1
.LL164:
	cmp	%o0, %o1
	bgeu	.LL165
	add	%l0, -1, %o2
	st	%o0, [%o4-4]
	st	%o1, [%i0+%o5]
.LL165:
	add	%l1, 1, %o1
	call	partition, 0
	mov	%i0, %o0
	sub	%o0, %l1, %g1
	sub	%l0, %o0, %o2
	cmp	%g1, %o2
	bg	.LL159
	mov	%o0, %o5
	ld	[%l2+%lo(stack_index)], %o3
	sethi	%hi(stack), %o0
	ld	[%o0+%lo(stack)], %g1
	add	%o3, 1, %o7
	sll	%o3, 2, %o4
	st	%l0, [%g1+%o4]
	add	%o5, 1, %o1
	sll	%o7, 2, %o2
	add	%o7, 1, %l0
	st	%o1, [%g1+%o2]
	st	%l0, [%l2+%lo(stack_index)]
	b	.LL42
	add	%o5, -1, %l0
.LL159:
	ld	[%l3+%lo(stack_index)], %o3
	sethi	%hi(stack), %o0
	ld	[%o0+%lo(stack)], %o2
	sll	%o3, 2, %o4
	add	%o3, 1, %o7
	add	%o5, -1, %g1
	st	%g1, [%o2+%o4]
	sll	%o7, 2, %o3
	st	%l1, [%o2+%o3]
	add	%o7, 1, %o1
	st	%o1, [%l3+%lo(stack_index)]
	b	.LL42
	add	%o5, 1, %l1
.LL158:
	sethi	%hi(stack_index), %o4
	ld	[%o4+%lo(stack_index)], %o3
	cmp	%o3, 0
	be	.LL21
	sethi	%hi(stack), %l1
	add	%o3, -1, %l0
	ld	[%l1+%lo(stack)], %o5
	st	%l0, [%o4+%lo(stack_index)]
	sll	%l0, 2, %o7
	ld	[%o5+%o7], %l1
	add	%o3, -2, %o2
	st	%o2, [%o4+%lo(stack_index)]
	sll	%o2, 2, %o0
	b	.LL42
	ld	[%o5+%o0], %l0
.LL21:
	sethi	%hi(stack), %l0
	ld	[%l0+%lo(stack)], %o0
	cmp	%o0, 0
	bne	.LL160
	nop
	cmp	%i1, 10
.LL162:
	bg	.LL45
	st	%g0, [%l0+%lo(stack)]
	mov	1, %o3
	cmp	%o3, %i1
	bge	.LL76
	mov	0, %o4
	sll	%o3, 2, %o2
.LL166:
	ld	[%i0+%o4], %l2
	ld	[%i0+%o2], %l3
	cmp	%l3, %l2
	bgeu	.LL48
	add	%o3, 1, %o3
	mov	%o2, %o4
.LL48:
	cmp	%o3, %i1
	bl	.LL166
	sll	%o3, 2, %o2
.LL76:
	ld	[%i0], %l1
	ld	[%i0+%o4], %o5
	st	%o5, [%i0]
	st	%l1, [%i0+%o4]
.LL54:
	mov	2, %o7
	cmp	%o7, %i1
	bge	.LL80
	add	%i1, -2, %o2
	cmp	%i1, 3
	bg	.LL161
	and	%o2, 3, %o0
.LL88:
	sll	%o7, 2, %l0
	add	%i0, %l0, %o2
	ld	[%o2-4], %l1
	mov	%o7, %o3
	ld	[%i0+%l0], %o4
	cmp	%o4, %l1
	bgeu	.LL103
	mov	%l0, %o2
.LL101:
	add	%i0, %o2, %l2
.LL167:
	ld	[%l2-4], %g1
	add	%o3, -1, %o3
	st	%g1, [%i0+%o2]
	sll	%o3, 2, %o2
	add	%i0, %o2, %o1
	ld	[%o1-4], %o5
	cmp	%o4, %o5
	blu	.LL167
	add	%i0, %o2, %l2
.LL103:
	add	%o7, 1, %o7
	cmp	%o7, %i1
	bge	.LL80
	st	%o4, [%i0+%o2]
.LL71:
	sll	%o7, 2, %l1
	add	%i0, %l1, %o4
	ld	[%o4-4], %l3
	mov	%o7, %o3
	ld	[%i0+%l1], %o4
	cmp	%o4, %l3
	bgeu	.LL108
	mov	%l1, %o2
.LL106:
	add	%i0, %o2, %o1
.LL168:
	ld	[%o1-4], %g1
	add	%o3, -1, %o3
	st	%g1, [%i0+%o2]
	sll	%o3, 2, %o2
	add	%i0, %o2, %o5
	ld	[%o5-4], %l0
	cmp	%o4, %l0
	blu	.LL168
	add	%i0, %o2, %o1
.LL108:
	add	%o7, 1, %l1
	sll	%l1, 2, %l3
	st	%o4, [%i0+%o2]
	add	%i0, %l3, %o2
	ld	[%o2-4], %l2
	mov	%l1, %o3
	ld	[%i0+%l3], %o4
	cmp	%o4, %l2
	bgeu	.LL113
	mov	%l3, %o2
.LL111:
	add	%i0, %o2, %o1
.LL169:
	ld	[%o1-4], %g1
	add	%o3, -1, %o3
	st	%g1, [%i0+%o2]
	sll	%o3, 2, %o2
	add	%i0, %o2, %o5
	ld	[%o5-4], %l0
	cmp	%o4, %l0
	blu	.LL169
	add	%i0, %o2, %o1
.LL113:
	add	%o7, 2, %l1
	st	%o4, [%i0+%o2]
	sll	%l1, 2, %l3
	add	%i0, %l3, %o4
	ld	[%o4-4], %l2
	mov	%l1, %o3
	ld	[%i0+%l3], %o4
	cmp	%o4, %l2
	bgeu	.LL118
	mov	%l3, %o2
.LL116:
	add	%i0, %o2, %o1
.LL170:
	ld	[%o1-4], %g1
	add	%o3, -1, %o3
	st	%g1, [%i0+%o2]
	sll	%o3, 2, %o2
	add	%i0, %o2, %o5
	ld	[%o5-4], %l0
	cmp	%o4, %l0
	blu	.LL170
	add	%i0, %o2, %o1
.LL118:
	add	%o7, 3, %l1
	sll	%l1, 2, %l3
	st	%o4, [%i0+%o2]
	add	%i0, %l3, %o2
	ld	[%o2-4], %l2
	mov	%l1, %o3
	ld	[%i0+%l3], %o4
	cmp	%o4, %l2
	bgeu	.LL123
	mov	%l3, %o2
.LL121:
	add	%i0, %o2, %o1
.LL171:
	ld	[%o1-4], %g1
	add	%o3, -1, %o3
	st	%g1, [%i0+%o2]
	sll	%o3, 2, %o2
	add	%i0, %o2, %o5
	ld	[%o5-4], %l0
	cmp	%o4, %l0
	blu	.LL171
	add	%i0, %o2, %o1
.LL123:
	add	%o7, 4, %o7
	cmp	%o7, %i1
	bl	.LL71
	st	%o4, [%i0+%o2]
	b,a	.LL80
.LL161:
	cmp	%o0, 0
	be	.LL71
	cmp	%o0, 1
	ble	.LL88
	cmp	%o0, 2
	ble	.LL172
	sll	%o7, 2, %o1
	ld	[%i0+8], %o3
	mov	2, %o4
	ld	[%i0+4], %g1
	cmp	%o3, %g1
	bgeu	.LL93
	sll	%o7, 2, %o2
	mov	8, %o2
.LL91:
	add	%i0, %o2, %o5
.LL173:
	ld	[%o5-4], %l1
	add	%o4, -1, %o4
	st	%l1, [%i0+%o2]
	sll	%o4, 2, %o2
	add	%i0, %o2, %l3
	ld	[%l3-4], %l2
	cmp	%o3, %l2
	blu	.LL173
	add	%i0, %o2, %o5
.LL93:
	st	%o3, [%i0+%o2]
	add	%o7, 1, %o7
	sll	%o7, 2, %o1
.LL172:
	add	%i0, %o1, %o4
	ld	[%o4-4], %l0
	mov	%o7, %o5
	ld	[%i0+%o1], %o4
	cmp	%o4, %l0
	bgeu	.LL98
	mov	%o1, %o2
.LL96:
	add	%i0, %o2, %l3
.LL174:
	ld	[%l3-4], %l2
	add	%o5, -1, %o5
	st	%l2, [%i0+%o2]
	sll	%o5, 2, %o2
	add	%i0, %o2, %g1
	ld	[%g1-4], %o3
	cmp	%o4, %o3
	blu	.LL174
	add	%i0, %o2, %l3
.LL98:
	st	%o4, [%i0+%o2]
	b	.LL88
	add	%o7, 1, %o7
.LL45:
	ld	[%i0+4], %o7
	ld	[%i0], %l0
	cmp	%o7, %l0
	bgeu	.LL126
	mov	0, %o2
	mov	4, %o2
.LL126:
	ld	[%i0+8], %o4
	ld	[%i0+%o2], %g1
	cmp	%o4, %g1
	blu,a	.LL129
	mov	8, %o2
.LL129:
	ld	[%i0+12], %o0
	ld	[%i0+%o2], %o3
	cmp	%o0, %o3
	blu,a	.LL132
	mov	12, %o2
.LL132:
	ld	[%i0+16], %l3
	ld	[%i0+%o2], %l2
	cmp	%l3, %l2
	blu,a	.LL135
	mov	16, %o2
.LL135:
	ld	[%i0+20], %o5
	ld	[%i0+%o2], %l1
	cmp	%o5, %l1
	blu,a	.LL138
	mov	20, %o2
.LL138:
	ld	[%i0+24], %o7
	ld	[%i0+%o2], %l0
	cmp	%o7, %l0
	blu,a	.LL141
	mov	24, %o2
.LL141:
	ld	[%i0+28], %o4
	ld	[%i0+%o2], %g1
	cmp	%o4, %g1
	blu,a	.LL144
	mov	28, %o2
.LL144:
	ld	[%i0+32], %o0
	ld	[%i0+%o2], %o3
	cmp	%o0, %o3
	blu,a	.LL147
	mov	32, %o2
.LL147:
	ld	[%i0+36], %l3
	ld	[%i0+%o2], %l2
	cmp	%l3, %l2
	blu,a	.LL150
	mov	36, %o2
.LL150:
	ld	[%i0+40], %o5
	ld	[%i0+%o2], %l1
	cmp	%o5, %l1
	blu,a	.LL153
	mov	40, %o2
.LL153:
	ld	[%i0], %l0
	ld	[%i0+%o2], %o1
	st	%o1, [%i0]
	b	.LL54
	st	%l0, [%i0+%o2]
.LL160:
	call	free, 0
	 nop
	b	.LL162
	cmp	%i1, 10
.LL80:
	nop
	ret
	restore
.LLfe1:
	.size	base_quicksort,.LLfe1-base_quicksort
	.align 4
	.type	partition,#function
	.proc	04
partition:
	!#PROLOGUE# 0
	save	%sp, -112, %sp
	!#PROLOGUE# 1
	sll	%i2, 2, %o0
	ld	[%i0+%o0], %o7
	mov	%i0, %g1
	add	%i1, -1, %i1
.LL5:
	add	%i1, 1, %i1
.LL208:
	sll	%i1, 2, %i4
	ld	[%g1+%i4], %i3
	cmp	%i3, %o7
	blu,a	.LL208
	add	%i1, 1, %i1
	add	%i2, -1, %i2
.LL209:
	sll	%i2, 2, %i5
	ld	[%g1+%i5], %i3
	cmp	%o7, %i3
	blu,a	.LL209
	add	%i2, -1, %i2
	cmp	%i1, %i2
	bge,a	.LL3
	ld	[%g1+%i4], %i3
	ld	[%g1+%i4], %i0
	st	%i3, [%g1+%i4]
	b	.LL5
	st	%i0, [%g1+%i5]
.LL3:
	ld	[%g1+%o0], %i2
	st	%i2, [%g1+%i4]
	st	%i3, [%g1+%o0]
	ret
	restore %g0, %i1, %o0
.LLfe2:
	.size	partition,.LLfe2-partition
	.ident	"GCC: (GNU) 3.2"
