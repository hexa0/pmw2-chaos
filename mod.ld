SECTIONS
{
	. = 0x01F00000;
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	_gp = ALIGN(16) + 0x7ff0;
}