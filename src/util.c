#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

#include "game/pmw2lib.h"

/// @brief assembles and writes a JAL instruction at the specified address
/// @param addr address to write to
/// @param to address to jal to
void assemble_jal_at(unsigned int addr, unsigned int to) {
	unsigned int instruction = (to >> 2) | 0x0C000000;

	*(volatile unsigned int*)addr = instruction;
}

/// @brief assembles and writes a J instruction at the specified address
/// @param addr address to write to
/// @param to address to j to
void assemble_j_at(unsigned int addr, unsigned int to) {
	unsigned int instruction = (0x08000000) | ((to & 0x0FFFFFFF) >> 2);
	*(volatile unsigned int*)addr = instruction;
}

/// @brief assembles and writes a NOP instruction at the specified address
/// @param addr address to write to
void assemble_nop_at(unsigned int addr) {
	*(volatile unsigned int*)addr = 0x00000000;
}

void patch_word(unsigned int addr, unsigned int data) {
	unsigned int swapped = ((data>>24)&0xff) |
						   ((data<<8)&0xff0000) |
						   ((data>>8)&0xff00) |
						   ((data<<24)&0xff000000);

	*(volatile unsigned int*)addr = swapped;
}

void patch_word_le(unsigned int addr, unsigned int instruction) {
	*(volatile unsigned int*)addr = instruction;
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

#define fmt(buf, format, ...) ({ \
	sprintf(buf, format, ##__VA_ARGS__); \
	(char*)(buf); \
})

/// @brief %f is fucked, so we have to reimplement it
char* float_to_str(char* out, float f, int precision) {
	char* res = out;
	
	if (f < 0) {
		*out++ = '-';
		f = -f;
	}

	float multiplier = 1.0f;
	for (int i = 0; i < precision; i++) {
		multiplier *= 10.0f;
	}

	int whole = (int)f;
	float fraction = f - (float)whole;

	int frac_part = (int)(fraction * multiplier + 0.5f);

	if (frac_part >= (int)multiplier) {
		frac_part = 0;
		whole++;
	}

	out += sprintf(out, "%d", whole);

	if (precision > 0) {
		*out++ = '.';
		char fmt_buf[8];
		sprintf(fmt_buf, "%%0%dd", precision);
		out += sprintf(out, fmt_buf, frac_part);
	}

	*out = '\0';
	
	return res;
}

/// @brief generate a random 0.0 - 1.0 float
float frand() {
	return (float)(rand()) / 2147483647.0f;
}

FVEC RotateVector(FVEC v, FVEC rot) {
    float pitch = rot.x;
    float yaw   = rot.y;

    float cosY = cosf(yaw);
    float sinY = sinf(yaw);
    float sinP = sinf(pitch);

    FVEC result;

    result.x = (v.x * cosY) + (v.z * sinY);
    result.z = (v.z * cosY) - (v.x * sinY);

    result.y = v.y + (v.z * -sinP);

    return result;
}