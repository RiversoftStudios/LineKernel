/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "floppy.h"
#include "idt.h"
#include "dma.h"
#include "timer.h"
#include "bios_tools.h"
#include "panic.h"
#include "printf.h"

#ifdef CONFIG_FLOPPY
/* FDC registers */
#define FDC_DOR 		0x3F2
#define FDC_MSR 		0x3F4
#define FDC_FIFO 		0x3F5
#define FDC_CCR 		0x3F7

#define READ_WRITE_TIMEOUT 100000

static volatile int fdc_irq_fired = 0;
static uint8_t floppy_dma_buffer[512] __attribute__((aligned(4096)));

static void fdc_irq_handler(struct regs* r)
{
	(void)r;
	fdc_irq_fired = 1;
}

static int wait_for_fdc_irq(uint32_t timeout_ms)
{
	uint32_t start = timer_get_ticks();

	while (!fdc_irq_fired) {
		if (timer_get_ticks() - start > timeout_ms) {
			return -1;			/* Timeout */
		}
		__asm__ volatile ("hlt");
	}
	fdc_irq_fired = 0;
	return 0;
}

static void fdc_write(uint8_t val)
{
	int timeout = READ_WRITE_TIMEOUT;

	while (((inb(FDC_MSR) & 0xC0) != 0x80) && --timeout > 0) {
		io_wait();
	}
	if (timeout <= 0) {
		printf("FDC: write timeout!\n");
	}
	outb(FDC_FIFO, val);
}

static uint8_t fdc_read(void)
{
	int timeout = READ_WRITE_TIMEOUT;

	while (((inb(FDC_MSR) & 0xC0) != 0xC0) && --timeout > 0) {
		io_wait();
	}
	if (timeout <= 0) {
		printf("FDC: read timeout!\n");
	}
	return inb(FDC_FIFO);
}

static void fdc_sense_interrupt(uint8_t* st0, uint8_t* cyl)
{
	fdc_write(0x08);			/* Sense Interrupt command */
	*st0 = fdc_read();
	*cyl = fdc_read();
}

static void fdc_reset(void)
{
	fdc_irq_fired = 0;
	/* Reset controller: Clear bit 2 of DOR (reset pulse) */
	outb(FDC_DOR, 0x00);
	sleep_ms(10);
	outb(FDC_DOR, 0x0C);		/* release reset (bit 2), enable DMA/IRQ (bit 3) */

	wait_for_fdc_irq(2000);

	/* Sense interrupt for all 4 possible drives */
	uint8_t st0, cyl;

	for (int i = 0; i < 4; i++) {
		fdc_sense_interrupt(&st0, &cyl);
	}

	/* Set CCR for 1.44MB Floppy (500 Kbps data rate = 0x00) */
	outb(FDC_CCR, 0x00);

	/* Specify drive timings (Specify command = 0x03) */
	/* SRT=8ms, HUT=240ms, HLT=16ms, ND=0 (DMA enabled) */
	fdc_write(0x03);
	fdc_write(0xDF);
	fdc_write(0x02);
}

static void fdc_motor(int drive, int on)
{
	if (on) {
		/* Bit 4 is Motor A (drive 0), Bit 5 is Motor B (drive 1) */
		/* Bit 0-1 is Drive select (00 = Drive A) */
		uint8_t dor_val = 0x0C | (1 << (drive + 4)) | drive;

		outb(FDC_DOR, dor_val);
		sleep_ms(300);			/* Wait for motor to spin up */
	} else {
		/* Turn off motors, reset select to drive 0, keep IRQ/DMA enabled */
		outb(FDC_DOR, 0x0C);
	}
}

static int fdc_recalibrate(int drive)
{
	fdc_motor(drive, 1);

	fdc_write(0x07);			/* Recalibrate command */
	fdc_write(drive);

	if (wait_for_fdc_irq(2000) < 0) {
		fdc_motor(drive, 0);
		return -1;				/* Timeout */
	}

	uint8_t st0, cyl;

	fdc_sense_interrupt(&st0, &cyl);
	fdc_motor(drive, 0);

	if (cyl != 0) {
		return -2;				/* Recalibration failed to reach track 0 */
	}
	return 0;
}

