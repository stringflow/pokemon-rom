# trainerclass.cpp
### rom_gettrainerclassmovechoicemodifications
`(ROM& rom, u8 trainer_class_id) -> u8`  
Returns the move choice modifications of a trainer class as a bit field.  
Bit 0 will be set if modification 1 is applied:  
Discourages moves that cause no damage but only a status ailment if player's mon already has one.  
Bit 1 will be set if modification 2 is applied:  
Slightly encourage moves with stat-modifying effects and other effects that fall in-between.  
Bit 2 will be set if modification 3 is applied (Good AI):  
Encourages moves that are effective against the player's mon (even if non-damaging).  
Discourage damaging moves that are ineffective or not very effective against the player's mon, unless there's no damaging move that deals at least neutral damage.
### rom_gettrainerclassname
`(ROM& rom, u8 trainer_class_id, char *dest) -> void`  
Writes trainer class' decoded name to a 13 size character buffer.  
The $50 terminator is not written. For glitched trainer classes, a string of format 'hexXX' is outputted instead.
### rom_gettrainerclasspicmoney
`(ROM& rom, u8 trainer_class_id, PicMoney *dest) -> void`  
Writes the trainer class' picture pointer and base money reward.
### rom_gettrainerclassrawname
`(ROM &rom, u8 trainer_class_id, u8 *dest) -> int`  
Writes the trainer class' raw name, including the $50 terminator byte.  
Returns the length of the name.  
If you do not know the name length, pass zero as the destination and use the return value to tell you how big the buffer should be.
### rom_gettrainerclassroster
`(ROM& rom, u8 trainer_class_id, u8 roster_index, u8 *dest) -> int`  
Writes trainer class' roster. The roster is stored in sequential order with the following format: [species1, level1, species2, level2, species3, level3, ...].  
Returns the number of pokemon in the team (NOT the number of bytes!).  
To allocate the destination buffer, you can either call this function with zero as the destination, or pass a 12 size buffer.
### rom_gettrainerclasssprite
`(ROM& rom, u8 trainer_class_id, bool flip, u8 *dest) -> void`  
Writes the 2bpp representation of a trainer class' sprite to a 784 size buffer.  
The sprite can be horizontally flipped with the flip flag.  
If the trainer class' pic pointer points outside of the ROM address space, garbage data may be written by this routine. If the bounding box of the sprite has a width of zero, no data will be written since loading those sprites in-game crashes the game.
### rom_gettrainermoneyreward
`(ROM& rom, u8 trainer_class_id, u8 roster_index) -> int`  
Returns the money reward the player receives for defeating a the trainer.
### rom_istrainerclassevil
`(ROM& rom, u8 trainer_class_id) -> bool`  
Returns whether or not the trainer class is considered to be evil. (for audio purposes)
### rom_istrainerclassfemale
`(ROM& rom, u8 trainer_class_id) -> bool`  
Returns whether or not the trainer class is female. (for audio purposes)
