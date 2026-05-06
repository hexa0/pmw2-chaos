#include "game/pmw2lib.h"
#include "settings.h"
#include "util.h"
#include <string.h>
#include <libsd.h>

typedef enum mod_menu_option_type {
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
	DefMultiLanguageString textOn;
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
	gStagedModSettings.widescreen = widescreenAspect;
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

static const DefMultiLanguageString ratios_43 = {"4:3", "4:3"};
static const DefMultiLanguageString ratios_169 = {"16:9", "16:9"};

static DefMultiLanguageString currentRatioText = {"4:3", "4:3"};

void widescreen_loading_bg_sprite_draw() {
	__asm__ volatile (
		"lui $v1, 0x3F00\n\t"
		"mtc1 $v1, $f1\n\t"
		"lui $v1, 0x3F40\n\t"
		"mtc1 $v1, $f2\n\t"

		"sub.s $f12, $f12, $f1\n\t"
		"mul.s $f12, $f12, $f2\n\t"
		"add.s $f12, $f12, $f1\n\t"

		"li $a3, 1\n\t"

		"j FontDrawSpriteZ\n\t"
		"nop\n\t"
	);
}

void widescreen_pacdot_collecting_draw() {
	__asm__ volatile (
		"li $a3, 1\n\t"

		"j FontDrawSpriteZ\n\t"
		"nop\n\t"
	);
}

void EnableWidescreen() {
	currentRatioText[0] = ratios_169[0];
	currentRatioText[1] = ratios_169[1];

	assemble_jal_at(0x001837F0, (unsigned int)&widescreen_loading_bg_sprite_draw);
	assemble_jal_at(0x001DBD08, (unsigned int)&widescreen_pacdot_collecting_draw);
	FlushCache(0);
	FlushCache(2);
}

void DisableWidescreen() {
	currentRatioText[0] = ratios_43[0];
	currentRatioText[1] = ratios_43[1];
	
	assemble_jal_at(0x001837F0, (unsigned int)&FontDrawSpriteZ);
	assemble_jal_at(0x001DBD08, (unsigned int)&FontDrawSpriteZ);
	FlushCache(0);
	FlushCache(2);
}

void ToggleWidescreen() {
	gStagedModSettings.widescreen = !gStagedModSettings.widescreen;
	widescreenAspect = gStagedModSettings.widescreen;

	if (widescreenAspect) {
		EnableWidescreen();
	}
	else {
		DisableWidescreen();
	}
}


static mod_menu_option_t chaosModOptions[] = {
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
		.buttonCallbackPtr = &ToggleWidescreen
	},
	{
		.text = &(DefMultiLanguageString){ "English", "" },
		.textOn = { "", /* SJIS */ "ニホンゴ" },
		.description = {
			"Toggles current language between\nEnglish/Japanese.",
			/* SJIS */ "ゲンゴを エィゴと ニホンゴで\nきりかえます。"
		},
		.type = OPTION_TOGGLE,
		.boolValuePtr = &CurrentLanguage
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
		.menuSize = 2
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
		.menuSize = 2
	},
	{
		.text = &(DefMultiLanguageString){
			"Developer Options"
		},
		.description = "shhhhh",
		.type = OPTION_MENU,
		.menuSize = 0,
		.menuPtr = developerOptions,
		.menuSize = 4
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
	}
};

#define ENTRY_HEIGHT 0.05f

int ModSettingsMenu(sceGsDBuffDc_dummy_t *db) {
	if (!inModSettingsMenu) {
		// setup menu
		inModSettingsMenu = true;
		modMenuPtr = rootModOptions;
		modMenuSize = 4;
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
					call_font_printf(0.5f, currentHeight, *option->boolValuePtr ? option->textOn[CurrentLanguage] : (*option->text)[CurrentLanguage]);
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
					modMenuSize = currentOption->menuSize;
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
			// cancel changes
			if (pads[0].btn.triangle && !pads[0].old_btn.triangle) {
				gStagedModSettings = gModSettings;
				inModSettingsMenu = false;
				Game_PopMainFuncs();
				Menu_MakePadSounds(0x10);

				widescreenAspect = gModSettings.widescreen;
				CurrentLanguage = gModSettings.currentLanguage;
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