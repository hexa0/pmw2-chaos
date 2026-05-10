#ifndef IS_CWRITER

#include "pwriter.c"

void init() {
	pinit();	
}

#endif

#ifdef IS_CWRITER

#include "cwriter.c"

void init() {
	cinit();	
}

#endif