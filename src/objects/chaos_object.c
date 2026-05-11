#include "../game/pmw2lib.h"
#include "../util.h"
#include "../settings.h"
#include "chaos_object.h"

int IsExcludedLevel() {
	if (strncmp(gLoadedLevelRarName, "levels/forest1.rar", 18) == 0) return 1;
	if (strncmp(gLoadedLevelRarName, "levels/arcadint.rar", 19) == 0) return 1;
	if (strncmp(gLoadedLevelRarName, "levels/maze", 11) == 0) return 1;

	return 0;
}

/// @brief at base difficulty we apply a new effect after so many seconds, this will be scaled with difficulty
#define CHAOS_EFFECT_RATE 25.0f / gModSettings.chaosEffectRateDiviser
#define CHAOS_SHOULD_APPLY gModSettings.chaosModActive && !gPacManOnMap && !pacNoControl && !IsExcludedLevel()

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

typedef enum effectIcon {
	EI_unknown = 0,
	EI_whacked_controls,
	EI_moon_walk
} effectIcon;

typedef struct chaos_effect_s
{
	/// @brief the handle to the function which will apply the effect
	rng_effect_hndl event;
	/// @brief if set to anything other then 0 this effect is grouped with others with the same id and multiple in the same group will not be applied at once
	byte group;
	/// @brief a readable name for the effect
	char name[25];
	/// @brief multiplier that controls how likely this effect is to occur
	float weight;
	/// @brief controls the lower bound limit of how long the effect will last
	float durationMin;
	/// @brief controls the upper bound limit of how long the effect will last, setting this to -1.0f will make the duration always be durationMin
	float durationMax;
	/// @brief the icon to use
	effectIcon icon;
	/// @brief ptr to the effect icon
	PMI *_icon;
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
			assemble_j_at(0x001DEF70, 0x001DF15C);
			drawPacManShadow = false;
			FlushCache(0);
			break;
		case effect_deactive:
			// BC1F
			*(volatile unsigned int*)0x001DEF70 = 0x45000089;
			drawPacManShadow = true;
			FlushCache(0);
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
			break;
		case effect_deactive:
			pacManObject->model->node_list[2]->scale[2] = 1.0f;
			break;
		default:
			break;
	}
}

/// @brief corrupts the graphics routine for drawing 2d sprites to not load the B color channel, making it yellow
void ChaosObject_Effect_PissUI(chaos_object_t *obj, randomEffectMessage message) {
	switch (message) {
		case effect_activate:
			assemble_nop_at(0x00112300);
			FlushCache(0);
			break;
		case effect_deactive:
			patch_word(0x00112300, 0x02540200);
			FlushCache(0);
			break;
		default:
			break;
	}
}

/// @brief makes pacman walk backwards
void ChaosObject_Effect_BackwardsSpeed(chaos_object_t *obj, randomEffectMessage message) {
	switch (message) {
		case effect_activate:
			pacManGlobalMovementTweak = -1.0f;
			break;
		case effect_deactive:
			pacManGlobalMovementTweak = 1.0f;
			break;
		default:
			break;
	}
}

/// @brief makes pacman walk slow
void ChaosObject_Effect_SlowSpeed(chaos_object_t *obj, randomEffectMessage message) {
	switch (message) {
		case effect_activate:
			pacManGlobalMovementTweak = 0.4f;
			break;
		case effect_deactive:
			pacManGlobalMovementTweak = 1.0f;
			break;
		default:
			break;
	}
}

static FVEC lastPacmanPosition;

