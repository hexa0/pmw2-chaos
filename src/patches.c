#include <stdio.h>

#include "game/pmw2lib.h"
#include "util.h"
#include "objects/debug_object.h"
#include "objects/chaos_object.h"
#include "objects/settings_object.h"
#include "options_menu.h"
#include "loadscrn.h"

void pre_sound_update()
{
	// fix ringing sound after rev rolling
	
	for (int i = 0; i < ACTIVE_SOUNDS_SIZE; i++)
	{
		// channels 0 and 1 are for the left and right channels of the music
		// i assume since the streaming is hard coded it doesn't ever set status to 1
		// so we need to manually skip those 2 voices

		if (active_sounds[i].status == 0 && i > 1)
		{
			// if the ID is 5 we only fix it if its name is SND_REVUP
			// error sounds go to channel 5 so this catches that
			// a better solution would be to hook soundStop to fix this bug though
			if (i == 5 && strncmp(active_sounds[i].name, "SND_REVUP", 9) == 1) {
				continue;
			}

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
	printf("removing CRT sync padding?\n");
	stub_func_at((unsigned int)&GiveTimeToPADforCALIBRATION);
}

void create_inventory_hook() {
	printf("CreatePacInventory\n");
	CreatePacInventory();
	printf("CreateDebugObject\n");
	CreateDebugObject();
	printf("CreateChaosObject\n");
	CreateChaosObject();
	printf("CreateSettingsObject\n");
	CreateSettingsObject();
}

void inject_create_inventory_hook() {
	printf("injecting create inventory hook\n");
	// CreatePacInventory call in Level_LoadData
	assemble_jal_at(0x0017F3DC, (unsigned int)&create_inventory_hook);
	// CreatePacInventory call in SetUpForLoadingScreen
	// assemble_jal_at(0x00183D74, (unsigned int)&create_inventory_hook);
}

void fast_startup_hook() {
	// mimics Game_Start but skips the Game_PlayMovie and Game_ShowLogos
	Game_Init();
	Game_DoShell();
}

void inject_fast_startup_hook() {
	printf("injecting create inventory hook\n");
	// Game_Start call in main
	assemble_jal_at(0x002B79A8, (unsigned int)&fast_startup_hook);
}

static const char* modSettingsText = "MOD SETTINGS";
static const char* modSettingsTextJP = /* SJIS */ "モッド設定";

void replace_screen_adjust_menu() {
	printf("adding mod settings menu\n");
	pauseMainMenu[3][0] = (char*)modSettingsText;
	pauseMainMenu[3][1] = (char*)modSettingsTextJP;
	pauseMapMenu[3][0] = (char*)modSettingsText;
	pauseMapMenu[3][1] = (char*)modSettingsTextJP;
	pauseMazeMenu[3][0] = (char*)modSettingsText;
	pauseMazeMenu[3][1] = (char*)modSettingsTextJP;
	pauseArcadeMenu[3][0] = (char*)modSettingsText;
	pauseArcadeMenu[3][1] = (char*)modSettingsTextJP;
	pauseArcadeGameNoMusicMenu[2][0] = (char*)modSettingsText;
	pauseArcadeGameNoMusicMenu[2][1] = (char*)modSettingsTextJP;
	pauseMenu[3][0] = (char*)modSettingsText;
	pauseMenu[3][1] = (char*)modSettingsTextJP;
	
	InitModSettings();
	assemble_j_at((unsigned int)UpdateScreenMenu, (unsigned int)&ModSettingsMenu);
}

void _closestream_host(int fd) {
	printf("_closestream_host %d\n", fd);

    if (fd >= 0) {
        sceClose(fd);
    }
}

// currently only get used for loading sound banks with our patches active since we don't use fixfordvd ourselves
int fixfordvd_hook(char *dst, char *filename) {
	sprintf(dst, "host:./netdata/%s", filename);
	
	printf("fixfordvd_hook %s\n", dst);
}

int _streamfilefrompc_host(char *filename, int *length) {
	printf("streaming rar %s\n", filename);

	char name[64];
	sprintf(name, "host:netdata/%s", filename);
    
    int fd = sceOpen(name, 0x0001);

    if (fd >= 0) {
        *length = sceLseek(fd, 0, 2);
		levelRarSize = *length;
		levelRarReadPosition = 0;
        sceLseek(fd, 0, 0);
    }
	else {
		// fixes loading screens stupidly relying on SCEECdSearchFile from the original code to resolve this stupidity
		sprintf(name, "host:netdata/levels/%s", filename);

		fd = sceOpen(name, 0x0001);

		if (fd >= 0) {
			*length = sceLseek(fd, 0, 2);
			levelRarSize = *length;
			levelRarReadPosition = 0;
			sceLseek(fd, 0, 0);
		}
		else {
			printf("rar %s has failed with code %d\n", name, fd);
		}
	}

    return fd;
}

void _startstreaming_host(int fd, unsigned char *buffer, int size) {
	int bytesRead = sceRead(fd, buffer, size);

	if (bytesRead < 0) {
        printf("rar streaming error: %d\n", fd);
    }
	else {
		levelRarReadPosition += bytesRead;
	}
}

void inject_host_fs() {
	printf("injecting host fs\n");

	assemble_j_at((unsigned int)_streamfilefrompc, (unsigned int)&_streamfilefrompc_host);
	assemble_j_at((unsigned int)_closestream, (unsigned int)&_closestream_host);
	assemble_j_at((unsigned int)_startstreaming, (unsigned int)&_startstreaming_host);
	assemble_j_at((unsigned int)fixfordvd, (unsigned int)&fixfordvd_hook);
}

void calculate_screen_clip() {
	view_screen[0][0] = view_screen[0][0] * aspectWidthScale;
    view_clip[0][0] = view_clip[0][0] * aspectWidthScale;
	// slightly extended for up to 21:9
	VU1_view_clip[0][0] = 1.14f;

	__asm__ volatile (
		"j 0x001193A0"
	);
}

void ShowLoadingProgress() {
	FontDefaults();
	SetFontAlignment(FONT_ALIGN_LEFT);

	fontCursorX = widescreenAspect ? 0.015f : 0.02f;
	fontCursorY = 0.02f;

	char textbuf[32];
	snprintf(textbuf, sizeof(textbuf), "progress: %s%%", float_to_str((char[4]){0}, LevelRarLoadProgress() * 100.0f, 0));
	
	DrawFont(textbuf);

	SetStatusCamera();
}

void inject_widescreen_bug_fixes() {
	printf("injecting widescreen bug fixes\n");

	// handled by settings.C now
	// assemble_jal_at(0x001837F0, (unsigned int)&widescreen_loading_bg_sprite_draw);
	// assemble_jal_at(0x001DBD08, (unsigned int)&widescreen_pacdot_collecting_draw);

	// fix weird fov scaling
	*(unsigned int*)0x5a2a3c = 0x3F7E0000;

	// possibly more aspect configurations via this?

	// allow us to control aspectWidthScale
	assemble_nop_at(0x001190EC);
	// allow us to control currentAspect
	// this variable is unused nvm
	// assemble_nop_at(0x001193A4);
	// assemble_nop_at(0x00119390);
	assemble_j_at(0x00119370, (unsigned int)&calculate_screen_clip);
	// alow us to manually calculate view_screen and view_clip
}

void inject_loading_progress() {
	printf("injecting loading progress bar\n");
	// SetStatusCamera callsite in LoadingBar
	assemble_jal_at(0x00183828, (unsigned int)&ShowLoadingProgress);
}