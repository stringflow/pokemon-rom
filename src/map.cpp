struct Connection {
    u8 map_id;
    u16 source;
    u16 destination;
    u8 length;
    u8 width;
    u8 alignment_y;
    u8 alignment_x;
    u16 window;
};

struct Warp {
    u8 y;
    u8 x;
    u8 destination_index;
    u8 destination_map;
};

struct Sign {
    u8 y;
    u8 x;
    u8 sign_id;
};

struct MapObject {
    u8 picture_id;
    u8 y;
    u8 x;
    u8 movement;
    u8 range_or_direction;
    u8 text_id;
    u8 var1;
    u8 var2;
};

struct MapHeader {
    u8 tileset;
    u8 height;
    u8 width;
    u16 block_pointer;
    u16 text_pointer;
    u16 script_pointer;
    u8 connection_flags;
    // NOTE(stringflow): The bank is not part of the map header struct in the game's data!!!
    // I have only added it to the struct because it is essential to know to address the
    // various pointers above. Anywhere you see "sizeof(MapHeader)-1", it is to account for
    // this additional byte.
    u8 bank; 
};

struct Map {
    MapHeader header;
    Connection connections[4];
    u16 objects_pointer;
    u8 border_block;
    u8 warp_count;
    Warp warps[32];
    u8 sign_count;
    Sign signs[16];
    u8 object_count;
    MapObject objects[16];
};

struct WildData {
    u8 encounter_rate;
    u8 encounter_slots[20];
};

int get_map_header_pointer(ROM& rom, u8 map_id) {
    u8 bank = rom["MapHeaderBanks"][map_id];
    u16 address = ((u16 *) rom["MapHeaderPointers"])[map_id];
    return bank << 16 | address;
}

// Writes the map header data. This does not include: map connections, border block,
// warps, signs, and map objects.
DLLEXPORT void rom_getmapheader(ROM& rom, u8 map_id, MapHeader *map_header) {
    int pointer = get_map_header_pointer(rom, map_id);
    memcpy(map_header, rom[pointer], sizeof(MapHeader)-1);
    map_header->bank = pointer >> 16;
}

// Writes the entire map header and map objects data.
DLLEXPORT void rom_getmap(ROM& rom, u8 map_id, Map *map) {
    rom_getmapheader(rom, map_id, &map->header);
    
    u8 *header_data = rom[get_map_header_pointer(rom, map_id) + sizeof(MapHeader)-1];
    for(int i = 0; i < 4; i++) {
        if((map->header.connection_flags & (1 << i)) != 0) {
            memcpy(&map->connections[i], header_data, sizeof(Connection));
            header_data += sizeof(Connection);
        } else {
            map->connections[i] = {0};
        }
    }
    map->objects_pointer = *((u16 *) header_data);
    
    u8 *objects_data = rom[map->header.bank << 16 | map->objects_pointer];
    map->border_block = *objects_data++;
    map->warp_count = *objects_data++;
    for(u8 warp_index = 0; warp_index < map->warp_count; warp_index++) {
        memcpy(&map->warps[warp_index], objects_data, sizeof(Warp));
        objects_data += sizeof(Warp);
    }
    map->sign_count = *objects_data++;
    for(u8 sign_index = 0; sign_index < map->sign_count; sign_index++) {
        memcpy(&map->signs[sign_index], objects_data, sizeof(Sign));
        objects_data += sizeof(Sign);
    }
    map->object_count = *objects_data++;
    for(u8 object_index = 0; object_index < map->object_count; object_index++) {
        MapObject *object = &map->objects[object_index];
        memcpy(object, objects_data, sizeof(MapObject));
        objects_data += sizeof(MapObject);
        // TODO(stringflow): should this be -4'ed or not?
        object->y -= 4;
        object->x -= 4;
        if(!(object->text_id & 0x40)) {
            object->var2 = 0;
            objects_data--;
            if(!(object->text_id & 0x80)) {
                object->var1 = 0;
                objects_data--;
            }
        }
    }
}

