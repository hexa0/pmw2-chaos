#include <stdarg.h>
#include "game/pacman.c"

#ifndef UTIL_C
#define UTIL_C

/// @brief assembles and writes a JAL instruction at the specified address
/// @param addr address to write to
/// @param to address to jal to
void assemble_jal_at(unsigned int addr, unsigned int to) {
	unsigned int instruction = (to >> 2) | 0x0C000000;

	*(volatile unsigned int*)addr = instruction;
}

/// @brief UNIMPLEMENTED, DO NOT USE!!!! assembles and writes a J instruction at the specified address
/// @param addr address to write to
/// @param to address to j to
void assemble_j_at(unsigned int addr, unsigned int to) {

}

/// @brief stubs the specified function
/// @param func_addr function to stub
void stub_func_at(unsigned int func_addr) {
	// jr $ra
	*(volatile unsigned int*)(func_addr) = 0x03e00008;
	// nop
	*(volatile unsigned int*)(func_addr + 4) = 0x00000000;
}

__asm__ (
    ".global __extendsfdf2\n"
    "__extendsfdf2:\n"
    "mfc1 $v0, $f12\n"
    "move $v1, $zero\n"
    "jr $ra\n"
    "nop\n"
);

void fmt(char *buf, char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
}

/// @brief %f is fucked, so we have to reimplement it
char* float_to_str(char* out, float f, int precision) {
	char* res = out;
	
	if (f < 0) {
		*out++ = '-';
		f = -f;
	}

	int whole = (int)f;
	out += sprintf(out, "%d", whole);

	if (precision > 0) {
		*out++ = '.';
		float fraction = f - (float)whole;
		
		float multiplier = 1.0f;
		for (int i = 0; i < precision; i++) multiplier *= 10.0f;
		
		int frac_part = (int)(fraction * multiplier + 0.5f);
		
		char fmt_buf[8];
		sprintf(fmt_buf, "%%0%dd", precision);
		out += sprintf(out, fmt_buf, frac_part);
	}

	*out = '\0';
	return res;
}

#endif // UTIL_C