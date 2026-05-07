#include "game/pmw2lib.h"
#include "patches.c"
#include <debug.h>
#include <stdio.h>

void LockEEForMS(int ms) {
	for (int i = 0; i < ms * 40000; i++) {
		__asm__ volatile("nop");
	}
}

static const char* slus_path = "cdrom0:\\SLUS_202.24;1";

typedef struct vfs_entry_s {
	char name[16];
	unsigned int size;
	unsigned int offset;
	unsigned char cksum;
} vfs_entry_t;

#define VFS_OFFSET 5242880

void log_scr(const char *format, ...) {
	static char buffer[512];
	va_list args;

	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	scr_printf("%s", buffer);
	printf("%s", buffer);
}

void Inject(BOOL isConsole) {
	log_scr("injecting hooks\n");

	inject_pre_sound_update();
	// inject_no_fmv();
	// inject_fast_startup_hook();

	// inject_level_update_hook();
	// inject_world_render_hook();
	if (!isConsole) {
		patch_out_sync_padding();
		inject_host_fs();
	}

	inject_create_inventory_hook();
	replace_screen_adjust_menu();
	inject_widescreen_bug_fixes();
	inject_loading_progress();
	inject_custom_asset_preloading();

	log_scr("flushing EE cache\n");
	FlushCache(0);
	log_scr("flushing data cache\n");
	FlushCache(2);
}

void RunMod(BOOL isConsole) {
	scr_clear();
	log_scr("Pac-Man World 2 (Chaos Edition)\n");

	if (isConsole) {
		log_scr("using console loader\n");
		int fd = sceOpen(slus_path, 0x0001);

		unsigned int total_entries = 0;
		vfs_entry_t vfs_table[16];

		if (fd >= 0) {
			log_scr("successfully got handle to %s (%d)\n", slus_path, fd);

			sceLseek(fd, VFS_OFFSET, 0);
			sceRead(fd, &total_entries, 4);
			sceRead(fd, vfs_table, sizeof(vfs_entry_t) * 16);

			log_scr("vfs at 0x%00X has %d entries\n", VFS_OFFSET, total_entries);

			for (int i = 0; i < total_entries; i++) {
				vfs_entry_t* entry = &vfs_table[i];
				log_scr("%s at 0x%00X (%d bytes)\n", entry->name, entry->offset, entry->size);
			}

			for (int i = 0; i < total_entries; i++) {
				vfs_entry_t* entry = &vfs_table[i];

				if (strcmp(entry->name, "RESTORE.BIN") == 0) {
					log_scr("reading %s into memory\n", entry->name);

					sceLseek(fd, entry->offset, 0);
					sceRead(fd, (void*)0x45ad10, entry->size);

					log_scr("flushing EE/data caches\n");

					FlushCache(0);
					FlushCache(2);

					log_scr("closing handle %d to %s\n", fd, slus_path);

					sceClose(fd);

					log_scr("verifying\n");

					unsigned char actual_cksum = 0;
					unsigned char* ptr = (unsigned char*)0x45ad10;

					for (unsigned int i = 0; i < entry->size; i++) {
						actual_cksum += ptr[i];
					}

					if (actual_cksum != entry->cksum) {
						log_scr("checksum error, expected \"%00X\" got \"%00X\"\n", entry->cksum, actual_cksum);

						while (1);
					}

					log_scr("checksum \"%00X\" is valid\n", entry->cksum);

					Inject(isConsole);

					LockEEForMS(7000);

					log_scr("jumping to 0x002B78C0\n");

					LockEEForMS(1000);

					__asm__ volatile (
						"la $v0, 0x002B78C0\n"
						"jr $v0\n"
						"nop\n"
					);
				}
			}

			log_scr("RESTORE.BIN is absent from the VFS");
			while (1);
		}
	}
	else {
		log_scr("using pnach loader\n");
		Inject(false);

		log_scr("booting\n");
		fast_startup_hook();
	}
}