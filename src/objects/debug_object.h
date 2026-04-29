#ifndef DEBUG_OBJECT_H
#define DEBUG_OBJECT_H

#include "debug_object.c"

void CreateDebugObject()
{
	obj_Setup(DebugObject, sizeof(debug_object_t), 0, 0, "debugObject", 1);
}

#endif // DEBUG_OBJECT_H