
/* Entry point */
ENTRY(_start)

SECTIONS
{
	/* Start laying out sections at 1MiB */
	. = 1M;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	/* Read-only data */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	/* Read-write data */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	/* Zeroed data */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
		*(.bootstrap_stack)
	}
}

