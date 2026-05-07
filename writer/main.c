#ifndef IS_CWRITER
#include <stdio.h>

/// @brief executes the mod
extern void RunMod(int isCWriter);

extern void Game_Init(void);
extern void Game_DoShell(void);
extern void Level_Init(void);

// extern int printf(char *fmt, ...);
extern int sceOpen(const char *filename, int flag);
unsigned int sceRead(int fd, void *buf, int size);
extern int sceClose(int fd);
extern unsigned int sceLseek(int fd, int offset, int whence);

#include "screen.c"

extern void FlushCache(int);
extern void iopStartUp(void);

typedef enum eHost {
	FILEHOST_UNIXDEV=0,
	FILEHOST_PRODG=1,
	FILEHOST_PROVIEW=2,
	FILEHOST_DVD=3
} eHost;

typedef enum eHost HOST;

// this is set to FILEHOST_DVD, but for host fs to work with sound this needs to be changed
// when we dispace event 1 (which triggers iop_audioInit) the game passes the g_filehost to AUDIO.IRX
// that then sets g_filehost in AUDIO.IRX
// also AUDIO.IRX also has symbols which is awesome
extern HOST g_filehost;

// sets g_filehost to FILEHOST_DVD;
extern void findfilehost();

/// @brief stubs the specified function
/// @param func_addr function to stub
void stub_func_at(unsigned int func_addr) {
	// jr $ra
	*(volatile unsigned int*)(func_addr) = 0x03e00008;
	// nop
	*(volatile unsigned int*)(func_addr + 4) = 0x00000000;
}

/// @brief assembles and writes a J instruction at the specified address
/// @param addr address to write to
/// @param to address to j to
void assemble_j_at(unsigned int addr, unsigned int to) {
	unsigned int instruction = (0x08000000) | ((to & 0x0FFFFFFF) >> 2);
	*(volatile unsigned int*)addr = instruction;
}

/// @brief initializes the global ctors in the base game via __do_global_ctors
extern void __main(void);

extern int isEmulator;

void stage2() {
	// printf("ws2\n");
	ShowLoad();

	char* file = "host:netdata/mod/mod.bin";
	
	int fd = sceOpen(file, 00);

	if (fd >= 0) {
		isEmulator = 1;
		int size = sceLseek(fd, 0, 2);
		sceLseek(fd, 0, 0);

		sceRead(fd, (void*)0x000A0000, size);
		sceClose(fd);

		// printf("read %d bytes from %s\n", size, file);
		
		// invalidate EE/data cache
		FlushCache(0);
		FlushCache(2);

		// printf("executing\n");
		RunMod(0);
	}
	else {
		// printf("err %d\n", fd);
		ShowReadError();
	}
}

void init() {
	// printf("ws1\n");

	__main();

	stub_func_at((unsigned int)findfilehost);
	// g_filehost = FILEHOST_DVD;
	g_filehost = FILEHOST_UNIXDEV;
	// Game_Start call-site
	assemble_j_at(0x002B79A8, (unsigned int)stage2);
	FlushCache(0);
	FlushCache(2);
}

#endif

#ifdef IS_CWRITER

#include "cwriter.c"

void init() {
	cinit();	
}

#endif