#include "pacman.h"

#ifndef PACMAN_C
#define PACMAN_C

void call_font_printf(float x, float y, const char* fmt) {
    __asm__ __volatile__ (
        "lwc1 $f12, %0\n"
        "lwc1 $f13, %1\n"
        "move $a0, %2\n"
        "jal font_printfXY\n"
        "nop\n"
        :
        : "m"(x), "m"(y), "r"(fmt)
        : "$4", "$31", "$f12", "$f13", "memory"
    );

    return;
}

void call_SetFontScale(float scale) {
	__asm__ __volatile__ (
		"lwc1 $f12, %0\n"
		"jal SetFontScale\n"
		"nop\n"
		:
		: "m"(scale)
		: "$f12", "$31", "memory"
	);

	return;
}

#endif