#include <nds.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "MainBg.h"
#include "MainBg2.h"
#include "MainBg2_manual.h"
#include "MainBg2_automatic.h"
#include "yang1.h"
#include "yang2.h"
#include "yin1.h"
#include "yin2.h"
#include "automaticOff1.h"
#include "automaticOff2.h"
#include "automaticOff3.h"
#include "automaticOn1.h"
#include "automaticOn2.h"
#include "automaticOn3.h"
#include "manualOff1.h"
#include "manualOff2.h"
#include "manualOff3.h"
#include "manualOn1.h"
#include "manualOn2.h"
#include "manualOn3.h"

typedef struct {	
	int index;
	u16* gfx;
	SpriteSize size;
	SpriteColorFormat format;
	int rotationIndex;
	int paletteAlpha;
	int x;
	int y;
} spriteInfo;

typedef struct {
	int id;
	int x;
	int y;
	int xdirection;
	u16* gfx;
} yinYangInfo;

int coinFlipSet[24];

enum yinyang { YIN = 0, YANG = 1 };
enum direction { LEFT = 0, RIGHT = 1 };
enum coinFace { HEADS = 0, TAILS = 1};

void initVideo() {
	vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
	vramSetBankB(VRAM_B_MAIN_SPRITE_0x06400000);
	vramSetBankC(VRAM_C_SUB_BG_0x06200000);
	vramSetBankD(VRAM_D_SUB_SPRITE);
	vramSetBankE(VRAM_E_LCD);
	vramSetBankF(VRAM_F_LCD);
	vramSetBankG(VRAM_G_LCD);
	vramSetBankH(VRAM_H_LCD);
	vramSetBankI(VRAM_I_LCD);

	videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_5_2D);
}

void initBackgrounds() {
	int bg2 = bgInit(2, BgType_Bmp16, BgSize_B16_256x256, 0,0);
	int bg2sub = bgInitSub(2, BgType_Bmp16, BgSize_B16_256x256, 2,0);
	decompress(MainBgBitmap, bgGetGfxPtr(bg2),  LZ77Vram);	
	decompress(MainBgBitmap, bgGetGfxPtr(bg2sub),  LZ77Vram);	
}

void initConsole(PrintConsole * console) {
	// consoleInit (PrintConsole * console, int layer, BgType type,
	// BgSize size, int mapBase, int tileBase, bool mainDisplay, bool loadGraphics)	
	consoleInit(console, 0, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true);
	consoleSelect(console);
	printf("\x1b[30m");
}

