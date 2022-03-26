// Returns whether moving between the source tile and the destination tile will result
// in a collision. Note that checking against a ledge tile will return true. 
// Checks for water based collisions if the flag is set. Returns false for passing from a
// water tile to a shore tile.
DLLEXPORT bool rom_collisioncheck(ROM& rom, u8 map_id, u8 src_tile, u8 dest_tile, bool is_surfing) {
    Map map;
    rom_getmap(rom, map_id, &map);
    
    if(is_surfing && rom_isshoretile(dest_tile)) {
        return false;
    }
    
    Tileset tileset;
    rom_gettileset(rom, map.header.tileset, &tileset);
    
    if(rom_istilepaircollision(rom, map.header.tileset, src_tile, dest_tile, is_surfing)) {
        return true;
    }
    
    u8 bank = rom.game == YELLOW ? 0x01 : 0x00;
    return !in_list(rom[bank << 16 | tileset.collision_pointer], dest_tile, 0xff);
}

// Returns whether moving from the source coordinates to the destination coordinates will
// result in a collision. See `rom_collisioncheck` for more details.
DLLEXPORT bool rom_canmove(ROM& rom, u8 map_id, u8 src_x, u8 src_y, u8 dest_x, u8 dest_y, bool is_surfing) {
    u8 src_tile = rom_getmaptile(rom, map_id, src_x, src_y);
    u8 dest_tile = rom_getmaptile(rom, map_id, dest_x, dest_y);
    return rom_collisioncheck(rom, map_id, src_tile, dest_tile, is_surfing);
}

// Checks if the map has a warp at the specified coordinates and retrieves the direction
// of that warp. The warp direction will be mapped to the joypad action required to take
// the warp.
//   $10 for right.
//   $20 for left.
//   $40 for up.
//   $80 for down.
DLLEXPORT void rom_warpcheck(ROM& rom, u8 map_id, u8 x, u8 y,
                             bool *warp, u8 *warp_direction) {
    *warp = false;
    *warp_direction = 0;
    
    Map map;
    rom_getmap(rom, map_id, &map);
    
    bool warp_found = false;
    for(int i = 0; i < map.warp_count; i++) {
        if(map.warps[i].x == x && map.warps[i].y == y) {
            warp_found = true;
            break;
        }
    }
    if(!warp_found) return;
    
    u8 warp_tile = rom_getmaptile(rom, map_id, x, y);
    if(rom_isdoortile(rom, map.header.tileset, warp_tile) ||
       rom_iswarptile(rom, map.header.tileset, warp_tile)) {
        *warp = true;
    } else {
        auto edge_of_map_warp_check = [map, x, y, warp, warp_direction]() {
            if(x == map.header.width * 2 - 1) *warp_direction = 0x10;
            else if(x == 0) *warp_direction = 0x20;
            else if(y == 0) *warp_direction = 0x40;
            else if(y == map.header.height * 2 - 1) *warp_direction = 0x80;
            *warp = *warp_direction != 0;
        };
        
        auto directional_warp_check = [&rom, map_id, map, x, y, warp, warp_direction]() {
            constexpr int directions[8] = {
                0, 1,
                0, -1,
                -1, 0,
                1, 0,
            };
            
            // NOTE(stringflow): index into WarpTileListPointers, 
            // 0=down, 1=up, 2=left, 3=right
            for(u8 list_index = 0; list_index < 4 && *warp == false; list_index++) {
                // NOTE(stringflow): Don't check against directions that would require going
                // out of bounds to reach
                if((list_index == 0 && y == 0) ||
                   (list_index == 1 && y == map.header.height * 2 - 1) || 
                   (list_index == 2 && x == map.header.width * 2 - 1) || 
                   (list_index == 3 && x == 0)) continue;
                
                u8 direction = 0x10 << (3-list_index);
                int in_front_of_player_x = x + directions[list_index*2+0];
                int in_front_of_player_y = y + directions[list_index*2+1];
                u8 tile_in_front_of_player = rom_getmaptile(rom, map_id, 
                                                            in_front_of_player_x,
                                                            in_front_of_player_y);
                
                // NOTE(stringflow): IsSSAnneBowWarpTileInFrontOfPlayer
                if(map_id == 0x63) {
                    if(tile_in_front_of_player == 0x15) {
                        *warp = true;
                        *warp_direction = direction;
                    }
                } else {
                    u16 address = ((u16 *) rom["WarpTileListPointers"])[list_index];
                    u8 *carpet_tiles = rom[0x30000 | address];
                    
                    if(in_list(carpet_tiles, tile_in_front_of_player, 0xff)) {
                        *warp = true;
                        *warp_direction = direction;
                    }
                }
            }
        };
        
        // NOTE(stringflow): https://github.com/pret/pokered/blob/master/home/overworld.asm#L719-L740
        if(map_id == 0x61) edge_of_map_warp_check();
        else if(map_id == 0xc7) directional_warp_check();
        else if(map_id == 0xc8) directional_warp_check();
        else if(map_id == 0xca) directional_warp_check();
        else if(map_id == 0x52) directional_warp_check();
        else if(map.header.tileset == 0x00) directional_warp_check();
        else if(map.header.tileset == 0x0d) directional_warp_check();
        else if(map.header.tileset == 0x0e) directional_warp_check();
        else if(map.header.tileset == 0x17) directional_warp_check();
        else edge_of_map_warp_check();
    }
}

// Returns if moving from the source coordinates to the destination coordinates will result
// in a ledge hop. The destination coordinates are the coordinates of the potential ledge 
// tile.
DLLEXPORT bool rom_ledgehopcheck(ROM& rom, u8 map_id, u8 src_x, u8 src_y, 
                                 u8 dest_x, u8 dest_y) {
    MapHeader map;
    rom_getmapheader(rom, map_id, &map);
    
    u8 player_tile = rom_getmaptile(rom, map_id, src_x, src_y);
    u8 ledge_tile = rom_getmaptile(rom, map_id, dest_x, dest_y);
    u8 input = 0;
    if(dest_x != src_x) input = dest_x > src_x ? 0x10 : 0x20;
    else if(dest_y != src_y) input = dest_y < src_y ? 0x40 : 0x80;
    
    return rom_isledgehop(rom, map.tileset, player_tile, ledge_tile, input);
}