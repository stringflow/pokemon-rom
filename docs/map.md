# map.cpp
### rom_blockstotiles
`(ROM& rom, u8 *blocks, int block_count, u8 width, int block_pointer, u8 *dest) -> void`  
Converts a block array into a tile array. The size of the destination buffer should be: blockCount * 16.
### rom_candig
`(ROM& rom, u8 map_id) -> bool`  
Returns if digging or using an escape rope is allowed on the map.
### rom_canridebike
`(ROM& rom, u8 map_id) -> bool`  
Returns if riding the bike is allowed on the map.
### rom_copyblocks
`(ROM& rom, int block_pointer, int block_count, u8 *dest) -> void`  
Copies the number of blocks from the blockPointer to a destination buffer.
### rom_getlastmap
`(ROM& rom, u8 map_id, u8 warp_index) -> u8`  
Returns the actual destination map that a wLastMap using warp brings you to.
### rom_getmap
`(ROM& rom, u8 map_id, Map *map) -> void`  
Writes the entire map header and map objects data.
### rom_getmapblocks
`(ROM& rom, u8 map_id, u8 *dest) -> int`  
Writes the blocks making up the map to the destination buffer. The size of the buffer should be the: mapwidth (in blocks) * mapheight (in blocks).  
Returns the map size in blocks.  
If you do not know the map dimensions, pass zero as the destination and use the return value to tell you how big the buffer should be.
### rom_getmapheader
`(ROM& rom, u8 map_id, MapHeader *map_header) -> void`  
Writes the map header data. This does not include: map connections, border block, warps, signs, and map objects.
### rom_getmapobjectpixels
`(ROM& rom, u8 picture_id, u8 direction, u8 *dest) -> void`  
Writes the map object's 2bpp representation to a 64 size buffer.  
The direction the sprite will be facing is inferred from the object's data, objects that move around will default to facing downwards.
### rom_getmappixels
`(ROM& rom, u8 map_id, u8 *dest) -> int`  
Writes the 2bpp representation of the map tiles to the destination buffer.  
The size of the buffer should be: mapwidth (in blocks) * mapheight (in blocks) * 256.  
Returns the map size in pixels.  
If you do not know the map dimensions, pass zero as the destination and use the return value to tell you how big the buffer should be.
### rom_getmaptile
`(ROM& rom, u8 map_id, int x, int y) -> u8`  
Returns the bottom left tile of at the given X and Y coordinate. If out of bounds coordinates are passed, the border block will be used.
### rom_getmaptiles
`(ROM& rom, u8 map_id, u8 *dest) -> int`  
Writes the tiles making up the map to the destination buffer. The size of the buffer should be the: mapwidth (in blocks) * mapheight (in blocks) * 16.  
Returns the map size in tiles.  
If you do not know the map dimensions, pass zero as the destination and use the return value to tell you how big the buffer should be.
### rom_getmapwilddata
`(ROM& rom, u8 map_id, WildData *dest) -> void`  
Fills out the supplied WildData struct with the map's encounter data.  
The slots are in sequential order from most common to least common.  
Each slot occupies 2 bytes, the first byte is equal to the pokemon's level, the second byte is equal to the species id.
### rom_getspritepixels
`(ROM& rom, u8 picture_id, u8 index, bool flip, u8 *dest) -> void`  
Writes the pictureId's 2bpp representation to a 64 size buffer.  
Different versions of the sprite can be indexed through the index parameter, for typical overworld sprites the versions would be: (0) facing down (1) facing up (2) facing left  
The sprite will be vertically flipped if the flag is set, making sprites that are facing right possible.
### rom_tilestopixels
`(ROM& rom, u8 *tiles, int tile_count, int gfx_pointer, u8 *dest) -> void`  
Converts a tile array into an array of 2bpp pixels. The size of the destination buffer should be: tileCount * 16.