/// @brief makes pacman walk fast
void ChaosObject_Effect_FastSpeed(chaos_object_t *obj, randomEffectMessage message) {
	switch (message) {
		case effect_activate:
			pacManGlobalMovementTweak = 2.0f;
			lastPacmanPosition = pacManObject->Head.pos;
			lastPacmanPosition.w = 0.0f;
			break;
		case effect_update:
			if (pacManObject->Head.pos.w != 0.0f || pacManObject->motion.speed.x > 4.0f || pacManObject->motion.speed.x < -4.0f || pacManObject->motion.speed.y > 4.0f || pacManObject->motion.speed.y < -4.0f || pacManObject->motion.speed.z > 4.0f || pacManObject->motion.speed.z < -4.0f) {
				pacManObject->Head.pos = lastPacmanPosition;
				pacManObject->motion.speed.x = 0.0f;
				pacManObject->motion.speed.y = 0.0f;
				pacManObject->motion.speed.z = 0.0f;
				printf("[FastSpeed] prevented pacman from entering the NaN zone\n");
			}
			else {
				lastPacmanPosition = pacManObject->Head.pos;
			}

			break;
		case effect_deactive:
			pacManGlobalMovementTweak = 1.0f;
			break;
		default:
			break;
	}
}

/// @brief makes pacman big
void ChaosObject_Effect_BigPacman(chaos_object_t *obj, randomEffectMessage message) {
	switch (message) {
		case effect_activate:
			pacManGlobalScale = 2.0f;
			break;
		case effect_deactive:
			pacManGlobalScale = 1.0f;
			break;
		default:
			break;
	}
}

/// @brief makes pacman very big
void ChaosObject_Effect_UltraBigPacman(chaos_object_t *obj, randomEffectMessage message) {
	switch (message) {
		case effect_activate:
			pacManGlobalScale = 3.0f;
			break;
		case effect_deactive:
			pacManGlobalScale = 1.0f;
			break;
		default:
			break;
	}
}

/// @brief makes pacman small
void ChaosObject_Effect_SmallPacman(chaos_object_t *obj, randomEffectMessage message) {
	switch (message) {
		case effect_activate:
			pacManGlobalScale = 0.5f;
			break;
		case effect_deactive:
			pacManGlobalScale = 1.0f;
			break;
		default:
			break;
	}
}

/// @brief makes pacman very small
void ChaosObject_Effect_UltraSmallPacman(chaos_object_t *obj, randomEffectMessage message) {
	switch (message) {
		case effect_activate:
			pacManGlobalScale = 0.25f;
			break;
		case effect_deactive:
			pacManGlobalScale = 1.0f;
			break;
		default:
			break;
	}
}

void ChaosObject_Effect_LowGravity(chaos_object_t *obj, randomEffectMessage message) {
	switch (message) {
		case effect_activate:
			GRAVITY = 0.1f;
			break;
		case effect_deactive:
			GRAVITY = 0.2f;
			break;
		default:
			break;
	}
}

void ChaosObject_Effect_HighGravity(chaos_object_t *obj, randomEffectMessage message) {
	switch (message) {
		case effect_activate:
			GRAVITY = 0.28f;
			break;
		case effect_deactive:
			GRAVITY = 0.2f;
			break;
		default:
			break;
	}
}

void ChaosObject_Effect_DoubleGameSpeed(chaos_object_t *obj, randomEffectMessage message) {
	switch (message) {
		case effect_activate:
			_engineSpeed = 2.0f;
			break;
		case effect_deactive:
			_engineSpeed = 1.0f;
			break;
		default:
			break;
	}
}

#define EFFECT_GROUP_NONE 0
#define EFFECT_GROUP_SPEED 1
#define EFFECT_GROUP_SIZE 2
#define EFFECT_GROUP_GRAVITY 3
#define EFFECT_GROUP_GAMESPEED 4

