/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

.global syscall_handler
.extern syscall_table

.code64
syscall_handler:
	# Save registers overwritten by syscall instruction (%rcx: return IP, %r11: RFLAGS)
	pushq %rcx
	pushq %r11

	# Align userspace registers with C calling convention
	# Userspace arg4 is in %r10, C expects it in %rcx
	movq %r10, %rcx

	# Check boundaries (If RAX >= [number], jump to bad_syscall)
	# REPLACEMAXSYSCALL
	cmpq $15, %rax
	jae .bad_syscall

	# Call syscall handler from table (pointers are 8 bytes on x86_64)
	call *syscall_table(,%rax,8)
	jmp .finish

.bad_syscall:
	movq $-255, %rax  # -ENOSYS

.finish:
	# Restore registers and return
	popq %r11
	popq %rcx
	sysretq
