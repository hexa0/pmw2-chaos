#include "game/pmw2lib.h"
#include "settings.h"
#include "util.h"
#include <string.h>
#include <libsd.h>
#include "widescreen.h"

typedef enum mod_menu_option_type {
	OPTION_ENDMENU,
	OPTION_MENU,
	OPTION_BUTTON,
	OPTION_TOGGLE,
	OPTION_SLIDER,
	OPTION_LABEL
} mod_menu_option_type;

typedef struct mod_menu_option_s mod_menu_option_t;

typedef void (*button_callback_hndl)();

struct mod_menu_option_s
{
	DefMultiLanguageString *text;
	DefMultiLanguageString *textOn;
	DefMultiLanguageString description;
	mod_menu_option_type type;
	BOOL *boolValuePtr;
	struct mod_menu_option_s* menuPtr;
	unsigned int menuSize;
	float sliderMin;
	float sliderMax;
	float *sliderPtr;
	button_callback_hndl buttonCallbackPtr;
};

mod_menu_option_t* modMenuPtr;
unsigned int modMenuSize;
unsigned int modMenuBackStackSize;
mod_menu_option_t* modMenuBackPtrStack[10];
unsigned int modMenuBackSizeStack[10];
unsigned int modMenuBackPositionStack[10];
unsigned int modMenuCurrentOption;

void ApplyModSettings() {
	gStagedModSettings.currentLanguage = CurrentLanguage;
	
	gModSettings = gStagedModSettings;
	svGameCurrent.UserPreference.CurrentLanguage = gModSettings.currentLanguage;

	inModSettingsMenu = false;
	Game_PopMainFuncs();
}

void EnterCameraMenu() {
	Game_PushMainFuncs();
	Game_AssignMainFuncs(DoCameraMenu, (RENDER_VU1_FUNC)0x0, (RENDER_VU1_TRANSPARENT_FUNC)0x0);
}

void EnterSoundTestMenu() {
	Game_PushMainFuncs();
	Game_AssignMainFuncs(DoSoundTest, (RENDER_VU1_FUNC)0x0, (RENDER_VU1_TRANSPARENT_FUNC)0x0);
}

void EnterMusicTestMenu() {
	Game_PushMainFuncs();
	Game_AssignMainFuncs(DoMusicTest, (RENDER_VU1_FUNC)0x0, (RENDER_VU1_TRANSPARENT_FUNC)0x0);
}

void EnterAdjustScreenMenu() {
	Game_PushMainFuncs();
	Game_AssignMainFuncs(UpdateScreenMenu, (RENDER_VU1_FUNC)0x0, (RENDER_VU1_TRANSPARENT_FUNC)0x0);
}

void EnterVibScreenMenu() {
	Game_PushMainFuncs();
	Game_AssignMainFuncs(VibOpt, (RENDER_VU1_FUNC)0x0, (RENDER_VU1_TRANSPARENT_FUNC)0x0);
}

void EnterBrightnessScreenMenu() {
	Game_PushMainFuncs();
	Game_AssignMainFuncs(BrightnessOpt, (RENDER_VU1_FUNC)0x0, (RENDER_VU1_TRANSPARENT_FUNC)0x0);
}

void EnterLangScreenMenu() {
	Game_PushMainFuncs();
	Game_AssignMainFuncs(LangOpt, (RENDER_VU1_FUNC)0x0, (RENDER_VU1_TRANSPARENT_FUNC)0x0);
}

void ToggleAspectRatio() {
	printf("%d\n", gStagedModSettings.aspectRatio);
	gStagedModSettings.aspectRatio++;
	printf("%d\n", gStagedModSettings.aspectRatio);

	if (gStagedModSettings.aspectRatio >= numAspectRatios) {
		gStagedModSettings.aspectRatio = 0;
	}

	SetAspectRatio(&gStagedModSettings.aspectRatio);
}

