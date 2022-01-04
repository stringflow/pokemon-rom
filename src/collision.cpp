// Returns whether moving between the source tile and the destination tile will result
// in a collision. Note that checking against a ledge tile will return true. 
// Checks for water based collisions if the flag is set. Returns false for passing from a
// water tile to a shore tile.
DLLEXPORT bool rom_collisioncheck(ROM& rom, u8 mapId, u8 srcTile, u8 destTile, bool water) {
    Map map;
    rom_getmap(rom, mapId, &map);
    
    if(water && rom_isshoretile(destTile)) {
        return false;
    }
    
    Tileset tileset;
    rom_gettileset(rom, map.header.tileset, &tileset);
    
    if(rom_istilepaircollision(rom, map.header.tileset, srcTile, destTile, water)) {
        return true;
    }
    
    u8 bank = rom.game == Yellow ? 0x01 : 0x00;
    return !inList(rom[bank << 16 | tileset.collisionPointer], destTile, 0xff);
}

// Returns whether moving from the source coordinates to the destination coordinates will
// result in a collision. See `rom_collisioncheck` for more details.
DLLEXPORT bool rom_canmove(ROM& rom, u8 mapId, u8 xSrc, u8 ySrc, u8 xDest, u8 yDest, bool water) {
    u8 srcTile = rom_getmaptile(rom, mapId, xSrc, ySrc);
    u8 destTile = rom_getmaptile(rom, mapId, xDest, yDest);
    return rom_collisioncheck(rom, mapId, srcTile, destTile, water);
}

// Checks if the map has a warp at the specified coordinates and retrieves the direction
// of that warp. The warp direction will be mapped to the joypad action required to take
// the warp.
//   $10 for right.
//   $20 for left.
//   $40 for up.
//   $80 for down.
DLLEXPORT void rom_warpcheck(ROM& rom, u8 mapId, u8 x, u8 y,
                             bool *warp, u8 *warpDirection) {
    *warp = false;
    *warpDirection = 0;
    
    Map map;
    rom_getmap(rom, mapId, &map);
    
    bool warpFound = false;
    for(int i = 0; i < map.warpCount; i++) {
        if(map.warps[i].x == x && map.warps[i].y == y) {
            warpFound = true;
            break;
        }
    }
    if(!warpFound) return;
    
    u8 warpTile = rom_getmaptile(rom, mapId, x, y);
    if(rom_isdoortile(rom, map.header.tileset, warpTile) ||
       rom_iswarptile(rom, map.header.tileset, warpTile)) {
        *warp = true;
    } else {
        auto edgeOfMapWarpCheck = [map, x, y, warp, warpDirection]() {
            if(x == map.header.width * 2 - 1) *warpDirection = 0x10;
            else if(x == 0) *warpDirection = 0x20;
            else if(y == 0) *warpDirection = 0x40;
            else if(y == map.header.height * 2 - 1) *warpDirection = 0x80;
            *warp = *warpDirection != 0;
        };
        
        auto directionalWarpCheck = [&rom, mapId, map, x, y, warp, warpDirection]() {
            constexpr int directions[8] = {
                0, 1,
                0, -1,
                -1, 0,
                1, 0,
            };
            
            // NOTE(stringflow): index into WarpTileListPointers, 
            // 0=down, 1=up, 2=left, 3=right
            for(u8 listIndex = 0; listIndex < 4 && *warp == false; listIndex++) {
                // NOTE(stringflow): Don't check against directions that would require going
                // out of bounds to reach
                if((listIndex == 0 && y == 0) ||
                   (listIndex == 1 && y == map.header.height * 2 - 1) || 
                   (listIndex == 2 && x == map.header.width * 2 - 1) || 
                   (listIndex == 3 && x == 0)) continue;
                
                u8 direction = 0x10 << (3-listIndex);
                int xInFrontOfPlayer = x + directions[listIndex*2+0];
                int yInFrontOfPlayer = y + directions[listIndex*2+1];
                u8 tileInFrontOfPlayer = rom_getmaptile(rom, mapId, 
                                                        xInFrontOfPlayer,
                                                        yInFrontOfPlayer);
                
                // NOTE(stringflow): IsSSAnneBowWarpTileInFrontOfPlayer
                if(mapId == 0x63) {
                    if(tileInFrontOfPlayer == 0x15) {
                        *warp = true;
                        *warpDirection = direction;
                    }
                } else {
                    u16 address = ((u16 *) rom["WarpTileListPointers"])[listIndex];
                    u8 *carpetTiles = rom[0x30000 | address];
                    
                    if(inList(carpetTiles, tileInFrontOfPlayer, 0xff)) {
                        *warp = true;
                        *warpDirection = direction;
                    }
                }
            }
        };
        
        // NOTE(stringflow): https://github.com/pret/pokered/blob/master/home/overworld.asm#L719-L740
        if(mapId == 0x61) edgeOfMapWarpCheck();
        else if(mapId == 0xc7) directionalWarpCheck();
        else if(mapId == 0xc8) directionalWarpCheck();
        else if(mapId == 0xca) directionalWarpCheck();
        else if(mapId == 0x52) directionalWarpCheck();
        else if(map.header.tileset == 0x00) directionalWarpCheck();
        else if(map.header.tileset == 0x0d) directionalWarpCheck();
        else if(map.header.tileset == 0x0e) directionalWarpCheck();
        else if(map.header.tileset == 0x17) directionalWarpCheck();
        else edgeOfMapWarpCheck();
    }
}

// Returns if moving from the source coordinates to the destination coordinates will result
// in a ledge hop. The destination coordinates are the coordinates of the potential ledge 
// tile.
DLLEXPORT bool rom_ledgehopcheck(ROM& rom, u8 mapId, u8 xSrc, u8 ySrc, u8 xDest, u8 yDest) {
    MapHeader map;
    rom_getmapheader(rom, mapId, &map);
    
    u8 playerTile = rom_getmaptile(rom, mapId, xSrc, ySrc);
    u8 ledgeTile = rom_getmaptile(rom, mapId, xDest, yDest);
    u8 input = 0;
    if(xDest != xSrc) input = xDest > xSrc ? 0x10 : 0x20;
    else if(yDest != ySrc) input = yDest < ySrc ? 0x40 : 0x80;
    
    return rom_isledgehop(rom, map.tileset, playerTile, ledgeTile, input);
}