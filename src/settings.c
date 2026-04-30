#include "game/pmw2lib.h"
#include "settings.h"
#include "util.h"

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
	DefMultiLanguageString text;
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
	inModSettingsMenu = false;
	Game_PopMainFuncs();
}

static mod_menu_option_t chaosModOptions[] = {
	{
		.text = {
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
		.text = {
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
		.text = { "4:3", "4:3" },
		.textOn = { "16:9", "16:9" },
		.description = {
			"Toggles aspect ratio.",
			/* SJIS */ "アスペクト比を 切り替えます。"
		},
		.type = OPTION_TOGGLE,
		.boolValuePtr = &widescreenAspect
	},
	{
		.text = { "English", "" },
		.textOn = { "", /* SJIS */ "ニホンゴ" },
		.description = {
			"Toggles current language between\nEnglish/Japanese.",
			/* SJIS */ "ゲンゴを エィゴと ニホンゴで\nきりかえます。"
		},
		.type = OPTION_TOGGLE,
		.boolValuePtr = &CurrentLanguage
	}
};

static mod_menu_option_t rootModOptions[] = {
	{
		.text = {
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
		.text = {
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
	// {
	// 	.text = "Developer Options",
	// 	.description = "shhhhh",
	// 	.type = OPTION_MENU,
	// 	.menuSize = 0
	// },
	{
		.text = {
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

int ModSettingsMenu(dummy_struct_t *db) {
	if (!inModSettingsMenu) {
		// setup menu
		inModSettingsMenu = true;
		modMenuPtr = rootModOptions;
		modMenuSize = 3;
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
					call_font_printf(0.5f, currentHeight, *option->boolValuePtr ? option->textOn[CurrentLanguage] : option->text[CurrentLanguage]);
					break;
				default:
					call_font_printf(0.5f, currentHeight, option->text[CurrentLanguage]);
					break;
			}
		}

		FontDefaults();
		SetFontAlignment(FONT_ALIGN_CENTERCENTER);
		SetFontWiggle(false);
		call_font_printf(0.5f, 0.9f, currentOption->description[CurrentLanguage]);

		// switch (currentOption->type) {
		// 	case OPTION_MENU:
		// 		FontDefaults();
		// 		SetFontAlignment(FONT_ALIGN_CENTERCENTER);
		// 		call_font_printf(0.5f, 0.6f, fmt((char[50]){0}, "menuPtr %00X menuSize %d", currentOption->menuPtr, currentOption->menuSize));
		// 		break;
		// 	case OPTION_TOGGLE:
		// 		FontDefaults();
		// 		SetFontAlignment(FONT_ALIGN_CENTERCENTER);
		// 		call_font_printf(0.5f, 0.6f, fmt((char[50]){0}, "boolValuePtr %00X %d", currentOption->boolValuePtr, *currentOption->boolValuePtr));
		// 		break;
		// 	case OPTION_SLIDER:
		// 		FontDefaults();
		// 		SetFontAlignment(FONT_ALIGN_CENTERCENTER);
		// 		call_font_printf(0.5f, 0.6f,
		// 			fmt((char[50]){0}, "sliderPtr %00X %s",
		// 				currentOption->sliderPtr,
		// 				float_to_str((char[16]){0}, *currentOption->sliderPtr, 4)
		// 			)
		// 		);
		// 		break;
		// 	case OPTION_BUTTON:
		// 		FontDefaults();
		// 		SetFontAlignment(FONT_ALIGN_CENTERCENTER);
		// 		call_font_printf(0.5f, 0.6f, fmt((char[50]){0}, "buttonCallbackPtr %00X", currentOption->buttonCallbackPtr));
		// 		break;
		// 	default:
		// 		break;
		// }

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
						playSoundSymbol(""); // force an error sound
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