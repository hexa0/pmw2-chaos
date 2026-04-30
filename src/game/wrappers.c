#include "game.h"

void call_font_printf(float x, float y, const char* fmt) {
    __asm__ __volatile__ (
        "lwc1 $f12, %0\n"
        "lwc1 $f13, %1\n"
        "move $a0, %2\n"
        "jal font_printfXY\n"
        "nop\n"
        :
        : "m"(x), "m"(y), "r"(fmt)
        : "$4", "$31", "$f12", "$f13", "memory"
    );

    return;
}

void call_SetFontScale(float scale) {
	__asm__ __volatile__ (
		"lwc1 $f12, %0\n"
		"jal SetFontScale\n"
		"nop\n"
		:
		: "m"(scale)
		: "$f12", "$31", "memory"
	);

	return;
}

void call_Font_SetSelectColor(float alpha) {
	__asm__ __volatile__ (
		"lwc1 $f12, %0\n"
		"jal Font_SetSelectColor\n"
		"nop\n"
		:
		: "m"(alpha)
		: "$f12", "$31", "memory"
	);

	return;
}

void call_Font_SetNonSelectColor(float alpha) {
	__asm__ __volatile__ (
		"lwc1 $f12, %0\n"
		"jal Font_SetNonSelectColor\n"
		"nop\n"
		:
		: "m"(alpha)
		: "$f12", "$31", "memory"
	);

	return;
}

void playSoundSymbol(char *symbol) {
	soundPlay(soundFindSymbol(symbol));
}

const char* GetPlayerActionName(PACRTN action) {
    unsigned int addr = (unsigned int)action;

    switch(addr) {
        case 0x1e9928: return "PMNormal";
        case 0x1e8160: return "PMRun";
        case 0x1e3f50: return "PMJump";
        case 0x1e50f8: return "PMJumpDown";
        case 0x1f1368: return "PMRollDownHill";
        case 0x1f4c90: return "PMSwim";
        case 0x1e7420: return "PMButtBounce";
        case 0x1f0260: return "PMRevUp";
        case 0x1e9488: return "PMStandingPunch";
        case 0x1f53c0: return "PMDolphin";
        case 0x1e8d20: return "PMRunPunch";
        case 0x1f54d0: return "PMJustLeftWater";
        case 0x1f0700: return "PMRevRolling";
        case 0x1eab50: return "PMHang";
        case 0x1ec770: return "PMSlideDown";
        case 0x1ea9f8: return "PMHang2Climb";
        case 0x1ea828: return "PMClimb2ClimbUp";
        case 0x1ea630: return "PMClimbUp";
        case 0x1e96d0: return "PMFallBack";
        case 0x1ec840: return "PMDoNothing";
        case 0x1ec518: return "PMDummyState";
        case 0x1eaf08: return "PMAIState";
        case 0x1ec7e8: return "PMJumpForce";
        case 0x172550: return "PMBurnInHell";
        case 0x172710: return "PMDie";
        case 0x255840: return "PMRunRadiallyAt";
        case 0x255bd8: return "PMRunAt";
        case 0x271d98: return "PMOnSwingPlat";
        case 0x109348: return "PMSucked";
        case 0x1094f8: return "PMHurled";
        case 0x1ec8e8: return "PMJumpPunch";
        case 0x1e4538: return "PMHighAirPunch";
        case 0x1ec8f0: return "PMKnocked";
        case 0x1e6368: return "PMRecoverButtBounce";
        case 0x15dda8: return "PMFrozenBlock";
        case 0x15e408: return "PMFrozenBlockMelt";
        case 0x15e598: return "PMFrozenBlockSleeping";
        case 0x1eba68: return "PMRunOnMaze";
        case 0x21f868: return "PMOnPooCannon";
        case 0x114650: return "PMOnBDoing";
        case 0x2731b8: return "PMPunchSwitch";
        case 0x1e5fa0: return "PMKnockedDown";
        case 0x1e0298: return "PMChainFly";
        case 0x1f0a88: return "PMRevSkid";
        case 0x1f1598: return "PMRevHelivate";
        case 0x1e60e0: return "PMButtBounce2Stand";
        case 0x1e9288: return "PMJab";
        case 0x1e5b60: return "PMFall2Land";
        case 0x1e4e20: return "PMParalyzed";
        case 0x1f5610: return "PMThrown";
        case 0x1ea398: return "PMHangJump";
        case 0x1e7fa8: return "PMSlipOnIce";
        case 0x1ec9d0: return "PMFall2Hang";
        case 0x1f54d8: return "PMSwimRevUp";
        case 0x1f46d8: return "PMTorpedo";
        case 0x1735e0: return "PMSquashed";
        case 0x1f1a80: return "PMSkating";
        case 0x1f4420: return "PMDeadTorpedo";
        case 0x1ef0d8: return "PMOceanLiner";
        case 0x1736a8: return "PMElectrified";
        case 0x173778: return "PMStunned";
        case 0x173828: return "PMBurnOuch";
        case 0x1eca58: return "PMFallDown";
        case 0x1eca78: return "PMWipeOut";
        case 0x1eecc0: return "PMOceanLinerTorpedo";
        case 0x1ee9d0: return "PMOceanLinerGlider";
        case 0x1f23b8: return "PMRollerSkating";
        case 0x1eca38: return "PMHangJumpDown";
        case 0x1f0d18: return "PMOnSteep";
        case 0x1f15a0: return "PMRollSkid";
        case 0x164a70: return "PMOnFrostUp";
        case 0x192a38: return "PMTeleportIn";
        case 0x192ae0: return "PMTeleportOut";
		
        default: return "???";
    }
}