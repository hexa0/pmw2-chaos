#include "settings.h"
#include "state.h"
// #include <tamtypes.h>
// #include <osd_config.h>

void CommitSettings() {
    unsigned char *s = (unsigned char*)&gStagedModSettings;
	unsigned char *d = (unsigned char*)&gModSettings;
	
    int size = sizeof(mod_settings_t);
    
    while (size--) {
        *d++ = *s++;
    }

	SetAspectRatio(&gModSettings.aspectRatio);
	CurrentLanguage = gModSettings.currentLanguage;
}

void RestoreSettings() {
    unsigned char *s = (unsigned char*)&gModSettings;
    unsigned char *d = (unsigned char*)&gStagedModSettings;

    int size = sizeof(mod_settings_t);
    
    while (size--) {
        *d++ = *s++;
    }

	SetAspectRatio(&gModSettings.aspectRatio);
	CurrentLanguage = gModSettings.currentLanguage;
}

int InitModSettings() {
	// defaults

	gStagedModSettings.chaosModActive = true;
	gStagedModSettings.chaosEffectDurationMultipler = 1.0f;
	gStagedModSettings.chaosEffectRateDiviser = 1.0f;
	gStagedModSettings.currentLanguage = svGameCurrent.UserPreference.CurrentLanguage;

	// sadly this causes a crash here so we detect emu instead
	// gModSettings.widescreen = configGetTvScreenType() == TV_SCREEN_169;

	gStagedModSettings.aspectRatio = RATIO_4_BY_3;

	if (isEmulator) {
        gStagedModSettings.aspectRatio = RATIO_16_BY_9;
    }

	CommitSettings();
}