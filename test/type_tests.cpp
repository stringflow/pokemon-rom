void typeName(std::string name, u8 type_id, std::string expected_name) {
    name_test(__FUNCTION__, name, type_id, expected_name, rom_gettypename);
}

void typespecial(std::string name, u8 type_id, bool expected_special) {
    TEST(name,
         format_value(expected_special),
         format_value(rom_istypespecial(type_id)));
}

void typematchup(std::string name, u8 attacking, u8 defending, f32 expected_effectiveness) {
    TEST(name,
         std::to_string(expected_effectiveness),
         std::to_string(rom_geteffectiveness(rom, attacking, defending)));
}

void typematchup(std::string name, u8 attacking, u8 defending1, u8 defending2, f32 expected_effectiveness) {
    TEST(name,
         std::to_string(expected_effectiveness),
         std::to_string(rom_geteffectiveness(rom, attacking, defending1) * 
                        rom_geteffectiveness(rom, attacking, defending2)));
}

void type_tests() {
    typeName("normal", 0x00, "NORMAL");
    typeName("bird", 0x06, "BIRD");
    typeName("fire", 0x14, "FIRE");
    typeName("dragon", 0x1a, "DRAGON");
    
    typespecial("normal", 0x00, false);
    typespecial("ghost", 0x08, false);
    typespecial("fire", 0x14, true);
    typespecial("dragon", 0x1a, true);
    
    typematchup("NormalxNormal", 0x00, 0x00, 1.0f);
    typematchup("NormalxGhost", 0x00, 0x08, 0.0f);
    typematchup("FightingxNormal", 0x01, 0x00, 2.0f);
    typematchup("WaterxGroundRock", 0x15, 0x04, 0x05, 4.0f);
    typematchup("IcexWaterIce", 0x19, 0x15, 0x19, 0.25f);
    typematchup("DragonxDragon", 0x1a, 0x1a, 2.0f);
}