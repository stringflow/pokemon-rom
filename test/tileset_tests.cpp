void tilesetheader(std::string name, u8 tilesetId, int expectedDataPointer) {
    Tileset tileset;
    rom_gettileset(rom, tilesetId, &tileset);
    TEST(name,
         formatBytes(rom[expectedDataPointer], sizeof(Tileset)),
         formatBytes(&tileset, sizeof(Tileset)));
}

void tilesetTests() {
    tilesetheader("overworld", 0, rom.game == Yellow ? 0x34558 : 0x347be);
    tilesetheader("plateau", 23, rom.game == Yellow ? 0x3466c : 0x348d2);
    if(rom.game == Yellow) tilesetheader("beachhouse", 24, 0x34678);
}