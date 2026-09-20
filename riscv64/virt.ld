/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */
/* Originally from: https://github.com/DonaldKellett/marvelos/blob/9c15a0d988e68f7997cd29e93ad9e3b0bd2bb00d/src/lds/riscv64-virt.ld
 *
 * Copyright (c) 2022 Donald Sebastian Leung
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. */

/* Entry symbol is _start */
ENTRY(_start)

/* RAM starts at 0x80000000 - see virt.dts for details */
. = 0x80200000;

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

  . = ALIGN(4096);
  __free_ram = .;
  . += 64 * 1024 * 1024; /* 64MB */
  __free_ram_end = .;
}