void initSprites(spriteInfo * sprites) {
	oamInit(&oamMain, SpriteMapping_1D_64, false);
	oamInit(&oamSub, SpriteMapping_1D_64, false);
	
	// spriteInfo {id, gfxPtr, SpriteSize, SpriteColorFormat, rotationIndex, paletteAlpha, x, y}
	
	// only one palette
	dmaCopy(yang1Pal, &SPRITE_PALETTE[0], yang1PalLen);
	
	// just two sprites (actually four, but logically two, since they are bigger than 64x64
	spriteInfo sprite1 = {0, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 0, 0, 0};
	spriteInfo sprite2 = {1, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 0, 0, 0};
	spriteInfo sprite3 = {2, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 0, 0, 0};
	spriteInfo sprite4 = {3, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 0, 0, 0};
	sprite1.gfx = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
	sprite2.gfx = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
	sprite3.gfx = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
	sprite4.gfx = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
	dmaCopy((u8*)yang1Tiles, sprite1.gfx, 2048);
	dmaCopy((u8*)yang2Tiles, sprite2.gfx, 2048);
	dmaCopy((u8*)yin1Tiles, sprite3.gfx, 2048);
	dmaCopy((u8*)yin2Tiles, sprite4.gfx, 2048);
	sprites[0] = sprite1;
	sprites[1] = sprite2;
	sprites[2] = sprite3;
	sprites[3] = sprite4;
	
	// manual and automatic buttons
	dmaCopy(manualOff1Pal, &SPRITE_PALETTE_SUB[1 * 16], manualOff1PalLen);
	dmaCopy(manualOff2Pal, &SPRITE_PALETTE_SUB[2 * 16], manualOff2PalLen);
	dmaCopy(manualOff3Pal, &SPRITE_PALETTE_SUB[3 * 16], manualOff3PalLen);
	dmaCopy(manualOn1Pal, &SPRITE_PALETTE_SUB[4 * 16], manualOn1PalLen);
	dmaCopy(manualOn2Pal, &SPRITE_PALETTE_SUB[5 * 16], manualOn2PalLen);
	dmaCopy(manualOn3Pal, &SPRITE_PALETTE_SUB[6 * 16], manualOn3PalLen);
	
	dmaCopy(automaticOff1Pal, &SPRITE_PALETTE_SUB[7 * 16], automaticOff1PalLen);
	dmaCopy(automaticOff2Pal, &SPRITE_PALETTE_SUB[8 * 16], automaticOff2PalLen);
	dmaCopy(automaticOff3Pal, &SPRITE_PALETTE_SUB[9 * 16], automaticOff3PalLen);
	dmaCopy(automaticOn1Pal, &SPRITE_PALETTE_SUB[10 * 16], automaticOn1PalLen);
	dmaCopy(automaticOn2Pal, &SPRITE_PALETTE_SUB[11 * 16], automaticOn2PalLen);
	dmaCopy(automaticOn3Pal, &SPRITE_PALETTE_SUB[12 * 16], automaticOn3PalLen);
	
	spriteInfo manualOff1 = {4, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 1, 0, 0};
	spriteInfo manualOff2 = {5, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 2, 0, 0};
	spriteInfo manualOff3 = {6, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 3, 0, 0};
	spriteInfo manualOn1 = {7, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 4, 0, 0};
	spriteInfo manualOn2 = {8, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 5, 0, 0};
	spriteInfo manualOn3 = {9, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 6, 0, 0};
	
	spriteInfo automaticOff1 = {10, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 7, 0, 0};
	spriteInfo automaticOff2 = {11, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 8, 0, 0};
	spriteInfo automaticOff3 = {12, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 9, 0, 0};
	spriteInfo automaticOn1 = {13, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 10, 0, 0};
	spriteInfo automaticOn2 = {14, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 11, 0, 0};
	spriteInfo automaticOn3 = {15, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 12, 0, 0};
	
	manualOff1.x = (SCREEN_WIDTH - 192) / 2;
	manualOff2.x = manualOff1.x + 64;
	manualOff3.x = manualOff2.x + 64;
	manualOff1.y = manualOff2.y = manualOff3.y = (SCREEN_HEIGHT - (64+64+2)) / 2;
	manualOn1.x = (SCREEN_WIDTH - 192) / 2;
	manualOn2.x = manualOn1.x + 64;
	manualOn3.x = manualOn2.x + 64;
	manualOn1.y = manualOn2.y = manualOn3.y = (SCREEN_HEIGHT / 2) - (64+1);
	
	automaticOff1.x = (SCREEN_WIDTH - 192) / 2;
	automaticOff2.x = automaticOff1.x + 64;
	automaticOff3.x = automaticOff2.x + 64;
	automaticOff1.y = automaticOff2.y = automaticOff3.y = (SCREEN_HEIGHT / 2) + 1;
	automaticOn1.x = (SCREEN_WIDTH - 192) / 2;
	automaticOn2.x = automaticOn1.x + 64;
	automaticOn3.x = automaticOn2.x + 64;
	automaticOn1.y = automaticOn2.y = automaticOn3.y = (SCREEN_HEIGHT / 2) + 1;
	
	manualOff1.gfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
	manualOff2.gfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
	manualOff3.gfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
	dmaCopy((u8*)manualOff1Tiles, manualOff1.gfx, 2048);
	dmaCopy((u8*)manualOff2Tiles, manualOff2.gfx, 2048);
	dmaCopy((u8*)manualOff3Tiles, manualOff3.gfx, 2048);
	manualOn1.gfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
	manualOn2.gfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
	manualOn3.gfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
	dmaCopy((u8*)manualOn1Tiles, manualOn1.gfx, 2048);
	dmaCopy((u8*)manualOn2Tiles, manualOn2.gfx, 2048);
	dmaCopy((u8*)manualOn3Tiles, manualOn3.gfx, 2048);

	automaticOff1.gfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
	automaticOff2.gfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
	automaticOff3.gfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
	automaticOn1.gfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
	automaticOn2.gfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
	automaticOn3.gfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);

	dmaCopy((u8*)automaticOff1Tiles, automaticOff1.gfx, 2048);
	dmaCopy((u8*)automaticOff2Tiles, automaticOff2.gfx, 2048);
	dmaCopy((u8*)automaticOff3Tiles, automaticOff3.gfx, 2048);
	dmaCopy((u8*)automaticOn1Tiles, automaticOn1.gfx, 2048);
	dmaCopy((u8*)automaticOn2Tiles, automaticOn2.gfx, 2048);
	dmaCopy((u8*)automaticOn3Tiles, automaticOn3.gfx, 2048);

	
	sprites[4] = manualOff1;
	sprites[5] = manualOff2;
	sprites[6] = manualOff3;
	sprites[7] = manualOn1;
	sprites[8] = manualOn2; 
	sprites[9] = manualOn3;
	sprites[10] = automaticOff1;
	sprites[11] = automaticOff2;
	sprites[12] = automaticOff3;
	sprites[13] = automaticOn1;
	sprites[14] = automaticOn2;
	sprites[15] = automaticOn3;
	
	oamSet(&oamSub, manualOff1.index, manualOff1.x, manualOff1.y, 0, 1, SpriteSize_64x64, SpriteColorFormat_16Color,
				manualOff1.gfx, -1, false, false, false, false, false);
	oamSet(&oamSub, manualOff2.index, manualOff2.x, manualOff2.y, 0, 2, SpriteSize_64x64, SpriteColorFormat_16Color,
				manualOff2.gfx, -1, false, false, false, false, false);
	oamSet(&oamSub, manualOff3.index, manualOff3.x, manualOff3.y, 0, 3, SpriteSize_64x64, SpriteColorFormat_16Color,
				manualOff3.gfx, -1, false, false, false, false, false);
	oamSet(&oamSub, automaticOff1.index, automaticOff1.x, automaticOff1.y, 0, 7, SpriteSize_64x64, SpriteColorFormat_16Color,
				automaticOff1.gfx, -1, false, false, false, false, false);
	oamSet(&oamSub, automaticOff2.index, automaticOff2.x, automaticOff2.y, 0, 8, SpriteSize_64x64, SpriteColorFormat_16Color,
				automaticOff2.gfx, -1, false, false, false, false, false);
	oamSet(&oamSub, automaticOff3.index, automaticOff3.x, automaticOff3.y, 0, 9, SpriteSize_64x64, SpriteColorFormat_16Color,
				automaticOff3.gfx, -1, false, false, false, false, false);
}

