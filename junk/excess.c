/* declaring 20 instances of yin and yangs, at random x coords
	int i;
	int next = YANG;
	for (i=0; i<YINGYANG_INSTANCES; i+=2) {
		// x should be (0 - (0...63))
		// y should increment with every iteration
		int x1 = 0;
		int x2 = x1 + 64;
		int y = 18 * (i/2);	// 18 being pixel height.
		spriteInfo sprite1 = {i, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 0, x1, y};
		spriteInfo sprite2 = {i+1, 0, SpriteSize_64x64, SpriteColorFormat_16Color, 0, 0, x2, y};
		sprite1.gfx = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
		sprite2.gfx = oamAllocateGfx(&oamMain, SpriteSize_64x64, SpriteColorFormat_16Color);
		if (next == YANG) {
			// yang
			dmaCopy((u8*)yang1Tiles, sprite1.gfx, 2048);
			dmaCopy((u8*)yang2Tiles, sprite2.gfx, 2048);
			next = YIN;
		}
		else {
			// yin
			dmaCopy((u8*)yin1Tiles, sprite1.gfx, 2048);
			dmaCopy((u8*)yin2Tiles, sprite2.gfx, 2048);
			next = YANG;
		}
		sprites[i] = sprite1;
		sprites[i+1] = sprite2;
	}
	*/