
struct IDT_Entry
{
	uint16_t isr_address_lo;
	uint16_t selector;
	uint8_t zero;
	uint8_t flags;
	uint16_t isr_address_hi;
};

static inline struct IDT_Entry
make_idt_interrupt_gate(void *addr, int section, int privilege)
{
	uint32_t addr_int = (uint32_t)addr;
	return (struct IDT_Entry) {
		.isr_address_lo = addr_int & 0xFFFF,
		.selector = section * 8,
		.zero = 0,
		.flags = 0x80 // Present
			| 0x0E // 32-bit interrupt gate
			| (privilege << 6 & 2), // Privilege
		.isr_address_hi = addr_int >> 16,
	};
}

static inline void load_idt(void *idt, uint32_t size)
{
	uint32_t idt_int = (uint32_t)idt;
	uint32_t idtr[2] = {
		idt_int << 16 | size,
		idt_int >> 16,
	};

	asm volatile ("lidt %0"
		: // No outputs
		: "m"(idtr)
	);
}

