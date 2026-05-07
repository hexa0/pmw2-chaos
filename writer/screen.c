#include <stdarg.h>

#include <sifrpc.h>
#include "debug/scr_printf.c"

#define B 0x0
#define E 0x1
#define I 0x2
#define N 0x3
#define O 0x4
#define R 0x5

static char* err = "\x1\x5\x5\x4\x5\xFF\x3\x4\x6\x0\x2\x3";
static char* load = "\x0\x5\x0\x6";

void ShowReadError() {
	sceSifInitRpc(0);
	init_scr();

	X = 40, 
	Y = 14;

	for (int i = 1; i < 13; i++) {
		scr_putchar(X * 7, Y * 8, 0xffffff, *err);
		X++;
		err++;
	}
	
	while (1) {
		
	}
}

void ShowLoad() {
	sceSifInitRpc(0);
	init_scr();

	X = 40, 
	Y = 14;

	for (int i = 1; i < 4; i++) {
		scr_putchar(X * 7, Y * 8, 0xffffff, *load);
		X++;
		load++;
	}

	load -= 3;
}

void ShowLoad2() {
	sceSifInitRpc(0);
	init_scr();

	X = 40, 
	Y = 14;

	for (int i = 1; i < 5; i++) {
		scr_putchar(X * 7, Y * 8, 0xffffff, *load);
		X++;
		load++;
	}

	load -= 4;
}