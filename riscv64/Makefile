# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios
ifeq (x$(CONFIG_VIRT_MACHINE), xy)
	LINKSCRIPT = virt
endif
LDFLAGS = -T riscv64/$(LINKSCRIPT).ld -ffreestanding -O2 -nostdlib
ARCH_OBJ = riscv64/boot.o riscv64/power.o riscv64/syscall.o riscv64/sbi.o
