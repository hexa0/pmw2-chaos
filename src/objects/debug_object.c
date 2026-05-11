#include "../game/pmw2lib.h"
#include "../util.h"
#include <string.h>

typedef struct debug_object_s
{
	OBJHEAD hd;
	BOOL pacmanNoclip;
	BOOL frameAdvance;
	FVEC noclipLocation;
	FVEC noclipSpeed;
	PACRTN oldState;
} debug_object_t;

static BOOL debugUIShown = false;

BOOL DebugToggle(BOOL *value, unsigned short currentButton, unsigned short oldButton) {
	if (currentButton && !oldButton) {
		*value = !(*value);

		if (*value) {
			playSoundSymbol("SND_EATDOT");
			return true;
		}
		else {
			playSoundSymbol("SND_15_EXIT_22");
			return true;
		}
	}

	return false;
}

int DebugObject(OBJHEAD *hd, messageType message, void *data)
{
	debug_object_t *obj = (debug_object_t *)hd;

	if (message == msg_render)
	{
		// toggle debug UI

		DebugToggle(&debugUIShown, pads[1].btn.cross, pads[1].old_btn.cross);
		DebugToggle(&widescreenAspect, pads[1].btn.left, pads[1].old_btn.left);

		if (DebugToggle(&obj->frameAdvance, pads[1].btn.up, pads[1].old_btn.up)) {
			if (obj->frameAdvance) {
				paused = 1;
			}
			else {
				paused = 0;
			}
		}

		if (obj->frameAdvance) {
			paused = 1;
			
			if (pads[1].btn.circle && !pads[1].old_btn.circle) {
				paused = 0;
			}
			else {
				pads[0].data = pads[0].old_data;
			}
		}

		if (DebugToggle(&obj->pacmanNoclip, pads[1].btn.down, pads[1].old_btn.down)) {
			if (obj->pacmanNoclip) {
				obj->noclipLocation = pacManObject->Head.pos;
				obj->noclipSpeed = pacManObject->motion.speed;
				obj->oldState = pacManObject->PMAction;
			}
			else {
				pacManObject->Head.pos = obj->noclipLocation;
				pacManObject->motion.speed = obj->noclipSpeed;
				pacManObject->PMAction = obj->oldState;
			}
		}

		if (obj->pacmanNoclip)
		{
			float padX = ((float)pads[0].analog[2] / 255.0f) - 0.5f;
			float padY = pads[0].btn.l2 ? 1.0f : pads[0].btn.r2 ? -1.0f : 0.0f;
			float padZ = ((float)pads[0].analog[3] / 255.0f) - 0.5f;

			FVEC localMove = { -padX * 2.0f, padY * 2.0f, padZ * 2.0f, 0 };
			FVEC worldMove = RotateVector(localMove, cameraRot);
		
			obj->noclipLocation.x += worldMove.x;
			obj->noclipLocation.y += worldMove.y;
			obj->noclipLocation.z += worldMove.z;

			pacManObject->Head.pos = obj->noclipLocation;
			pacManObject->motion.speed = obj->noclipSpeed;
			pacManObject->PMAction = PMDummyState;
		}
		
		if (debugUIShown)
		{
			FontDefaults();
			SetFontShadow(1);
			SetFontColor(255, 255, 255);
			SetFontGradient(1);
			SetFontGradientTopColor(180, 0, 0, 100);
			SetFontGradientBottomColor(180, 180, 0, 100);
			call_SetFontScale(0.5);
			SetFontAlignment(FONT_ALIGN_LEFT);
			call_font_printf(0.02, 0.05, fmt((char[200]){0},
				"pacManObject %p\ndebugObject %p\nlowestYpos %s\nmotion.speed.y %s\nPMAction %s",
				&pacManObject,
				&obj,
				float_to_str((char[16]){0}, pacManObject->lowestYpos, 4),
				float_to_str((char[16]){0}, pacManObject->motion.speed.y, 4),
				GetPlayerActionName(pacManObject->PMAction),
				ScreenFaderActive(),
				ScreenFaderFading()
			));
		}
	}

	return 0;
}