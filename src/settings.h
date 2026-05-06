#ifndef SETTINGS_H
#define SETTINGS_H

typedef struct mod_settings_s {
	float chaosEffectRateDiviser;
	float chaosEffectDurationMultipler;
	int currentLanguage;
	BOOL widescreen;
} mod_settings_t;

mod_settings_t gModSettings;
mod_settings_t gStagedModSettings;
BOOL inModSettingsMenu;

int InitModSettings() {
	gModSettings.chaosEffectDurationMultipler = 1.0f;
	gModSettings.chaosEffectRateDiviser = 1.0f;
	gModSettings.currentLanguage = svGameCurrent.UserPreference.CurrentLanguage;
	gModSettings.widescreen = false;

	gStagedModSettings = gModSettings;
}

#include "settings.c"

#endif // SETTINGS_H