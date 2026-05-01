#include "game/pmw2lib.h"
#include "patches.c"
#include <debug.h>
#include <stdio.h>

void LockEEForMS(int ms) {
    for (int i = 0; i < ms * 40000; i++) {
        __asm__ volatile("nop");
    }
}

void RunMod(BOOL isConsole) {
	scr_clear();
	scr_printf("Pac-Man World 2 (Chaos Edition)\n");

	if (isConsole) {
		scr_printf("looking for restore.bin\n");

		scr_printf("trying host:restore.bin\n");
		char* r_file = "host:restore.bin";
		int rfd = sceOpen(r_file, 0);

		if (rfd < 0) {
			// this won't work, we need to use mass once we get that working
			scr_printf("trying mc0:PMW2/RESTORE.BIN\n");

			r_file = "mc0:PMW2/RESTORE.BIN";
			rfd = sceOpen(r_file, 0);
		}

		if (rfd >= 0) {
			int size = sceLseek(rfd, 0, 2);
			sceLseek(rfd, 0, 0);

			scr_printf("restoring %d bytes\n", size);
			sceRead(rfd, (void*)0x45ad10, size);
			sceClose(rfd);
		}
		else {
			scr_printf("restore.bin is absent, now exiting");
			LockEEForMS(100);
			scr_printf("!");
			LockEEForMS(100);
			scr_printf("!");
			LockEEForMS(100);
			scr_printf("!");
			LockEEForMS(10000);
			return;
		}
	}
	
	scr_printf("injecting hooks\n");

	inject_pre_sound_update();
	// inject_no_fmv();
	// fast_startup();
	// inject_level_update_hook();
	// inject_world_render_hook();
	patch_out_sync_padding();
	inject_create_inventory_hook();
	replace_screen_adjust_menu();

	scr_printf("flushing EE cache\n");
	FlushCache(0);
	scr_printf("flushing data cache\n");
	FlushCache(2);

	scr_printf("run Game_Init\n");
	Game_Init();
	scr_printf("run Game_DoShell\n");
	Game_DoShell();
}