void moveYinYangs(int totalIndex, yinYangInfo * yinYangs) {
	int i;
	for (i=0; i<totalIndex; i+=2) {
		if (yinYangs[i].xdirection == LEFT) {
			// move to the left
			yinYangs[i].x -= 1;
			yinYangs[i+1].x -= 1;
			
			// move around unseen yangs
			if (yinYangs[i].x+96 <= 0) {
				int j;
				int xHigh = 0;
				for (j=0; j < totalIndex; j+=2) {
					if (yinYangs[i].xdirection == yinYangs[j].xdirection &&
							i != j &&
							yinYangs[i].y == yinYangs[j].y &&
							yinYangs[j].x > xHigh) {
						xHigh = yinYangs[j].x;
					}
				}
				yinYangs[i].x = xHigh + 95;
				yinYangs[i+1].x = yinYangs[i].x + 64;
			}
			oamSet(&oamMain, i, yinYangs[i].x, yinYangs[i].y, 0, 0, SpriteSize_64x64, SpriteColorFormat_16Color,
				yinYangs[i].gfx, -1, false, false, false, false, false);
			oamSet(&oamMain, i+1, yinYangs[i+1].x, yinYangs[i+1].y, 0, 0, SpriteSize_64x64, SpriteColorFormat_16Color,
				yinYangs[i+1].gfx, -1, false, false, false, false, false);
		}
		else {
			yinYangs[i].x += 1;
			yinYangs[i+1].x += 1;
			if (yinYangs[i].x >= 256) {
				int j;
				int xLow = 256;
				for (j=0; j < totalIndex; j+=2) {
					if (yinYangs[i].xdirection == yinYangs[j].xdirection &&
							i != j &&
							yinYangs[i].y == yinYangs[j].y &&
							yinYangs[j].x < xLow) {
						xLow = yinYangs[j].x;
					}
				}
/*					if (yinYangs[i].y == debugY && xLow != 256) {
					printf("xLow = %d; yinYangs[%d].x = %d\n",
						xLow, i, xLow-95);
				}
*/					yinYangs[i].x = xLow - 95;
				yinYangs[i+1].x = yinYangs[i].x + 64;
			}
			oamSet(&oamMain, i, yinYangs[i].x, yinYangs[i].y, 0, 0, SpriteSize_64x64, SpriteColorFormat_16Color,
				yinYangs[i].gfx, -1, false, false, false, false, false);
			oamSet(&oamMain, i+1, yinYangs[i+1].x, yinYangs[i+1].y, 0, 0, SpriteSize_64x64, SpriteColorFormat_16Color,
				yinYangs[i+1].gfx, -1, false, false, false, false, false);
			/*
			if (yinYangs[i].y == debugY) {
				printf("yinYangs[%d].x == %d\n", i, yinYangs[i].x);
			}
			*/
		}
	}
}

