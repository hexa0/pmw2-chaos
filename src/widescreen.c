#include "widescreen.h"
#include "game/pmw2lib.h"
#include "util.h"

void widescreen_loading_bg_sprite_draw() {
	__asm__ volatile (
		"lui $v1, 0x3F00\n"
		"mtc1 $v1, $f1\n"
		
		"la $v1, aspectWidthScale\n"
		"lwc1 $f2, 0($v1)\n"

		"sub.s $f12, $f12, $f1\n"
		"mul.s $f12, $f12, $f2\n"
		"add.s $f12, $f12, $f1\n"

		"li $a3, 1\n"
		"j FontDrawSpriteZ\n"
		"nop\n"
	);
}

void widescreen_pacdot_collecting_draw() {
	__asm__ volatile (
		"li $a3, 1\n"

		"j FontDrawSpriteZ\n"
		"nop\n"
	);
}

void non_standard_fix_spr_aspect_correct() {
	// make sprAspectCorrect actually use aspectWidthScale
	unsigned int aspect_width_addr = (unsigned int)&aspectWidthScale;

	unsigned int hi = (aspect_width_addr + 0x8000) >> 16;
	unsigned int lui_instr = 0x3C010000 | (hi & 0xFFFF);
	
	unsigned int lo = aspect_width_addr & 0xFFFF;
	unsigned int lwc1_instr = 0xC4210000 | (lo & 0xFFFF);

	patch_word_le(0x0015A7A8, lui_instr);
	patch_word_le(0x0015A7AC, lwc1_instr);
}

void non_standard_fix_spr_aspect_correct_revert() {
	patch_word_le(0x0015A7A8, 0x3C013F40);
	patch_word_le(0x0015A7AC, 0x44810800);
}

void UseAspect43() {
	widescreenAspect = false;
	currentAspect = (4.0f / 3.0f);
	aspectWidthScale = (4.0f / 3.0f) / currentAspect;
	assemble_jal_at(0x001837F0, (unsigned int)&FontDrawSpriteZ);
	assemble_jal_at(0x001DBD08, (unsigned int)&FontDrawSpriteZ);
	non_standard_fix_spr_aspect_correct_revert();
	FlushCache(0);
	FlushCache(2);
}

void UseAspect169() {
	widescreenAspect = true;
	currentAspect = (16.0f / 9.0f);
	aspectWidthScale = (4.0f / 3.0f) / currentAspect;
	assemble_jal_at(0x001837F0, (unsigned int)&widescreen_loading_bg_sprite_draw);
	assemble_jal_at(0x001DBD08, (unsigned int)&widescreen_pacdot_collecting_draw);
	non_standard_fix_spr_aspect_correct_revert();
	FlushCache(0);
	FlushCache(2);
}

void UseAspect1610() {
	widescreenAspect = true;
	currentAspect = (16.0f / 10.0f);
	aspectWidthScale = (4.0f / 3.0f) / currentAspect;
	assemble_jal_at(0x001837F0, (unsigned int)&widescreen_loading_bg_sprite_draw);
	assemble_jal_at(0x001DBD08, (unsigned int)&widescreen_pacdot_collecting_draw);
	non_standard_fix_spr_aspect_correct();
	FlushCache(0);
	FlushCache(2);
}

void UseAspect219() {
	widescreenAspect = true;
	currentAspect =(21.0f / 9.0f);
	aspectWidthScale = (4.0f / 3.0f) / currentAspect;
	assemble_jal_at(0x001837F0, (unsigned int)&widescreen_loading_bg_sprite_draw);
	assemble_jal_at(0x001DBD08, (unsigned int)&widescreen_pacdot_collecting_draw);
	non_standard_fix_spr_aspect_correct();
	FlushCache(0);
	FlushCache(2);
}