// this is heavily stripped from the original ps2dev source code to reduce to the bare minimum we need (to keep the pnach small)

/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
*/

/**
 * @file
 * EE UGLY DEBUG ON SCREEN
 */

#include <stdio.h>
#include <tamtypes.h>
#include <sifcmd.h>
#include <kernel.h>
#include <rom0_info.h>
#include <stdarg.h>
#include <debug.h>
#include "font.c"

static short int X, Y;
static u32 bgcolor = 0;

struct t_setupscr
{
    u64 dd0[6];
    u32 dw0[2];
    u64 dd1[1];
    u16 dh[4];
    u64 dd2[21];
};

struct t_setupchar
{
    u64 dd0[4];
    u32 dw0[1];
    u16 x, y;
    u64 dd1[1];
    u32 dw1[2];
    u64 dd2[5];
};

static void Init_GS(int interlace, int omode, int ffmd)
{
    // Reset GS
    *(vu64 *)0x12001000 = 0x200;
    // Mask interrupts
    GsPutIMR(0xff00);
    // Configure GS CRT
    SetGsCrt(interlace, omode, ffmd);
}

static void SetVideoMode(void)
{
    unsigned dma_addr;
    unsigned val1;
    unsigned val2;
    unsigned val3;
    unsigned val4;
    unsigned val4_lo;

    /*	DISPLAY1	0x0983227c001bf9ff
            DX = 0x27C (636)
            DY = 0x032 (50)

            MAGH = 0x03 (4x)
            MAGV = 0x01 (2x)

            DW = 0x9FF (2560)
            DH = 0x1BF (447)	*/

    asm volatile("        .set push               \n"
                 "        .set noreorder          \n"
                 "        lui     %4, 0x001b      \n"
                 "        lui     %5, 0x0983      \n"
                 "        lui     %0, 0x1200      \n"
                 "        ori     %4, %4, 0xf9ff  \n"
                 "        ori     %5, %5, 0x227c  \n"
                 "        li      %1, 0xff62      \n"
                 "        dsll32  %4, %4, 0       \n"
                 "        li      %3, 0x1400      \n"
                 "        sd      %1, 0(%0)       \n"
                 "        or      %4, %4, %5      \n"
                 "        sd      %3, 0x90(%0)    \n"
                 "        sd      %4, 0xa0(%0)    \n"
                 "        .set pop                \n"
                 : "=&r"(dma_addr), "=&r"(val1), "=&r"(val2),
                   "=&r"(val3), "=&r"(val4), "=&r"(val4_lo));
}

static void Dma02Wait(void)
{
    unsigned dma_addr;
    unsigned status;

    asm volatile("        .set push               \n"
                 "        .set noreorder          \n"
                 "        lui   %0, 0x1001        \n"
                 "        lw    %1, -0x6000(%0)   \n"
                 "1:      andi  %1, %1, 0x100     \n" // Wait until STR bit of D2_CHCR = 0
                 "        nop                     \n"
                 "        nop                     \n"
                 "        nop                     \n"
                 "        nop                     \n"
                 "        bnel  %1, $0, 1b        \n"
                 "        lw    %1, -0x6000(%0)   \n"
                 "        .set pop                \n"
                 : "=&r"(dma_addr), "=&r"(status));
}

static void DmaReset(void)
{
    unsigned dma_addr;
    unsigned temp, temp2;

    // This appears to have been based on code from Sony that initializes DMA channels 0-9, in bulk.
    asm volatile("        .set push               \n"
                 "        .set noreorder          \n"
                 "        lui   %0, 0x1001        \n"
                 "        sw    $0, -0x5f80(%0)   \n" // D2_SADR = 0. Documented to not exist, but is done.
                 "        sw    $0, -0x6000(%0)   \n" // D2_CHCR = 0
                 "        sw    $0, -0x5fd0(%0)   \n" // D2_TADR = 0
                 "        sw    $0, -0x5ff0(%0)   \n" // D2_MADR = 0
                 "        sw    $0, -0x5fb0(%0)   \n" // D2_ASR1 = 0
                 "        sw    $0, -0x5fc0(%0)   \n" // D2_ASR0 = 0
                 "        lui   %1, 0             \n"
                 "        ori   %1, %1, 0xff1f    \n"
                 "        sw    %1, -0x1ff0(%0)   \n" // Clear all interrupt status under D_STAT, other than SIF0, SIF1 & SIF2.
                 "        lw    %1, -0x1ff0(%0)   \n"
                 "        lui   %2, 0xff1f        \n"
                 "        and   %1, %1, %2        \n" // Clear all interrupt masks under D_STAT, other SIF0, SIF1 & SIF2. Writing a 1 reverses the bit.
                 "        sw    %1, -0x1ff0(%0)   \n"
                 "        sw    $0, -0x2000(%0)   \n" // D_CTRL = 0
                 "        sw    $0, -0x1fe0(%0)   \n" // D_PCR = 0
                 "        sw    $0, -0x1fd0(%0)   \n" // D_SQWC = 0
                 "        sw    $0, -0x1fb0(%0)   \n" // D_RBOR = 0
                 "        sw    $0, -0x1fc0(%0)   \n" // D_RBSR = 0
                 "        lw    %1, -0x2000(%0)   \n"
                 "        ori   %1, %1, 1         \n" // D_CTRL (DMAE 1)
                 "        sw    %1, -0x2000(%0)   \n"
                 "        .set pop                \n"
                 : "=&r"(dma_addr), "=&r"(temp), "=&r"(temp2));
}

