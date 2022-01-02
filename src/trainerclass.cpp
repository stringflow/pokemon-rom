constexpr u8 OPP_ID_OFFSET = 200;

struct PicMoney {
    u16 picPointer;
    u16 baseMoney;
};

// Writes the trainer class' raw name, including the $50 terminator byte. 
// Returns the length of the name.
// If you do not know the name length, pass zero as the destination and use the return
// value to tell you how big the buffer should be.
DLLEXPORT int rom_gettrainerclassrawname(ROM &rom, u8 trainerClassId, u8 *dest) {
    return getListElement(dest, rom["TrainerNames"], trainerClassId - OPP_ID_OFFSET - 1);
}

// Writes trainer class' decoded name to a 13 size character buffer. 
// The $50 terminator is not written. For glitched trainer classes, a string of format
// 'hexXX' is outputted instead.
DLLEXPORT void rom_gettrainerclassname(ROM& rom, u8 trainerClassId, char *dest) {
    std::string name;
    if(trainerClassId <= OPP_ID_OFFSET || trainerClassId > 0xf7) {
        name = hexString(trainerClassId);
    } else {
        name = getListElement(rom["TrainerNames"], trainerClassId - OPP_ID_OFFSET - 1);
    }
    strcpy(dest, name.c_str());
}

// Writes trainer class' roster. The roster is stored in sequential order
// with the following format: [species1, level1, species2, level2, species3, level3, ...].
// Returns the number of pokemon in the team (NOT the number of bytes!).
// To allocate the destination buffer, you can either call this function with zero 
// as the destination, or pass a 12 size buffer.
DLLEXPORT int rom_gettrainerclassroster(ROM& rom, u8 trainerClassId, u8 rosterIndex, u8 *dest) {
    int dataPointer = 0xe0000 | ((u16 *) rom["TrainerDataPointers"])[trainerClassId - OPP_ID_OFFSET - 1];
    u8 *data = rom[dataPointer];
    
    int teamSize;
    for(int i = 0; i <= rosterIndex; i++) {
        u8 format = *data++;
        u8 level = format;
        u8 species;
        
        teamSize = 0;
        while((species = *data++) != 0x00) {
            if(format == 0xff) {
                level = species;
                species = *data++;
            }
            
            if(dest && i == rosterIndex) {
                dest[teamSize*2+0] = species;
                dest[teamSize*2+1] = level;
            }
            
            teamSize++;
        }
    }
    
    return teamSize;
}

// Writes the trainer class' picture pointer and base money reward.
DLLEXPORT void rom_gettrainerclasspicmoney(ROM& rom, u8 trainerClassId, PicMoney *dest) {
    // NOTE(stringflow): The table is 5 bytes wide and an additional byte is allocated for                            the base money but never loaded. Because of this I chose to store                            the base money as a 16-bit integer and ignore the last byte.
    int dataOffset = (trainerClassId - OPP_ID_OFFSET - 1) * (sizeof(PicMoney)+1);
    u16 *data = (u16 *) (rom["TrainerPicAndMoneyPointers"] + dataOffset);
    dest->picPointer = *data++;
    dest->baseMoney = decodeBCD((u8 *) data, 2);
}

// Returns the money reward the player receives for defeating a the trainer.
DLLEXPORT int rom_gettrainermoneyreward(ROM& rom, u8 trainerClassId, u8 rosterIndex) {
    int rosterSize = rom_gettrainerclassroster(rom, trainerClassId, rosterIndex, nullptr);
    u8 roster[rosterSize];
    rom_gettrainerclassroster(rom, trainerClassId, rosterIndex, roster);
    u8 lastMonLevel = roster[rosterSize * 2 - 1];
    
    PicMoney picmoney;
    rom_gettrainerclasspicmoney(rom, trainerClassId, &picmoney);
    
    return lastMonLevel * picmoney.baseMoney;
}

// Writes the 2bpp representation of a trainer class' sprite to a 784 size buffer.
// The sprite can be horizontally flipped with the flip flag.
// If the trainer class' pic pointer points outside of the ROM address space, garbage data
// may be written by this routine. If the bounding box of the sprite has a width of zero, no
// data will be written since loading those sprites in-game crashes the game.
DLLEXPORT void rom_gettrainerclasssprite(ROM& rom, u8 trainerClassId, bool flip, u8 *dest) {
    PicMoney picmoney;
    rom_gettrainerclasspicmoney(rom, trainerClassId, &picmoney);
    picDecompress(rom[0x130000 | picmoney.picPointer], 0x77, dest, true, flip);
}

// Returns whether or not the trainer class is female. (for audio purposes)
DLLEXPORT bool rom_istrainerclassfemale(ROM& rom, u8 trainerClassId) {
    return inList(rom["FemaleTrainerList"], trainerClassId, 0xff);
}

// Returns whether or not the trainer class is considered to be evil. (for audio purposes)
DLLEXPORT bool rom_istrainerclassevil(ROM& rom, u8 trainerClassId) {
    return inList(rom["EvilTrainerList"], trainerClassId, 0xff);
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
DLLEXPORT u8 rom_gettrainerclassmovechoicemodifications(ROM& rom, u8 trainerClassId) {
    u8 *data = rom["TrainerClassMoveChoiceModifications"];
    u8 result;
    for(int i = 0; i <= (trainerClassId - OPP_ID_OFFSET - 1); i++) {
        result = 0;
        u8 bit;
        while((bit = *data++) != 0) {
            result |= 1 << (bit - 1);
        }
    }
    
    return result;
}

// TODO(stringflow): LoneMoves, TeamMoves