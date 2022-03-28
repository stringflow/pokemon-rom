# type.cpp
### rom_geteffectiveness
`(ROM& rom, u8 attacking_type_id, u8 defending_type_id) -> f32`  
Returns the damage multiplier for the type matchup. It returns the following values: 2.0 for super effective.  
1.0 for neutral.  
0.5 for not very effective.  
0.0 for immunity.  
For duel-typing call this function twice with each of the defending types and multiply the results.
### rom_gettypename
`(ROM& rom, u8 type_id, char *dest) -> void`  
Writes the item's decoded name to a 13 size character buffer. The $50 terminator is not written. For glitched types, a string of format 'hexXX' is outputted instead.
### rom_istypespecial
`(u8 type_id) -> bool`  
Returns whether the type is a special type or a physical type.
