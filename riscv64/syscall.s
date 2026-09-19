/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

.global syscall_handler
.extern syscall_table

.align 4
syscall_handler:
	# 1. Allocate context frame on kernel stack and save registers
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

	# Save sstatus to preserve trap states
	csrr t0, sstatus
	sd t0, 128(sp)

	# 2. Verify exception cause (U-mode ecall = 8)
	csrr t0, scause
	li t1, 8
	beq t0, t1, .is_syscall

	# Non-syscall traps (page faults, interrupts, illegal instrs, etc.)
	j .panic_trap

.is_syscall:
	# 3. Boundary check (a7 holds syscall index)
	# REPLACEMAXSYSCALL
	li t0, 15
	bgeu a7, t0, .bad_syscall

	# 4. Invoke the C handler from syscall_table
	la t1, syscall_table
	slli t2, a7, 3 # Index * 8 (64-bit pointers)
	add t2, t1, t2
	ld t3, 0(t2) # Load function pointer

	# Arguments (a0-a5) are already set up according to calling convention
	jalr ra, t3

	# Save C return value (returned in a0) into stack slot for restoring to user a0
	sd a0, 112(sp)
	j .finish

.bad_syscall:
	li a0, -255 # -ENOSYS
	sd a0, 112(sp)

.finish:
	# 5. Advance sepc past the 4-byte ecall instruction
	csrr t0, sepc
	addi t0, t0, 4
	csrw sepc, t0

	# 6. Restore kernel/user registers and return to S/U mode
	ld t0, 128(sp)
	csrw sstatus, t0

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
	ld a0, 112(sp) # Loads the return value into user a0
	ld a7, 120(sp)
	addi sp, sp, 256

	sret # Return from supervisor trap

.panic_trap:
1: j 1b