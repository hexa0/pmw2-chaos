#ifndef WIDESCREEN_H
#define WIDESCREEN_H

#include "game/pmw2lib.h"

typedef enum {
	RATIO_4_BY_3,
	RATIO_16_BY_9,
	RATIO_16_BY_10,
	RATIO_21_BY_9,
	numAspectRatios
} AspectRatio;

static char* ratio_43 = "Aspect Ratio (4:3)";
static char* ratio_169 = "Aspect Ratio (16:9)";
static char* ratio_1610 = "Aspect Ratio (16:10)";
static char* ratio_219 = "Aspect Ratio (21:9)";

static DefMultiLanguageString currentRatioText = {"?:?", "?:?"};

#include "widescreen.c"

void SetAspectRatio(AspectRatio *ratio) {
	switch (*ratio) {
		case RATIO_4_BY_3:
			UseAspect43();
			currentRatioText[0] = ratio_43;
			currentRatioText[1] = ratio_43;
			break;
		case RATIO_16_BY_9:
			UseAspect169();
			currentRatioText[0] = ratio_169;
			currentRatioText[1] = ratio_169;
			break;
		case RATIO_16_BY_10:
			UseAspect1610();
			currentRatioText[0] = ratio_1610;
			currentRatioText[1] = ratio_1610;
			break;
		case RATIO_21_BY_9:
			UseAspect219();
			currentRatioText[0] = ratio_219;
			currentRatioText[1] = ratio_219;
			break;
		default:
			break;
	}
}

#endif