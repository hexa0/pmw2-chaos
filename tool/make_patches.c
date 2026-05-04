#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define GAME_CRC "E7EA3288"
#define GAME_ID_STR  "SLUS_202.24"
#define WRITER_CODE_CAVE_ADDR 0x0009F000
#define HOOK_CALL_ADDR 0x002B7870

#define TOTAL_SYMBOLS 12176

typedef struct {
    uint32_t addr;
    char name[128];
} Symbol;

Symbol symbols[TOTAL_SYMBOLS];
int symbol_count = 0;

void LoadSymbols() {
    FILE *f = fopen("game.syms", "r");
    if (!f) return;

    char line[256];
    while (fgets(line, sizeof(line), f) && symbol_count < TOTAL_SYMBOLS) {
        char name[128];
        uint32_t addr;
        if (sscanf(line, "%s = 0x%x;", name, &addr) == 2) {
            symbols[symbol_count].addr = addr;
            strncpy(symbols[symbol_count].name, name, 127);
            symbol_count++;
        }
    }
    fclose(f);
}

const char* GetCollision(uint32_t addr) {
    for (int i = 0; i < symbol_count; i++) {
        if (symbols[i].addr == addr) {
            return symbols[i].name;
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) { fprintf(stderr, "bad args\n"); return 1; }

    LoadSymbols();
    uint32_t entry_ram_addr = (uint32_t)strtoul(argv[1], NULL, 16);
    FILE *bin = fopen(".tmp/writer/code.bin", "rb");
    if (!bin) { fprintf(stderr, "could not open .tmp/writer/code.bin\n"); return 1; }

    char pnach_name[64];
    sprintf(pnach_name, "build/%s.pnach", GAME_CRC);
    FILE *f_pnach = fopen(pnach_name, "w");

    char cht_name[64];
    sprintf(cht_name, "build/%s.cht", GAME_ID_STR);
    FILE *f_cht = fopen(cht_name, "w");

    fprintf(f_pnach, "gametitle=Pac-Man World 2 (U)(SLUS-20224) (Greatest Hits)\n");
	fprintf(f_pnach, "[Chaos Edition]\n");
	fprintf(f_pnach, "author=hexa.pet\n");

	fprintf(f_cht, "\"Pac-Man World 2 (U)(SLUS-20224) (Greatest Hits) /ID %s\"\n", GAME_ID_STR);
	fprintf(f_cht, "Mastercode\n");
	fprintf(f_cht, "902B7908 0C098530\n");
	// fprintf(f_cht, "90100008 3C02005B\n");
    fprintf(f_cht, "Chaos_Edition\n");


    uint32_t jal_val = (entry_ram_addr >> 2) | 0x0C000000;

	fprintf(f_pnach, "// code\n");

    uint32_t word;
    uint32_t current_addr = WRITER_CODE_CAVE_ADDR;

    while (fread(&word, 4, 1, bin) == 1) {
        const char* collision = GetCollision(current_addr);
        if (collision) printf("patch at 0x%08x overwrites symbol: %s\n", current_addr, collision);

        fprintf(f_pnach, "patch=0,EE,%08x,word,%08x\n", current_addr, word);
        fprintf(f_cht, "0%07X %08X\n", current_addr & 0x0FFFFFFF, word);

        current_addr += 4;
    }

	fprintf(f_cht, "2%07X %08X", HOOK_CALL_ADDR & 0x0FFFFFFF, jal_val);

    fprintf(f_pnach, "// hook to call custom init\n");
	fprintf(f_pnach, "patch=0,EE,%08X,word,%08X", HOOK_CALL_ADDR, jal_val);

    fclose(bin);
    fclose(f_pnach);
    fclose(f_cht);

    printf("created patches for: \n%s\n%s\nspace: 0x%08x -> 0x%08x\n", pnach_name, cht_name, WRITER_CODE_CAVE_ADDR, current_addr);

    return 0;
}