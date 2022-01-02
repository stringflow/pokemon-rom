# move.cpp
### rom_gethmmove
`(ROM& rom, u8 hm) -> u8`  
Returns the id of the move that the HM teaches.  
Pass the number of the HM (e.g. 1 for HM01) or the id of the HM item.
### rom_getmove
`(ROM& rom, u8 moveId, Move *move) -> void`  
Writes the move's header data.
### rom_getmovename
`(ROM& rom, u8 moveId, char *dest) -> void`  
Writes the move's decoded name to a 13 size character buffer. The $50 terminator is not written. For glitched items, a string of format 'hexXX' is outputted instead.
### rom_getmoverawname
`(ROM& rom, u8 moveId, u8 *dest) -> int`  
Writes the move's raw name, including the $50 terminator byte.  
Returns the length of the name.  
If you do not know the name length, pass zero as the destination and use the return value to tell you how big the buffer should be.
### rom_gettmmove
`(ROM& rom, u8 tm) -> u8`  
Returns the id of the move that the TM teaches.  
Pass the number of the TM (e.g. 1 for TM01) or the id of the TM item.
### rom_ismovehighcrit
`(ROM& rom, u8 moveId) -> bool`  
Returns whether or not the critical hit chance of the move is 4x as likely.