// Copies the number of blocks from the blockPointer to a destination buffer.
DLLEXPORT void rom_copyblocks(ROM& rom, int block_pointer, int block_count, u8 *dest) {
    memcpy(dest, rom[block_pointer], block_count);
}

// Writes the blocks making up the map to the destination buffer. The size of the buffer
// should be the: mapwidth (in blocks) * mapheight (in blocks).
// Returns the map size in blocks.
// If you do not know the map dimensions, pass zero as the destination and use the return
// value to tell you how big the buffer should be.
DLLEXPORT int rom_getmapblocks(ROM& rom, u8 map_id, u8 *dest) {
    MapHeader map;
    rom_getmapheader(rom, map_id, &map);
    int map_size = map.width * map.height;
    if(dest) rom_copyblocks(rom, map.bank << 16 | map.block_pointer, map_size, dest);
    return map_size;
}

// Converts a block array into a tile array. The size of the destination buffer should be:
// blockCount * 16.
DLLEXPORT void rom_blockstotiles(ROM& rom, u8 *blocks, int block_count, u8 width, int block_pointer, u8 *dest) {
    int length = block_count - block_count % width;
    for(int i = 0; i < length; i++) {
        u8 *tiles = rom[block_pointer + blocks[i] * 16];
        for(int j = 0; j < 4; j++) {
            int src_offset = j * 4;
            int dest_offset = (i / width) * (width * 4) * 4 + j * (width * 4) + (i % width) * 4;
            memcpy(dest + dest_offset, tiles + src_offset, 4);
        }
    }
}

// Writes the tiles making up the map to the destination buffer. The size of the buffer
// should be the: mapwidth (in blocks) * mapheight (in blocks) * 16.
// Returns the map size in tiles.
// If you do not know the map dimensions, pass zero as the destination and use the return
// value to tell you how big the buffer should be.
DLLEXPORT int rom_getmaptiles(ROM& rom, u8 map_id, u8 *dest) {
    MapHeader map;
    rom_getmapheader(rom, map_id, &map);
    int map_size = map.width * map.height;
    
    if(dest) {
        Tileset tileset;
        rom_gettileset(rom, map.tileset, &tileset);
        u8 blocks[map_size];
        rom_getmapblocks(rom, map_id, blocks);
        rom_blockstotiles(rom,
                          blocks,
                          map_size,
                          map.width,
                          tileset.bank << 16 | tileset.block_pointer,
                          dest);
    }
    
    return map_size * 16;
}

// Converts a tile array into an array of 2bpp pixels. The size of the destination buffer
// should be: tileCount * 16.
DLLEXPORT void rom_tilestopixels(ROM& rom, u8 *tiles, int tile_count, int gfx_pointer, u8 *dest) {
    for(int i = 0; i < tile_count; i++) {
        memcpy(dest + i * 16, rom[gfx_pointer + tiles[i] * 16], 16);
    }
}

// Writes the 2bpp representation of the map tiles to the destination buffer.
// The size of the buffer should be: mapwidth (in blocks) * mapheight (in blocks) * 256.
// Returns the map size in pixels.
// If you do not know the map dimensions, pass zero as the destination and use the return
// value to tell you how big the buffer should be.
DLLEXPORT int rom_getmappixels(ROM& rom, u8 map_id, u8 *dest) {
    MapHeader map;
    rom_getmapheader(rom, map_id, &map);
    int map_size = map.width * map.height * 16;
    
    if(dest) {
        u8 tiles[map_size];
        Tileset tileset; 
        rom_gettileset(rom, map.tileset, &tileset);
        rom_getmaptiles(rom, map_id, tiles);
        rom_tilestopixels(rom,
                          tiles,
                          map_size,
                          tileset.bank << 16 | tileset.gfx_pointer,
                          dest);
    }
    
    return map_size * 16;
}

