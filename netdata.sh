#!/bin/bash

GAME_PATH="$1"
NETDATA_DIR="build/netdata"

if [ -z "$GAME_PATH" ]; then
	echo "no game path specified"
	exit 1
fi

if [ ! -f "$GAME_PATH/SYSTEM.CNF" ]; then
	echo "specified path is not a valid copy of PMW2"
	exit 1
fi

mkdir -p "$NETDATA_DIR"

mappings=(
	"LEVELS:levels"
	"SOUND:sound"
	"MOVIES:movies"
	"MUSIC:music"
	"MODULES:modules"
)

for mapping in "${mappings[@]}"; do
	SRC="${mapping%%:*}"
	DEST="${mapping#*:}"
	
	ACTUAL_SRC=$(find "$GAME_PATH" -maxdepth 1 -iname "$SRC")
	
	if [ -d "$ACTUAL_SRC" ]; then
		echo "copy $SRC to $NETDATA_DIR/$DEST"
		cp --preserve=timestamps -r "$ACTUAL_SRC" "$NETDATA_DIR/$DEST"
	else
		echo "we are missing $SRC??"
	fi
done

echo "normalizing filenames"
find "$NETDATA_DIR" -depth | while read -r path; do
	DIR=$(dirname "$path")
	NAME=$(basename "$path")
	LOWER_NAME=$(echo "$NAME" | tr '[:upper:]' '[:lower:]')
	
	if [ "$NAME" != "$LOWER_NAME" ]; then
		mv "$path" "$DIR/$LOWER_NAME"
	fi
done

echo "extracting"

export_extract() {
	local rar_file="$1"
	local extract_dir="${rar_file%.rar}"
	
	mkdir -p "$extract_dir"
	
	unrar x -idq -o+ "$rar_file" "$extract_dir/"
	
	if [ $? -eq 0 ]; then
		echo "extracted $(basename "$rar_file")"
	else
		echo "failed $(basename "$rar_file")"
		return 1
	fi
}

export -f export_extract
find "$NETDATA_DIR/levels" -type f -name "*.rar" -print0 | xargs -0 -P 4 -n 1 bash -c 'export_extract "$@"' _

exit 0