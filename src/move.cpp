struct Move {
    u8 animation;
    u8 effect;
    u8 power;
    u8 type;
    u8 accuracy;
    u8 pp;
};

// Writes the move's header data.
DLLEXPORT void rom_getmove(ROM& rom, u8 move_id, Move *move) {
    u8 *data = rom["Moves"] + (u8) (move_id - 1) * sizeof(Move);
    memcpy(move, data, sizeof(Move));   
}

// Writes the move's raw name, including the $50 terminator byte. 
// Returns the length of the name.
// If you do not know the name length, pass zero as the destination and use the return
// value to tell you how big the buffer should be.
DLLEXPORT int rom_getmoverawname(ROM& rom, u8 move_id, u8 *dest) {
    return get_list_element(dest, rom["MoveNames"], move_id-1);    
}

// Writes the move's decoded name to a 13 size character buffer. The $50 terminator
// is not written. For glitched items, a string of format 'hexXX' is outputted instead.
DLLEXPORT void rom_getmovename(ROM& rom, u8 move_id, char *dest) {
    if(move_id == 0 || (move_id > 0xa5 && !is_machine(move_id))) {
        hex_string(dest, move_id);
    } else {
        get_list_element(dest, rom["MoveNames"], move_id-1);
    }
}

// Returns the id of the move that the TM teaches. 
// Pass the number of the TM (e.g. 1 for TM01) or the id of the TM item.
DLLEXPORT u8 rom_gettmmove(ROM& rom, u8 tm) {
    u8 array_index;
    if(tm >= TM01) {
        array_index = tm - TM01;
    } else {
        array_index = tm - 1;
    }
    return rom["TechnicalMachines"][array_index];
}

// Returns the id of the move that the HM teaches. 
// Pass the number of the HM (e.g. 1 for HM01) or the id of the HM item.
DLLEXPORT u8 rom_gethmmove(ROM& rom, u8 hm) {
    u8 array_index;
    if(hm >= HM01) {
        array_index = hm - HM01;
    } else {
        array_index = hm - 1;
    }
    return rom["TechnicalMachines"][array_index + 50]; // NOTE(stringflow): HMs start at 51
}

// Returns whether or not the critical hit chance of the move is 4x as likely.
DLLEXPORT bool rom_ismovehighcrit(ROM& rom, u8 move_id) {
    return in_list(rom["HighCriticalMoves"], move_id, 0xff);
}