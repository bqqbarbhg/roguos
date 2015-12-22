
#include <stdint.h>

#ifdef __linux__
#error "Not a cross-compiler"
#endif

#ifndef __i386__
#error "Expected i386 architecture"
#endif

#include "inline_asm.c"
#include "gdt.c"
#include "idt.c"
#include "pic.c"

// Interrupt descriptor table
struct IDT_Entry idt[256];

struct TSS tss;

struct GDT_Entry gdt[4];

volatile uint16_t *vga_mem = (uint16_t*)0xB8000;

void keyboard_isr();
void flush_segment_registers();

void keyboard_handler()
{
	inb(0x60);

	static int index = 80;
	vga_mem[index++] = 15 << 8 | '.';
}

void kernel_main()
{
	cli();

	gdt[0] = (struct GDT_Entry) { 0 };
	gdt[1] = make_gdt_entry(0, 0xFFFFFFFF, GDT_RING0 | GDT_CODE | GDT_CODE_READABLE);
	gdt[2] = make_gdt_entry(0, 0xFFFFFFFF, GDT_RING0 | GDT_DATA | GDT_DATA_WRITABLE);
	gdt[3] = make_gdt_entry(&tss, sizeof(tss), GDT_RING0 | GDT_TSS);

	load_gdt(gdt, sizeof(gdt));
	flush_segment_registers();

	// Map PIC interrupts to INT 0x20-0x2F
	pic_initialize(0x20);

	// Keyboard handler
	idt[0x20 + 1] = make_idt_interrupt_gate(keyboard_isr, 1, 0);

	// Load the IDT
	load_idt(idt, sizeof(idt));
	sti();

	// Enable keyboard interrutps
	pic_enable_irq(1);

	const char *build_info = "Built on " __DATE__ " " __TIME__;

	volatile uint16_t *out = vga_mem;
	for (const char *c = build_info; *c; c++) {
		*out++ = 15 << 8 | *c;
	}

	for (;;) { }
}

