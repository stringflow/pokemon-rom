void tilesetheader(std::string name, u8 tileset_id, int expected_data_pointer) {
    Tileset tileset;
    rom_gettileset(rom, tileset_id, &tileset);
    TEST(name,
         format_bytes(rom[expected_data_pointer], sizeof(Tileset)),
         format_bytes(&tileset, sizeof(Tileset)));
}

void tilepaircollision(std::string name, u8 tileset_id, u8 tile1, u8 tile2, bool expected_collision) {
    TEST(name + "-" + std::to_string(tile1) + "x" + std::to_string(tile2),
         format_value(expected_collision),
         format_value(rom_istilepaircollision(rom, tileset_id, tile1, tile2, false)));
}

void doortile(std::string name, u8 tileset_id, u8 tile, bool expected_door) {
    TEST(name + "-" + std::to_string(tile),
         format_value(expected_door),
         format_value(rom_isdoortile(rom, tileset_id, tile)));
}

void warptile(std::string name, u8 tileset_id, u8 tile, bool expected_warp) {
    TEST(name + "-" + std::to_string(tile),
         format_value(expected_warp),
         format_value(rom_iswarptile(rom, tileset_id, tile)));
}

void tileset_tests() {
    tilesetheader("overworld", 0, rom.game == YELLOW ? 0x34558 : 0x347be);
    tilesetheader("plateau", 23, rom.game == YELLOW ? 0x3466c : 0x348d2);
    if(rom.game == YELLOW) tilesetheader("beachhouse", 24, 0x34678);
    
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