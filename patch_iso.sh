LBA=$(isoinfo -i "game.iso" -l -R | grep "SLUS_202.24" | awk -F'[' '{print $2}' | awk '{print $1}')

dd if="build/game.elf" of="game.iso" bs=2048 seek=$LBA conv=notrunc

echo $BYTE_OFFSET