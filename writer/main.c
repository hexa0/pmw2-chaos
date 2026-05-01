#include <stdio.h>

/// @brief executes the mod
extern void RunMod(int isCWriter);

extern void Game_Init(void);
extern void Game_DoShell(void);
extern void Level_Init(void);

#ifndef IS_CWRITER
	// extern int printf(char *fmt, ...);
	extern int sceOpen(const char *filename, int flag);
	unsigned int sceRead(int fd, void *buf, int size);
	extern int sceClose(int fd);
	extern unsigned int sceLseek(int fd, int offset, int whence);
#endif

extern void FlushCache(int);

#include "screen.c"

#ifdef IS_CWRITER
	#include <unistd.h>
	#include <fcntl.h>
	#include <loadfile.h>
	#include <kernel.h>
	#include <sifrpc.h>
	#include <string.h>

	extern unsigned char usbd_irx[];
    extern unsigned int usbd_irx_len;

    extern unsigned char usbhdfsd_irx[];
    extern unsigned int usbhdfsd_irx_len;
#endif

void init() {

	ShowLoad();

	#ifdef IS_CWRITER
		int ret;
		int ret2;
		int fd;
		int size;
		

		for(int i = 0; i < 20000000; i++) { __asm__ volatile("nop"); }

		while(SifDmaStat(-1) >= 0);

		printf("SifExecModuleBuffer usbd.irx\n");
		ret = SifExecModuleBuffer(usbd_irx, usbd_irx_len, 0, NULL, &ret);

		if (ret < 0) {
			printf("SIF load failed: %d\n", ret);
			ShowReadError();
		}

		for(int i = 0; i < 1000000; i++) { __asm__ volatile("nop"); }

		while(SifDmaStat(-1) >= 0);

		printf("SifExecModuleBuffer usbhdfsd.irx\n");
		ret2 = SifExecModuleBuffer(usbhdfsd_irx, usbhdfsd_irx_len, 0, NULL, &ret2);

		if (ret2 < 0) {
			// TODO: figure out why this fails
			printf("SIF load 2 failed: %d\n", ret2);
			ShowReadError();
		}

		printf("waiting for usb\n");
		for(int i = 0; i < 10000000; i++) { __asm__ volatile("nop"); }

		char* file = "mass:PMW2/MOD.BIN";
		printf("open %s\n", file);

		fd = open(file, O_RDONLY);
		if (fd < 0) {
			printf("open failed: %d\n", fd);
			ShowReadError();
			return;
		}

		size = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
		printf("found %d bytes\n", size);

		unsigned char* dest = (unsigned char*)0x01F00000;
		
		int read_res = read(fd, dest, size);
		close(fd);

		if (read_res != size) {
			printf("read mismatch: %d/%d\n", read_res, size);
			ShowReadError();
			return;
		}

		printf("read successful!\n");

		FlushCache(0);
		FlushCache(2);

		RunMod(1);
	#endif

	#ifndef IS_CWRITER
		char* file = "host:mod.bin";
		
		int fd = sceOpen(file, 00);

		if (fd >= 0) {

			int size = sceLseek(fd, 0, 2);
			sceLseek(fd, 0, 0);

			sceRead(fd, (void*)0x01F00000, size);
			sceClose(fd);

			printf("read %d bytes from %s\n", size, file);
			
			// invalidate EE/data cache
			FlushCache(0);
			FlushCache(2);

			printf("executing\n");
			RunMod(0);

			printf("running game\n");
		}
		else {
			ShowReadError();
		}
	#endif
}