/*
	8259 Programmable Interrupt Controller

	PIC 8259 propagates interrupts to the CPU. It supports up to 8 connected
	devices which can send interrupt requests (IRQ). IRQs are referred to with
	zero based numbering starting from IRQ0.

	In modern setups there are two of these controllers (PIC1 and PIC2) which
	are routed in a cascading manner where the PIC2 is connected to the IRQ2 of
	the first PIC.
*/

// x86 IO ports of the two PICs
#define PORT_PIC1_CMD 0x20
#define PORT_PIC1_DATA 0x21
#define PORT_PIC2_CMD 0xA0
#define PORT_PIC2_DATA 0xA1

// Initialize the PIC to send interrupts with vectors at `base..base+15`
// base: Beginning of the interrupt vector block mapped to the PIC interrupts,
//       has to be a multiple of 8.
void pic_initialize(uint8_t base)
{
	// Issue initialization command words (ICWs) to the PICs

	// ICW1: Start the initialization sequence
	outb(PORT_PIC1_CMD, 0x11);
	outb(PORT_PIC2_CMD, 0x11);

	// ICW2: Set the INT vector offsets
	outb(PORT_PIC1_DATA, base);
	outb(PORT_PIC2_DATA, base + 8);

	// ICW3: Slave status
	outb(PORT_PIC1_DATA, 1 << 2); // Has a slave at IRQ2 (bitfield)
	outb(PORT_PIC2_DATA, 2); // Is a slave at IRQ2

	// ICW4: Operation mode
	outb(PORT_PIC1_DATA, 0x01); // x86 mode, no automatic end of interrupt
	outb(PORT_PIC2_DATA, 0x01); // x86 mode, no automatic end of interrupt

	// Mask all interrupts
	outb(PORT_PIC1_DATA, 0xff);
	outb(PORT_PIC2_DATA, 0xff);
}

// Signal the end of an interrupt handler to the PIC
// irq: The interrupt request which generated the interrupt
inline static void pic_end_interrupt(int irq)
{
	if (irq >= 8) {
		outb(PORT_PIC2_CMD, 0x20);
	}
	outb(PORT_PIC1_CMD, 0x20);
}

// Enable interrupt handler for the IRQ `irq`
// irq: Interrupt request index, in 0..15
inline static void pic_enable_irq(int irq)
{
	if (irq < 8) {
		uint8_t mask = inb(PORT_PIC1_DATA);
		outb(PORT_PIC1_DATA, mask & ~(1 << irq));
	} else {
		uint8_t mask = inb(PORT_PIC2_DATA);
		outb(PORT_PIC2_DATA, mask & ~(1 << (irq - 8)));
	}
}

