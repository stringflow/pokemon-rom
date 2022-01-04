void map(std::string name, u8 mapId, 
         int expectedHeaderPointer, 
         std::vector<int> expectedConnectionPointers,
         u8 expectedBorderBlock,
         u8 expectedWarpCount,
         u8 expectedSignCount,
         u8 expectedObjectCount,
         u8 expectedItemCount,
         u8 expectedTrainerCount) {
    Map map;
    rom_getmap(rom, mapId, &map);
    
    TEST(name + "-header",
         formatBytes(rom[expectedHeaderPointer], sizeof(MapHeader)-1),
         formatBytes(&map.header, sizeof(MapHeader)-1));
    
    for(int i = 0; i < 4; i++) {
        int expectedConnectionPointer = expectedConnectionPointers[i];
        Connection expectedConnection = expectedConnectionPointer ?
            *((Connection *) rom[expectedConnectionPointer]) : Connection {0};
        
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
             formatBytes(&expectedConnection, sizeof(Connection)),
             formatBytes(&map.connections[i], sizeof(Connection)));
    }
    
    TEST(name + "-borderblock",
         formatValue(expectedBorderBlock),
         formatValue(map.borderBlock));
    
    TEST(name + "-warpcount",
         formatValue(expectedWarpCount),
         formatValue(map.warpCount));
    
    TEST(name + "-signcount",
         formatValue(expectedSignCount),
         formatValue(map.signCount));
    
    TEST(name + "-objectcount",
         formatValue(expectedObjectCount),
         formatValue(map.objectCount));
    
    u8 itemCount = 0;
    u8 trainerCount = 0;
    for(u8 i = 0; i < expectedObjectCount; i++) {
        MapObject object = map.objects[i];
        if(object.textId & 0x40) trainerCount++;
        if(object.textId & 0x80) itemCount++;
    }
    
    TEST(name + "-itemcount",
         formatValue(expectedItemCount),
         formatValue(itemCount));
    
    TEST(name + "-trainercount",
         formatValue(expectedTrainerCount),
         formatValue(trainerCount));
}

void mapblocks(std::string name, u8 mapId, u32 expectedCrc) {
    int size = rom_getmapblocks(rom, mapId, nullptr);
    u8 blocks[size];
    rom_getmapblocks(rom, mapId, blocks);
    TEST(name,
         formatValue(expectedCrc),
         formatValue(crc32(blocks, size)));
}

void maptiles(std::string name, u8 mapId, u32 expectedCrc) {
    int size = rom_getmaptiles(rom, mapId, nullptr);
    u8 tiles[size];
    rom_getmaptiles(rom, mapId, tiles);
    TEST(name,
         formatValue(expectedCrc),
         formatValue(crc32(tiles, size)));
}

void mappixels(std::string name, u8 mapId, u32 expectedCrc) {
    int size = rom_getmappixels(rom, mapId, nullptr);
    u8 pixels[size];
    rom_getmappixels(rom, mapId, pixels);
    TEST(name,
         formatValue(expectedCrc),
         formatValue(crc32(pixels, size)));
}

void lastmap(std::string name, u8 mapId, u8 warpIndex, u8 expectedLastMap) {
    TEST(name,
         formatValue(expectedLastMap),
         formatValue(rom_getlastmap(rom, mapId, warpIndex)));
}

void mapwild(std::string name, u8 mapId, u8 expectedRate, std::vector<u8> expectedSlots) {
    WildData wilddata;
    rom_getmapwilddata(rom, mapId, &wilddata);
    TEST(name + "-encounterrate",
         formatValue(expectedRate),
         formatValue(wilddata.encounterRate));
    for(int i = 0; i < 10; i++) {
        TEST(name + "-slot#" + std::to_string(i),
             formatValue(expectedSlots[i * 2 + 0] << 8 | expectedSlots[i * 2 + 1]),
             formatValue(wilddata.encounterSlots[i * 2 + 0] << 8 |
                         wilddata.encounterSlots[i * 2 + 1]));
    }
}

void canbike(std::string name, u8 mapId, bool expectedBikeRiding) {
    TEST(name,
         formatValue(expectedBikeRiding),
         formatValue(rom_canridebike(rom, mapId)));
}

void candig(std::string name, u8 mapId, bool expectedDig) {
    TEST(name,
         formatValue(expectedDig),
         formatValue(rom_candig(rom, mapId)));
}

