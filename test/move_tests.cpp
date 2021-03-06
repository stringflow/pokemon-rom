void moveheader(std::string name, u8 move_id, int expected_data_pointer) {
    Move move;
    rom_getmove(rom, move_id, &move);
    TEST(name, 
         format_bytes(rom[expected_data_pointer], sizeof(Move)),
         format_bytes(&move, sizeof(Move)));
}

void moverawname(std::string name, u8 move_id, int expected_data_pointer) {
    rawname_test(__FUNCTION__, name, move_id, expected_data_pointer, rom_getmoverawname);
}

void moverawname(std::string name, u8 move_id) {
    rawname_test(__FUNCTION__, name, move_id, rom_getmoverawname, rom_getmovename);
}

void movename(std::string name, u8 move_id, std::string expected_name) {
    name_test(__FUNCTION__, name, move_id, expected_name, rom_getmovename);
}

void movetm(std::string name, u8 tm, u8 expected_move) {
    u8 got_move1 = rom_gettmmove(rom, tm);
    u8 got_move2 = rom_gettmmove(rom, tm + TM01 - 1);
    TEST(name, 
         format_value(expected_move << 8 | expected_move), format_value(got_move1 << 8 | got_move2));
}

void movehm(std::string name, u8 hm, u8 expected_move) {
    u8 got_move1 = rom_gethmmove(rom, hm);
    u8 got_move2 = rom_gethmmove(rom, hm + HM01 - 1);
    TEST(name, 
         format_value(expected_move << 8 | expected_move), format_value(got_move1 << 8 | got_move2));
}

void movehighcrit(std::string name, u8 move, bool expected_high_crit) {
    bool got_high_crit = rom_ismovehighcrit(rom, move);
    TEST(name,  
         format_value(expected_high_crit), 
         format_value(got_high_crit));
}

void move_tests() {
    moveheader("pound", 0x01, 0xe4000);
    moveheader("karatechop", 0x02, 0xe4006);
    moveheader("struggle", 0xa5, 0xe43d8);
    moveheader("hexa6", 0xa6, 0xe43de);
    moveheader("hexc3", 0xc3, 0xe448c);
    moveheader("hm01", 0xc4, 0xe4492);
    moveheader("hm05", 0xc8, 0xe44aa);
    moveheader("tm01", 0xc9, 0xe44b0);
    moveheader("tm55", 0xff, 0xe45f4);
    
    moverawname("pound", 0x01,      rom.game == YELLOW ? 0x2f4000 : 0x2c4000);
    moverawname("karatechop", 0x02, rom.game == YELLOW ? 0x2f4006 : 0x2c4006);
    moverawname("struggle", 0xa5,   rom.game == YELLOW ? 0x2f4606 : 0x2c4606);
    moverawname("hm01", 0xc4);
    moverawname("hm05", 0xc8);
    moverawname("tm01", 0xc9);
    moverawname("tm55", 0xff);
    
    movename("pound", 0x01, "POUND");
    movename("karatechop", 0x02, "KARATE CHOP");
    movename("struggle", 0xa5, "STRUGGLE");
    movename("hexa6", 0xa6, "hexa6");
    movename("hexc3", 0xc3, "hexc3");
    movename("hm01", 0xc4, "HM01");
    movename("hm05", 0xc8, "HM05");
    movename("tm01", 0xc9, "TM01");
    movename("tm55", 0xff, "TM55");
    
    movetm("megapunch", 1, 0x05);
    movetm("razorwind", 2, 0x0d);
    movetm("cut", 51, 0x0f);
    movetm("fly", 52, 0x13);
    movetm("flash", 55, 0x94);
    
    movehm("cut", 1, 0x0f);
    movehm("fly", 2, 0x13);
    
    movehighcrit("pound", 0x01, false);
    movehighcrit("karatechop", 0x02, true);
    movehighcrit("hex00", 0x00, false);
    movehighcrit("hexff", 0xff, false);
}