#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <loadfile.h>
#include <kernel.h>
#include <sifrpc.h>
#include <debug.h>


/// @brief executes the mod
extern void RunMod(int isCWriter);

extern void iopStartUp(void);

extern void FlushCache(int);
void log_scr(const char *format, ...) {
    static char buffer[512]; // Static to save stack space
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    scr_printf("%s", buffer);
    printf("%s", buffer);
}

extern int sceOpen(const char *filename, int flag);
unsigned int sceRead(int fd, void *buf, int size);
extern int sceClose(int fd);
extern unsigned int sceLseek(int fd, int offset, int whence);

static const char* slus_path = "cdrom0:\\SLUS_202.24;1";

typedef struct vfs_entry_s {
    char name[16];
    unsigned int size;
    unsigned int offset;
	unsigned char cksum;
} vfs_entry_t;

#define VFS_OFFSET 5242880

void cinit() {
	iopStartUp();
	
	sceSifInitRpc(0);
	init_scr();
	log_scr("init PMW2 cwriter loader\n");

	// this actually loads now but our drivers are not able to read mass yet
	// ill investigate this later or if someone smarter knows how to fix this
	// though we are running after the game loads its own IOP drivers and also
	// re-use the sceRead/Write/ETC functions from the game symbols, so that's
	// probably our issue
	
	// LoadInjectedIopDrivers(fd);

	int fd = sceOpen(slus_path, 0x0001);

	unsigned int total_entries = 0;
    vfs_entry_t vfs_table[16];

	if (fd >= 0) {
		log_scr("successfully got handle to %s (%d)\n", slus_path, fd);

		sceLseek(fd, VFS_OFFSET, 0);
		sceRead(fd, &total_entries, 4);
		sceRead(fd, vfs_table, sizeof(vfs_entry_t) * 16);

		log_scr("vfs at 0x%00X has %d entries\n", VFS_OFFSET, total_entries);

		for (unsigned int i = 0; i < total_entries; i++) {
			vfs_entry_t* entry = &vfs_table[i];
			log_scr("%s at 0x%00X (%d bytes)\n", entry->name, entry->offset, entry->size);
		}

		for (unsigned int i = 0; i < total_entries; i++) {
			vfs_entry_t* entry = &vfs_table[i];

			if (strcmp(entry->name, "MOD.BIN") == 0) {
				log_scr("reading %s into memory\n", entry->name);

				sceLseek(fd, entry->offset, 0);
				sceRead(fd, (void*)0x000A0000, entry->size);

				log_scr("flushing EE/data caches\n");

				FlushCache(0);
				FlushCache(2);

				log_scr("closing handle %d to %s\n", fd, slus_path);

				sceClose(fd);

				log_scr("verifying\n");

				unsigned char actual_cksum = 0;
				unsigned char* ptr = (unsigned char*)0x000A0000;

				for (unsigned int j = 0; j < entry->size; j++) {
					actual_cksum += ptr[j];
				}

				if (actual_cksum != entry->cksum) {
					log_scr("checksum error, expected \"%00X\" got \"%00X\"\n", entry->cksum, actual_cksum);

					while (1);
				}

				log_scr("checksum \"%00X\" is valid\n", entry->cksum);

				log_scr("jumping exec to mod at 0x%00X\n", &RunMod);

				RunMod(1);
			}
		}

	}
	else {
		log_scr("failed to read \"%s\" cannot read VFS to initialize mod\n", slus_path);
		while (1);
	}
}