int initYinYangs(spriteInfo * sprites, yinYangInfo * yinYangs) {
	int index = 0;
	// random num: (int)((double)rand() / ((double)RAND_MAX + 1) * N);
	int next = (int)((double)rand() / ((double)RAND_MAX + 1) * 2);
	int x = -1 * (int)((double)rand() / ((double)RAND_MAX + 1) * 32) + -96;
	int y = -1 * (int)((double)rand() / ((double)RAND_MAX + 1) * 18);
	int xdirection = (int)((double)rand() / ((double)RAND_MAX + 1) * 2);
//	int debugY;
	while (y < 192) {
		// 352 = 256 + 96
		while (x < 352) {	
			if (next == YANG) {
				oamSet(&oamMain, index, x, y, 0, 0, SpriteSize_64x64, SpriteColorFormat_16Color,
					sprites[0].gfx, -1, false, false, false, false, false);
				oamSet(&oamMain, index+1, x+64, y, 0, 0, SpriteSize_64x64, SpriteColorFormat_16Color,
					sprites[1].gfx, -1, false, false, false, false, false);
				yinYangInfo yang1 = {index, x, y, xdirection, sprites[0].gfx};
				yinYangInfo yang2 = {index+1, x+64, y, xdirection, sprites[1].gfx};
				yinYangs[index] = yang1;
				yinYangs[index+1] = yang2;
				x += 96;
				if (x+96 >= 352)
					next = YANG;
				else 
					next = YIN;
				index += 2;
			}
			else {
				oamSet(&oamMain, index, x, y, 0, 0, SpriteSize_64x64, SpriteColorFormat_16Color,
					sprites[2].gfx, -1, false, false, false, false, false);
				oamSet(&oamMain, index+1, x+64, y, 0, 0, SpriteSize_64x64, SpriteColorFormat_16Color,
					sprites[3].gfx, -1, false, false, false, false, false);
				yinYangInfo yin1 = {index, x, y, xdirection, sprites[2].gfx};
				yinYangInfo yin2 = {index+1, x+64, y, xdirection, sprites[3].gfx};
				yinYangs[index] = yin1;
				yinYangs[index+1] = yin2;
				x += 96;
				next = YANG;
				index += 2;
			}
		}
		
		y += 18;
		
/*		if (y < 50) {
			debugY = y;
		}*/
		
		x = -1 * (int)((double)rand() / ((double)RAND_MAX + 1) * 32) + -96;
		
		if (xdirection == LEFT)
			xdirection = RIGHT;
		else
			xdirection = LEFT;
			
		next = (int)((double)rand() / ((double)RAND_MAX + 1) * 2);
	}
	
	return index;
}

int within(touchPosition * touch, spriteInfo sprite) {
	if (touch->px >= sprite.x && touch->px <= sprite.x+64 &&
		touch->py >= sprite.y && touch->py <= sprite.y+64) {
			return 1;
		}
	return 0;
}