static mod_menu_option_t chaosModOptions[] = {
		{
		.text = &(DefMultiLanguageString){
			"Mod Disabled",
		},
		.textOn = &(DefMultiLanguageString){
			"Mod Enabled",
		},
		.type = OPTION_TOGGLE,
		.boolValuePtr = &gStagedModSettings.chaosModActive,
	},
	{
		.text = &(DefMultiLanguageString){
			"Random Effect Rate Modifier",
			/* SJIS */ "ランダムコウカ ハッセイリツ"
		},
		.description = {
			"Increases/Decreases how often\nrandom effects will be applied.",
			/* SJIS */ "ランダムコウカの ハッセイヒン度を\nヘンコウします。"
		},
		.type = OPTION_SLIDER,
		.sliderMin = 0.0f,
		.sliderMax = 10.0f,
		.sliderPtr = &gStagedModSettings.chaosEffectDurationMultipler
	},
	{
		.text = &(DefMultiLanguageString){
			"Random Effect Duration Modifier",
			/* SJIS */ "ランダムコウカ ジゾクジカン"
		},
		.description = {
			"Increases/Decreases how long\nrandom effects will last.",
			/* SJIS */ "ランダムコウカの ジゾクジカンを\nヘンコウします。"
		},
		.type = OPTION_SLIDER,
		.sliderMin = 0.0f,
		.sliderMax = 10.0f,
		.sliderPtr = &gStagedModSettings.chaosEffectRateDiviser
	},
	{
		.type = OPTION_ENDMENU
	}
};

static mod_menu_option_t gameExtraOptions[] = {
	{
		.text = &currentRatioText,
		.description = {
			"Toggles aspect ratio.",
			/* SJIS */ "アスペクト比を 切り替えます。"
		},
		.type = OPTION_BUTTON,
		.buttonCallbackPtr = &ToggleAspectRatio
	},
	{
		.text = &(DefMultiLanguageString){ "English", "" },
		.textOn = &(DefMultiLanguageString){ "", /* SJIS */ "ニホンゴ" },
		.description = {
			"Toggles current language between\nEnglish/Japanese.",
			/* SJIS */ "ゲンゴを エィゴと ニホンゴで\nきりかえます。"
		},
		.type = OPTION_TOGGLE,
		.boolValuePtr = &CurrentLanguage
	},
	{
		.type = OPTION_ENDMENU
	}
};

static mod_menu_option_t developerOptions[] = {
	{
		.text = &(DefMultiLanguageString){ "LangOpt" },
		.description = { "THIS WILL SOFTLOCK YOU,\nthis is the vanilla language select UI"},
		.type = OPTION_BUTTON,
		.buttonCallbackPtr = &EnterLangScreenMenu
	},
	{
		.text = &(DefMultiLanguageString){ "Camera Menu" },
		.description = { "camera debug information" },
		.type = OPTION_BUTTON,
		.buttonCallbackPtr = &EnterCameraMenu
	},
	{
		.text = &(DefMultiLanguageString){ "Sound Test" },
		.description = { "sound test, a little buggy but nice"},
		.type = OPTION_BUTTON,
		.buttonCallbackPtr = &EnterSoundTestMenu
	},
	{
		.text = &(DefMultiLanguageString){ "Music Test" },
		.description = { "loads netdata/music/jeff.mus.mus\nbecause jeff can't call\nplayMusic right apparently"},
		.type = OPTION_BUTTON,
		.buttonCallbackPtr = &EnterMusicTestMenu
	},
	{
		.type = OPTION_ENDMENU
	}
};

static mod_menu_option_t rootModOptions[] = {
	{
		.text = &(DefMultiLanguageString){
			"Chaos Edition Settings",
			/* SJIS */ "カオスエディション セッテイ"
		},
		.description = {
			"Adjust difficulty & more.",
			/* SJIS */ "ムズカシサなどを チョウセイします。"
		},
		.type = OPTION_MENU,
		.menuPtr = chaosModOptions,
	},
	{
		.text = &(DefMultiLanguageString){
			"Pacman World 2",
			/* SJIS */ "パックマンワールド 2"
		},
		.description = {
			"Adjust additional\nPac-Man World 2™ settings.",
			/* SJIS */ "パックマンワールド 2の\nツイカセッテイを チョウセイします。"
		},
		.type = OPTION_MENU,
		.menuPtr = gameExtraOptions,
	},
	{
		.text = &(DefMultiLanguageString){
			"Developer Options"
		},
		.description = "shhhhh",
		.type = OPTION_MENU,
		.menuPtr = developerOptions,
	},
	{
		.text = &(DefMultiLanguageString){
			"Apply Changes",
			/* SJIS */ "セッテイを ホゾン"
		},
		.description = {
			"Press *D to apply changes.",
			/* SJIS */ "*D ボタンを オシテ テキヨウシマス。"
		},
		.type = OPTION_BUTTON,
		.buttonCallbackPtr = ApplyModSettings
	},
	{
		.type = OPTION_ENDMENU
	}
};

#define ENTRY_HEIGHT 0.05f

