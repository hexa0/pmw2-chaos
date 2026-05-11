while IFS= read -r -d '' file; do
	echo "process: $file"
	nmustool enc "$file" "$NETDATA_DIR/music/${file%.*}.mus"
done < <(find . -type f -name "*.wav" -print0)