void generateCoinFlipSet(int * coinFlipSet) {
	int i;
	for (i=0; i<24; ++i) {
		coinFlipSet[i] = (int)((double)rand() / ((double)RAND_MAX + 1) * 2);
	}
}

int calculateHexagram(int * coinFlipSet, int * hexagram) {
	int i, j, k;
	int * lowerTrigram;
	int * upperTrigram;
	for (i=0; i<24; i+=4) {
		if (coinFlipSet[i] == HEADS) {
			if (coinFlipSet[i+1] == HEADS) {
				j = 2;
			}
			else {
				j = 3;
			}
		}
		else {
			j = 3;
		}
		
		if (coinFlipSet[i+2] == HEADS) {
			if (coinFlipSet[i+3] == HEADS) {
				k = 4;
			}
			else {
				k = 5;
			}
		}
		else {
			if (coinFlipSet[i+3] == HEADS) {
				k = 5;
			}
			else {
				k = 6;
			}
		}
		hexagram[i/4] = j + k;
	}
	
	if (hexagram[0] % 2 == 0) {
		if (hexagram[1] % 2 == 0) {
			if (hexagram[2] % 2 == 0) {
				// k'un
				static int kun[] = { 12, 16, 8, 23, 2, 20, 35, 45 };
				lowerTrigram = kun;
			}
			else {
				// kên
				static int ken[] = { 33, 62, 39, 52, 15, 53, 56, 31 };
				lowerTrigram = ken;
			}
		}
		else {
			if (hexagram[2] % 2 == 0) {
				// k'an
				static int kan[] = { 6, 40, 29, 4, 7, 59, 64, 47 };
				lowerTrigram = kan;
			}
			else {
				// sun
				static int sun[] = { 44, 32, 48, 18, 46, 57, 50, 28 };
				lowerTrigram = sun;
			}
		}
	}
	else {
		if (hexagram[1] % 2 == 0) {
			if (hexagram[2] % 2 == 0) {
				// chên
				static int chen[] = { 25, 51, 3, 27, 24, 42, 21, 17 };
				lowerTrigram = chen;
			}
			else {
				// li 
				static int li[] = { 13, 55, 63, 22, 36, 37, 30, 49 };
				lowerTrigram = li;
			}
		}
		else {
			if (hexagram[2] % 2 == 0) {
				// tui
				static int tui[] = { 10, 54, 60, 41, 19, 61, 38, 58 };
				lowerTrigram = tui;
			}
			else {
				// ch'ien
				static int chien[] = { 1, 34, 5, 26, 11, 9, 14, 43 };
				lowerTrigram = chien;
			}
		}
	}
	
	if (hexagram[3] % 2 == 0) {
		if (hexagram[4] % 2 == 0) {
			if (hexagram[5] % 2 == 0) {
				// k'un
				static int kun[] = { 11, 24, 7, 15, 2, 46, 36, 19 };
				upperTrigram = kun;
			}
			else {
				// kên 
				static int ken[] = { 26, 27, 4, 52, 23, 18, 22, 41 };
				upperTrigram = ken;
			}
		}
		else {
			if (hexagram[5] % 2 == 0) {
				// k'an
				static int kan[] = { 5, 3, 29, 39, 8, 48, 63, 60 };
				upperTrigram = kan;
			}
			else {
				// sun
				static int sun[] = { 9, 42, 59, 53, 20, 57, 37, 61 };
				upperTrigram = sun;
			}
		}
	}
	else {
		if (hexagram[4] % 2 == 0) {
			if (hexagram[5] % 2 == 0) {
				// chên
				static int chen[] = { 34, 51, 40, 62, 16, 32, 55, 54 };
				upperTrigram = chen;
			}
			else {
				// li
				static int li[] = { 14, 21, 64, 56, 35, 50, 30, 38 };
				upperTrigram = li;
			}
		}
		else {
			if (hexagram[5] % 2 == 0) {
				// tui
				static int tui[] = { 43, 17, 47, 31, 45, 28, 49, 58 };
				upperTrigram = tui;
			}
			else {
				// ch'ien
				static int chien[] = { 1, 25, 6, 33, 12, 44, 13, 10 };
				upperTrigram = chien;
			}
		}
	}
	int hexId;
	for (i=0; i<8; ++i) {
		for (j=0; j<8; ++j) {
			if (lowerTrigram[i] == upperTrigram[j]) {
				hexId = lowerTrigram[i];
			}
		}
		// printf("lower:upper::%d:%d\n", lowerTrigram[i], upperTrigram[i]);
	}
	// printf("hexid: %d\n", hexId);
	
	printf("\n");
	for (i=5; i>=0; --i) {
		printf("hexagram[%d]: %d ", i, hexagram[i]);
		if (hexagram[i] % 2 == 0) {
			printf("---  ---\n");
		}
		else {
			printf("--------\n");
		}
	}
	return hexId;
}