int GetSize(mod_menu_option_t *menu) {
	int i = 0;
	while (menu[i].type != OPTION_ENDMENU) i++;
	return i;
}

int ModSettingsMenu(sceGsDBuffDc_dummy_t *db) {
	if (!inModSettingsMenu) {
		// setup menu
		inModSettingsMenu = true;
		modMenuPtr = rootModOptions;
		modMenuSize = GetSize(rootModOptions);
		modMenuCurrentOption = 0;
		modMenuBackStackSize = 0;
	}
	else {
		// menu move

		if (pads[0].btn.up && !pads[0].old_btn.up) {
			if (modMenuCurrentOption == 0) {
				modMenuCurrentOption = modMenuSize - 1;
			}
			else {
				modMenuCurrentOption--;
			}
			playSoundSymbol("SND_13_HIGHLIGHT1_22");
		}

		if (pads[0].btn.down && !pads[0].old_btn.down) {
			if (modMenuCurrentOption == modMenuSize - 1) {
				modMenuCurrentOption = 0;
			}
			else {
				modMenuCurrentOption++;
			}
			playSoundSymbol("SND_13_HIGHLIGHT1_22");
		}

		mod_menu_option_t* currentOption = &modMenuPtr[modMenuCurrentOption];

		float currentHeight = 0.5f - ((ENTRY_HEIGHT * modMenuSize) / 2.0f);

		for (int i = 0; i < modMenuSize; i++) {
			mod_menu_option_t* option = &modMenuPtr[i];

			currentHeight += ENTRY_HEIGHT;

			FontDefaults();
			SetFontAlignment(FONT_ALIGN_CENTERCENTER);
			if (i == modMenuCurrentOption) {
				SetFontWiggle(true);
				call_Font_SetSelectColor(1.0f);
			}
			else {
				call_Font_SetNonSelectColor(1.0f);
			}

			switch (option->type) {
				case OPTION_TOGGLE:
					call_font_printf(0.5f, currentHeight, *option->boolValuePtr ? (*option->textOn)[CurrentLanguage] : (*option->text)[CurrentLanguage]);
					break;
				default:
					call_font_printf(0.5f, currentHeight, (*option->text)[CurrentLanguage]);
					break;
			}
		}

		FontDefaults();
		SetFontAlignment(FONT_ALIGN_CENTERCENTER);
		SetFontWiggle(false);
		call_font_printf(0.5f, 0.9f, currentOption->description[CurrentLanguage]);

		// interact check

		if (pads[0].btn.cross && !pads[0].old_btn.cross) {

			switch (currentOption->type) {
				case OPTION_BUTTON:
					playSoundSymbol("SND_EATDOT");

					currentOption->buttonCallbackPtr();
					break;
				case OPTION_MENU:
					playSoundSymbol("SND_EATDOT");

					if (modMenuBackStackSize == 10) {
						inModSettingsMenu = false;
						soundPlayError();
						Game_PopMainFuncs();
						return 0;
					}

					modMenuBackPtrStack[modMenuBackStackSize] = modMenuPtr;
					modMenuBackSizeStack[modMenuBackStackSize] = modMenuSize;
					modMenuBackPositionStack[modMenuBackStackSize] = modMenuCurrentOption;
					modMenuBackStackSize++;

					modMenuPtr = currentOption->menuPtr;
					modMenuSize = GetSize(modMenuPtr);
					modMenuCurrentOption = 0;
					break;
				case OPTION_TOGGLE:
					*currentOption->boolValuePtr = !(*currentOption->boolValuePtr);

					if (*currentOption->boolValuePtr) {
						playSoundSymbol("SND_EATDOT");
					}
					else {
						Menu_MakePadSounds(0x10);
					}
					break;
				default:
					break;
			}
		}
		
		// back menu check

		if (modMenuPtr == rootModOptions) {
			// restore changes if not applied

			if (pads[0].btn.triangle && !pads[0].old_btn.triangle) {
				RestoreSettings();
				inModSettingsMenu = false;
				
				Game_PopMainFuncs();
				Menu_MakePadSounds(0x10);
			}
		}
		else {
			if (pads[0].btn.triangle && !pads[0].old_btn.triangle) {
				modMenuPtr = modMenuBackPtrStack[modMenuBackStackSize - 1];
				modMenuSize = modMenuBackSizeStack[modMenuBackStackSize - 1];
				modMenuCurrentOption = modMenuBackPositionStack[modMenuBackStackSize - 1];
				modMenuBackStackSize--;
				Menu_MakePadSounds(0x10);
			}
		}
	}

	return 0;
}