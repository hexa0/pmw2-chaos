#include "../game/pmw2lib.h"
#include "../util.h"

static float gRateDiviser = 1.0f;
static float gDurationMultipler = 1.0f;

/// @brief at base difficulty we apply a new effect after so many seconds, this will be scaled with difficulty
#define CHAOS_EFFECT_RATE 20.0f / gRateDiviser

typedef struct chaos_object_s
{
	OBJHEAD hd;
	float nextEffect;
	float timer;
	int totalActiveEffects;
	int activeEffectIds[16];
	float activeEffectExpirations[16];
 
} chaos_object_t;

typedef enum randomEffectMessage {
	/// @brief sent when the effect is first created
	effect_activate,
	/// @brief sent when the effect runs out
	effect_deactive,
	/// @brief sent every frame during while the effect is active 
	effect_update
} randomEffectMessage;

typedef void (*rng_effect_hndl)(chaos_object_t *obj, randomEffectMessage message);

typedef struct chaos_effect_s
{
	/// @brief the handle to the function which will apply the effect
	rng_effect_hndl event;
	/// @brief a readable name for the effect
	char name[25];
	/// @brief multiplier that controls how likely this effect is to occur
	float weight;
	/// @brief controls the lower bound limit of how long the effect will last
	float durationMin;
	/// @brief controls the upper bound limit of how long the effect will last, setting this to -1.0f will make the duration always be durationMin
	float durationMax;
} chaos_effect_t;

void ChaosObject_Effect_WhackControls(chaos_object_t *obj, randomEffectMessage message) {
	switch (message) {
		case effect_activate:
			pacManObject->controls_whacked = true;
			break;
		case effect_deactive:
			pacManObject->controls_whacked = false;
			break;
		default:
			break;
	}
}

void ChaosObject_Effect_Invisible(chaos_object_t *obj, randomEffectMessage message) {
	switch (message) {
		case effect_activate:
			pacManObject->model->num_nodes = 0;
			drawPacManShadow = false;
			break;
		case effect_deactive:
			pacManObject->model->num_nodes = 124;
			drawPacManShadow = true;
			break;
		default:
			break;
	}
}

void ChaosObject_Effect_PancakePac(chaos_object_t *obj, randomEffectMessage message) {
	switch (message) {
		case effect_activate:
			pacManObject->model->node_list[2]->scale[1] = 0.1f;
			break;
		case effect_deactive:
			pacManObject->model->node_list[2]->scale[1] = 1.0f;
			break;
		default:
			break;
	}
}

void ChaosObject_Effect_PaperPac(chaos_object_t *obj, randomEffectMessage message) {
	switch (message) {
		case effect_activate:
			pacManObject->model->node_list[2]->scale[2] = 0.1f;
			drawPacManShadow = false;
			break;
		case effect_deactive:
			pacManObject->model->node_list[2]->scale[2] = 1.0f;
			drawPacManShadow = true;
			break;
		default:
			break;
	}
}

static const chaos_effect_t gChaosEffects[] = {
	{
		.event = ChaosObject_Effect_WhackControls,
		.name = "Whacked Controls",
		.weight = 0.7f,
		.durationMin = 20.0f,
		.durationMax = 35.0f
	},
	{
		.event = ChaosObject_Effect_Invisible,
		.name = "Invisible",
		.weight = 1.0f,
		.durationMin = 30.0f,
		.durationMax = 35.0f
	},
	{
		.event = ChaosObject_Effect_PaperPac,
		.name = "Paper Pac",
		.weight = 1.0f,
		.durationMin = 30.0f,
		.durationMax = 35.0f
	},
	{
		.event = ChaosObject_Effect_PancakePac,
		.name = "Pancake Pac",
		.weight = 0.4f,
		.durationMin = 30.0f,
		.durationMax = 35.0f
	}
};

#define RANDOM_EFFECTS_SIZE (sizeof(gChaosEffects) / sizeof(chaos_effect_t))

