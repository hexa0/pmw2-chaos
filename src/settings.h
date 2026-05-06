#ifndef SETTINGS_H
#define SETTINGS_H

#include "widescreen.h"

typedef struct mod_settings_s {
	BOOL chaosModActive;
	float chaosEffectRateDiviser;
	float chaosEffectDurationMultipler;
	int currentLanguage;
	AspectRatio aspectRatio;
} mod_settings_t;

mod_settings_t gModSettings;
mod_settings_t gStagedModSettings;
BOOL inModSettingsMenu;

#include "settings.c"

#endif // SETTINGS_H