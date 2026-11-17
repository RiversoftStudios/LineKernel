#include "LineKernel.h"

#include "kernelcheck.h"
#include "version.h"
#include "LineRenderer.h"
#include "LineMemory.h"
#include "LineInput.h"
#include "LineColor.h"
#include "syscall_init.h"
#include "version.h"
#include "LineKernel/contents_t.h"
#ifdef CONFIG_PS2_KEYBOARD
#include "ps2_keyboard.h"
#endif
#ifdef CONFIG_SERIAL_CONSOLE
#include "serial.h"
#endif
#ifdef ARCH_riscv64
#include "sbi.h"
#endif

#ifdef ARCH_i386
#include "gdt.h"
#include "idt.h"
#include "timer.h"
#endif
#ifdef CONFIG_FLOPPY
#include "floppy.h"
#endif

drivesformat_t drive = nodrive;
filesystemformat_t fs = nofilesystem;
int has_working_drive = 0;
char cmdline_str[100];

void handle_single_argument(char* arg)
{
	if (strcmp(arg, "drive=nodrive") == 0) {
		drive = nodrive;
	}
	if (strcmp(arg, "drive=fdrive1") == 0) {
		drive = fdrive1;
	}
	if (strcmp(arg, "fs=fat12") == 0) {
		fs = fat12;
	}
	if (strcmp(arg, "fs=nofilesystem") == 0) {
		fs = nofilesystem;
	}
}

void parse_cmdline(char* cmdline)
{
	if (!cmdline)
		return;

	char* token = cmdline;

	while (*token != '\0') {
		while (*token == ' ')
			token++;
		if (*token == '\0')
			break;

		char* arg_start = token;

		while (*token != '\0' && *token != ' ') {
			token++;
		}

		if (*token == ' ') {
			*token = '\0';
			token++;
		}

		handle_single_argument(arg_start);
	}
}

void initialize_start(void)
{
	terminal_initialize();
	terminal_writestring("Booting LineKernel v" VERSION "...\n\n");
	terminal_writestring("LineRenderer initialized.\n");

	terminal_writestring("Command line: ");
	terminal_writestring(cmdline_str);
	terminal_writestring("\n");

	heap_init();
	terminal_writestring("Memory allocation initialized.\n");

#ifdef ARCH_i386
	gdt_init();
	terminal_writestring("GDT initialized.\n");
	idt_init();
	terminal_writestring("IDT initialized.\n");
	timer_init(1000);
	terminal_writestring("System Timer (PIT) initialized at 1000Hz.\n");

	__asm__ volatile ("sti");

	terminal_writestring("Interrupts enabled.\n");
#endif

	keyboard_init();
	terminal_writestring("Keyboard input initialized.\n");

#ifdef CONFIG_DRIVE
	terminal_writestring("Initializing storage drive...\n");
	if (drive_init(drive) == 0) {
		printf("Drive %s initialized successfully.\n", drive_name(drive));
		printf("Checking for %s filesystem...\n", filesystem_name(fs));
		has_working_drive = check_filesystem(drive, fs);
		if (has_working_drive == 1) {
			printf("%s filesystem detected on %s.\n", filesystem_name(fs), drive_name(drive));
		} else {
			printf("Filesystem not found.\n");
		}
	} else {
		printf("Failed to initialize drive %s.\n", drive_name(drive));
	}
#endif

	syscall_init();
	terminal_writestring("Syscalls initialized.\n");
}

void kernel_main(void)
{
	initialize_start();

	terminal_writestring("Core services initialized.\n\n");
	we_are_running();

	for (;;) {
		terminal_write_for_char(get_char());
	}
}
