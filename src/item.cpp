// Writes item's raw name (including the $50 terminator byte) to a buffer. 
// Returns the length of the name.
// If you do not know the name length, pass zero as the destination and use the return
// value to tell you how big the buffer should be.
DLLEXPORT int rom_getitemrawname(ROM& rom, u8 itemId, u8 *dest) {
    return getListElement(dest, rom["ItemNames"], itemId-1);
}

// Writes the item's decoded name to a 13 size character buffer. The $50 terminator
// is not written. For glitched items, a string of format 'hexXX' is outputted instead.
DLLEXPORT void rom_getitemname(ROM& rom, u8 itemId, char *dest) {
    std::string name;
    // NOTE(stringflow): check for glitch items
    if(itemId < 0x01 || (itemId > 0x61 && !isMachine(itemId))) {
        name = hexString(itemId);
    } else {
        name = getListElement(rom["ItemNames"], itemId-1);
    }
    strcpy(dest, name.c_str());
}

// Returns a 16-bit pointer that is jumped to when the item is used.
DLLEXPORT u16 rom_getitemexecutionpointer(ROM& rom, u8 itemId) {
    // TODO(stringflow): maybe return the bank? bank 3 for $4000..$7fff addresses
    if(isMachine(itemId)) return rom.symbols["ItemUseTMHM"];
    u8 listIndex = itemId - 1;
    u8 listOffset = listIndex * 2;
    return *((u16 *) (rom["ItemUsePtrTable"] + listOffset));
}

// Returns whether or not the item is a key item.
DLLEXPORT bool rom_iskeyitem(ROM& rom, u8 itemId) {
    return bitArray(rom["KeyItemFlags"], itemId - 1);
}

// Returns whether or not the item is usable as a guard drink.
DLLEXPORT bool rom_isguarddrink(ROM& rom, u8 itemId) {
    return inList(rom["GuardDrinksList"], itemId, 0x00);
}

// Returns the price of the item. Half this number to get the sell price of the item.
DLLEXPORT int rom_getitemprice(ROM& rom, u8 itemId) {
    // TODO(stringflow): figure out why does VendingPrices exists.
    // The data is correctly put in ItemPrices.
    if(isHiddenMachine(itemId)) {
        return 0; // NOTE(stringflow): HMs are priceless
    } else if(isTechnicalMachine(itemId)) {
        u8 thousands = nybbleArray(rom["TechnicalMachinePrices"], itemId - TM01); 
        return thousands * 1000;
    } else {
        u8 arrayIndex = (u8) (itemId - 1);
        return decodeBCD(rom["ItemPrices"] + arrayIndex * 3, 3);
    }
}