#!/bin/bash

mkdir -p build build/tool .tmp

echo building tools
gcc tool/make_patches.c -o build/tool/make_patches
gcc tool/encode_shift_jis.c -o build/tool/encode_shift_jis

echo GCC preprocessing

rm -rf .tmp/*
mkdir .tmp/mod/ .tmp/writer/ .tmp/cwriter/
mips64r5900el-ps2-elf-gcc -E -C src/main.c -march=r5900 -I$PS2SDK/ee/lib -I$PS2SDK/ee/include -I$PS2SDK/common/include -D_EE > .tmp/mod/main.i

echo converting shift-JIS

build/tool/encode_shift_jis .tmp/mod/main.i

echo compiling mod

mips64r5900el-ps2-elf-gcc -c .tmp/mod/main.i -march=r5900 -O2 -fno-asynchronous-unwind-tables -fno-exceptions -fno-common -ffreestanding -I$PS2SDK/ee/lib -I$PS2SDK/ee/include -I$PS2SDK/common/include -D_EE
mv *.o .tmp/mod
echo linking mod
mips64r5900el-ps2-elf-ld -R game.syms -L$PS2SDK/ee/lib -L$PS2DEV/gsKit/lib -L$PS2SDK/ports/lib -L$PS2SDK/ports/lib -L$PS2DEV/ee/mips64r5900el-ps2-elf/lib -L$PS2DEV/ee/lib/gcc/mips64r5900el-ps2-elf/15.2.0 -zmax-page-size=128 .tmp/mod/*.o -ldebug -lkernel -lc -lgcc -T mod.ld -Ttext 0x01F00000 -o .tmp/mod/code.elf
echo extracting mod binary
mips64r5900el-ps2-elf-objcopy -O binary .tmp/mod/code.elf .tmp/mod/code.bin
cp .tmp/mod/code.bin build/MOD.BIN
printf "RunMod = 0x$(mips64r5900el-ps2-elf-nm .tmp/mod/code.elf | grep -w "RunMod" | cut -d' ' -f1);" >> .tmp/mod/code.syms
echo compiling writer
mips64r5900el-ps2-elf-gcc -c writer/main.c -march=r5900 -Oz -fno-asynchronous-unwind-tables -fno-exceptions -fno-common -ffreestanding -I$PS2SDK/ee/lib -I$PS2SDK/ee/include -I$PS2SDK/common/include -D_EE
mv *.o .tmp/writer
echo linking writer
mips64r5900el-ps2-elf-ld -R game.syms -R .tmp/mod/code.syms -L$PS2SDK/ee/lib -L$PS2DEV/gsKit/lib -L$PS2SDK/ports/lib -L$PS2SDK/ports/lib -L$PS2DEV/ee/mips64r5900el-ps2-elf/lib -L$PS2DEV/ee/lib/gcc/mips64r5900el-ps2-elf/15.2.0 -zmax-page-size=128 -T writer.ld -Ttext 0x01E00000 -zmax-page-size=128 .tmp/writer/*.o -o .tmp/writer/code.elf
echo extracting writer binary
mips64r5900el-ps2-elf-objcopy -O binary .tmp/writer/code.elf .tmp/writer/code.bin
echo generating cwriter irx headers

echo "__attribute__((aligned(16))) unsigned char usbd_irx[] = {" > .tmp/cwriter/irx_data.c
xxd -i < writer/usbd.irx >> .tmp/cwriter/irx_data.c
echo "};" >> .tmp/cwriter/irx_data.c
echo "unsigned int usbd_irx_len = $(stat -c%s writer/usbd.irx);" >> .tmp/cwriter/irx_data.c

echo "__attribute__((aligned(16))) unsigned char usbhdfsd_irx[] = {" >> .tmp/cwriter/irx_data.c
xxd -i < writer/usbhdfsd.irx >> .tmp/cwriter/irx_data.c
echo "};" >> .tmp/cwriter/irx_data.c
echo "unsigned int usbhdfsd_irx_len = $(stat -c%s writer/usbhdfsd.irx);" >> .tmp/cwriter/irx_data.c


mips64r5900el-ps2-elf-gcc -c .tmp/cwriter/irx_data.c -march=r5900 -O2 -o .tmp/cwriter/irx_data.o
echo compiling cwriter
mips64r5900el-ps2-elf-gcc -c writer/main.c -march=r5900 -Oz -DIS_CWRITER -fno-asynchronous-unwind-tables -fno-exceptions -fno-common -ffreestanding -I$PS2SDK/ee/lib -I$PS2SDK/ee/include -I$PS2SDK/common/include -D_EE
mv *.o .tmp/cwriter
echo linking cwriter
mips64r5900el-ps2-elf-ld -R game.syms -R .tmp/mod/code.syms -L$PS2SDK/ee/lib -L$PS2DEV/gsKit/lib -L$PS2SDK/ports/lib -L$PS2SDK/ports/lib -L$PS2DEV/ee/mips64r5900el-ps2-elf/lib -L$PS2DEV/ee/lib/gcc/mips64r5900el-ps2-elf/15.2.0 -zmax-page-size=128 -T cwriter.ld -Ttext 0x45ad10 -zmax-page-size=128 .tmp/cwriter/*.o -lmc -lkernel -o .tmp/cwriter/code.elf
echo extracting cwriter binary
mips64r5900el-ps2-elf-objcopy -O binary .tmp/cwriter/code.elf .tmp/cwriter/code.bin
echo creating restore.bin
VIRT_ADDR_HEX=$(mips64r5900el-ps2-elf-nm SLUS_202.24 | grep -w "actions" | cut -d' ' -f1)
VIRT_ADDR_DEC=$((16#$VIRT_ADDR_HEX))
FILE_OFFSET=$((VIRT_ADDR_DEC - 16#100000 + 16#1000))
LOADER_SIZE=$(stat -c%s ".tmp/cwriter/code.bin")
dd if=SLUS_202.24 of=build/RESTORE.BIN bs=1024 skip=$FILE_OFFSET count=$LOADER_SIZE iflag=skip_bytes,count_bytes
echo creating game.elf
cp SLUS_202.24 build/game.elf
echo injecting code.bin
dd if=.tmp/cwriter/code.bin of=build/game.elf bs=1 seek=$FILE_OFFSET conv=notrunc
echo finding cwriter init
CWRITER_INIT_HEX=$(mips64r5900el-ps2-elf-nm .tmp/cwriter/code.elf | grep -w "init" | cut -d' ' -f1)
CWRITER_INIT_DEC=$(printf "%d" 0x$CWRITER_INIT_HEX)
echo assembling jal
JAL_OPCODE=$(( 0x0C000000 | (CWRITER_INIT_DEC >> 2) ))
b0=$(( JAL_OPCODE & 0xFF ))
b1=$(( (JAL_OPCODE >> 8) & 0xFF ))
b2=$(( (JAL_OPCODE >> 16) & 0xFF ))
b3=$(( (JAL_OPCODE >> 24) & 0xFF ))
printf "$(printf '\\%o\\%o\\%o\\%o' $b0 $b1 $b2 $b3)" > .tmp/hook.bin
HOOK_FILE_OFFSET=$(( 0x002B79A8 - 0x100000 + 0x1000 ))
echo injecting hook at $(printf "%x" $HOOK_FILE_OFFSET)
dd if=.tmp/hook.bin of=build/game.elf bs=1 seek=$HOOK_FILE_OFFSET conv=notrunc

echo creating patches
build/tool/make_patches 0x$(mips64r5900el-ps2-elf-nm .tmp/writer/code.elf | grep -w "init" | cut -d' ' -f1)