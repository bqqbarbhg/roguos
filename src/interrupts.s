.intel_syntax noprefix

.global keyboard_isr

keyboard_isr:
	pushad

	mov al, 0x20
	out 0x20, al

	call keyboard_handler

	popad
	iret

