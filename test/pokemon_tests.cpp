void basestats(std::string name, u8 species, int expected_data_pointer) {
    BaseStats stats;
    rom_getmonbasestats(rom, species, &stats);
    TEST(name, 
         format_bytes(rom[expected_data_pointer], sizeof(BaseStats)),
         format_bytes(&stats, sizeof(BaseStats)));
}

void monrawname(std::string name, u8 species, int expected_data_pointer) {
    rawname_test(__FUNCTION__, name, species, expected_data_pointer, rom_getmonrawname);
}

void monname(std::string name, u8 species, std::string expected_name) {
    name_test(__FUNCTION__, name, species, expected_name, rom_getmonname);
}

void monicon(std::string name, u8 species, u8 expected_icon) {
    TEST(name,
         format_value(expected_icon),
         format_value(rom_getmonicon(rom, species)));
}

void mondexentry(std::string name, u8 species, 
                 std::string expected_species_type,
                 u8 expected_feet, u8 expected_inches,
                 f32 expected_weight,
                 std::string expected_description) {
    DexEntry dex_entry;
    rom_getmondexentry(rom, species, &dex_entry);
    
    TEST(name + "-type",
         expected_species_type,
         std::string(dex_entry.type));
    
    TEST(name + "-height",
         format_value(expected_feet << 8 | expected_inches),
         format_value(dex_entry.feet << 8| dex_entry.inches));
    
    TEST(name + "-weight",
         std::to_string(expected_weight),
         std::to_string(dex_entry.weight));
    
    TEST(name + "-description",
         expected_description,
         dex_entry.description);
}

void moncrydata(std::string name, u8 species, int expected_data_pointer) {
    CryData crydata;
    rom_getmoncrydata(rom, species, &crydata);
    TEST(name, 
         format_bytes(rom[expected_data_pointer], sizeof(CryData)),
         format_bytes(&crydata, sizeof(CryData)));
}

// TODO(stringflow): sprite, cry tests?

void pokemon_tests() {
    basestats("bulbasaur", 0x99, 0xe43de);
    basestats("victreebel", 0xbe, 0xe4b86);
    basestats("mew", 0x15, rom.game == YELLOW ? 0xe5446 : 0x1425b);
    basestats("missingno", 0x1f, 0xe5fc2);
    basestats("hex00", 0x00, rom.game == YELLOW ? 0xe5702 : 0xe5fc2);
    basestats("hexff", 0xff, rom.game == YELLOW ? 0xe50fe : 0xe446a);
    
    // TODO(stringflow): test basestats of special missingnos (should be all zeros?)
    
    monrawname("bulbasaur", 0x99,  rom.game == YELLOW ? 0x3a45f0 : 0x7480e);
    monrawname("victreebel", 0xbe, rom.game == YELLOW ? 0x3a4762 : 0x74980);
    monrawname("mew", 0x15,        rom.game == YELLOW ? 0x3a40c8 : 0x742e6);
    monrawname("missingno", 0x1f,  rom.game == YELLOW ? 0x3a412c : 0x7434a);
    monrawname("hex00", 0x00,      rom.game == YELLOW ? 0x3a49f6 : 0x74c14);
    monrawname("hexff", 0xff,      rom.game == YELLOW ? 0x3a49ec : 0x74c0a);
    
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
                rom.game == YELLOW ?
                "Walks on its hind legs. Shows signs of intelligence. Its armor-like hide even repels molten lava." :
                "Protected by an armor-like hide, it is capable of living in molten lava of 3,600 degrees.");
    mondexentry("bulbasaur", 0x99,
                "SEED", 
                2, 4, 
                15.0f, 
                rom.game == YELLOW ?
                "It can go for days without eating a single morsel. In the bulb on its back, it stores energy." :
                "A strange seed was planted on its back at birth. The plant sprouts and grows with this POKéMON.");
    mondexentry("victreebel", 0xbe,
                "FLYCATCHER", 
                5, 7, 
                34.0f,
                rom.game == YELLOW ?
                "Lures prey with the sweet aroma of honey. Swallowed whole, the prey is melted in a day, bones and all." :
                "Said to live in huge colonies deep in jungles, although no one has ever returned from there.");
    
    moncrydata("rhydon", 0x01,     rom.game == YELLOW ? 0xe5462 : 0xe5446);
    moncrydata("bulbasaur", 0x99,  rom.game == YELLOW ? 0xe562a : 0xe560e);
    moncrydata("victreebel", 0xbe, rom.game == YELLOW ? 0xe5699 : 0xe567d);
    moncrydata("mew", 0x15,        rom.game == YELLOW ? 0xe549e : 0xe5482);
    moncrydata("missingno", 0x1f,  rom.game == YELLOW ? 0xe54bc : 0xe54a0);
    moncrydata("hex00", 0x00,      rom.game == YELLOW ? 0xe575f : 0xe5743);
    moncrydata("hexff", 0xff,      rom.game == YELLOW ? 0xe575c : 0xe5740);
}
