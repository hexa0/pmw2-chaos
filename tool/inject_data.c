#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_FILES 16
#define TABLE_SIZE 512
#define NAME_LEN 16

typedef struct vfs_entry_s {
    char name[NAME_LEN];
    uint32_t size;
    uint32_t offset;
	unsigned char cksum;
} vfs_entry_t;

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <target_elf> <base_offset> <file...>\n", argv[0]);
        return 1;
    }

    char *target_path = argv[1];
    long base_offset = atol(argv[2]);
    int file_count = argc - 3;

    FILE *f_elf = fopen(target_path, "rb+");
    if (!f_elf) { perror("error opening ELF"); return 1; }

    vfs_entry_t table[MAX_FILES];
    memset(table, 0, sizeof(table));

    long current_data_offset = base_offset + TABLE_SIZE;
    
    for (int i = 0; i < file_count && i < MAX_FILES; i++) {
        char *src_path = argv[i + 3];
        FILE *f_src = fopen(src_path, "rb");
        if (!f_src) { printf("skip %s\n", src_path); continue; }

        fseek(f_src, 0, SEEK_END);
        uint32_t size = ftell(f_src);
        fseek(f_src, 0, SEEK_SET);

        char *filename = strrchr(src_path, '/');
        strncpy(table[i].name, filename ? filename + 1 : src_path, NAME_LEN - 1);
        table[i].size = size;
        table[i].offset = (uint32_t)current_data_offset;
        table[i].cksum = 0;

        fseek(f_elf, current_data_offset, SEEK_SET);
        unsigned char buffer[4096];
        size_t bytes;
        while ((bytes = fread(buffer, 1, sizeof(buffer), f_src)) > 0) {
            for(size_t j = 0; j < bytes; j++) {
                table[i].cksum += buffer[j];
            }
            fwrite(buffer, 1, bytes, f_elf);
        }
        
        current_data_offset += size;
        fclose(f_src);
        printf("packed %s (size: %u, offset: %u, cksum: 0x%02X)\n", 
               table[i].name, table[i].size, table[i].offset, table[i].cksum);
    }

    fseek(f_elf, base_offset, SEEK_SET);
    uint32_t count_u32 = (uint32_t)file_count;
    fwrite(&count_u32, sizeof(uint32_t), 1, f_elf);
    fwrite(table, sizeof(vfs_entry_t), MAX_FILES, f_elf);

    fclose(f_elf);
    return 0;
}