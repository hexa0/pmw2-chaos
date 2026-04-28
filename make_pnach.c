#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define GAME_CRC "E7EA3288"
#define CODE_CAVE_ADDR 0x00556A90
#define MAIN_CALL_ADDR 0x002B7870

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "bad args\n");
		return 1;
	}

	uint32_t entry_ram_addr = (uint32_t)strtoul(argv[1], NULL, 16);

	FILE *bin = fopen(".tmp/main.bin", "rb");

	if (!bin)
	{
		fprintf(stderr, "Error: Could not open .tmp/main.bin\n");
		return 1;
	}

	char filename[32];
	sprintf(filename, "build/%s.pnach", GAME_CRC);
	FILE *out = fopen(filename, "w");

	fprintf(out, "gametitle=Pac-Man World 2 (U)(SLUS-20224) (Greatest Hits)\n");
	fprintf(out, "comment=Mod Test\n\n");
	fprintf(out, "[Mod]\n");
	fprintf(out, "author=hexa.pet\n");

	uint8_t byte;
	uint32_t current_addr = CODE_CAVE_ADDR;

	fprintf(out, "// code\n");

	while (fread(&byte, 1, 1, bin) == 1)
	{
		// unlike the hook we set this to apply on startup only
		// sadly this breaks hot reloading though, but it's the price to pay so that
		// values aren't constantly reset
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

	for (int i = 0; i < 4; i++)
	{
		fprintf(out, "patch=0,EE,%08x,byte,%02x\n", MAIN_CALL_ADDR + i, hook_bytes[i]);
	}

	fclose(out);
	printf("created patch %s\n", filename);

	return 0;
}