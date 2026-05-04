SECTIONS
{
	. = 0x000A0000;
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	_gp = ALIGN(16) + 0x7ff0;
}