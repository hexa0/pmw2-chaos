SECTIONS
{
	. = 0x45ad10;
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	_gp = ALIGN(16) + 0x7ff0;
}