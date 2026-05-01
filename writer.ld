SECTIONS
{
	. = 0x01E00000;
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	_gp = ALIGN(16) + 0x7ff0;
}