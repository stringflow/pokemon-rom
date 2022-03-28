# tileset.cpp
### rom_gettileset
`(ROM& rom, u8 tileset_id, Tileset *tileset) -> void`  
Writes the tileset's header data.
### rom_isdoortile
`(ROM& rom, u8 tileset_id, u8 tile) -> bool`  
Returns if the tile is a door tile.  
Used to tell the game when to automatically path the player one tile down.
### rom_isledgehop
`(ROM& rom, u8 tileset_id, u8 player_tile, u8 ledge_tile, u8 input) -> bool`  
Returns if moving from the player tile to the ledge tile with the input will result in a ledge hop. The input is expected to be the joypad value.  
$10 for right.  
$20 for left.  
$40 for up.  
$80 for down.
### rom_isshoretile
`(u8 tile) -> bool`  
Returns if the tile is a shore tile.
### rom_istilepaircollision
`(ROM& rom, u8 tileset_id, u8 tile1, u8 tile2, bool water) -> bool`  
Returns if moving from tile 1 to tile 2 results in a tile pair collision.  
Used for simulate differences in elevation.
### rom_iswarptile
`(ROM& rom, u8 tileset_id, u8 tile) -> bool`  
Returns if the tile is a warp tile.
