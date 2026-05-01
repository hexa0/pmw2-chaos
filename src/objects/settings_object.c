#include "../game/pmw2lib.h"
#include "../util.h"
#include "../settings.h"
#include <string.h>

typedef struct settings_object_s
{
	OBJHEAD hd;
 
} settings_object_t;

char* const cinema_txt = (char*)0x583BF8;
const char* cinema_txt_en = "cinema.txt";
const char* cinema_txt_jp = "jcinema.txt";

void SettingsObject_Update(settings_object_t *obj)
{
	if (!inModSettingsMenu) {
		widescreenAspect = gModSettings.widescreen;
		CurrentLanguage = svGameCurrent.UserPreference.CurrentLanguage;

		// absolute jcinema.txt
		if (CurrentLanguage == 0) {
			strcpy(cinema_txt, cinema_txt_en);
		}
		else if (CurrentLanguage == 1) {
			strcpy(cinema_txt, cinema_txt_jp);
		}
	}
}

int SettingsObject(OBJHEAD *hd, messageType message, void *data)
{
	settings_object_t *obj = (settings_object_t *)hd;

	switch (message)
	{
		case msg_init:
			// this fixes loading a save set to JP while the menu was on english
			// otherwise it tries to load cinema instead of jcinema
			// which would be absolutely NOT jcinema
			SettingsObject_Update(obj);
			break;
		case msg_processFrame:
			SettingsObject_Update(obj);
			break;
		default:
			break;
	}

	return 0;
}