static int fdc_seek(int drive, uint8_t cylinder, uint8_t head)
{
	fdc_motor(drive, 1);

	fdc_write(0x0F);			/* Seek command */
	fdc_write((head << 2) | drive);
	fdc_write(cylinder);

	if (wait_for_fdc_irq(2000) < 0) {
		fdc_motor(drive, 0);
		return -1;				/* Timeout */
	}

	uint8_t st0, cyl;

	fdc_sense_interrupt(&st0, &cyl);
	fdc_motor(drive, 0);

	if (cyl != cylinder) {
		return -2;				/* Seek error */
	}
	return 0;
}

static int fdc_transfer(int drive, uint8_t cylinder, uint8_t head, uint8_t sector, int is_write)
{
	/* 1. Program DMA Channel 2 */
	uint8_t dma_mode = is_write ? DMA_MODE_READ : DMA_MODE_WRITE;

	floppy_dma_init(floppy_dma_buffer, 512, dma_mode);

	/* 2. Turn motor on */
	fdc_motor(drive, 1);

	/* 3. Seek to correct cylinder/head */
	if (fdc_seek(drive, cylinder, head) < 0) {
		fdc_motor(drive, 0);
		return -1;
	}

	/* 4. Send command (Read or Write) */
	/* 0x46 = Read Data command with MFM bit set */
	/* 0x45 = Write Data command with MFM bit set */
	fdc_write(is_write ? 0x45 : 0x46);
	fdc_write((head << 2) | drive);
	fdc_write(cylinder);
	fdc_write(head);
	fdc_write(sector);
	fdc_write(2);				/* Sector size code: 2 = 512 bytes */
	fdc_write(18);				/* End of track sector count (1.44MB has 18 sectors per track) */
	fdc_write(0x1B);			/* Gap length (standard MFM gap 3 length) */
	fdc_write(0xFF);			/* Data length (ignored) */

	/* 5. Wait for interrupt */
	if (wait_for_fdc_irq(2000) < 0) {
		fdc_motor(drive, 0);
		return -2;				/* Timeout */
	}

	/* 6. Read 7 status bytes */
	uint8_t st0 = fdc_read();
	uint8_t st1 = fdc_read();
	uint8_t st2 = fdc_read();
	uint8_t r_cyl = fdc_read();
	uint8_t r_head = fdc_read();
	uint8_t r_sec = fdc_read();
	uint8_t r_sz = fdc_read();

	(void)r_cyl;
	(void)r_head;
	(void)r_sec;
	(void)r_sz;

	/* 7. Turn motor off */
	fdc_motor(drive, 0);

	/* Check status code (ST0 bits 6-7 must be 00 for normal termination) */
	if ((st0 & 0xC0) != 0x00) {
		printf("FDC error: ST0=0x%02X ST1=0x%02X ST2=0x%02X\n", st0, st1, st2);
		return -3;
	}

	return 0;
}

int floppy_read_sector(uint32_t lba, uint8_t* out_buf)
{
	uint8_t cyl, head, sec;

	sec = (lba % 18) + 1;
	head = (lba / 18) % 2;
	cyl = lba / 36;

	for (int i = 0; i < 3; i++) {
		int res = fdc_transfer(0, cyl, head, sec, 0);

		if (res == 0) {
			for (int j = 0; j < 512; j++) {
				out_buf[j] = floppy_dma_buffer[j];
			}
			return 0;
		}
		fdc_reset();
	}
	return -1;
}

int floppy_write_sector(uint32_t lba, const uint8_t* in_buf)
{
	uint8_t cyl, head, sec;

	sec = (lba % 18) + 1;
	head = (lba / 18) % 2;
	cyl = lba / 36;

	for (int j = 0; j < 512; j++) {
		floppy_dma_buffer[j] = in_buf[j];
	}

	for (int i = 0; i < 3; i++) {
		int res = fdc_transfer(0, cyl, head, sec, 1);

		if (res == 0) {
			return 0;
		}
		fdc_reset();
	}
	return -1;
}

int floppy_init(void)
{
	irq_install_handler(6, fdc_irq_handler);

	fdc_reset();

	if (fdc_recalibrate(0) < 0) {
		printf("Floppy: calibration failed for drive fdrive1\n");
		return -1;
	}

	printf("Floppy: drive fdrive1 initialized.\n");
	return 0;
}
#endif
