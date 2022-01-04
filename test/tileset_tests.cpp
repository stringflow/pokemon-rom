void tilesetheader(std::string name, u8 tilesetId, int expectedDataPointer) {
    Tileset tileset;
    rom_gettileset(rom, tilesetId, &tileset);
    TEST(name,
         formatBytes(rom[expectedDataPointer], sizeof(Tileset)),
         formatBytes(&tileset, sizeof(Tileset)));
}

void tilepaircollision(std::string name, u8 tilesetId, u8 tile1, u8 tile2, bool expectedCollision) {
    TEST(name + "-" + std::to_string(tile1) + "x" + std::to_string(tile2),
         formatValue(expectedCollision),
         formatValue(rom_istilepaircollision(rom, tilesetId, tile1, tile2, false)));
}

void doortile(std::string name, u8 tilesetId, u8 tile, bool expectedDoor) {
    TEST(name + "-" + std::to_string(tile),
         formatValue(expectedDoor),
         formatValue(rom_isdoortile(rom, tilesetId, tile)));
}

void warptile(std::string name, u8 tilesetId, u8 tile, bool expectedWarp) {
    TEST(name + "-" + std::to_string(tile),
         formatValue(expectedWarp),
         formatValue(rom_iswarptile(rom, tilesetId, tile)));
}

void tilesetTests() {
    tilesetheader("overworld", 0, rom.game == Yellow ? 0x34558 : 0x347be);
    tilesetheader("plateau", 23, rom.game == Yellow ? 0x3466c : 0x348d2);
    if(rom.game == Yellow) tilesetheader("beachhouse", 24, 0x34678);
    
    tilepaircollision("forest", 3, 0x30, 0x2e, true);
    tilepaircollision("forest", 3, 0x55, 0x2e, true);
    tilepaircollision("forest", 3, 0x20, 0x2e, true);
    tilepaircollision("forest", 3, 0x5e, 0x2e, true);
    tilepaircollision("forest", 3, 0x5f, 0x2e, true);
    tilepaircollision("forest", 3, 0x2e, 0x5f, true);
    tilepaircollision("overworld", 0, 0x5f, 0x2e, false);
    
    doortile("overworld", 0, 0x1b, true);
    doortile("overworld", 0, 0x58, true);
    doortile("forest", 3, 0x1b, false);
    doortile("forest", 3, 0x58, false);
    
    warptile("overworld", 0, 0x1b, true);
    warptile("overworld", 0, 0x58, true);
    warptile("forest", 3, 0x1b, false);
    warptile("forest", 3, 0x58, false);
    warptile("museum", 10, 0x3b, true);
    warptile("museum", 10, 0x1a, true);
    warptile("museum", 10, 0x1c, true);
}