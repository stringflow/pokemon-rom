// Writes item's raw name (including the $50 terminator byte) to a buffer. 
// Returns the length of the name.
// If you do not know the name length, pass zero as the destination and use the return
// value to tell you how big the buffer should be.
DLLEXPORT int rom_getitemrawname(ROM& rom, u8 item_id, u8 *dest) {
    return get_list_element(dest, rom["ItemNames"], item_id-1);
}

// Writes the item's decoded name to a 13 size character buffer. The $50 terminator
// is not written. For glitched items, a string of format 'hexXX' is outputted instead.
DLLEXPORT void rom_getitemname(ROM& rom, u8 item_id, char *dest) {
    std::string name;
    // NOTE(stringflow): check for glitch items
    if(item_id < 0x01 || (item_id > 0x61 && !is_machine(item_id))) {
        name = hex_string(item_id);
    } else {
        name = get_list_element(rom["ItemNames"], item_id-1);
    }
    strcpy(dest, name.c_str());
}

// Returns a 16-bit pointer that is jumped to when the item is used.
DLLEXPORT u16 rom_getitemexecutionpointer(ROM& rom, u8 item_id) {
    // TODO(stringflow): maybe return the bank? bank 3 for $4000..$7fff addresses
    if(is_machine(item_id)) return rom.symbols["ItemUseTMHM"];
    u8 list_index = item_id - 1;
    u8 list_offset = list_index * 2;
    return *((u16 *) (rom["ItemUsePtrTable"] + list_offset));
}

// Returns whether or not the item is a key item.
DLLEXPORT bool rom_iskeyitem(ROM& rom, u8 item_id) {
    return bit_array(rom["KeyItemFlags"], item_id - 1);
}

// Returns whether or not the item is usable as a guard drink.
DLLEXPORT bool rom_isguarddrink(ROM& rom, u8 item_id) {
    return in_list(rom["GuardDrinksList"], item_id, 0x00);
}

// Returns the price of the item. Half this number to get the sell price of the item.
DLLEXPORT int rom_getitemprice(ROM& rom, u8 item_id) {
    // TODO(stringflow): figure out why does VendingPrices exists.
    // The data is correctly put in ItemPrices.
    if(is_hidden_machine(item_id)) {
        return 0; // NOTE(stringflow): HMs are priceless
    } else if(is_technical_machine(item_id)) {
        u8 thousands = nybble_array(rom["TechnicalMachinePrices"], item_id - TM01); 
        return thousands * 1000;
    } else {
        u8 arrayIndex = (u8) (item_id - 1);
        return decode_bcd(rom["ItemPrices"] + arrayIndex * 3, 3);
    }
}