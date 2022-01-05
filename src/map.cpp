struct Connection {
    u8 mapId;
    u16 source;
    u16 destination;
    u8 length;
    u8 width;
    u8 yAlignment;
    u8 xAlignment;
    u16 window;
};

struct Warp {
    u8 y;
    u8 x;
    u8 destinationIndex;
    u8 destinationMap;
};

struct Sign {
    u8 y;
    u8 x;
    u8 signId;
};

struct MapObject {
    u8 pictureId;
    u8 y;
    u8 x;
    u8 movement;
    u8 rangeOrDirection;
    u8 textId;
    u8 var1;
    u8 var2;
};

struct MapHeader {
    u8 tileset;
    u8 height;
    u8 width;
    u16 blockPointer;
    u16 textPointer;
    u16 scriptPointer;
    u8 connectionFlags;
    // NOTE(stringflow): The bank is not part of the map header struct in the game's data!!!
    // I have only added it to the struct because it is essential to know to address the
    // various pointers above. Anywhere you see "sizeof(MapHeader)-1", it is to account for
    // this additional byte.
    u8 bank; 
};

struct Map {
    MapHeader header;
    Connection connections[4];
    u16 objectsPointer;
    u8 borderBlock;
    u8 warpCount;
    Warp warps[32];
    u8 signCount;
    Sign signs[16];
    u8 objectCount;
    MapObject objects[16];
};

struct WildData {
    u8 encounterRate;
    u8 encounterSlots[20];
};

int getMapHeaderPointer(ROM& rom, u8 mapId) {
    u8 bank = rom["MapHeaderBanks"][mapId];
    u16 address = ((u16 *) rom["MapHeaderPointers"])[mapId];
    return bank << 16 | address;
}

// Writes the map header data. This does not include: map connections, border block,
// warps, signs, and map objects.
DLLEXPORT void rom_getmapheader(ROM& rom, u8 mapId, MapHeader *mapheader) {
    int pointer = getMapHeaderPointer(rom, mapId);
    memcpy(mapheader, rom[pointer], sizeof(MapHeader)-1);
    mapheader->bank = pointer >> 16;
}

// Writes the entire map header and map objects data.
DLLEXPORT void rom_getmap(ROM& rom, u8 mapId, Map *map) {
    rom_getmapheader(rom, mapId, &map->header);
    
    u8 *headerData = rom[getMapHeaderPointer(rom, mapId) + sizeof(MapHeader)-1];
    for(int i = 0; i < 4; i++) {
        if((map->header.connectionFlags & (1 << i)) != 0) {
            memcpy(&map->connections[i], headerData, sizeof(Connection));
            headerData += sizeof(Connection);
        } else {
            map->connections[i] = {0};
        }
    }
    map->objectsPointer = *((u16 *) headerData);
    
    u8 *objectsData = rom[map->header.bank << 16 | map->objectsPointer];
    map->borderBlock = *objectsData++;
    map->warpCount = *objectsData++;
    for(u8 warpIndex = 0; warpIndex < map->warpCount; warpIndex++) {
        memcpy(&map->warps[warpIndex], objectsData, sizeof(Warp));
        objectsData += sizeof(Warp);
    }
    map->signCount = *objectsData++;
    for(u8 signIndex = 0; signIndex < map->signCount; signIndex++) {
        memcpy(&map->signs[signIndex], objectsData, sizeof(Sign));
        objectsData += sizeof(Sign);
    }
    map->objectCount = *objectsData++;
    for(u8 objectIndex = 0; objectIndex < map->objectCount; objectIndex++) {
        MapObject *object = &map->objects[objectIndex];
        memcpy(object, objectsData, sizeof(MapObject));
        objectsData += sizeof(MapObject);
        // TODO(stringflow): should this be -4'ed or not?
        object->y -= 4;
        object->x -= 4;
        if(!(object->textId & 0x40)) {
            object->var2 = 0;
            objectsData--;
            if(!(object->textId & 0x80)) {
                object->var1 = 0;
                objectsData--;
            }
        }
    }
}

// Copies the number of blocks from the blockPointer to a destination buffer.
DLLEXPORT void rom_copyblocks(ROM& rom, int blockPointer, int blockCount, u8 *dest) {
    memcpy(dest, rom[blockPointer], blockCount);
}

