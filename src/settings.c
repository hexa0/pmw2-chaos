#include "game/pmw2lib.h"
#include "settings.h"
#include "util.h"
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

// void _startstreaming_host(int fd, unsigned char *buffer, int size) {
// 	printf("_startstreaming_host %d 0x%00X %d\n", fd, &buffer, size);

// 	int bytesRead = sceRead(fd, buffer, size);

// 	if (bytesRead < 0) {
//         printf("host streaming error: %d\n", fd);
//     }
// }

// void _closestream_host(int fd) {
// 	printf("_closestream_host %d\n", fd);

//     if (fd >= 0) {
//         sceClose(fd);
//     }
// }

// char* MakeCaps(char *text) {
// 	char *p = text;

// 	while (*p != '\0') {
// 		if (*p >= 'a' && *p <= 'z') {
// 			*p -= 0x20;
// 		}
		
// 		p++;
// 	}

// 	return text;
// }

// int fixfordvd_host(char *dst, char *filename) {
// 	sprintf(dst, "host:%s", filename);

// 	MakeCaps(strchr(dst, ':'));
	
// 	printf("fixfordvd_host %s\n", dst);
// }

// int _streamfilefrompc_host(char *filename, int *length) {
// 	printf("_streamfilefrompc_host %s %d\n", filename, length);

// 	char name[64];
//     fixfordvd_host(name, filename);
    
//     int fd = sceOpen(name, 0x0001);

//     if (fd >= 0) {
//         *length = sceLseek(fd, 0, 2);
//         sceLseek(fd, 0, 0);
//     }
// 	else {
// 		// fixes loading screens stupidly relying on SCEECdSearchFile from the original code
// 		sprintf(name, "host:NETDATA/LEVELS/%s", MakeCaps(filename));

// 		fd = sceOpen(name, 0x0001);

// 		if (fd >= 0) {
// 			*length = sceLseek(fd, 0, 2);
// 			sceLseek(fd, 0, 0);
// 		}
// 		else {
// 			printf("_streamfile_host error %s %d\n", name, fd);
// 		}
// 	}

//     return fd;
// }

// void InjectHostFS() {
// 	// strcpy((char *)0x0057EBD8, "host:%s");
// 	assemble_j_at((unsigned int)_streamfilefrompc, (unsigned int)&_streamfilefrompc_host);
// 	assemble_j_at((unsigned int)_closestream, (unsigned int)&_closestream_host);
// 	assemble_j_at((unsigned int)_startstreaming, (unsigned int)&_startstreaming_host);

// 	FlushCache(0);
// 	FlushCache(1);

// 	playSoundSymbol("SND_CHCKPNT1");
// 	// assemble_j_at((unsigned int)fixfordvd, (unsigned int)&fixfordvd_host);
// }

// void UndoHostEEMusicTransfer() {
// 	assemble_jal_at((unsigned int)0x00175FA4, (unsigned int)&ee_musicTransferNowait);
// }

void ReloadAudio() {
	musicStop();
	iopUnload();
	soundInit();
	// musicInit();
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

static mod_menu_option_t developerOptions[] = {
	{
		.text = { "LangOpt" },
		.type = OPTION_BUTTON,
		.buttonCallbackPtr = &EnterLangScreenMenu
	},
	{
		.text = { "Camera Menu" },
		.type = OPTION_BUTTON,
		.buttonCallbackPtr = &EnterCameraMenu
	},
	{
		.text = { "Sound Test" },
		.type = OPTION_BUTTON,
		.buttonCallbackPtr = &EnterSoundTestMenu
	},
	{
		.text = { "Music Test" },
		.type = OPTION_BUTTON,
		.buttonCallbackPtr = &EnterMusicTestMenu
	},
	{
		.text = { "Load LEVEL RARS from host:" },
		.description = { "VERY EXPERIMENTAL\nthis will ONLY load the RARs\ni spent many hours trying to\nget sound to work just not possible\n with my skills"},
		.type = OPTION_BUTTON,
		// .buttonCallbackPtr = &InjectHostFS
	},
	{
		.text = { "Reload AUDIO>IRX" },
		.type = OPTION_BUTTON,
		.buttonCallbackPtr = &ReloadAudio
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
	{
		.text = "Developer Options",
		.description = "shhhhh",
		.type = OPTION_MENU,
		.menuSize = 0,
		.menuPtr = developerOptions,
		.menuSize = 6
	},
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

int ModSettingsMenu(sceGsDBuffDc_dummy_t *db) {
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