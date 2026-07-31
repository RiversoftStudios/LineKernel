/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */
/* Originally from: https://github.com/DonaldKellett/marvelos/blob/9c15a0d988e68f7997cd29e93ad9e3b0bd2bb00d/src/lds/riscv64-virt.ld */

/* Entry symbol is _start */
ENTRY(_start)

/* RAM starts at 0x80000000 - see virt.dts for details */
. = 0x80000000;

SECTIONS {
  /**
	* ELF file layout: text section at bottom, followed by data and bss
	* sections
	*
	* Global pointer starts where BSS ends
	* The heap also starts right after the BSS section, growing upwards
	* The stack grows downwards, starting from the end of RAM
	* This mean there's a chance stack and heap collide, but we'll assume
	* it never happens
	*/
  .text : ALIGN(4K) {
	*(.init);
	*(.text);
  }
  .data : ALIGN(4K) {
	*(.data);
  }
  .bss : ALIGN(4K) {
	PROVIDE(__bss_start = .);
	*(.bss);
	PROVIDE(__bss_end = .);
	PROVIDE(__global_pointer = .);
	PROVIDE(__heap_start = .);
  }
  . = 0x88000000;
  PROVIDE(__heap_end = .);
  PROVIDE(__stack_top = .);
}