// Writes the blocks making up the map to the destination buffer. The size of the buffer
// should be the: mapwidth (in blocks) * mapheight (in blocks).
// Returns the map size in blocks.
// If you do not know the map dimensions, pass zero as the destination and use the return
// value to tell you how big the buffer should be.
DLLEXPORT int rom_getmapblocks(ROM& rom, u8 mapId, u8 *dest) {
    MapHeader map;
    rom_getmapheader(rom, mapId, &map);
    int mapSize = map.width * map.height;
    if(dest) rom_copyblocks(rom, map.bank << 16 | map.blockPointer, mapSize, dest);
    return mapSize;
}

// Converts a block array into a tile array. The size of the destination buffer should be:
// blockCount * 16.
DLLEXPORT void rom_blockstotiles(ROM& rom, u8 *blocks, int blockCount, u8 width, int blockPointer, u8 *dest) {
    int length = blockCount - blockCount % width;
    for(int i = 0; i < length; i++) {
        u8 *tiles = rom[blockPointer + blocks[i] * 16];
        for(int j = 0; j < 4; j++) {
            int srcOffset = j * 4;
            int destOffset = (i / width) * (width * 4) * 4 + j * (width * 4) + (i % width) * 4;
            memcpy(dest + destOffset, tiles + srcOffset, 4);
        }
    }
}

// Writes the tiles making up the map to the destination buffer. The size of the buffer
// should be the: mapwidth (in blocks) * mapheight (in blocks) * 16.
// Returns the map size in tiles.
// If you do not know the map dimensions, pass zero as the destination and use the return
// value to tell you how big the buffer should be.
DLLEXPORT int rom_getmaptiles(ROM& rom, u8 mapId, u8 *dest) {
    MapHeader map;
    rom_getmapheader(rom, mapId, &map);
    int mapSize = map.width * map.height;
    
    if(dest) {
        Tileset tileset;
        rom_gettileset(rom, map.tileset, &tileset);
        u8 blocks[mapSize];
        rom_getmapblocks(rom, mapId, blocks);
        rom_blockstotiles(rom,
                          blocks,
                          mapSize,
                          map.width,
                          tileset.bank << 16 | tileset.blockPointer,
                          dest);
    }
    
    return mapSize * 16;
}

// Converts a tile array into an array of 2bpp pixels. The size of the destination buffer
// should be: tileCount * 16.
DLLEXPORT void rom_tilestopixels(ROM& rom, u8 *tiles, int tileCount, int gfxPointer, u8 *dest) {
    for(int i = 0; i < tileCount; i++) {
        memcpy(dest + i * 16, rom[gfxPointer + tiles[i] * 16], 16);
    }
}

// Writes the 2bpp representation of the map tiles to the destination buffer.
// The size of the buffer should be: mapwidth (in blocks) * mapheight (in blocks) * 256.
// Returns the map size in pixels.
// If you do not know the map dimensions, pass zero as the destination and use the return
// value to tell you how big the buffer should be.
DLLEXPORT int rom_getmappixels(ROM& rom, u8 mapId, u8 *dest) {
    MapHeader map;
    rom_getmapheader(rom, mapId, &map);
    int mapSize = map.width * map.height * 16;
    
    if(dest) {
        u8 tiles[mapSize];
        Tileset tileset; 
        rom_gettileset(rom, map.tileset, &tileset);
        rom_getmaptiles(rom, mapId, tiles);
        rom_tilestopixels(rom,
                          tiles,
                          mapSize,
                          tileset.bank << 16 | tileset.gfxPointer,
                          dest);
    }
    
    return mapSize * 16;
}