int GetRandomEffect(chaos_object_t *obj) {
	float totalWeight = 0.0f;

	for (int i = 0; i < RANDOM_EFFECTS_SIZE; i++) {
		BOOL alreadyAdded = false;
		
		for (int j = 0; j < obj->totalActiveEffects; j++) {
			if (obj->activeEffectIds[j] == i) {
				alreadyAdded = true;
				break;
			}
		}

		if (!alreadyAdded) {
			totalWeight += gChaosEffects[i].weight;
		}
	}

	float threshold = frand() * totalWeight;

	for (int i = 0; i < RANDOM_EFFECTS_SIZE; i++) {
		BOOL alreadyAdded = false;

		for (int j = 0; j < obj->totalActiveEffects; j++) {
			if (obj->activeEffectIds[j] == i) {
				alreadyAdded = true;
				break;
			}
		}

		if (!alreadyAdded) {
			if (threshold < gChaosEffects[i].weight) {
				return i;
			}
			
			threshold -= gChaosEffects[i].weight;
		}
	}

	return -1;
}

BOOL ChaosObject_AddRandomEffect(chaos_object_t *obj)
{
	int effectId = GetRandomEffect(obj);

	if (effectId < 0) {
		return false;
	}

	const chaos_effect_t* effect = &gChaosEffects[effectId];

	float durationMax = (effect->durationMax <= -1.0f) ? effect->durationMin : effect->durationMax;

	obj->activeEffectIds[obj->totalActiveEffects] = effectId;
	obj->activeEffectExpirations[obj->totalActiveEffects] = obj->timer + (effect->durationMin + (frand() * (durationMax - effect->durationMin)));
	obj->totalActiveEffects++;

	effect->event(obj, effect_activate);

	return true;
}

void ChaosObject_Init(chaos_object_t *obj)
{
	obj->timer = 0.0f;
	obj->nextEffect = CHAOS_EFFECT_RATE;
	obj->totalActiveEffects = 0;
}

void ChaosObject_Render(chaos_object_t *obj)
{
	if (!ScreenFaderActive() && !gPacManOnMap && !pacNoControl) {	
		char effectListBuf[512];
		char* effectListBufPtr = effectListBuf;

		*effectListBufPtr = '\0';

		for (int i = 0; i < obj->totalActiveEffects; i++) {
			int id = obj->activeEffectIds[i];
			float remaining = obj->activeEffectExpirations[i] - obj->timer;
			const char* name = gChaosEffects[id].name;

			int written = sprintf(effectListBufPtr, "%s: %s\n", 
				name, 
				float_to_str((char[16]){0}, remaining, 1)
			);

			effectListBufPtr += written;
		}

		FontDefaults();
		SetFontShadow(1);
		SetFontColor(255, 255, 255);
		SetFontGradient(1);
		SetFontGradientTopColor(180, 0, 0, 100);
		SetFontGradientBottomColor(180, 180, 0, 100);
		call_SetFontScale(0.5);
		SetFontAlignment(FONT_ALIGN_LEFT);
		call_font_printf(0.02, 0.02, fmt((char[512]){0},
			"Next Effect In: %s\n\n\n\n\n\n%s",
			float_to_str((char[16]){0}, obj->nextEffect - obj->timer, 1),
			effectListBuf
		));
		FontDefaults();
	}
}

void ChaosObject_Process(chaos_object_t *obj)
{
	if (!ScreenFaderActive())
	{
		obj->timer = obj->timer + (gameTime - oldGameTime);

		for (int i = 0; i < obj->totalActiveEffects; i++) {
			int effectId = obj->activeEffectIds[i];
        	const chaos_effect_t* effect = &gChaosEffects[effectId];

			if (obj->timer >= obj->activeEffectExpirations[i]) {
				effect->event(obj, effect_deactive);

				for (int j = i; j < obj->totalActiveEffects - 1; j++) {
					obj->activeEffectIds[j] = obj->activeEffectIds[j + 1];
					obj->activeEffectExpirations[j] = obj->activeEffectExpirations[j + 1];
				}

				obj->totalActiveEffects--;
				i--;
			}
			else {
				effect->event(obj, effect_update);
			}
		}
	}

	if (obj->timer > obj->nextEffect && obj->totalActiveEffects < 16) {
		if (ChaosObject_AddRandomEffect(obj)) {
			obj->nextEffect = obj->nextEffect + CHAOS_EFFECT_RATE;
		}
	}
}

int ChaosObject(OBJHEAD *hd, messageType message, void *data)
{
	chaos_object_t *obj = (chaos_object_t *)hd;

	switch (message)
	{
		case msg_init:
			ChaosObject_Init(obj);
			break;
		case msg_processFrame:
			ChaosObject_Process(obj);
			break;
		case msg_render:
			ChaosObject_Render(obj);
			break;
		default:
			break;
	}

	return 0;
}