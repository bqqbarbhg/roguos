.intel_syntax noprefix

# GRUB multiboot magic values
.set FLAGS, (1<<0) | (1<<1)
.set MAGIC, 0x1BADB002
.set CHECK, -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECK

# Create a small stack for bootstrapping
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16*1024
stack_top:

# Entry-point
.section .text
.global _start
.type _start, @function
_start:

	# Setup the stack
	mov esp, stack_top

	# Call to C
	call kernel_main

	# Hang if kernel returns
	cli
.L_hang:
	hlt
	jmp .L_hang

# Hackish: Maybe move somewhere else
.global flush_segment_registers
.type flush_segment_registers, @function
flush_segment_registers:
	jmp 0x08:reload_cs
reload_cs:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret
