/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "dma.h"
#include "bios_tools.h"
#include "panic.h"

#define DMA_MASK_REG 			0x0A
#define DMA_MODE_REG 			0x0B
#define DMA_CLEAR_FF_REG 		0x0C
#define DMA_CH2_ADDR_REG 		0x04
#define DMA_CH2_COUNT_REG 		0x05
#define DMA_CH2_PAGE_REG 		0x81

void floppy_dma_init(uint8_t* buffer, uint32_t length, uint8_t mode)
{
	uint32_t phys_addr = (uintptr_t) buffer;
	uint32_t count = length - 1;	/* DMA expects length - 1 */

	/* Ensure address is in low memory (below 16MB) and doesn't cross 64KB boundary */
	if (phys_addr + length > 0xFFFFFF) {
		panic("DMA buffer physical address 0x%08X is above 16MB boundary", phys_addr);
		return;
	}
	if ((phys_addr >> 16) != ((phys_addr + count) >> 16)) {
		panic("DMA buffer at 0x%08X crosses a 64KB physical page boundary", phys_addr);
		return;
	}

	/* 1. Mask channel 2 (disable it temporarily) */
	outb(DMA_MASK_REG, 0x04 | 2);	/* bit 2 sets mask, bits 0-1 specify channel 2 */

	/* 2. Clear byte pointer flip-flop */
	outb(DMA_CLEAR_FF_REG, 0x00);

	/* 3. Set the address (low byte, then high byte) */
	outb(DMA_CH2_ADDR_REG, (uint8_t) (phys_addr & 0xFF));
	outb(DMA_CH2_ADDR_REG, (uint8_t) ((phys_addr >> 8) & 0xFF));

	/* 4. Clear flip-flop again */
	outb(DMA_CLEAR_FF_REG, 0x00);

	/* 5. Set the count (low byte, then high byte) */
	outb(DMA_CH2_COUNT_REG, (uint8_t) (count & 0xFF));
	outb(DMA_CH2_COUNT_REG, (uint8_t) ((count >> 8) & 0xFF));

	/* 6. Set the page address */
	outb(DMA_CH2_PAGE_REG, (uint8_t) ((phys_addr >> 16) & 0xFF));

	/* 7. Set DMA Mode */
	outb(DMA_MODE_REG, mode);

	/* 8. Unmask channel 2 (enable it) */
	outb(DMA_MASK_REG, 0x00 | 2);	/* bit 2 clear (enable), bits 0-1 channel 2 */
}
