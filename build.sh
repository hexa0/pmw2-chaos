BUILD_DIR="build"
TMP_DIR=".tmp"

mkdir -p $BUILD_DIR $TMP_DIR

mips64r5900el-ps2-elf-gcc -c src/main.c -o .tmp/main.o -march=r5900 -O2 -fno-asynchronous-unwind-tables -fno-exceptions -fno-common -ffreestanding -D_EE
mips64r5900el-ps2-elf-ld -T linker.ld -R game.syms -Ttext 0x00556A90 -o .tmp/main.elf .tmp/main.o
mips64r5900el-ps2-elf-objcopy -O binary .tmp/main.elf .tmp/main.bin

ENTRY_ADDR=0x$(mips64r5900el-ps2-elf-nm .tmp/main.elf | grep -w "init" | cut -d' ' -f1)

gcc make_pnach.c -o .tmp/make_pnach
.tmp/make_pnach $ENTRY_ADDR