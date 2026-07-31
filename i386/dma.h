/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#ifndef __DMA_H__
#define __DMA_H__

#include <stdint.h>

#define DMA_MODE_READ		0x46	/* Read from disk (transfers memory -> FDC write) */
#define DMA_MODE_WRITE		0x4A	/* Write to disk (transfers FDC read -> memory) */

void floppy_dma_init(uint8_t * buffer, uint32_t length, uint8_t mode);

#endif