void landcollision(std::string name, u8 mapId, u8 xSrc, u8 ySrc, u8 xDest, u8 yDest,  bool expectedCollision) {
    TEST(name + "-" + 
         std::to_string(xSrc) + "," + std::to_string(ySrc) + "->" +
         std::to_string(xDest) + "," + std::to_string(yDest),
         formatValue(expectedCollision),
         formatValue(rom_canmove(rom, mapId, xSrc, ySrc, xDest, yDest, false)));
}

void watercollision(std::string name, u8 mapId, u8 xSrc, u8 ySrc, u8 xDest, u8 yDest,  bool expectedCollision) {
    TEST(name + "-" + 
         std::to_string(xSrc) + "," + std::to_string(ySrc) + "->" +
         std::to_string(xDest) + "," + std::to_string(yDest),
         formatValue(expectedCollision),
         formatValue(rom_canmove(rom, mapId, xSrc, ySrc, xDest, yDest, true)));
}

void warp(std::string name, u8 mapId, u8 x, u8 y, bool expectedWarp, u8 expectedWarpDirection) {
    bool gotWarp;
    u8 gotWarpDirection;
    rom_warpcheck(rom, mapId, x, y, &gotWarp, &gotWarpDirection);
    
    std::string testName = name + "-" + std::to_string(x) + "," + std::to_string(y);
    TEST(testName,
         formatValue(expectedWarp),
         formatValue(gotWarp));
    TEST(testName + "-direction",
         formatValue(expectedWarpDirection),
         formatValue(gotWarpDirection));
}

void ledgehop(std::string name, u8 mapId, u8 xSrc, u8 ySrc, u8 xDest, u8 yDest,  bool expectedHop) {
    TEST(name + "-" + 
         std::to_string(xSrc) + "," + std::to_string(ySrc) + "->" +
         std::to_string(xDest) + "," + std::to_string(yDest),
         formatValue(expectedHop),
         formatValue(rom_ledgehopcheck(rom, mapId, xSrc, ySrc, xDest, yDest)));
}

void mapTests() {
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
        rom.game == Yellow ? 0x64754 : 0x6474e,
        rom.game == Yellow ? 
        std::vector<int> { 0x6475e, 0x64769, 0x64774, 0x6477f } :
        std::vector<int> { 0x64758, 0x64763, 0x6476e, 0x64779 },
        rom.game == Yellow ? 0xa : 0xf, 
        0xa,
        6,
        11,
        0,
        1);
    
    map("mtmoon1f", 59, 
        rom.game == Yellow ? 0x125953 : 0x1259bc,
        { 0, 0, 0, 0 },
        0x3, 
        5,
        1,
        13,
        6,
        7);
    
    mapblocks("pallettown", 0, 1935553529);
    maptiles("pallettown", 0, 3461947911);
    mappixels("pallettown", 0, rom.game == Yellow ? 1761811878 : 381491816);
    
    lastmap("mtmoon1f", 59, 0, 15);
    lastmap("redshouse1f", 37, 0, 0);
    if(rom.game == Yellow) lastmap("beachhouse", 248, 0, 30);
    
    mapwild("pallettown", 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 });
    
    mapwild("route1", 12, 25, 
            rom.game == Yellow ? 
            std::vector<u8> { 3, 0x24, 4, 0x24, 2, 0xa5, 3, 0xa5, 2, 0x24, 3, 0x24, 5, 0x24, 4, 0xa5, 6, 0x24, 7, 0x24 } :
            std::vector<u8> { 3, 0x24, 3, 0xa5, 3, 0xa5, 2, 0xa5, 2, 0x24, 3, 0x24, 3, 0x24, 4, 0xa5, 4, 0x24, 5, 0x24 });
    
    std::vector<u8> route22WildData;
    switch(rom.game) {
        case Red: {
            route22WildData = { 3, 0xa5, 3, 0x03, 4, 0xa5, 4, 0x03, 2, 0xa5, 2, 0x03, 3, 0x05, 5, 0x05, 3, 0x0f, 4, 0x0f };
        } break;
        case Blue: {
            route22WildData = { 3, 0xa5, 3, 0x0f, 4, 0xa5, 4, 0x0f, 2, 0xa5, 2, 0x0f, 3, 0x05, 5, 0x05, 3, 0x03, 4, 0x03 };
        } break;
        case Yellow: {
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
    candig("billshouse", 88, rom.game == Yellow ? false : true);
    
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