int main() {
	touchPosition touch;
	spriteInfo sprites[128];
	yinYangInfo yinYangs[128];	//128 for simplicitys sake
	PrintConsole mainScreen;
	srand ( time(NULL) );
	
	initVideo();
	initBackgrounds();
	initSprites(sprites);
	initConsole(&mainScreen);
	consoleClear();
	
	int totalIndex = initYinYangs(sprites, yinYangs);
	
	int keys;
	int automaticOn = 0;
	int manualOn = 0;
	int frameCnt = 0;
	int flipCnt = 0;
	int hexagram[6];
	generateCoinFlipSet(coinFlipSet);
	while (true) {
		scanKeys();
		touchRead(&touch);
		keys = keysHeld();
		swiWaitForVBlank();
		consoleClear();
		if (keys) {
			if (keys & KEY_TOUCH) {
				touchRead(&touch);
				if (within(&touch, sprites[4]) ||
						within(&touch, sprites[5]) ||
						within(&touch, sprites[6])) {
					// manual
					manualOn = 1;
					oamClearSprite(&oamSub, sprites[4].index);
					oamClearSprite(&oamSub, sprites[5].index);
					oamClearSprite(&oamSub, sprites[6].index);
					
/*					dmaCopy((u8*)manualOn1Tiles, sprites[4].gfx, 2048);
					dmaCopy((u8*)manualOn2Tiles, sprites[5].gfx, 2048);
					dmaCopy((u8*)manualOn3Tiles, sprites[6].gfx, 2048);
*/					
					oamSet(&oamSub, sprites[7].index, sprites[7].x, sprites[7].y, 0, 4, SpriteSize_64x64, SpriteColorFormat_16Color,
								sprites[7].gfx, -1, false, false, false, false, false);
					oamSet(&oamSub, sprites[8].index, sprites[8].x, sprites[8].y, 0, 5, SpriteSize_64x64, SpriteColorFormat_16Color,
								sprites[8].gfx, -1, false, false, false, false, false);
					oamSet(&oamSub, sprites[9].index, sprites[9].x, sprites[9].y, 0, 6, SpriteSize_64x64, SpriteColorFormat_16Color,
								sprites[9].gfx, -1, false, false, false, false, false);
				}
				else if (within(&touch, sprites[10]) ||
							within(&touch, sprites[11]) ||
							within(&touch, sprites[12])) {
					// automatic
					automaticOn = 1;
					oamClearSprite(&oamSub, sprites[10].index);
					oamClearSprite(&oamSub, sprites[11].index);
					oamClearSprite(&oamSub, sprites[12].index);
					
/*					dmaCopy((u8*)automaticOn1Tiles, sprites[10].gfx, 2048);
					dmaCopy((u8*)automaticOn2Tiles, sprites[11].gfx, 2048);
					dmaCopy((u8*)automaticOn3Tiles, sprites[12].gfx, 2048);
*/					
					oamSet(&oamSub, sprites[13].index, sprites[13].x, sprites[13].y, 0, 10, SpriteSize_64x64, SpriteColorFormat_16Color,
								sprites[13].gfx, -1, false, false, false, false, false);
					oamSet(&oamSub, sprites[14].index, sprites[14].x, sprites[14].y, 0, 11, SpriteSize_64x64, SpriteColorFormat_16Color,
								sprites[14].gfx, -1, false, false, false, false, false);
					oamSet(&oamSub, sprites[15].index, sprites[15].x, sprites[15].y, 0, 12, SpriteSize_64x64, SpriteColorFormat_16Color,
								sprites[15].gfx, -1, false, false, false, false, false);
				}
			}
			if (keys & KEY_B) {
				if (automaticOn) {
					automaticOn = 0;
				}
				else if (manualOn) {
					manualOn = 0;
				}
				frameCnt = 0;
				flipCnt = 0;
				generateCoinFlipSet(coinFlipSet);
			}
		}
		else {
			if (!automaticOn && !manualOn) {
				oamClearSprite(&oamSub, sprites[7].index);
				oamClearSprite(&oamSub, sprites[8].index);
				oamClearSprite(&oamSub, sprites[9].index);
				oamClearSprite(&oamSub, sprites[13].index);
				oamClearSprite(&oamSub, sprites[14].index);
				oamClearSprite(&oamSub, sprites[15].index);
			
				oamSet(&oamSub, sprites[4].index, sprites[4].x, sprites[4].y, 0, 1, SpriteSize_64x64, SpriteColorFormat_16Color,
							sprites[4].gfx, -1, false, false, false, false, false);
				oamSet(&oamSub, sprites[5].index, sprites[5].x, sprites[5].y, 0, 2, SpriteSize_64x64, SpriteColorFormat_16Color,
							sprites[5].gfx, -1, false, false, false, false, false);
				oamSet(&oamSub, sprites[6].index, sprites[6].x, sprites[6].y, 0, 3, SpriteSize_64x64, SpriteColorFormat_16Color,
							sprites[6].gfx, -1, false, false, false, false, false);
				oamSet(&oamSub, sprites[10].index, sprites[10].x, sprites[10].y, 0, 7, SpriteSize_64x64, SpriteColorFormat_16Color,
							sprites[10].gfx, -1, false, false, false, false, false);
				oamSet(&oamSub, sprites[11].index, sprites[11].x, sprites[11].y, 0, 8, SpriteSize_64x64, SpriteColorFormat_16Color,
							sprites[11].gfx, -1, false, false, false, false, false);
				oamSet(&oamSub, sprites[12].index, sprites[12].x, sprites[12].y, 0, 9, SpriteSize_64x64, SpriteColorFormat_16Color,
							sprites[12].gfx, -1, false, false, false, false, false);
			}
		}
		if (automaticOn || manualOn) {
			int i;
			for(i=4; i<16; ++i) {
				oamClearSprite(&oamSub, sprites[i].index);
			}
			printf("Press B to go back...\n");
			
			if (automaticOn) {
				int SPEED = 7;
				printf("AUTOMATIC MODE\n");
				if (flipCnt < 24) {
					frameCnt++;
					for (i=0; i<flipCnt; ++i) {
						if (coinFlipSet[i] == 0) {
							printf("flip #%d: H\n", i+1);
						}
						else if (coinFlipSet[i] == 1) {
							printf("flip #%d: T\n", i+1);
						}
					}
					
					if (frameCnt < SPEED && flipCnt < 24) {
						if (frameCnt % 2 == 0) {
							printf("flip #%d: H\n", flipCnt+1);
						}
						else {
							printf("flip #%d: T\n", flipCnt+1);
						}
					}
					else {
						if (flipCnt < 24) {
							if (coinFlipSet[flipCnt] == 0) {
								printf("flip #%d: H\n", flipCnt+1);
								flipCnt++;
								frameCnt = 0;
							}
							else if (coinFlipSet[flipCnt] == 1) {
								printf("flip #%d: T\n", flipCnt+1);
								flipCnt++;
								frameCnt = 0;
							}
						}
					}
				}
				else {
					// lets show this hexagram
					int hexId = calculateHexagram(coinFlipSet, hexagram);
					printf("\nhexagram: %d\n", hexId);
				}
			}
			else if (manualOn) {
				printf("MANUAL MODE\n");
				printf("\n just do it by yourself!\n");
			}
			else {
				printf("this will never get printed... ever...\n");
				printf("i peed myself during naptime in kindergarden...\n");
			}
		}
		moveYinYangs(totalIndex, yinYangs);
		swiWaitForVBlank();
		oamUpdate(&oamMain);
		oamUpdate(&oamSub);
	}
}