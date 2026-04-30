#include "../game/pmw2lib.h"
#include "../util.h"
#include "../settings.h"

typedef struct settings_object_s
{
	OBJHEAD hd;
 
} settings_object_t;

void SettingsObject_Process(settings_object_t *obj)
{
	if (!inModSettingsMenu) {
		widescreenAspect = gModSettings.widescreen;
		CurrentLanguage = gModSettings.currentLanguage;
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