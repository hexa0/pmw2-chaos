#include "../game/pacman.c"
#include "../util.c"

void DrawDebugText(char *chars)
{
	FontDefaults();
	SetFontShadow(0);
	SetFontColor(255, 255, 255);
	// SetFontGradient(1);
	// SetFontWiggle(0);
	// SetFontGradientTopColor(180, 0, 0, 100);
	// SetFontGradientBottomColor(180, 180, 0, 100);
	call_SetFontScale(0.5);
	SetFontAlignment(FONT_ALIGN_LEFT);
	call_font_printf(0, 0, chars);
}

typedef struct debug_object_s
{
	OBJHEAD hd;
	BOOL pacmanHasADHD;
	BOOL pacmanScrewWithScale;
	BOOL pacmanNoclip;
	char debugTextBuf[150];
	char fl1[10];
	char fl2[10];
	FVEC noclipLocation;
	FVEC noclipSpeed;
} debug_object_t;

static BOOL debugUIShown = false;

int DebugObject(OBJHEAD *hd, messageType message, void *data)
{
	debug_object_t *obj = (debug_object_t *)hd;

	if (message == msg_init)
	{
		obj->pacmanHasADHD = false;
		obj->pacmanScrewWithScale = false;
	}

	if (message == msg_processFrame)
	{
		// toggle debug UI

		if (pads[0].old_btn.l2 && !pads[0].btn.l2)
		{
			debugUIShown = !debugUIShown;

			if (debugUIShown)
			{
				soundPlay(soundFindSymbol("SND_EATDOT"));
			}
			else
			{
				soundPlay(soundFindSymbol("SND_15_EXIT_22"));
			}
		}

		if (!pads[0].btn.circle & pads[0].old_btn.circle) {
			QueueHandler_Disable();
			vmain("logo.pss");
		}

		if (pads[0].btn.l2 && (pads[0].btn.up && !pads[0].old_btn.up))
		{
			soundPlay(soundFindSymbol("SND_TAKEOFF3"));
			obj->pacmanHasADHD = !obj->pacmanHasADHD;
		}

		if (pads[0].btn.l2 && (pads[0].btn.right && !pads[0].old_btn.right))
		{
			soundPlay(soundFindSymbol("SND_TAKEOFF3"));
			obj->pacmanScrewWithScale = !obj->pacmanScrewWithScale;

			pacManObject->model->node_list[2]->scale[0] = 1;
			pacManObject->model->node_list[2]->scale[1] = 1;
			pacManObject->model->node_list[2]->scale[2] = 1;
		}

		if (pads[0].btn.l2 && (pads[0].btn.left && !pads[0].old_btn.left))
		{
			soundPlay(soundFindSymbol("SND_TAKEOFF3"));
			widescreenAspect = !widescreenAspect;
		}

		if (pads[0].btn.l2 && (pads[0].btn.down && !pads[0].old_btn.down))
		{
			soundPlay(soundFindSymbol("SND_TAKEOFF3"));
			obj->pacmanNoclip = !obj->pacmanNoclip;
		}
	}

	if (message == msg_render)
	{
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

		// DrawPadDisconnectedString();

		if (obj->pacmanScrewWithScale)
		{
			// if (!pacManObject->OnGround) {
			pacManObject->model->node_list[2]->scale[0] = ((sinf(gameTime * 16) + 1) / 2) * 0.5f;
			pacManObject->model->node_list[2]->scale[1] = ((cosf(gameTime * 16) + 1) / 2) * 0.5f;
			pacManObject->model->node_list[2]->scale[2] = ((sinf(gameTime * 16) + 1) / 2) * 0.5f;
			// }
			// else {
			// 	pacManObject->model->node_list[2]->scale[0] = 1;
			// 	pacManObject->model->node_list[2]->scale[1] = 1;
			// 	pacManObject->model->node_list[2]->scale[2] = 1;
			// }
		}

		if (obj->pacmanHasADHD)
		{
			// makes pacman spam his idle animation as a test lmao
			pacManObject->IdleFrame = 0;
		}

		if (debugUIShown)
		{
			fmt(obj->debugTextBuf,
				"\n\npacManObject %p\ndebugObject %p\nlowestYpos %s\nmotion.speed.y %s\nPMAction %d\npacmanHasADHD %d\npacmanScrewWithScale %d",
				&pacManObject,
				&obj,
				float_to_str(obj->fl1, pacManObject->lowestYpos, 4),
				float_to_str(obj->fl2, pacManObject->motion.speed.y, 4),
				pacManObject->PMAction,
				obj->pacmanHasADHD,
				obj->pacmanScrewWithScale);

			FontDefaults();
			SetFontShadow(1);
			SetFontColor(255, 255, 255);
			SetFontGradient(1);
			SetFontGradientTopColor(180, 0, 0, 100);
			SetFontGradientBottomColor(180, 180, 0, 100);
			call_SetFontScale(0.5);
			SetFontAlignment(FONT_ALIGN_LEFT);
			call_font_printf(0, 0, obj->debugTextBuf);
		}
	}

	return 0;
}

void CreateDebugObject()
{
	obj_Setup(DebugObject, sizeof(debug_object_t), 0, 0, "debugObject", 1);
}