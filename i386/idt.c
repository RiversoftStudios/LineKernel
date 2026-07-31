/* SPDX-License-Identifier: GPL-3.0-or-later
 * SPDX-FileCopyrightText: Copyright (C) 2026 Riversoft Studios */

#include "idt.h"
#include "bios_tools.h"
#include "panic.h"
#include "printf.h"

static struct idt_entry idt[256];
static struct idt_ptr idtp;
static irq_handler_t handlers[256];

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void isq11(void);

extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

extern void syscall_handler(void);

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
	idt[num].base_low = (base & 0xFFFF);
	idt[num].base_high = (base >> 16) & 0xFFFF;
	idt[num].sel = sel;
	idt[num].always0 = 0;
	idt[num].flags = flags;
}

static void pic_remap(void)
{
	/* ICW1 */
	outb(0x20, 0x11);
	io_wait();
	outb(0xA0, 0x11);
	io_wait();

	/* ICW2 (Master vector 0x20, Slave vector 0x28) */
	outb(0x21, 0x20);
	io_wait();
	outb(0xA1, 0x28);
	io_wait();

	/* ICW3 */
	outb(0x21, 0x04);
	io_wait();
	outb(0xA1, 0x02);
	io_wait();

	/* ICW4 */
	outb(0x21, 0x01);
	io_wait();
	outb(0xA1, 0x01);
	io_wait();

	/* Clear mask registers (enable all IRQs) */
	outb(0x21, 0x00);
	io_wait();
	outb(0xA1, 0x00);
	io_wait();
}

void idt_init(void)
{
	idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
	idtp.base = (uintptr_t) & idt;

	for (int i = 0; i < 256; i++) {
		idt_set_gate(i, 0, 0, 0);
		handlers[i] = 0;
	}

	pic_remap();

	/* Set exception gates */
	idt_set_gate(0, (uintptr_t) isr0, 0x08, 0x8E);
	idt_set_gate(1, (uintptr_t) isr1, 0x08, 0x8E);
	idt_set_gate(2, (uintptr_t) isr2, 0x08, 0x8E);
	idt_set_gate(3, (uintptr_t) isr3, 0x08, 0x8E);
	idt_set_gate(4, (uintptr_t) isr4, 0x08, 0x8E);
	idt_set_gate(5, (uintptr_t) isr5, 0x08, 0x8E);
	idt_set_gate(6, (uintptr_t) isr6, 0x08, 0x8E);
	idt_set_gate(7, (uintptr_t) isr7, 0x08, 0x8E);
	idt_set_gate(8, (uintptr_t) isr8, 0x08, 0x8E);
	idt_set_gate(9, (uintptr_t) isr9, 0x08, 0x8E);
	idt_set_gate(10, (uintptr_t) isr10, 0x08, 0x8E);
	idt_set_gate(11, (uintptr_t) isr11, 0x08, 0x8E);
	idt_set_gate(12, (uintptr_t) isr12, 0x08, 0x8E);
	idt_set_gate(13, (uintptr_t) isr13, 0x08, 0x8E);
	idt_set_gate(14, (uintptr_t) isr14, 0x08, 0x8E);
	idt_set_gate(15, (uintptr_t) isr15, 0x08, 0x8E);
	idt_set_gate(16, (uintptr_t) isr16, 0x08, 0x8E);
	idt_set_gate(17, (uintptr_t) isr17, 0x08, 0x8E);
	idt_set_gate(18, (uintptr_t) isr18, 0x08, 0x8E);
	idt_set_gate(19, (uintptr_t) isr19, 0x08, 0x8E);
	idt_set_gate(20, (uintptr_t) isr20, 0x08, 0x8E);
	idt_set_gate(21, (uintptr_t) isr21, 0x08, 0x8E);
	idt_set_gate(22, (uintptr_t) isr22, 0x08, 0x8E);
	idt_set_gate(23, (uintptr_t) isr23, 0x08, 0x8E);
	idt_set_gate(24, (uintptr_t) isr24, 0x08, 0x8E);
	idt_set_gate(25, (uintptr_t) isr25, 0x08, 0x8E);
	idt_set_gate(26, (uintptr_t) isr26, 0x08, 0x8E);
	idt_set_gate(27, (uintptr_t) isr27, 0x08, 0x8E);
	idt_set_gate(28, (uintptr_t) isr28, 0x08, 0x8E);
	idt_set_gate(29, (uintptr_t) isr29, 0x08, 0x8E);
	idt_set_gate(30, (uintptr_t) isr30, 0x08, 0x8E);
	idt_set_gate(31, (uintptr_t) isr31, 0x08, 0x8E);

	/* Set IRQ gates */
	idt_set_gate(32, (uintptr_t) irq0, 0x08, 0x8E);
	idt_set_gate(33, (uintptr_t) irq1, 0x08, 0x8E);
	idt_set_gate(34, (uintptr_t) irq2, 0x08, 0x8E);
	idt_set_gate(35, (uintptr_t) irq3, 0x08, 0x8E);
	idt_set_gate(36, (uintptr_t) irq4, 0x08, 0x8E);
	idt_set_gate(37, (uintptr_t) irq5, 0x08, 0x8E);
	idt_set_gate(38, (uintptr_t) irq6, 0x08, 0x8E);
	idt_set_gate(39, (uintptr_t) irq7, 0x08, 0x8E);
	idt_set_gate(40, (uintptr_t) irq8, 0x08, 0x8E);
	idt_set_gate(41, (uintptr_t) irq9, 0x08, 0x8E);
	idt_set_gate(42, (uintptr_t) irq10, 0x08, 0x8E);
	idt_set_gate(43, (uintptr_t) irq11, 0x08, 0x8E);
	idt_set_gate(44, (uintptr_t) irq12, 0x08, 0x8E);
	idt_set_gate(45, (uintptr_t) irq13, 0x08, 0x8E);
	idt_set_gate(46, (uintptr_t) irq14, 0x08, 0x8E);
	idt_set_gate(47, (uintptr_t) irq15, 0x08, 0x8E);

	/* Syscall handling! */
	idt_set_gate(128, (uintptr_t) syscall_handler, 0x08, 0xEF);

	__asm__ volatile ("lidt %0"::"m" (idtp));
}

void idt_register_handler(int num, irq_handler_t handler)
{
	handlers[num] = handler;
}

void irq_install_handler(int irq, irq_handler_t handler)
{
	handlers[32 + irq] = handler;
}

void irq_uninstall_handler(int irq)
{
	handlers[32 + irq] = 0;
}

static const char* exception_messages[] = {
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",
	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
	"Reserved", "Reserved", "Reserved", "Reserved", "Reserved",
	"Reserved", "Reserved", "Reserved"
};

void common_handler(struct regs* r)
{
	if (r->int_no < 32) {
		/* Exception handling */
		panic("Exception: %s (err code: %d) at EIP: 0x%08X",
			exception_messages[r->int_no], r->err_code, r->eip);
	} else if (r->int_no >= 32 && r->int_no < 48) {
		/* Call registered IRQ handler if present */
		irq_handler_t handler = handlers[r->int_no];

		if (handler) {
			handler(r);
		}

		/* Send EOI to PICs */
		if (r->int_no >= 40) {
			outb(0xA0, 0x20);	/* Slave PIC EOI */
		}
		outb(0x20, 0x20);		/* Master PIC EOI */
	}
}