// Writes the pictureId's 2bpp representation to a 64 size buffer.
// Different versions of the sprite can be indexed through the index parameter, for typical
// overworld sprites the versions would be:
//   (0) facing down
//   (1) facing up
//   (2) facing left
// The sprite will be vertically flipped if the flag is set, making sprites that are
// facing right possible.
DLLEXPORT void rom_getspritepixels(ROM& rom, u8 pictureId, u8 index, bool flip, u8 *dest) {
    u8 *spritesheet = rom["SpriteSheetPointerTable"] + (u8)(pictureId - 1) * 4;
    u8 *spritePointer = rom[*(spritesheet + 3) << 16 | *((u16 *) spritesheet)];
    memcpy(dest, spritePointer + index * 64, 64);
    
    if(flip) {
        for(int row = 0; row < 2; row++) {
            for(int i = 0; i < 16; i++) {
                int srcIndex = row * 32 + i;
                int destIndex = srcIndex + 16;
                u8 temp = dest[destIndex];
                dest[destIndex] = reverseBits(dest[srcIndex]);
                dest[srcIndex] = reverseBits(temp);
            }
        }
    }
}

// Writes the map object's 2bpp representation to a 64 size buffer.
// The direction the sprite will be facing is inferred from the object's data, objects that
// move around will default to facing downwards.
DLLEXPORT void rom_getmapobjectpixels(ROM& rom, u8 pictureId, u8 direction, u8 *dest) {
    u8 index = 0;
    bool flip = false;
    switch(direction) {
        case 0xd1: {
            index = 1;
        } break;
        case 0xd2: {
            index = 2;
        } break;
        case 0xd3: {
            index = 2;
            flip = true;
        } break;
    }
    
    rom_getspritepixels(rom, pictureId, index, flip, dest);
}

// Returns the bottom left tile of at the given X and Y coordinate. If out of bounds
// coordinates are passed, the border block will be used.
DLLEXPORT u8 rom_getmaptile(ROM& rom, u8 mapId, int x, int y) {
    Map map;
    rom_getmap(rom, mapId, &map);
    
    Tileset tileset;
    rom_gettileset(rom, map.header.tileset, &tileset);
    
    u8 block;
    if(x < 0 || y < 0 || x >= map.header.width * 2 || y >= map.header.height * 2) {
        block = map.borderBlock;
    } else {
        int blockIndex = x / 2 + y / 2 * map.header.width;
        block = *rom[map.header.bank << 16 | map.header.blockPointer + blockIndex];
    }
    
    int xBlock = x & 1;
    int yBlock = y & 1;
    int tileIndex = xBlock * 2 + yBlock * 8 + 4;
    return *rom[tileset.bank << 16 | tileset.blockPointer + block * 16 + tileIndex];
}

// Returns the actual destination map that a wLastMap using warp brings you to.
DLLEXPORT u8 rom_getlastmap(ROM& rom, u8 mapId, u8 warpIndex) {
    u16 key = mapId << 8 | warpIndex;
    std::map<u16, u8> map = rom.game == Yellow ? yLastMapDestinations : rbLastMapDestinations;
    return map.count(key) ? map[key] : 0xff;
}

// Fills out the supplied WildData struct with the map's encounter data.
// The slots are in sequential order from most common to least common.
// Each slot occupies 2 bytes, the first byte is equal to the pokemon's level, 
// the second byte is equal to the species id.
DLLEXPORT void rom_getmapwilddata(ROM& rom, u8 mapId, WildData *dest) {
    int pointer = 0x30000 | ((u16 *) rom["WildDataPointers"])[mapId];
    if(pointer == rom.symbols["NothingWildMons"]) {
        dest->encounterRate = 0;
        memset(dest->encounterSlots, 0, 20);
    } else {
        u8 *data = rom[pointer];
        dest->encounterRate = *data++;
        memcpy(dest->encounterSlots, data, 20);
    }
}

// Returns if riding the bike is allowed on the map.
DLLEXPORT bool rom_canridebike(ROM& rom, u8 mapId) {
    if(mapId == 0x22 || mapId == 0x09) return true;
    
    MapHeader map;
    rom_getmapheader(rom, mapId, &map);
    return inList(rom["BikeRidingTilesets"], map.tileset, 0xff);
}

// Returns if digging or using an escape rope is allowed on the map.
DLLEXPORT bool rom_candig(ROM& rom, u8 mapId) {
    if(mapId == 0xf7) return false;
    if(mapId == 0x58 && rom.game == Yellow) return false;
    if(mapId == 0x5a && rom.game == Yellow) return false;
    
    MapHeader map;
    rom_getmapheader(rom, mapId, &map);
    return inList(rom["EscapeRopeTilesets"], map.tileset, 0xff);
}