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

void IncreaseAnimScale(KEY_FRAME frames[], int count) {
	if (count == 0) {
		return;
	}
	
	KEY_FRAME* firstFrame = &frames[0];

	for (int i = 0; i < count; i++) {
		KEY_FRAME* frame = &frames[i];
		// KEY_FRAME* lastFrame = &frames[i == 0 ? 0 : i - 1];

		frame->vx = firstFrame->vx;
		frame->vy = firstFrame->vy;
		frame->vz = firstFrame->vz;

		// frame->vx = (lastFrame->vx - frame->vx) * 2.0f;
		// frame->vy = (firstFrame->vy - frame->vy) * 2.0f;
		// frame->vz = (lastFrame->vz - frame->vz) * 2.0f;
	}
}

int DebugObject(OBJHEAD *hd, messageType message, void *data)
{
	debug_object_t *obj = (debug_object_t *)hd;

	if (message == msg_init)
	{

		// globalAnimArray[1]->anim_node_tpls = globalAnimArray[32]->anim_node_tpls;

		for (int i = 0; i < 128; i++) {
			if (globalAnimArray[i]) {
				ANIM_TPL* node = globalAnimArray[i];

				// node->anim_node_tpls = globalAnimArray[32]->anim_node_tpls;

				// printf("%d %s\n", i, node->name);

				for (int j = 0; j < node->num_anim_node_tpls; j++) {
					ANIM_NODE_TPL* anim_node = node->anim_node_tpls[j];

					// for (int k = 0; k < anim_node->num_t_kframes; k++) {
					// 	KEY_FRAME* frame = &anim_node->first_t_kframe[k];
					// 	frame->vx += (frand() - 0.5f);
					// 	frame->vy += (frand() - 0.5f);
					// 	frame->vz += (frand() - 0.5f);
					// }

					// for (int k = 0; k < anim_node->num_r_kframes; k++) {
					// 	KEY_FRAME* frame = &anim_node->first_r_kframe[k];
					// 	frame->vx += (frand() - 0.5f);
					// 	frame->vy += (frand() - 0.5f);
					// 	frame->vz += (frand() - 0.5f);
					// }
				
					// for (int k = 0; k < anim_node->num_s_kframes; k++) {
					// 	KEY_FRAME* frame = &anim_node->first_s_kframe[k];
					// 	frame->vx += (frand() - 0.5f);
					// 	frame->vy += (frand() - 0.5f);
					// 	frame->vz += (frand() - 0.5f);
					// }

					// for (int k = 0; k < anim_node->num_t_kframes; k++) {
					// 	KEY_FRAME* frame = &anim_node->first_t_kframe[k];
					// 	frame->vx *= 2.0f;
					// 	frame->vy *= 2.0f;
					// 	frame->vz *= 2.0f;
					// }

					// IncreaseAnimScale(anim_node->first_t_kframe, anim_node->num_t_kframes);
					// IncreaseAnimScale(anim_node->first_s_kframe, anim_node->num_s_kframes);
					// IncreaseAnimScale(anim_node->first_r_kframe, anim_node->num_r_kframes);
				}
				// globalAnimArray[i]->total_anim_time = 0.0f;
			}
		}
	}

	if (message == msg_render)
	{
		// toggle debug UI

		DebugToggle(&debugUIShown, pads[1].btn.cross, pads[1].old_btn.cross);
		DebugToggle(&widescreenAspect, pads[1].btn.left, pads[1].old_btn.left);
		DebugToggle(&obj->pacmanNoclip, pads[1].btn.down, pads[1].old_btn.down);
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

		if (!obj->pacmanNoclip)
		{
			obj->noclipLocation = pacManObject->Head.pos;
			obj->noclipSpeed = pacManObject->motion.speed;
		}
		else
		{
			pacManObject->Head.pos = obj->noclipLocation;
			pacManObject->motion.speed = obj->noclipSpeed;
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