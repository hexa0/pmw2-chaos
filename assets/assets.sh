#!/bin/bash

NETDATA_DIR=$(realpath "../build/netdata")
BUILD_DIR=$(realpath "$NETDATA_DIR/levels/pacman/mod")
SOURCE_DIR=$(realpath "../build/")

if [ -d "$BUILD_DIR" ] && [ "$BUILD_DIR" != "/" ] && [ "$BUILD_DIR" != "$HOME" ]; then
	echo "clearing build directory"
	rm -rf "$BUILD_DIR"
fi

echo "creating folder structure"
find . -type d -not -path '*/.*' -exec mkdir -p "$BUILD_DIR/{}" \;

echo "processing .source files"
while IFS= read -r -d '' file; do
	echo "process: $file"
	
	RAW_SRC_PATH=$(cat "$file" | xargs)
	
	if [ -z "$RAW_SRC_PATH" ]; then
		echo "Warning: $file is empty, skipping."
		continue
	fi

	REL_DIR=$(dirname "$file")
	CLEAN_REL_DIR="${REL_DIR#./}"
	
	if [ "$CLEAN_REL_DIR" = "." ]; then
		DEST_DIR="$BUILD_DIR"
	else
		DEST_DIR="$BUILD_DIR/$CLEAN_REL_DIR"
	fi

	FULL_SRC_PATH="$SOURCE_DIR/$RAW_SRC_PATH"

	if [ -f "$FULL_SRC_PATH" ]; then
		cp --preserve=timestamps "$FULL_SRC_PATH" "$DEST_DIR/"
	else
		echo "could not find $FULL_SRC_PATH"
	fi
done < <(find . -type f -name "*.source" -print0)

echo "processing .build.sh files"
while IFS= read -r -d '' file; do
	echo "process: $file"

	SCRIPT_DIR=$(dirname "$(realpath "$file")")
	
	REL_DIR=$(dirname "$file")
	CLEAN_REL_DIR="${REL_DIR#./}"
	
	if [ "$CLEAN_REL_DIR" = "." ]; then
		TARGET_PATH="$BUILD_DIR"
	else
		TARGET_PATH="$BUILD_DIR/$CLEAN_REL_DIR"
	fi
	
	export TARGET_DIR="$TARGET_PATH"
	export NETDATA_DIR="$NETDATA_DIR"
	
	chmod +x "$file"
	(
		cd "$SCRIPT_DIR" || return 1
		"./$(basename "$file")"
	)
	
	if [ $? -ne 0 ]; then
		echo "error building $file"
	fi
done < <(find . -type f -name "*.build.sh" -print0)

echo "removing empty folders"
find "$BUILD_DIR" -mindepth 1 -type d -empty -delete

echo "repacking levels"

(
	source ../.env
	cd ../build/netdata/levels/

	export WINEPREFIX=$WINEPREFIX
	export WINEDEBUG=-all
	export EGL_LOG_LEVEL=fatal

	(
		LEVEL="pacman"
		cd $LEVEL
		
		echo "repacking $LEVEL.rar"
		rm -f ../$LEVEL.rar
		wine C:/WinRAR/Rar.Exe a -inul -r -md64 -m5 -mde -s -tk -av- -ri15:0 "../$LEVEL.rar" \*
		echo "repacked $LEVEL.rar"
	)
)

echo "making symlinks"
mkdir -p "../build/netdata/mod"
ln -s $(realpath "../build/MOD.BIN") $(realpath "../build/netdata/mod/mod.bin")