#include "game/pmw2lib.h"
#include "util.h"
#include "objects/debug_object.h"
#include "objects/chaos_object.h"

void pre_sound_update()
{
	// fix ringing sound after rev rolling
	
	for (int i = 0; i < ACTIVE_SOUNDS_SIZE; i++)
	{
		// channels 0 and 1 are for the left and right channels of the music
		// i assume since the streaming is hard coded it doesn't ever set status to 1
		// so we need to manually skip those 2 voices

		// we have to strncmp here since the fallback error sound is otherwise muted if the sound symbol is not loaded from the current sound bank
		// since we check for SND_REVUP directly if other sounds have changing pitches we'd need to account for them here too
		// a better patch would be to make soundStop set voll and volr to 0, but im too lazy for that
		if (active_sounds[i].status == 0 && i > 1 && strncmp(active_sounds[i].name, "SND_REVUP", 9) == 0)
		{
			active_sounds[i].voll = 0;
			active_sounds[i].volr = 0;
		}
	}

	// run the actual sound update after our hook
	soundUpdate();

	return;
}

void inject_pre_sound_update()
{
	printf("injecting pre_sound hook\n");

	// 0x0029938C is the call-site of soundUpdate
	assemble_jal_at(0x0029938C, (unsigned int)&pre_sound_update);
}

void inject_no_fmv()
{
	printf("stub vmain (no fmv)\n");
	stub_func_at((unsigned int)&vmain);
}

void level_update_hook()
{
	Level_Update();

	return;
}

void render_world_objects_hook()
{
	renderWorldObjects();

	FontDefaults();
	SetFontShadow(1);
	SetFontWiggle(1);
	SetFontScale(1.0);
	SetFontAlignment(FONT_ALIGN_CENTER);
	font_printfXY(0.5, 0.5, "test");
}

void inject_level_update_hook()
{
	printf("injecting level update hook\n");
	// call-site from Quest_Update
	assemble_jal_at(0x0022B278, (unsigned int)&level_update_hook);
}

void inject_world_render_hook()
{
	printf("injecting render world objects hook\n");
	// call-site from Quest_Update
	assemble_jal_at(0x0017F2EC, (unsigned int)&render_world_objects_hook);
}

void patch_out_sync_padding() {
	stub_func_at((unsigned int)&GiveTimeToPADforCALIBRATION);
}

void fast_game_start() {
	Game_Init();
	Game_DoShell();
}

void create_inventory_hook() {
	CreatePacInventory();
	CreateDebugObject();
	CreateChaosObject();
}

void inject_create_inventory_hook() {
	printf("injecting create inventory hook\n");
	// CreatePacInventory call in Level_LoadData
	assemble_jal_at(0x0017F3DC, (unsigned int)&create_inventory_hook);
	// CreatePacInventory call in SetUpForLoadingScreen
	// assemble_jal_at(0x00183D74, (unsigned int)&create_inventory_hook);
}

void fast_startup() {
	printf("replacing Game_Start (skip intro fmvs)\n");
	// Game_Start call in main
	assemble_jal_at(0x002B79A8, (unsigned int)&fast_game_start);
}