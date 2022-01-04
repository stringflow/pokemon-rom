# collision.cpp
### rom_canmove
`(ROM& rom, u8 mapId, u8 xSrc, u8 ySrc, u8 xDest, u8 yDest, bool water) -> bool`  
Returns whether moving from the source coordinates to the destination coordinates will result in a collision. See `rom_collisioncheck` for more details.
### rom_collisioncheck
`(ROM& rom, u8 mapId, u8 srcTile, u8 destTile, bool water) -> bool`  
Returns whether moving between the source tile and the destination tile will result in a collision. Note that checking against a ledge tile will return true.  
Checks for water based collisions if the flag is set. Returns false for passing between water tiles to a shore tile.
### rom_ledgehopcheck
`(ROM& rom, u8 mapId, u8 xSrc, u8 ySrc, u8 xDest, u8 yDest) -> bool`  
Returns if moving from the source coordinates to the destination coordinates will result in a ledge hop. The destination coordinates are the coordinates of the potential ledge tile. Assumes that the tileset is overworld.
### rom_warpcheck
`(ROM& rom, u8 mapId, u8 x, u8 y, bool *warp, u8 *warpDirection) -> void`  
Checks if the map has a warp at the specified coordinates and retrieves the direction of that warp. The warp direction will be mapped to the joypad action required to take the warp.  
$10 for right $20 for left $40 for up $80 for down
