/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

.include "kernel/syscalls_asm.inc"

.global syscall_handler
.extern syscall_table

syscall_handler:
	# Save syscall number
	pushl %eax
	# Argument 5
	pushl %edi
	# Argument 4
	pushl %esi
	# Argument 3
	pushl %edx
	# Argument 2
	pushl %ecx
	# Argument 1
	pushl %ebx

	# Check boundaries (If EAX >= [number], jump to bad_syscall)
	cmpl $MAX_SYSCALL, %eax
	jae .bad_syscall

	call *syscall_table(,%eax,4)
	jmp .finish

.bad_syscall:
	# Return -255 (-ENOSYS) in EAX
	movl $-255, %eax

.finish:
	# Clean up arguments from stack and restore registers
	# Skip EBX (or pop %ebx)
	addl $4, %esp
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	# Skip original EAX push
	addl $4, %esp

	# Clean 32-bit interrupt return
	iret