/**
 * Initiates a normal-mode DMA transfer over the GIF.
 * @param addr The address of the data to be transfered, which must be 16 byte aligned.
 * @param size The size (in 16 byte quads) of the data to be transfered.
 */

static void progdma(void *addr, int size)
{
    unsigned dma_addr;
    unsigned temp;

    asm volatile("        .set push               \n"
                 "        .set noreorder          \n"
                 "        lui   %0, 0x1001        \n"
                 "        sw    %3, -0x5fe0(%0)   \n" // D2_QWC
                 "        sw    %2, -0x5ff0(%0)   \n" // D2_MADR
                 "        li    %1, 0x101         \n" // STR 1, DIR 1
                 "        sw    %1, -0x6000(%0)   \n" // D2_CHCR
                 "        .set pop                \n"
                 : "=&r"(dma_addr), "=&r"(temp)
                 : "r"(addr), "r"(size));
}

void init_scr(void)
{
    static struct t_setupscr setupscr __attribute__((aligned(16))) = {
        {0x100000000000800E, 0xE, 0xA0000, 0x4C, 0x8C, 0x4E}, // GIFtag (REGS: A+D, NREG 1, FLG PACKED, EOP, NLOOP 14), FRAME_1 (PSM PSMCT32, FBW 10, FBP 0), ZBUF_1 (PSM PSMZ32, ZBP 140)
        {27648, 30976},
        {0x18},                                                           // XYOFFSET_1 (OFX 1728.0, OFY 1936.0)
        {0, 639, 0, 223},                                                 // SCISSOR_1 (SCAX0 0, SCAX1 639, SCAY0 0, SCAY1 223)
        {0x40, 1, 0x1a, 1, 0x46, 0, 0x45, 0x70000,                        // PRMODECONT (AC PRIM), COLCLAMP (CLAMP 1), DTHE (DTHE 0)
         0x47, 0x30000, 0x47, 6, 0, 0x3F80000000000000, 1, 0x79006C00, 5, // TEST_1 (ZTST GREATER, ZTE 1), TEST_1 (ZTST ALWAYS, ZTE 1), PMODE (CRTMD 1, EN2 1), RGBAQ (Q 1.0, A 0, B 0, G 0, R 0), XYZ2 (Z 0.0, Y 1728.0, X 1936.0)
         0x87009400, 5, 0x70000, 0x47}                                    // XYZ2 (Z 0, Y 2160.0, X 2368.0), TEST_1 (ZTST GREATER, ZTE 1)
    };

    X = Y = 0;
    DmaReset();

    Init_GS(1, 2, 0); // Interlaced, NTSC/PAL and FIELD mode

    SetVideoMode();
    Dma02Wait();
    progdma(&setupscr, 15);
    Dma02Wait();
}

void scr_putchar(int x, int y, u32 color, int ch)
{
    static struct t_setupchar setupchar __attribute__((aligned(16))) = {
        {0x1000000000000004, 0xE, 0xA000000000000, 0x50},
        {0}, 100, 100, {0x51}, {8, 8},
        {0x52, 0, 0x53, 0x800000000008010, 0}
    };
    static u32 charmap[64] __attribute__((aligned(16)));
    
    int i, j;
    u8 *font;

    struct t_setupchar *sc = (struct t_setupchar *)((u32)&setupchar | 0x20000000);
    sc->x = x;
    sc->y = y;

    progdma(&setupchar, 6);

    font = &msx[ch * 8];

    u32 *cm = (u32 *)((u32)charmap | 0x20000000);
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            cm[i * 8 + j] = (font[i] & (128 >> j)) ? color : bgcolor;
        }
    }

    Dma02Wait();
    progdma(charmap, 16);
    Dma02Wait();
}