void trainerclassrawname(std::string name, u8 trainer_class_id, int expected_data_pointer) {
    rawname_test(__FUNCTION__, name, trainer_class_id, expected_data_pointer, rom_gettrainerclassrawname);
}

void trainerclassname(std::string name, u8 trainer_class_id, std::string expected_name) {
    name_test(__FUNCTION__, name, trainer_class_id, expected_name, rom_gettrainerclassname);
}

void trainerclassroster(std::string name, u8 trainer_class_id, u8 roster_index, std::vector<u8> expected_data) {
    int size = rom_gettrainerclassroster(rom, trainer_class_id, roster_index, 0) * 2;
    u8 roster[size];
    rom_gettrainerclassroster(rom, trainer_class_id, roster_index, roster);
    TEST(name + "#" + std::to_string(roster_index),
         format_bytes(expected_data.data(), expected_data.size()),
         format_bytes(roster, size));
}

void trainermoney(std::string name, u8 trainer_class_id, u8 roster_index, int expected_money) {
    int got_money = rom_gettrainermoneyreward(rom, trainer_class_id, roster_index);
    TEST(name + "#" + std::to_string(roster_index),
         format_value(expected_money),
         format_value(got_money));
}

void trainerclassfemale(std::string name, u8 trainer_class_id, bool expected_female) {
    TEST(name,
         format_value(expected_female),
         format_value(rom_istrainerclassfemale(rom, trainer_class_id)));
}

void trainerclassevil(std::string name, u8 trainer_class_id, bool expected_evil) {
    TEST(name,
         format_value(expected_evil),
         format_value(rom_istrainerclassevil(rom, trainer_class_id)));
}

void trainerclassmovechoicemods(std::string name, u8 trainer_class_id, u8 expected_mods) {
    TEST(name,
         format_value(expected_mods),
         format_value(rom_gettrainerclassmovechoicemodifications(rom, trainer_class_id)));
}

void trainerclass_tests() {
    trainerclassrawname("youngster", 201, rom.game == YELLOW ? 0xe597e : 0xe59ff);
    trainerclassrawname("lance", 247,     rom.game == YELLOW ? 0xe5b00 : 0xe5b81);
    
    trainerclassname("youngster", 201, "YOUNGSTER");
    trainerclassname("lance", 247, "LANCE");
    
    trainerclassroster("youngster", 201, 0, { 0xa5, 0x0b, 0x6c, 0x0b });
    trainerclassroster("youngster", 201, 1, { 0x05, 0x0e });
    trainerclassroster("lance", 247, 0, { 0x16, 0x3a, 0x59, 0x38, 0x59, 0x38, 0xab, 0x3c, 0x42, 0x3e });
    
    trainermoney("youngster", 201, 0, 165);
    trainermoney("bugcatcher", 202, 0, rom.game == YELLOW ? 70 : 60);
    // NOTE(stringflow): cerulean rival, different roster id because in yellow because he can
    // only have eevee.
    trainermoney("rival1", 225, rom.game == YELLOW ? 2 : 6, 595);
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