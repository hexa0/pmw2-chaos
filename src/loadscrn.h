int levelRarSize = 0;
int levelRarReadPosition = 0;

static inline float LevelRarLoadProgress() {
	return (float)levelRarReadPosition / (float)levelRarSize;
}