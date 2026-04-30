#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define GAME_CRC "E7EA3288"
#define CODE_CAVE_ADDR 0x01F00000
#define MAIN_CALL_ADDR 0x002B7870

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

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "bad args\n");
        return 1;
    }

    LoadSymbols();

    uint32_t entry_ram_addr = (uint32_t)strtoul(argv[1], NULL, 16);
    FILE *bin = fopen(".tmp/patch.bin", "rb");

    if (!bin) {
        fprintf(stderr, "could not open .tmp/patch.bin\n");
        return 1;
    }

    char filename[32];
    sprintf(filename, "build/%s.pnach", GAME_CRC);
    FILE *out = fopen(filename, "w");

    fprintf(out, "gametitle=Pac-Man World 2 (U)(SLUS-20224) (Greatest Hits)\n");
    fprintf(out, "[Chaos Edition]\n");
    fprintf(out, "author=hexa.pet\n");

    uint8_t byte;
    uint32_t current_addr = CODE_CAVE_ADDR;

    fprintf(out, "// code\n");

    while (fread(&byte, 1, 1, bin) == 1) {
        const char* collision = GetCollision(current_addr);
        if (collision) {
            printf("patch at 0x%08x overwrites symbol: %s\n", current_addr, collision);
        }

        fprintf(out, "patch=0,EE,%08x,byte,%02x\n", current_addr, byte);
        current_addr++;
    }

    fclose(bin);

    uint32_t jal_val = (entry_ram_addr >> 2) | 0x0C000000;
    uint8_t hook_bytes[4];
    hook_bytes[0] = jal_val & 0xFF;
    hook_bytes[1] = (jal_val >> 8) & 0xFF;
    hook_bytes[2] = (jal_val >> 16) & 0xFF;
    hook_bytes[3] = (jal_val >> 24) & 0xFF;

    fprintf(out, "// init hook\n");
    for (int i = 0; i < 4; i++) {
        fprintf(out, "patch=0,EE,%08x,byte,%02x\n", MAIN_CALL_ADDR + i, hook_bytes[i]);
    }

    fclose(out);
    printf("created patch %s 0x%08x -> 0x%08x\n", filename, CODE_CAVE_ADDR, current_addr);

    return 0;
}