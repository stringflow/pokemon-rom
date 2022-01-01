void trainerclassrawname(std::string name, u8 trainerclassId, int expectedDataPointer) {
    rawnameTest(__FUNCTION__, name, trainerclassId, expectedDataPointer, rom_gettrainerclassrawname);
}

void trainerclassname(std::string name, u8 trainerclassId, std::string expectedName) {
    nameTest(__FUNCTION__, name, trainerclassId, expectedName, rom_gettrainerclassname);
}

void trainerclassroster(std::string name, u8 trainerclassId, u8 rosterIndex, std::vector<u8> expectedData) {
    int size = rom_gettrainerclassroster(rom, trainerclassId, rosterIndex, 0) * 2;
    u8 roster[size];
    rom_gettrainerclassroster(rom, trainerclassId, rosterIndex, roster);
    TEST(name + "#" + std::to_string(rosterIndex),
         formatBytes(expectedData.data(), expectedData.size()),
         formatBytes(roster, size));
}

void trainermoney(std::string name, u8 trainerclassId, u8 rosterIndex, int expectedMoney) {
    int gotMoney = rom_gettrainermoneyreward(rom, trainerclassId, rosterIndex);
    TEST(name + "#" + std::to_string(rosterIndex),
         formatValue(expectedMoney),
         formatValue(gotMoney));
}

void trainerclassfemale(std::string name, u8 trainerclassId, bool expectedFemale) {
    TEST(name,
         formatValue(expectedFemale),
         formatValue(rom_istrainerclassfemale(rom, trainerclassId)));
}

void trainerclassevil(std::string name, u8 trainerclassId, bool expectedEvil) {
    TEST(name,
         formatValue(expectedEvil),
         formatValue(rom_istrainerclassevil(rom, trainerclassId)));
}

void trainerclassmovechoicemods(std::string name, u8 trainerclassId, u8 expectedMods) {
    TEST(name,
         formatValue(expectedMods),
         formatValue(rom_gettrainerclassmovechoicemodifications(rom, trainerclassId)));
}

void trainerclassTests() {
    trainerclassrawname("youngster", 201, rom.game == Yellow ? 0xe597e : 0xe59ff);
    trainerclassrawname("lance", 247,     rom.game == Yellow ? 0xe5b00 : 0xe5b81);
    
    trainerclassname("youngster", 201, "YOUNGSTER");
    trainerclassname("lance", 247, "LANCE");
    
    trainerclassroster("youngster", 201, 0, { 0xa5, 0x0b, 0x6c, 0x0b });
    trainerclassroster("youngster", 201, 1, { 0x05, 0x0e });
    trainerclassroster("lance", 247, 0, { 0x16, 0x3a, 0x59, 0x38, 0x59, 0x38, 0xab, 0x3c, 0x42, 0x3e });
    
    trainermoney("youngster", 201, 0, 165);
    trainermoney("bugcatcher", 202, 0, rom.game == Yellow ? 70 : 60);
    // NOTE(stringflow): cerulean rival, different roster id because in yellow because he can
    // only have eevee.
    trainermoney("rival1", 225, rom.game == Yellow ? 2 : 6, 595);
    trainermoney("lance", 247, 0, 6138);
    
    trainerclassfemale("youngster", 201, false);
    trainerclassfemale("lass", 203, true);
    trainerclassfemale("jr_trainer_f", 206, true);
    trainerclassfemale("beauty", 218, true);
    trainerclassfemale("cooltrainer_f", 232, true);
    trainerclassfemale("lance", 247, false);
    
    trainerclassevil("youngster", 201, false);
    trainerclassevil("gambler", 217, true);
    trainerclassevil("rocker", 220, true);
    trainerclassevil("juggler", 221, true);
    trainerclassevil("chief", 227, true);
    trainerclassevil("scientist", 228, true);
    trainerclassevil("giovanni", 229, true);
    trainerclassevil("rocket", 230, true);
    trainerclassevil("lance", 247, false);
    
    trainerclassmovechoicemods("youngster", 201, 0);
    trainerclassmovechoicemods("bugcatcher", 202, 1);
    trainerclassmovechoicemods("lorelei", 244, 7);
    trainerclassmovechoicemods("lance", 247, 5);
}