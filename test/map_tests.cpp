void map(std::string name, u8 map_id, 
         int expected_header_pointer, 
         std::vector<int> expected_connection_pointers,
         u8 expected_border_block,
         u8 expected_warp_count,
         u8 expected_sign_count,
         u8 expected_object_count,
         u8 expected_item_count,
         u8 expected_trainer_count) {
    Map map;
    rom_getmap(rom, map_id, &map);
    
    TEST(name + "-header",
         format_bytes(rom[expected_header_pointer], sizeof(MapHeader)-1),
         format_bytes(&map.header, sizeof(MapHeader)-1));
    
    for(int i = 0; i < 4; i++) {
        int expected_connection_pointer = expected_connection_pointers[i];
        Connection expected_connection = expected_connection_pointer ?
            *((Connection *) rom[expected_connection_pointer]) : Connection {0};
        
        std::string direction;
        switch(i) {
            case 0: {
                direction = "east";
            } break;
            case 1: {
                direction = "west";
            } break;
            case 2: {
                direction = "south";
            } break;
            case 3: {
                direction = "north";
            } break;
        }
        
        TEST(name + "-" + direction + "_connection",
             format_bytes(&expected_connection, sizeof(Connection)),
             format_bytes(&map.connections[i], sizeof(Connection)));
    }
    
    TEST(name + "-borderblock",
         format_value(expected_border_block),
         format_value(map.border_block));
    
    TEST(name + "-warpcount",
         format_value(expected_warp_count),
         format_value(map.warp_count));
    
    TEST(name + "-signcount",
         format_value(expected_sign_count),
         format_value(map.sign_count));
    
    TEST(name + "-objectcount",
         format_value(expected_object_count),
         format_value(map.object_count));
    
    u8 item_count = 0;
    u8 trainer_count = 0;
    for(u8 i = 0; i < expected_object_count; i++) {
        MapObject object = map.objects[i];
        if(object.text_id & 0x40) trainer_count++;
        if(object.text_id & 0x80) item_count++;
    }
    
    TEST(name + "-itemcount",
         format_value(expected_item_count),
         format_value(item_count));
    
    TEST(name + "-trainercount",
         format_value(expected_trainer_count),
         format_value(trainer_count));
}

void mapblocks(std::string name, u8 map_id, u32 expected_crc) {
    int size = rom_getmapblocks(rom, map_id, nullptr);
    u8 blocks[size];
    rom_getmapblocks(rom, map_id, blocks);
    TEST(name,
         format_value(expected_crc),
         format_value(crc32(blocks, size)));
}

void maptiles(std::string name, u8 map_id, u32 expected_crc) {
    int size = rom_getmaptiles(rom, map_id, nullptr);
    u8 tiles[size];
    rom_getmaptiles(rom, map_id, tiles);
    TEST(name,
         format_value(expected_crc),
         format_value(crc32(tiles, size)));
}

void mappixels(std::string name, u8 map_id, u32 expected_crc) {
    int size = rom_getmappixels(rom, map_id, nullptr);
    u8 pixels[size];
    rom_getmappixels(rom, map_id, pixels);
    TEST(name,
         format_value(expected_crc),
         format_value(crc32(pixels, size)));
}

void lastmap(std::string name, u8 map_id, u8 warp_index, u8 expected_last_map) {
    TEST(name,
         format_value(expected_last_map),
         format_value(rom_getlastmap(rom, map_id, warp_index)));
}

void mapwild(std::string name, u8 map_id, u8 expected_rate, std::vector<u8> expected_slots) {
    WildData wilddata;
    rom_getmapwilddata(rom, map_id, &wilddata);
    TEST(name + "-encounterrate",
         format_value(expected_rate),
         format_value(wilddata.encounter_rate));
    for(int i = 0; i < 10; i++) {
        TEST(name + "-slot#" + std::to_string(i),
             format_value(expected_slots[i * 2 + 0] << 8 | 
                          expected_slots[i * 2 + 1]),
             format_value(wilddata.encounter_slots[i * 2 + 0] << 8 |
                          wilddata.encounter_slots[i * 2 + 1]));
    }
}

void canbike(std::string name, u8 map_id, bool expected_bike_riding) {
    TEST(name,
         format_value(expected_bike_riding),
         format_value(rom_canridebike(rom, map_id)));
}

void candig(std::string name, u8 map_id, bool expected_dig) {
    TEST(name,
         format_value(expected_dig),
         format_value(rom_candig(rom, map_id)));
}

void landcollision(std::string name, u8 map_id, u8 src_x, u8 src_y, u8 dest_x, u8 dest_y,  bool expected_collision) {
    TEST(name + "-" + 
         std::to_string(src_x) + "," + std::to_string(src_y) + "->" +
         std::to_string(dest_x) + "," + std::to_string(dest_y),
         format_value(expected_collision),
         format_value(rom_canmove(rom, map_id, src_x, src_y, dest_x, dest_y, false)));
}

void watercollision(std::string name, u8 map_id, u8 src_x, u8 src_y, u8 dest_x, u8 dest_y,  bool expected_collision) {
    TEST(name + "-" + 
         std::to_string(src_x) + "," + std::to_string(src_y) + "->" +
         std::to_string(dest_x) + "," + std::to_string(dest_y),
         format_value(expected_collision),
         format_value(rom_canmove(rom, map_id, src_x, src_y, dest_x, dest_y, true)));
}

