# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios

VERSION = 0.1.0.983

ifdef VERBOSE
    Q =
else
    Q = @
	MAKE_FLAGS += --no-print-directory
	CLEANMAKE_FLAGS = --silent
	SILENT_OUTPUT = > /dev/null
endif

ifneq ("$(wildcard kernel/kconfig.mk)","")
	include kernel/kconfig.mk
endif

ARCH ?= i386
include $(ARCH)/Makefile
include arches/$(ARCH).mk
ifneq (x$(META_ARCH),x)
	include $(META_ARCH)/Makefile
endif

include kernel/Makefile
ifneq ($(META_ARCH),)
	META_ARCH_FLAGS = -I$(META_ARCH) -DMETA_ARCH_$(META_ARCH)
else
	META_ARCH_FLAGS =
endif

ifdef LLVM
	CC = clang --target=$(ARCH_TARGET)
	LDFLAGS += -Wl,-z,noexecstack
else # GCC
	CC = $(ARCH_TARGET)-gcc
endif
HOSTCC ?= cc
HOSTCFLAGS ?= -O2 -pipe
QEMU = $(ARCH_QEMU)
CFLAGS ?= -O2 -pipe
CFLAGS += -Ikernel -Ithird-party -I$(ARCH) $(META_ARCH_FLAGS) -Iinclude -DARCH_$(ARCH) -DARCH=\"$(ARCH)\" -DVERSION=\"$(VERSION)\" -DPRINTF_DISABLE_SUPPORT_FLOAT -std=gnu99 -ffreestanding -Wall -Wextra -Wmissing-declarations -Wstack-protector -fstack-protector-all -Wno-ignored-attributes $(ARCH_CFLAGS)
OBJ = $(ARCH_OBJ) $(META_ARCH_OBJ) $(KERNEL_OBJ)

CONSOLECONFIG = kconfig/frontends/conf/kconfig-conf

all:
	$(Q)$(MAKE) $(MAKE_FLAGS) kernel/kconfig.mk kernel/kconfig.h
	$(Q)$(MAKE) $(MAKE_FLAGS) LineKernel LineKernel.gz

tools/config2%: ./tools/config2%.c
	@echo "HOSTCC $< -> $@"
	$(Q)$(HOSTCC) $(HOSTCFLAGS) $< -o $@

kernel/kconfig.h: .config tools/config2header
	@echo "CONFIG2 .config -> $@"
	$(Q)tools/config2header .config > kernel/kconfig.h

kernel/kconfig.mk: .config tools/config2makefile
	@echo "CONFIG2 .config -> $@"
	$(Q)tools/config2makefile .config > kernel/kconfig.mk

Kconfig: Kconfig.template
	@echo "CP $^ -> $@"
	$(Q)cp Kconfig.template Kconfig
	sed -i "s/{ARCH}/$(ARCH)/g" Kconfig
	sed -i "s/{VERSION}/$(VERSION)/g" Kconfig

kconfig/frontends/%:
	@echo "CONFIGURE kconfig"
	$(Q)cd kconfig; ./configure $(SILENT_OUTPUT)
	@echo "MAKE kconfig"
	$(Q)cd kconfig; $(MAKE) $(MAKE_FLAGS)

askconfig: Kconfig $(CONSOLECONFIG)
	$(CONSOLECONFIG) Kconfig --oldaskconfig

defconfig: allyesconfig
allyesconfig: Kconfig $(CONSOLECONFIG)
	$(CONSOLECONFIG) Kconfig --allyesconfig

allnoconfig: Kconfig $(CONSOLECONFIG)
	$(CONSOLECONFIG) Kconfig --allnoconfig

LineKernel: $(OBJ)
	@echo "LD $(words $(filter %, $^)) objects -> $@"
	$(Q)$(CC) -o LineKernel $(LDFLAGS) $(OBJ)

%.gz: %
	@echo "GZIP $< -> $@"
	$(Q)gzip -9 -n -f -k $<

%.o: %.c
	@echo "CC $< -> $@"
	$(Q)$(CC) -c $< -o $@ $(CFLAGS)

%.o: %.s
	@echo "AS $< -> $@"
	$(Q)$(CC) -c $< -o $@

run: all
	@echo "QEMU LineKernel"
	$(Q)$(QEMU) -m 2G -kernel LineKernel $(QEMUARGS)

LineKernel.iso: all
	@echo "CP LineKernel.gz -> iso/boot/LineKernel.gz"
	$(Q)cp LineKernel.gz iso/boot/LineKernel.gz
	grub-mkrescue -o LineKernel.iso iso

run-iso: LineKernel.iso
	@echo "QEMU $^"
	$(Q)$(QEMU) -cdrom $^ $(QEMUARGS)

CLEANOBJ = LineKernel LineKernel.gz iso/boot/LineKernel.gz LineKernel.iso $(OBJ) kernel/kconfig.h kernel/kconfig.mk tools/config2header tools/config2makefile
DISTCLEANOBJ = Kconfig .config .config.old kernel/LineKernel.qcow2 docs/

clean:
	@echo "RM $(words $(filter %, $(CLEANOBJ))) files"
	$(Q)rm -f $(CLEANOBJ)
	@echo "CLEAN kconfig"
	$(Q)cd kconfig; $(MAKE) $(MAKE_FLAGS) $(CLEANMAKEFLAGS) clean || true

distclean: clean
	@echo "RM $(words $(filter %, $(DISTCLEANOBJ))) files"
	$(Q)rm -rf $(DISTCLEANOBJ)
	@echo "DISTCLEAN kconfig"
	$(Q)cd kconfig; $(MAKE) $(MAKE_FLAGS) $(CLEANMAKEFLAGS) distclean || true

.PHONY: all Kconfig run run-iso clean distclean
