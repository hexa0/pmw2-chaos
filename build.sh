BUILD_DIR="build"
TMP_DIR=".tmp"

mkdir -p $BUILD_DIR $TMP_DIR

rm -rf .tmp/*
mips64r5900el-ps2-elf-gcc -E -C src/main.c -march=r5900 -D_EE > .tmp/main.i

# jp text
python3 -c "
import re
import sys

def to_sjis_hex(match):
    sjis_bytes = match.group(1).encode('shift_jis', errors='replace')
    return '\"{}\"'.format(''.join(f'\\\\x{b:02x}' for b in sjis_bytes))

with open('.tmp/main.i', 'r') as f:
    code = f.read()

processed = re.sub(r'/\* SJIS \*/\s*\"([^\"]+)\"', to_sjis_hex, code)

with open('.tmp/main.c', 'w') as f:
    f.write(processed)
"

mips64r5900el-ps2-elf-gcc -c .tmp/main.c -march=r5900 -O2 -fno-asynchronous-unwind-tables -fno-exceptions -fno-common -ffreestanding -D_EE
mv *.o .tmp/
mips64r5900el-ps2-elf-ld -T linker.ld -R game.syms -Ttext 0x01F00000 -o .tmp/patch.elf .tmp/*.o
mips64r5900el-ps2-elf-objcopy -O binary .tmp/patch.elf .tmp/patch.bin

ENTRY_ADDR=0x$(mips64r5900el-ps2-elf-nm .tmp/patch.elf | grep -w "init" | cut -d' ' -f1)

gcc make_pnach.c -o .tmp/make_pnach
.tmp/make_pnach $ENTRY_ADDR