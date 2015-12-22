
// Task state segment
struct TSS
{
	// Link to the previous TSS
	uint32_t link;

	// Ring 0 state
	uint32_t esp0;
	uint32_t ss0;

	// Ring 1 state
	uint32_t esp1;
	uint32_t ss1;

	// Ring 2 state
	uint32_t esp2;
	uint32_t ss2;

	// Registers
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t epb;
	uint32_t esi;
	uint32_t edi;

	// Segment registers
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;

	// LDT segment selector
	uint32_t ldtr;

	// IO map base address
	uint32_t io_base;
};

struct GDT_Entry
{
	uint16_t limit_lo;
	uint16_t base_lo;
	uint8_t base_mid;
	uint8_t access;
	uint8_t flags_limit;
	uint8_t base_hi;

} __attribute__((packed));

// Access privilege
#define GDT_RING0 (0 << 6)
#define GDT_RING1 (1 << 6)
#define GDT_RING2 (2 << 6)
#define GDT_RING3 (3 << 6)

// Type
#define GDT_DATA (1 << 4 | 0 << 3)
#define GDT_CODE (1 << 4 | 1 << 3)
#define GDT_SYSTEM (1 << 3)

// Growing direction (only for data)
#define GDT_GROW_DOWN (0 << 2)
#define GDT_GROW_UP (1 << 2)

// Allow lower privilege code to execute this segment
#define GDT_CONFORMING (1 << 2)

// Readable selector (only for code)
#define GDT_CODE_READABLE (1 << 1)

// Writable selector (only for data)
#define GDT_DATA_WRITABLE (1 << 1)

// Is the sector present at the moment
#define GDT_PRESENT 1

// Flags for TSS segment
#define GDT_TSS (GDT_SYSTEM | (1 << 3) | GDT_PRESENT)

struct GDT_Entry
make_gdt_entry(void *base, uint32_t size, uint8_t flags)
{
	uint32_t limit = size;
	uint32_t base_int = (uint32_t)base;
	uint8_t granularity = 0;

	// The limit field only has 20 bits, but there is a granularity bit which
	// scales the limit into 4KiB chunks.
	if (limit > (1 << 20) - 1) {
		// assert((limit & 0xFFF) == 0xFFF)
		limit >>= 12;
		granularity = 1;
	}

	return (struct GDT_Entry) {
		.limit_lo = limit & 0xFFFF,
		.base_lo = base_int & 0xFFFF,
		.base_mid = (base_int >> 16) & 0xFF,
		.access = (1 << 7) // Present
			| flags,
		.flags_limit = (limit >> 16 & 0xF)
			| granularity << 7 // Granularity
			| 1 << 6, // 32-bit selector
		.base_hi = base_int >> 24,
	};
}

void load_gdt(struct GDT_Entry *gdt, uint32_t size)
{
	uint32_t gdt_int = (uint32_t)gdt;
	uint32_t gdtr[2];
	gdtr[0] = gdt_int << 16 | size;
	gdtr[1] = gdt_int >> 16;

	asm volatile ("lgdt %0"
		: // No outputs
		: "m"(gdtr)
	);
}