// Writes the pictureId's 2bpp representation to a 64 size buffer.
// Different versions of the sprite can be indexed through the index parameter, for typical
// overworld sprites the versions would be:
//   (0) facing down
//   (1) facing up
//   (2) facing left
// The sprite will be vertically flipped if the flag is set, making sprites that are
// facing right possible.
DLLEXPORT void rom_getspritepixels(ROM& rom, u8 picture_id, u8 index, bool flip, u8 *dest) {
    u8 *spritesheet = rom["SpriteSheetPointerTable"] + (u8) (picture_id - 1) * 4;
    u8 *sprite_pointer = rom[*(spritesheet + 3) << 16 | *((u16 *) spritesheet)];
    memcpy(dest, sprite_pointer + index * 64, 64);
    
    if(flip) {
        for(int row = 0; row < 2; row++) {
            for(int i = 0; i < 16; i++) {
                int src_index = row * 32 + i;
                int dest_index = src_index + 16;
                u8 temp = dest[dest_index];
                dest[dest_index] = reverse_bits(dest[src_index]);
                dest[src_index] = reverse_bits(temp);
            }
        }
    }
}

// Writes the map object's 2bpp representation to a 64 size buffer.
// The direction the sprite will be facing is inferred from the object's data, objects that
// move around will default to facing downwards.
DLLEXPORT void rom_getmapobjectpixels(ROM& rom, u8 picture_id, u8 direction, u8 *dest) {
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
    
    rom_getspritepixels(rom, picture_id, index, flip, dest);
}

// Returns the bottom left tile of at the given X and Y coordinate. If out of bounds
// coordinates are passed, the border block will be used.
DLLEXPORT u8 rom_getmaptile(ROM& rom, u8 map_id, int x, int y) {
    Map map;
    rom_getmap(rom, map_id, &map);
    
    Tileset tileset;
    rom_gettileset(rom, map.header.tileset, &tileset);
    
    u8 block;
    if(x < 0 || y < 0 || x >= map.header.width * 2 || y >= map.header.height * 2) {
        block = map.border_block;
    } else {
        int block_index = x / 2 + y / 2 * map.header.width;
        block = *rom[map.header.bank << 16 | map.header.block_pointer + block_index];
    }
    
    int block_x = x & 1;
    int block_y = y & 1;
    int tile_index = block_x * 2 + block_y * 8 + 4;
    return *rom[tileset.bank << 16 | tileset.block_pointer + block * 16 + tile_index];
}

// Returns the actual destination map that a wLastMap using warp brings you to.
DLLEXPORT u8 rom_getlastmap(ROM& rom, u8 map_id, u8 warp_index) {
    u16 key = map_id << 8 | warp_index;
    std::map<u16, u8> map = (rom.game == YELLOW ? 
                             yellow_last_map_destinations :
                             rb_last_map_destinations);
    return map.count(key) ? map[key] : 0xff;
}

// Fills out the supplied WildData struct with the map's encounter data.
// The slots are in sequential order from most common to least common.
// Each slot occupies 2 bytes, the first byte is equal to the pokemon's level, 
// the second byte is equal to the species id.
DLLEXPORT void rom_getmapwilddata(ROM& rom, u8 map_id, WildData *dest) {
    int pointer = 0x30000 | ((u16 *) rom["WildDataPointers"])[map_id];
    if(pointer == rom.symbols["NothingWildMons"]) {
        dest->encounter_rate = 0;
        memset(dest->encounter_slots, 0, 20);
    } else {
        u8 *data = rom[pointer];
        dest->encounter_rate = *data++;
        memcpy(dest->encounter_slots, data, 20);
    }
}

// Returns if riding the bike is allowed on the map.
DLLEXPORT bool rom_canridebike(ROM& rom, u8 map_id) {
    if(map_id == 0x22 || map_id == 0x09) return true;
    
    MapHeader map;
    rom_getmapheader(rom, map_id, &map);
    return in_list(rom["BikeRidingTilesets"], map.tileset, 0xff);
}

// Returns if digging or using an escape rope is allowed on the map.
DLLEXPORT bool rom_candig(ROM& rom, u8 map_id) {
    if(map_id == 0xf7) return false;
    if(map_id == 0x58 && rom.game == YELLOW) return false;
    if(map_id == 0x5a && rom.game == YELLOW) return false;
    
    MapHeader map;
    rom_getmapheader(rom, map_id, &map);
    return in_list(rom["EscapeRopeTilesets"], map.tileset, 0xff);
}