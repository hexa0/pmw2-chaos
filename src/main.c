#include "game/pmw2lib.h"
#include "patches.c"

void init() {
	/// do ctors
	__main();

	printf("injecting hooks\n");

	inject_pre_sound_update();
	// inject_no_fmv();
	fast_startup();
	inject_level_update_hook();
	inject_world_render_hook();
	patch_out_sync_padding();
	inject_create_inventory_hook();
	replace_screen_adjust_menu();

	printf("flushing EE cache\n");
	FlushCache(0);

	return;
}