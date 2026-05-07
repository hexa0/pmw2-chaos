#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
	if (argc < 4) {
		fprintf(stderr, "bad args");
		return 1;
	}

	char *target_path = argv[1];
	char *irx_path = argv[2];
	long offset = atol(argv[3]);

	FILE *f_elf = fopen(target_path, "rb+");
	if (!f_elf) {
		perror("Error opening target ELF");
		return 1;
	}

	FILE *f_irx = fopen(irx_path, "rb");
	if (!f_irx) {
		perror("Error opening IRX source");
		fclose(f_elf);
		return 1;
	}

	fseek(f_irx, 0, SEEK_END);
	uint32_t irx_size = (uint32_t)ftell(f_irx);
	fseek(f_irx, 0, SEEK_SET);

	if (fseek(f_elf, offset, SEEK_SET) != 0) {
		perror("error seeking in ELF");
		fclose(f_irx);
		fclose(f_elf);
		return 1;
	}

	if (fwrite(&irx_size, sizeof(uint32_t), 1, f_elf) != 1) {
		perror("error writing size header");
	}

	char buffer[4096];
	size_t bytes_read;
	while ((bytes_read = fread(buffer, 1, sizeof(buffer), f_irx)) > 0) {
		if (fwrite(buffer, 1, bytes_read, f_elf) != bytes_read) {
			perror("error writing IRX data");
			break;
		}
	}

	printf("injected %s (%u bytes) at offset %ld\n", irx_path, irx_size, offset);

	fclose(f_irx);
	fclose(f_elf);
	return 0;
}