static chaos_effect_t gChaosEffects[] = {
	{
		.event = ChaosObject_Effect_WhackControls,
		.name = "Whacked Controls",
		.icon = EI_whacked_controls,
		.group = EFFECT_GROUP_NONE,
		.weight = 0.7f,
		.durationMin = 16.5f,
		.durationMax = 32.0f
	},
	{
		.event = ChaosObject_Effect_Invisible,
		.name = "Invisible",
		.group = EFFECT_GROUP_NONE,
		.weight = 1.0f,
		.durationMin = 25.0f,
		.durationMax = 78.0f
	},
	{
		.event = ChaosObject_Effect_PaperPac,
		.name = "Paper Pac",
		.group = EFFECT_GROUP_NONE,
		.weight = 0.9f,
		.durationMin = 30.0f,
		.durationMax = 70.0f
	},
	{
		.event = ChaosObject_Effect_PancakePac,
		.name = "Pancake Pac",
		.group = EFFECT_GROUP_NONE,
		.weight = 0.9f,
		.durationMin = 30.0f,
		.durationMax = 70.0f
	},
	{
		.event = ChaosObject_Effect_PissUI,
		.name = "Piss UI",
		.group = EFFECT_GROUP_NONE,
		.weight = 0.9f,
		.durationMin = 32.0f,
		.durationMax = 78.0f
	},
	{
		.event = ChaosObject_Effect_SlowSpeed,
		.name = "Slow Speed",
		.group = EFFECT_GROUP_SPEED,
		.weight = 0.8f,
		.durationMin = 8.0f,
		.durationMax = 23.0f
	},
	{
		// TODO: make it so we can set the bitmask for level types the effect should be available for
		// in the water levels this sends you into the fucking edge of the universe
		.event = ChaosObject_Effect_FastSpeed,
		.name = "Fast Speed",
		.group = EFFECT_GROUP_SPEED,
		// a little glitchy sometimes so this is reduced until it is fixed
		.weight = 0.7f,
		.durationMin = 15.0f,
		.durationMax = 30.0f
	},
	{
		.event = ChaosObject_Effect_BackwardsSpeed,
		.name = "Moonwalk",
		.icon = EI_moon_walk,
		.group = EFFECT_GROUP_SPEED,
		.weight = 1.0f,
		.durationMin = 10.0f,
		.durationMax = 30.0f
	},
	{
		// TODO: possibly reduce fall damage for this effect
		.event = ChaosObject_Effect_BigPacman,
		.name = "BigPac",
		.group = EFFECT_GROUP_SIZE,
		.weight = 0.9f,
		.durationMin = 10.0f,
		.durationMax = 16.0f
	},
	{
		.event = ChaosObject_Effect_UltraBigPacman,
		.name = "UltraBigPac",
		.group = EFFECT_GROUP_SIZE,
		.weight = 0.7f,
		.durationMin = 10.0f,
		.durationMax = 14.0f
	},
	{
		.event = ChaosObject_Effect_UltraSmallPacman,
		.name = "UltraSmallPac",
		.group = EFFECT_GROUP_SIZE,
		.weight = 0.5f,
		.durationMin = 6.0f,
		.durationMax = 16.0f
	},
	{
		.event = ChaosObject_Effect_SmallPacman,
		.name = "SmallPac",
		.group = EFFECT_GROUP_SIZE,
		.weight = 0.8f,
		.durationMin = 8.0f,
		.durationMax = 14.0f
	},
	{
		.event = ChaosObject_Effect_LowGravity,
		.name = "Low Gravity",
		.group = EFFECT_GROUP_GRAVITY,
		.weight = 1.1f,
		.durationMin = 13.0f,
		.durationMax = 17.0f
	},
	{
		.event = ChaosObject_Effect_HighGravity,
		.name = "High Gravity",
		.group = EFFECT_GROUP_GRAVITY,
		.weight = 1.0f,
		.durationMin = 10.0f,
		.durationMax = 12.0f
	},
	{
		.event = ChaosObject_Effect_DoubleGameSpeed,
		.name = "Double Game Speed",
		.group = EFFECT_GROUP_GAMESPEED,
		.weight = 0.7f,
		.durationMin = 10.0f,
		.durationMax = 15.0f
	}
	
	// possible ideas?

	/* shitty internet
		causes you to get rubber banded back randomly
		sometimes it will ruberband but then unruberband quickly after as well with rng
	*/ 

	/* fixed camera
		locks the camera in place (it will still look at pacman)
		i could just stub CarlsComplexCameraCamera to do this,
		or i can nop the jal in CrashCamUpdate (0x00138184)
		that would be easier to restore
		nvm it doesn't handle when the camera is a tower or rail camera
		we could probably bypass camera volumes then
	*/

	/* effect wiki
		once you encountered an effect it should have information about it in a menu you can check
	*/

	/* effect icons
		once i reverse engineer pmi textures (haha more like pmo) we can have a sprite sheet of effect icons
	*/

	/* sound effects?
		possible have some custom sounds when effects apply and what not
		that would add some polish
	*/

	/* zero gravity
		make it super short because this could just outright kill you if you take fall damage
		like at most 3 seconds, probably 1 to 2 though
	*/
};

