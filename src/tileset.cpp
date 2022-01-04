struct Tileset {
    u8 bank;
    u16 blockPointer;
    u16 gfxPointer;
    u16 collisionPointer;
    u8 counterTiles[3];
    u8 grassTile;
    u8 animations;
};

struct TilePairCollision {
    u8 tileset;
    u8 tile1;
    u8 tile2;
};

struct Ledge {
    u8 playerDirection;
    u8 playerTile;
    u8 ledgeTile;
    u8 inputRequired;
};

// Writes the tileset's header data.
DLLEXPORT void rom_gettileset(ROM& rom, u8 tilesetId, Tileset *tileset) {
    u8 *data = rom["Tilesets"] + tilesetId * sizeof(Tileset);
    memcpy(tileset, data, sizeof(Tileset));
}

// Returns if moving from tile 1 to tile 2 results in a tile pair collision.
// Used for simulate differences in elevation.
DLLEXPORT bool rom_istilepaircollision(ROM& rom, u8 tilesetId, u8 tile1, u8 tile2,
                                       bool water) {
    std::string label = water ? "TilePairCollisionsWater" : "TilePairCollisionsLand";
    for(TilePairCollision *pair = (TilePairCollision *) rom[label];
        pair->tileset != 0xff;
        pair++) {
        if(pair->tileset == tilesetId && 
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
DLLEXPORT bool rom_isledgehop(ROM& rom, u8 tilesetId, u8 playerTile, u8 ledgeTile,
                              u8 input) {
    if(tilesetId != 0) return false;
    
    for(Ledge *ledge = (Ledge *) rom["LedgeTiles"];
        ledge->playerDirection != 0xff;
        ledge++) {
        if(ledge->playerTile == playerTile && ledge->ledgeTile == ledgeTile && ledge->inputRequired == input) {
            return true;
        }
    }
    return false;
}

// Returns if the tile is a door tile. 
// Used to tell the game when to automatically path the player one tile down.
DLLEXPORT bool rom_isdoortile(ROM& rom, u8 tilesetId, u8 tile) {
    u8 *doorPointers = rom["DoorTileIDPointers"];
    int listIndex = findByteTerminated(doorPointers, tilesetId, 0xff, 3);
    if(listIndex == -1) return false;
    
    u16 address = *(u16 *) (doorPointers + listIndex * 3 + 1);
    u8 *doors = rom[0x60000 | address];
    return inList(doors, tile, 0x00);
}

// Returns if the tile is a warp tile.
DLLEXPORT bool rom_iswarptile(ROM& rom, u8 tilesetId, u8 tile) {
    u8 *data = rom[0x30000 | ((u16 *) rom["WarpTileIDPointers"])[tilesetId]];
    return inList(data, tile, 0xff);
}

// Returns if the tile is a shore tile.
DLLEXPORT bool rom_isshoretile(u8 tile) {
    return tile == 0x14 || tile == 0x32 || tile == 0x48;
}