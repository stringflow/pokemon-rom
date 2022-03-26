struct Tileset {
    u8 bank;
    u16 block_pointer;
    u16 gfx_pointer;
    u16 collision_pointer;
    u8 counter_tiles[3];
    u8 grass_tile;
    u8 animations;
};

struct TilePairCollision {
    u8 tileset;
    u8 tile1;
    u8 tile2;
};

struct Ledge {
    u8 player_direction;
    u8 player_tile;
    u8 ledge_tile;
    u8 input_required;
};

// Writes the tileset's header data.
DLLEXPORT void rom_gettileset(ROM& rom, u8 tileset_id, Tileset *tileset) {
    u8 *data = rom["Tilesets"] + tileset_id * sizeof(Tileset);
    memcpy(tileset, data, sizeof(Tileset));
}

// Returns if moving from tile 1 to tile 2 results in a tile pair collision.
// Used for simulate differences in elevation.
DLLEXPORT bool rom_istilepaircollision(ROM& rom, u8 tileset_id, u8 tile1, u8 tile2,
                                       bool water) {
    std::string label = water ? "TilePairCollisionsWater" : "TilePairCollisionsLand";
    for(TilePairCollision *pair = (TilePairCollision *) rom[label];
        pair->tileset != 0xff;
        pair++) {
        if(pair->tileset == tileset_id && 
           ((pair->tile1 == tile1 && pair->tile2 == tile2) ||
            (pair->tile1 == tile2 && pair->tile2 == tile1))) {
            return true;
        }
    }
    return false;
}

// Returns if moving from the player tile to the ledge tile with the input will result
// in a ledge hop. The input is expected to be the joypad value.
//   $10 for right.
//   $20 for left.
//   $40 for up.
//   $80 for down.
DLLEXPORT bool rom_isledgehop(ROM& rom, u8 tileset_id, u8 player_tile, u8 ledge_tile,
                              u8 input) {
    if(tileset_id != 0) return false;
    
    for(Ledge *ledge = (Ledge *) rom["LedgeTiles"];
        ledge->player_direction != 0xff;
        ledge++) {
        if(ledge->player_tile == player_tile && ledge->ledge_tile == ledge_tile && ledge->input_required == input) {
            return true;
        }
    }
    return false;
}

// Returns if the tile is a door tile. 
// Used to tell the game when to automatically path the player one tile down.
DLLEXPORT bool rom_isdoortile(ROM& rom, u8 tileset_id, u8 tile) {
    u8 *door_pointers = rom["DoorTileIDPointers"];
    int list_index = find_byte_terminated(door_pointers, tileset_id, 0xff, 3);
    if(list_index == -1) return false;
    
    u16 address = *(u16 *) (door_pointers + list_index * 3 + 1);
    u8 *doors = rom[0x60000 | address];
    return in_list(doors, tile, 0x00);
}

// Returns if the tile is a warp tile.
DLLEXPORT bool rom_iswarptile(ROM& rom, u8 tileset_id, u8 tile) {
    u8 *data = rom[0x30000 | ((u16 *) rom["WarpTileIDPointers"])[tileset_id]];
    return in_list(data, tile, 0xff);
}

// Returns if the tile is a shore tile.
DLLEXPORT bool rom_isshoretile(u8 tile) {
    return tile == 0x14 || tile == 0x32 || tile == 0x48;
}