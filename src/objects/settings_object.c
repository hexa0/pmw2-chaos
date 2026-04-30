#include "../game/pmw2lib.h"
#include "../util.h"
#include "../settings.h"

typedef struct settings_object_s
{
	OBJHEAD hd;
 
} settings_object_t;

char* const gameCinemaBuffer = (char*)0x583BF8;
const char* cinema_en = "cinema.txt";
const char* cinema_jp = "jcinema.txt";

void SettingsObject_Process(settings_object_t *obj)
{
	if (!inModSettingsMenu) {
		widescreenAspect = gModSettings.widescreen;
		CurrentLanguage = gModSettings.currentLanguage;

		// absolute jcinema.txt
		if (CurrentLanguage == 0) {
			strcpy(gameCinemaBuffer, cinema_en);
		}
		else if (CurrentLanguage == 1) {
			strcpy(gameCinemaBuffer, cinema_jp);
		}
	}
}

int SettingsObject(OBJHEAD *hd, messageType message, void *data)
{
	settings_object_t *obj = (settings_object_t *)hd;

	switch (message)
	{
		case msg_processFrame:
			SettingsObject_Process(obj);
			break;
		default:
			break;
	}

	return 0;
}