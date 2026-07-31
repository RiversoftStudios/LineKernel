/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

/* Allocate space for our early 4-level page tables.
 * We must align page tables to 4KB boundaries. */
.section .bss
.align 4096
pml4_table:
	.skip 4096
pdp_table:
	.skip 4096
pd_table:
	.skip 4096

.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.global _start
.type _start, @function

/* Must be 32-bits here! */
.code32

_start:
	/* 1. Setup 32-bit stack pointer */
	mov $stack_top, %esp

	/* Save multiboot data */
	mov %ebx, %esi /* Magic */
	mov %eax, %edi /* MBI Pointer */

	/* 2. Verify CPUID and Long Mode availability */
	pushf
	pop %eax
	mov %eax, %ecx
	xor $1 << 21, %eax
	push %eax
	popf
	pushf
	pop %eax
	push %ecx
	popf
	cmp %ecx, %eax
	je .no_cpuid

	mov $0x80000000, %eax
	cpuid
	cmp $0x80000001, %eax
	jb .no_long_mode

	mov $0x80000001, %eax
	cpuid
	test $1 << 29, %edx
	jz .no_long_mode

	/* 3. Setup Early Page Tables */
	movl $pdp_table, %eax
	orl $0x3, %eax
	movl %eax, pml4_table

	movl $pdp_table, %eax
	orl $0x3, %eax
	movl %eax, pml4_table

	movl $pd_table, %eax
	orl $0x3, %eax
	movl %eax, pdp_table

	movl $0, %ecx
.map_pd_table:
	movl $0x200000, %eax
	mull %ecx
	orl $0x83, %eax
	movl %eax, pd_table(, %ecx, 8)
	inc %ecx
	cmp $512, %ecx
	jne .map_pd_table

	/* 4. Enable PAE and load PML4 */
	mov %cr4, %eax
	or $1 << 5, %eax
	mov %eax, %cr4

	mov $pml4_table, %eax
	mov %eax, %cr3

	/* 5. Enable Long Mode in EFER MSR */
	mov $0xC0000080, %ecx
	rdmsr
	or $1 << 8, %eax
	wrmsr

	/* 6. Enable Paging */
	mov %cr0, %eax
	or $1 << 31, %eax
	mov %eax, %cr0

	/* 7. Load 64-bit Global Descriptor Table */
	lgdt gdt64_pointer

	/* 8. Far Jump to 64-bit Long Mode code segment */
	/* Note: Explicitly using ljmpl tells the assembler to use a 32-bit operand
		for the jump while still generating the correct AT&T syntax. */
	ljmpl $0x08, $_start64

.no_cpuid:
.no_long_mode:
	cli
1:	hlt
	jmp 1b

/* --- 64-bit Code Segment --- */
.code64
_start64:
	/* Reload data segment registers for long mode */
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss

	mov %edi, %edi /* Magic */
	mov %esi, %esi /* MBI Pointer */

	call multiboot_pre_main

	cli
1:	hlt
	jmp 1b

/* --- Global Descriptor Table Data --- */
.section .rodata
.align 8
gdt64:
	.quad 0x0000000000000000 /* Null Descriptor */
	.quad 0x00209A0000000000 /* 64-bit Code Descriptor (Exec/Read, Long Mode bit) */
	.quad 0x0000920000000000 /* 64-bit Data Descriptor (Read/Write) */
gdt64_pointer:
	.word . - gdt64 - 1 /* GDT Limit */
	.long gdt64 /* GDT Base Address */
