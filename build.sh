BUILD_DIR="build"
TMP_DIR=".tmp"

mkdir -p $BUILD_DIR $TMP_DIR

rm -rf .tmp/*
mips64r5900el-ps2-elf-gcc -c src/main.c -march=r5900 -O2 -fno-asynchronous-unwind-tables -fno-exceptions -fno-common -ffreestanding -D_EE
mv *.o .tmp/
mips64r5900el-ps2-elf-ld -T linker.ld -R game.syms -Ttext 0x00556A90 -o .tmp/patch.elf .tmp/*.o
mips64r5900el-ps2-elf-objcopy -O binary .tmp/patch.elf .tmp/patch.bin

ENTRY_ADDR=0x$(mips64r5900el-ps2-elf-nm .tmp/patch.elf | grep -w "init" | cut -d' ' -f1)

gcc make_pnach.c -o .tmp/make_pnach
.tmp/make_pnach $ENTRY_ADDR