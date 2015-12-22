static inline void outb(uint16_t port, uint8_t val)
{
	asm volatile ("outb %0, %1"
		: // No outputs
		: "a"(val), "Nd"(port)
	);
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm volatile ("inb %1, %0"
		: "=a"(ret)
		: "Nd"(port)
	);
	return ret;
}

static inline void cli()
{
	asm volatile ("cli" ::: "memory");
}

static inline void sti()
{
	asm volatile ("sti" ::: "memory");
}


