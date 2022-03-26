constexpr u8 OPP_ID_OFFSET = 200;

struct PicMoney {
    u16 pic_pointer;
    u16 base_money;
};

// Writes the trainer class' raw name, including the $50 terminator byte. 
// Returns the length of the name.
// If you do not know the name length, pass zero as the destination and use the return
// value to tell you how big the buffer should be.
DLLEXPORT int rom_gettrainerclassrawname(ROM &rom, u8 trainer_class_id, u8 *dest) {
    return get_list_element(dest, rom["TrainerNames"], trainer_class_id - OPP_ID_OFFSET-1);
}

// Writes trainer class' decoded name to a 13 size character buffer. 
// The $50 terminator is not written. For glitched trainer classes, a string of format
// 'hexXX' is outputted instead.
DLLEXPORT void rom_gettrainerclassname(ROM& rom, u8 trainer_class_id, char *dest) {
    std::string name;
    if(trainer_class_id <= OPP_ID_OFFSET || trainer_class_id > 0xf7) {
        name = hex_string(trainer_class_id);
    } else {
        name = get_list_element(rom["TrainerNames"], trainer_class_id - OPP_ID_OFFSET-1);
    }
    strcpy(dest, name.c_str());
}

// Writes trainer class' roster. The roster is stored in sequential order
// with the following format: [species1, level1, species2, level2, species3, level3, ...].
// Returns the number of pokemon in the team (NOT the number of bytes!).
// To allocate the destination buffer, you can either call this function with zero 
// as the destination, or pass a 12 size buffer.
DLLEXPORT int rom_gettrainerclassroster(ROM& rom, u8 trainer_class_id, u8 roster_index, u8 *dest) {
    int data_pointer = 0xe0000 | ((u16 *) rom["TrainerDataPointers"])[trainer_class_id - OPP_ID_OFFSET-1];
    u8 *data = rom[data_pointer];
    
    int team_size;
    for(int i = 0; i <= roster_index; i++) {
        u8 format = *data++;
        u8 level = format;
        u8 species;
        
        team_size = 0;
        while((species = *data++) != 0x00) {
            if(format == 0xff) {
                level = species;
                species = *data++;
            }
            
            if(dest && i == roster_index) {
                dest[team_size*2+0] = species;
                dest[team_size*2+1] = level;
            }
            
            team_size++;
        }
    }
    
    return team_size;
}

// Writes the trainer class' picture pointer and base money reward.
DLLEXPORT void rom_gettrainerclasspicmoney(ROM& rom, u8 trainer_class_id, PicMoney *dest) {
    // NOTE(stringflow): The table is 5 bytes wide and an additional byte is allocated for                            the base money but never loaded. Because of this I chose to store                            the base money as a 16-bit integer and ignore the last byte.
    int data_offset = (trainer_class_id - OPP_ID_OFFSET-1) * (sizeof(PicMoney)+1);
    u16 *data = (u16 *) (rom["TrainerPicAndMoneyPointers"] + data_offset);
    dest->pic_pointer = *data++;
    dest->base_money = decode_bcd((u8 *) data, 2);
}

// Returns the money reward the player receives for defeating a the trainer.
DLLEXPORT int rom_gettrainermoneyreward(ROM& rom, u8 trainer_class_id, u8 roster_index) {
    int roster_size = rom_gettrainerclassroster(rom, trainer_class_id, roster_index, 0);
    u8 roster[roster_size];
    rom_gettrainerclassroster(rom, trainer_class_id, roster_index, roster);
    u8 last_mon_level = roster[roster_size * 2 - 1];
    
    PicMoney picmoney;
    rom_gettrainerclasspicmoney(rom, trainer_class_id, &picmoney);
    
    return last_mon_level * picmoney.base_money;
}

// Writes the 2bpp representation of a trainer class' sprite to a 784 size buffer.
// The sprite can be horizontally flipped with the flip flag.
// If the trainer class' pic pointer points outside of the ROM address space, garbage data
// may be written by this routine. If the bounding box of the sprite has a width of zero, no
// data will be written since loading those sprites in-game crashes the game.
DLLEXPORT void rom_gettrainerclasssprite(ROM& rom, u8 trainer_class_id, bool flip, u8 *dest) {
    PicMoney picmoney;
    rom_gettrainerclasspicmoney(rom, trainer_class_id, &picmoney);
    pic_decompress(rom[0x130000 | picmoney.pic_pointer], 0x77, dest, true, flip);
}

// Returns whether or not the trainer class is female. (for audio purposes)
DLLEXPORT bool rom_istrainerclassfemale(ROM& rom, u8 trainer_class_id) {
    return in_list(rom["FemaleTrainerList"], trainer_class_id, 0xff);
}

// Returns whether or not the trainer class is considered to be evil. (for audio purposes)
DLLEXPORT bool rom_istrainerclassevil(ROM& rom, u8 trainer_class_id) {
    return in_list(rom["EvilTrainerList"], trainer_class_id, 0xff);
}

// Returns the move choice modifications of a trainer class as a bit field.
// Bit 0 will be set if modification 1 is applied:
//   Discourages moves that cause no damage but only a status ailment if player's mon
//   already has one.
// Bit 1 will be set if modification 2 is applied:
//   Slightly encourage moves with stat-modifying effects and other effects that fall
//   in-between.
// Bit 2 will be set if modification 3 is applied (Good AI):
//   Encourages moves that are effective against the player's mon (even if non-damaging).
//   Discourage damaging moves that are ineffective or not very effective against the
//   player's mon, unless there's no damaging move that deals at least neutral damage.
DLLEXPORT u8 rom_gettrainerclassmovechoicemodifications(ROM& rom, u8 trainer_class_id) {
    u8 *data = rom["TrainerClassMoveChoiceModifications"];
    u8 result;
    for(int i = 0; i <= (trainer_class_id - OPP_ID_OFFSET - 1); i++) {
        result = 0;
        u8 bit;
        while((bit = *data++) != 0) {
            result |= 1 << (bit - 1);
        }
    }
    
    return result;
}

// TODO(stringflow): LoneMoves, TeamMoves