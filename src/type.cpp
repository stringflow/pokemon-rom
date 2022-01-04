struct TypeEffectiveness {
    u8 attacking;
    u8 defending;
    u8 effectiveness;
};

// Writes the item's decoded name to a 13 size character buffer. The $50 terminator
// is not written. For glitched types, a string of format 'hexXX' is outputted instead.
DLLEXPORT void rom_gettypename(ROM& rom, u8 typeId, char *dest) {
    std::string name;
    if(typeId > 0x1a) {
        name = hexString(typeId);
    } else {
        u16 address = ((u16 *) rom["TypeNames"])[typeId];
        name = decodeString(rom[0x90000 | address]);
    }
    strcpy(dest, name.c_str());
}

// Returns whether the type is a special type or a physical type.
DLLEXPORT bool rom_istypespecial(u8 typeId) {
    return typeId > 0x13;
}

// Returns the damage multiplier for the type matchup. It returns the following values:
//   2.0 for super effective.
//   1.0 for neutral.
//   0.5 for not very effective.
//   0.0 for immunity.
// For duel-typing call this function twice with each of the defending types 
// and multiply the results.
DLLEXPORT f32 rom_geteffectiveness(ROM& rom, u8 attackingTypeId, u8 defendingTypeId) {
    for(TypeEffectiveness *matchup = (TypeEffectiveness *) rom["TypeEffects"];
        matchup->attacking != 0xff;
        matchup++) {
        if(matchup->attacking == attackingTypeId && matchup->defending == defendingTypeId) {
            return (f32) matchup->effectiveness / 10.0f;
        }
    }
    return 1.0f;
}