/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

.global syscall_handler
.extern syscall_table

.align 4
syscall_handler:
	# 1. Allocate context frame and save registers to avoid corrupting user state
	addi sp, sp, -256
	sd ra, 0(sp)
	sd t0, 8(sp)
	sd t1, 16(sp)
	sd t2, 24(sp)
	sd t3, 32(sp)
	sd t4, 40(sp)
	sd t5, 48(sp)
	sd t6, 56(sp)
	sd a1, 64(sp)
	sd a2, 72(sp)
	sd a3, 80(sp)
	sd a4, 88(sp)
	sd a5, 96(sp)
	sd a6, 104(sp)
	sd a0, 112(sp)
	sd a7, 120(sp)

	# 2. Check if the trap cause was an ecall (cause 8: U-mode, 9: S-mode, 11: M-mode)
	csrr t0, mcause
	li t1, 8
	beq t0, t1, .is_syscall
	li t1, 9
	beq t0, t1, .is_syscall
	li t1, 11
	beq t0, t1, .is_syscall

	# Non-syscall traps: loop forever
	j .panic_trap

.is_syscall:
	# 3. Check boundaries (If a7 >= [number], jump to bad_syscall)
	# REPLACEMAXSYSCALL
	li t0, 15
	bgeu a7, t0, .bad_syscall

	# 4. Invoke the C handler
	la t1, syscall_table
	# Multiply index (a7) by 8 (64-bit pointers)
	slli t2, a7, 3
	add t2, t1, t2
	# Load function pointer
	ld t3, 0(t2)
	# Call function (arguments are already in a0-a5)
	jalr ra, t3

	# 5. Save the C return value (in a0) to the stack frame slot for a0
	sd a0, 112(sp)
	j .finish

.bad_syscall:
	li a0, -255 # -ENOSYS
	sd a0, 112(sp)

.finish:
	# 6. CRITICAL: Advance the exception program counter (mepc) by 4 bytes.
	# The PC saved during ecall points to the ecall instruction itself.
	csrr t0, mepc
	addi t0, t0, 4
	csrw mepc, t0

	# 7. Restore registers and return via mret
	ld ra, 0(sp)
	ld t0, 8(sp)
	ld t1, 16(sp)
	ld t2, 24(sp)
	ld t3, 32(sp)
	ld t4, 40(sp)
	ld t5, 48(sp)
	ld t6, 56(sp)
	ld a1, 64(sp)
	ld a2, 72(sp)
	ld a3, 80(sp)
	ld a4, 88(sp)
	ld a5, 96(sp)
	ld a6, 104(sp)
	# Restores C return value to user a0
	ld a0, 112(sp)
	ld a7, 120(sp)
	addi sp, sp, 256
	mret

.panic_trap:
1:	j 1b