void warp(std::string name, u8 map_id, u8 x, u8 y, bool expected_warp, u8 expected_warp_direction) {
    bool got_warp;
    u8 got_warp_direction;
    rom_warpcheck(rom, map_id, x, y, &got_warp, &got_warp_direction);
    
    std::string test_name = name + "-" + std::to_string(x) + "," + std::to_string(y);
    TEST(test_name,
         format_value(expected_warp),
         format_value(got_warp));
    TEST(test_name + "-direction",
         format_value(expected_warp_direction),
         format_value(got_warp_direction));
}

void ledgehop(std::string name, u8 map_id, u8 src_x, u8 src_y, u8 dest_x, u8 dest_y,  bool expected_hop) {
    TEST(name + "-" + 
         std::to_string(src_x) + "," + std::to_string(src_y) + "->" +
         std::to_string(dest_x) + "," + std::to_string(dest_y),
         format_value(expected_hop),
         format_value(rom_ledgehopcheck(rom, map_id, src_x, src_y, dest_x, dest_y)));
}

void map_tests() {
    map("pallettown", 0, 
        0x642a1, 
        { 0, 0, 0x642ab, 0x642b6 }, 
        0xb, 
        3,
        4,
        3,
        0,
        0);
    
    map("ceruleancity", 3, 
        rom.game == YELLOW ? 0x64754 : 0x6474e,
        rom.game == YELLOW ? 
        std::vector<int> { 0x6475e, 0x64769, 0x64774, 0x6477f } :
        std::vector<int> { 0x64758, 0x64763, 0x6476e, 0x64779 },
        rom.game == YELLOW ? 0xa : 0xf, 
        0xa,
        6,
        11,
        0,
        1);
    
    map("mtmoon1f", 59, 
        rom.game == YELLOW ? 0x125953 : 0x1259bc,
        { 0, 0, 0, 0 },
        0x3, 
        5,
        1,
        13,
        6,
        7);
    
    mapblocks("pallettown", 0, 1935553529);
    maptiles("pallettown", 0, 3461947911);
    mappixels("pallettown", 0, rom.game == YELLOW ? 1761811878 : 381491816);
    
    lastmap("mtmoon1f", 59, 0, 15);
    lastmap("redshouse1f", 37, 0, 0);
    if(rom.game == YELLOW) lastmap("beachhouse", 248, 0, 30);
    
    mapwild("pallettown", 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
    
    mapwild("route1", 12, 25, 
            rom.game == YELLOW ? 
            std::vector<u8> { 3, 0x24, 4, 0x24, 2, 0xa5, 3, 0xa5, 2, 0x24, 3, 0x24, 5, 0x24, 4, 0xa5, 6, 0x24, 7, 0x24 } :
            std::vector<u8> { 3, 0x24, 3, 0xa5, 3, 0xa5, 2, 0xa5, 2, 0x24, 3, 0x24, 3, 0x24, 4, 0xa5, 4, 0x24, 5, 0x24 });
    
    std::vector<u8> route22WildData;
    switch(rom.game) {
        case RED: {
            route22WildData = { 3, 0xa5, 3, 0x03, 4, 0xa5, 4, 0x03, 2, 0xa5, 2, 0x03, 3, 0x05, 5, 0x05, 3, 0x0f, 4, 0x0f };
        } break;
        case BLUE: {
            route22WildData = { 3, 0xa5, 3, 0x0f, 4, 0xa5, 4, 0x0f, 2, 0xa5, 2, 0x0f, 3, 0x05, 5, 0x05, 3, 0x03, 4, 0x03 };
        } break;
        case YELLOW: {
            route22WildData = { 2, 0x03, 2, 0x0f, 3, 0x39, 3, 0xa5, 4, 0x03, 4, 0x0f, 5, 0x39, 2, 0x05, 4, 0x05, 6, 0x05 };
        } break;
    }
    mapwild("route22", 33, 25, route22WildData);
    
    canbike("pallettown", 0, true);
    canbike("route23", 34, true);
    canbike("indigoplateau", 9, true);
    canbike("gatehouse", 47, false);
    
    candig("pallettown", 0, false);
    candig("viridianforest", 51, true);
    candig("billshouse", 88, rom.game == YELLOW ? false : true);
    
    landcollision("mtmoonb2f", 61, 20, 17, 20, 16, false);
    landcollision("mtmoonb2f", 61, 20, 17, 21, 17, false);
    landcollision("mtmoonb2f", 61, 20, 17, 19, 17, true);
    landcollision("mtmoonb2f", 61, 28, 12, 28, 11, true);
    landcollision("mtmoonb2f", 61, 28, 11, 28, 12, true);
    
    watercollision("cinnabar", 8, 1, 0, 2, 0, false);
    watercollision("cinnabar", 8, 1, 0, 0, 0, true);
    watercollision("cinnabar", 8, 3, 4, 4, 4, false);
    watercollision("cinnabar", 8, 1, 0, 0, 0, true);
    
    warp("ssanne", 95, 2, 6, true, 0);
    warp("ssanne", 95, 3, 6, false, 0);
    warp("ssanne", 95, 26, 0, true, 0x40);
    warp("ssanne", 95, 27, 0, true, 0x40);
    warp("redshouse1f", 37, 7, 1, true, 0x00);
    warp("redshouse1f", 37, 2, 7, true, 0x80);
    warp("redshouse1f", 37, 3, 7, true, 0x80);
    
    ledgehop("route1", 12, 4, 4, 4, 5, true);
    ledgehop("route1", 12, 10, 26, 10, 27, true);
    ledgehop("route1", 12, 9, 18, 9, 19, false);
    ledgehop("route4", 15, 44, 8, 45, 8, true);
    ledgehop("route4", 15, 51, 8, 50, 8, true);
}