#define RANDOM_EFFECTS_SIZE (sizeof(gChaosEffects) / sizeof(chaos_effect_t))
#define RANDOM_EFFECT_EXCLUDED obj->activeEffectIds[j] == i || (gChaosEffects[i].group != 0 && gChaosEffects[obj->activeEffectIds[j]].group == gChaosEffects[i].group)

int GetRandomEffect(chaos_object_t *obj) {
	float totalWeight = 0.0f;

	for (int i = 0; i < RANDOM_EFFECTS_SIZE; i++) {
		BOOL excluded = false;
		
		for (int j = 0; j < obj->totalActiveEffects; j++) {
			if (RANDOM_EFFECT_EXCLUDED) {
				excluded = true;
				break;
			}
		}

		if (!excluded) {
			totalWeight += gChaosEffects[i].weight;
		}
	}

	float threshold = frand() * totalWeight;

	for (int i = 0; i < RANDOM_EFFECTS_SIZE; i++) {
		BOOL excluded = false;

		for (int j = 0; j < obj->totalActiveEffects; j++) {
			if (RANDOM_EFFECT_EXCLUDED) {
				excluded = true;
				break;
			}
		}

		if (!excluded) {
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
	obj->activeEffectExpirations[obj->totalActiveEffects] = obj->timer + ((effect->durationMin + (frand() * (durationMax - effect->durationMin))) * gModSettings.chaosEffectDurationMultipler);
	obj->totalActiveEffects++;

	printf("activating %s\n", effect->name);
	effect->event(obj, effect_activate);

	return true;
}

void ChaosObject_Init(chaos_object_t *obj)
{
	obj->timer = 0.0f;
	obj->nextEffect = -0.001f; // this is an absolute hack, but since when init is called the CHAOS_SHOULD_APPLY isn't ready do this
	obj->totalActiveEffects = 0;
}

void ChaosObject_Process(chaos_object_t *obj)
{
	if (!ScreenFaderActive() && CHAOS_SHOULD_APPLY)
	{
		obj->timer = obj->timer + ((gameTime - oldGameTime) / _engineSpeed);

		for (int i = 0; i < obj->totalActiveEffects; i++) {
			const chaos_effect_t* effect = &gChaosEffects[obj->activeEffectIds[i]];

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

	if (CHAOS_SHOULD_APPLY && obj->timer > obj->nextEffect && obj->totalActiveEffects < 16) {
		if (ChaosObject_AddRandomEffect(obj)) {
			obj->nextEffect = obj->nextEffect + CHAOS_EFFECT_RATE;
		}
	}
}

char hudTextScratchpad[512];
char hudFloatScratchpad[128];
char *scratchPtrs[2];

void AdvanceScratchPtr(int i) {
	int len = 1;

	char* p = scratchPtrs[i];
	while (*p != '\0') {
		len++;
		p++;
	}

	scratchPtrs[i] += len;
}

void ChaosObject_Render(chaos_object_t *obj)
{
	if (!ScreenFaderActive() && CHAOS_SHOULD_APPLY) {	
		scratchPtrs[0] = (char*)&hudTextScratchpad;
		scratchPtrs[1] = (char*)&hudFloatScratchpad;

		float hudX = 0.02f / (aspectWidthScale / 2.0f + 1.0f);
		float hudY = 0.02f;

		FontDefaults();
		
		SetFontShadow(1);
		SetFontColor(255, 255, 255);
		SetFontGradient(1);
		SetFontGradientTopColor(180, 0, 0, 100);
		SetFontGradientBottomColor(180, 180, 0, 100);
		call_SetFontScale(0.5);
		SetFontAlignment(FONT_ALIGN_LEFT);

		char* nextEffectInDuration = float_to_str(scratchPtrs[1], obj->nextEffect - obj->timer, 1);
		AdvanceScratchPtr(1);

		fmt(scratchPtrs[0],
			"NEXT EFFECT IN: %s",
			nextEffectInDuration
		);

		call_font_printf(hudX, hudY, scratchPtrs[0]);
		AdvanceScratchPtr(0);

		float lineHeight = FontHeight() * 1.3f;

		for (int i = 0; i < obj->totalActiveEffects; i++) {
			chaos_effect_t* effect = &gChaosEffects[obj->activeEffectIds[i]];

			char* effectDuration = float_to_str(scratchPtrs[1], obj->activeEffectExpirations[i] - obj->timer, 1);
			AdvanceScratchPtr(1);

			fmt(scratchPtrs[0],
				"%s: %s",
				effect->name,
				effectDuration
			);
			
			call_font_printf(
				hudX + (0.04f * aspectWidthScale),
				hudY + (lineHeight * ((float)i + 5.0f)),
				scratchPtrs[0]
			);
			AdvanceScratchPtr(0);
		}

		FontDefaults();

		call_SetFontScale(0.5);
		SetFontAlignment(FONT_ALIGN_LEFT);

		float iconSize = FontHeight() * 1.2f;

		for (int i = 0; i < obj->totalActiveEffects; i++) {
			chaos_effect_t* effect = &gChaosEffects[obj->activeEffectIds[i]];

			call_FontDrawSprite(
				hudX,
				hudY + (lineHeight * ((float)i + 5.0f)) - 0.002f,
				0,
				effect->_icon,
				iconSize * aspectWidthScale,
				iconSize,
				0,
				true
			);
		};

		FontDefaults();
	}
}

void ChaosObject_Delete(chaos_object_t *obj)
{
	// deactivate all effects when the chaosObject is removed
	for (int i = 0; i < obj->totalActiveEffects; i++) {
		int id = obj->activeEffectIds[i];
		printf("deactivating %s\n", gChaosEffects[id].name);
		gChaosEffects[id].event(obj, effect_deactive);
	}

	// this is a REALLY ugly place to do this, but whatever

	printf("clearing starTex and blurTex references\n");

	starTex = 0x0;
	blurTex = 0x0;
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
		case msg_delete:
			ChaosObject_Delete(obj);
			break;
		default:
			break;
	}

	return 0;
}

/// @brief this assume we are already in the mod directory
void LoadEffectAssets() {
	DownFileDirectory("effecticon");

	for (int i = 0; i < RANDOM_EFFECTS_SIZE; i++) {
		// we might be overloading the vram as exiting levels shows the level pad as a corrupted texture
		// but if that continues to be the only issue then ill ignore it, because who gives a shit about just that one texture breaking
		switch (gChaosEffects[i].icon) {
			case EI_whacked_controls:
				gChaosEffects[i]._icon = FindOrLoadTextureTo("whacked_controls.pmi",2);
				break;
			case EI_moon_walk:
				gChaosEffects[i]._icon = FindOrLoadTextureTo("moon_walk.pmi",2);
				break;
			default:
				// an intentionally really stupid icon so i remember to make an icon/fix it
				gChaosEffects[i]._icon = FindOrLoadTextureTo("effect_temp.pmi",2);
		}
	}

	UpFileDirectory(); // effecticon
}