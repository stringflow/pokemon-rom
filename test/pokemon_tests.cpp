void basestats(std::string name, u8 species, int expectedDataPointer) {
    BaseStats stats;
    rom_getmonbasestats(rom, species, &stats);
    TEST(name, 
         formatBytes(rom[expectedDataPointer], sizeof(BaseStats)),
         formatBytes(&stats, sizeof(BaseStats)));
}

void monrawname(std::string name, u8 species, int expectedDataPointer) {
    rawnameTest(__FUNCTION__, name, species, expectedDataPointer, rom_getmonrawname);
}

void monname(std::string name, u8 species, std::string expectedName) {
    nameTest(__FUNCTION__, name, species, expectedName, rom_getmonname);
}

void monicon(std::string name, u8 species, u8 expectedIcon) {
    TEST(name,
         formatValue(expectedIcon),
         formatValue(rom_getmonicon(rom, species)));
}

void mondexentry(std::string name, u8 species, 
                 std::string expectedSpeciesType,
                 u8 expectedFeet, u8 expectedInches,
                 f32 expectedWeight,
                 std::string expectedDescription) {
    DexEntry dexentry;
    rom_getmondexentry(rom, species, &dexentry);
    
    TEST(name + "-type",
         expectedSpeciesType,
         std::string(dexentry.type));
    
    TEST(name + "-height",
         formatValue(expectedFeet << 8 | expectedInches),
         formatValue(dexentry.feet << 8| dexentry.inches));
    
    TEST(name + "-weight",
         std::to_string(expectedWeight),
         std::to_string(dexentry.weight));
    
    TEST(name + "-description",
         expectedDescription,
         dexentry.description);
}

void moncrydata(std::string name, u8 species, int expectedDataPointer) {
    CryData crydata;
    rom_getmoncrydata(rom, species, &crydata);
    TEST(name, 
         formatBytes(rom[expectedDataPointer], sizeof(CryData)),
         formatBytes(&crydata, sizeof(CryData)));
}

// TODO(stringflow): sprite, cry tests?

void pokemonTests() {
    basestats("bulbasaur", 0x99, 0xe43de);
    basestats("victreebel", 0xbe, 0xe4b86);
    basestats("mew", 0x15, rom.game == Yellow ? 0xe5446 : 0x1425b);
    basestats("missingno", 0x1f, 0xe5fc2);
    basestats("hex00", 0x00, rom.game == Yellow ? 0xe5702 : 0xe5fc2);
    basestats("hexff", 0xff, rom.game == Yellow ? 0xe50fe : 0xe446a);
    
    // TODO(stringflow): test basestats of special missingnos (should be all zeros?)
    
    monrawname("bulbasaur", 0x99,  rom.game == Yellow ? 0x3a45f0 : 0x7480e);
    monrawname("victreebel", 0xbe, rom.game == Yellow ? 0x3a4762 : 0x74980);
    monrawname("mew", 0x15,        rom.game == Yellow ? 0x3a40c8 : 0x742e6);
    monrawname("missingno", 0x1f,  rom.game == Yellow ? 0x3a412c : 0x7434a);
    monrawname("hex00", 0x00,      rom.game == Yellow ? 0x3a49f6 : 0x74c14);
    monrawname("hexff", 0xff,      rom.game == Yellow ? 0x3a49ec : 0x74c0a);
    
    monname("rhydon", 0x01, "RHYDON");
    monname("bulbasaur", 0x99, "BULBASAUR");
    monname("victreebel", 0xbe, "VICTREEBEL");
    monname("mew", 0x15, "MEW");
    monname("missingno", 0x1f, "MISSINGNO.");
    monname("hex00", 0x00, "hex00");
    monname("hexff", 0xff, "hexff");
    
    monicon("rhydon", 0x01, 0x0);
    monicon("bulbasaur", 0x99, 0x7);
    monicon("mew", 0x15, 0x0);
    
    mondexentry("rhydon", 0x01, 
                "DRILL", 
                6, 3, 
                265.0f, 
                rom.game == Yellow ?
                "Walks on its hind legs. Shows signs of intelligence. Its armor-like hide even repels molten lava." :
                "Protected by an armor-like hide, it is capable of living in molten lava of 3,600 degrees.");
    mondexentry("bulbasaur", 0x99,
                "SEED", 
                2, 4, 
                15.0f, 
                rom.game == Yellow ?
                "It can go for days without eating a single morsel. In the bulb on its back, it stores energy." :
                "A strange seed was planted on its back at birth. The plant sprouts and grows with this POKéMON.");
    mondexentry("victreebel", 0xbe,
                "FLYCATCHER", 
                5, 7, 
                34.0f,
                rom.game == Yellow ?
                "Lures prey with the sweet aroma of honey. Swallowed whole, the prey is melted in a day, bones and all." :
                "Said to live in huge colonies deep in jungles, although no one has ever returned from there.");
    
    moncrydata("rhydon", 0x01,     rom.game == Yellow ? 0xe5462 : 0xe5446);
    moncrydata("bulbasaur", 0x99,  rom.game == Yellow ? 0xe562a : 0xe560e);
    moncrydata("victreebel", 0xbe, rom.game == Yellow ? 0xe5699 : 0xe567d);
    moncrydata("mew", 0x15,        rom.game == Yellow ? 0xe549e : 0xe5482);
    moncrydata("missingno", 0x1f,  rom.game == Yellow ? 0xe54bc : 0xe54a0);
    moncrydata("hex00", 0x00,      rom.game == Yellow ? 0xe575f : 0xe5743);
    moncrydata("hexff", 0xff,      rom.game == Yellow ? 0xe575c : 0xe5740);
}
