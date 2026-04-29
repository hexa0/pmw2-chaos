#ifndef CHAOS_OBJECT_H
#define CHAOS_OBJECT_H

#include "chaos_object.c"

void CreateChaosObject()
{
	obj_Setup(ChaosObject, sizeof(chaos_object_t), 0, 0, "chaosObject", 1);
}

#endif // CHAOS_OBJECT_H