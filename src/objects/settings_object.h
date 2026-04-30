#ifndef SETTINGS_OBJECT_H
#define SETTINGS_OBJECT_H

#include "settings_object.c"

void CreateSettingsObject()
{
	obj_Setup(SettingsObject, sizeof(settings_object_t), 0, 0, "settingsObject", 1);
}

#endif // SETTINGS_OBJECT_H