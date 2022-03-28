# item.cpp
### rom_getitemexecutionpointer
`(ROM& rom, u8 item_id) -> u16`  
Returns a 16-bit pointer that is jumped to when the item is used.
### rom_getitemname
`(ROM& rom, u8 item_id, char *dest) -> void`  
Writes the item's decoded name to a 13 size character buffer. The $50 terminator is not written. For glitched items, a string of format 'hexXX' is outputted instead.
### rom_getitemprice
`(ROM& rom, u8 item_id) -> int`  
Returns the price of the item. Half this number to get the sell price of the item.
### rom_getitemrawname
`(ROM& rom, u8 item_id, u8 *dest) -> int`  
Writes item's raw name (including the $50 terminator byte) to a buffer.  
Returns the length of the name.  
If you do not know the name length, pass zero as the destination and use the return value to tell you how big the buffer should be.
### rom_isguarddrink
`(ROM& rom, u8 item_id) -> bool`  
Returns whether or not the item is usable as a guard drink.
### rom_iskeyitem
`(ROM& rom, u8 item_id) -> bool`  
Returns whether or not the item is a key item.
