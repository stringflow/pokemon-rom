constexpr int MAX_SPECIES_NAME_LENGTH = 10;

struct BaseStats {
    u8 pokedex;
    u8 hp;
    u8 attack;
    u8 defense;
    u8 speed;
    u8 special;
    u8 type1;
    u8 type2;
    u8 catch_rate;
    u8 base_exp;
    u8 front_pic_dimensions;
    u16 front_pic;
    u16 back_pic;
    u8 base_moves[4];
    u8 growth_rate;
    u8 learnset[7];
    u8 padding; // NOTE(stringflow): unused
};

struct DexEntry {
    char type[12];
    u8 feet;
    u8 inches;
    f32 weight;
    char description[128];
};

struct CryData {
    u8 base_cry;
    u8 pitch;
    u8 length;
};

bool is_glitch_mon(u8 species) {
    return species < 0x01 || species > 0xbe;
}

// Returns the pokedex number of a pokemon.
DLLEXPORT u8 rom_getpokedexnumber(ROM& rom, u8 species) {
    return *(rom["PokedexOrder"] + species);
}

// Writes the pokemon's basestats.
DLLEXPORT void rom_getmonbasestats(ROM &rom, u8 species, BaseStats *stats) {
    u8 *data;
    if(species == 0x15 && rom.game != YELLOW) {
        data = rom["MewBaseStats"];
    } else {
        u8 pokedex = rom_getpokedexnumber(rom, species - 1) - 1;
        data = rom["BaseStats"] + pokedex * sizeof(BaseStats);
    }
    
    if(species == 0xb6) {
        stats->front_pic_dimensions = 0x66;
        stats->front_pic = (u16) rom.symbols["FossilKabutopsPic"];
    } else if(species == 0xb7) {
        stats->front_pic_dimensions = 0x66;
        stats->front_pic = (u16) rom.symbols["GhostPic"];
    } else if(species == 0xb8) {
        stats->front_pic_dimensions = 0x77;
        stats->front_pic = (u16) rom.symbols["FossilAerodactylPic"];
    } else {
        memcpy(stats, data, sizeof(BaseStats));
    }
}

// Writes pokemon's raw name to a 10 size byte buffer. It will only include the
// $50 terminator for names of <10 length.
DLLEXPORT int rom_getmonrawname(ROM& rom, u8 species, u8 *dest) {
    u8 *data = rom["MonsterNames"];
    data += (u8)(species - 1) * MAX_SPECIES_NAME_LENGTH;
    if(dest) memcpy(dest, data, MAX_SPECIES_NAME_LENGTH);
    return MAX_SPECIES_NAME_LENGTH;
}

// Writes pokemon's decoded name to a 11 size character buffer. The $50 
// terminator is not written. For glitched pokemon, a string of format 'hexXX' is outputted
// instead with the exception of MISSINGNO.
DLLEXPORT void rom_getmonname(ROM& rom, u8 species, char *dest) {
    u8 rawname_buffer[MAX_SPECIES_NAME_LENGTH];
    rom_getmonrawname(rom, species, rawname_buffer);
    
    std::string name;
    if(is_glitch_mon(species)) {
        name = hex_string(species);
    } else {
        name = decode_string(rawname_buffer, MAX_SPECIES_NAME_LENGTH);
    }
    
    strcpy(dest, name.c_str());
}

// Returns the icon a pokemon will use in the party screen.
DLLEXPORT u8 rom_getmonicon(ROM& rom, u8 species) {
    return nybble_array(rom["MonPartyData"], rom_getpokedexnumber(rom, species - 1) - 1);
}

// Writes the dex entry of a pokemon. Requesting the dex entry of glitched pokemon
// will currently result in undefined behavior.
DLLEXPORT void rom_getmondexentry(ROM& rom, u8 species, DexEntry *dex_entry) {
    int dex_pointer = 0x100000 | ((u16 *) rom["PokedexEntryPointers"])[(u8) (species - 1)];
    u8 *data = rom[dex_pointer];
    std::string species_type = decode_string(data);
    strcpy(dex_entry->type, species_type.c_str());
    data += find_byte(data, 0x50) + 1;
    dex_entry->feet = *data++;
    dex_entry->inches = *data++;
    dex_entry->weight = *data++;
    dex_entry->weight += *data++ << 8;
    dex_entry->weight /= 10.0f;
    data++; // NOTE(stringflow): skip over text_far opcode
    int description_pointer = *data++ + 1; // NOTE(stringflow): skip over text opcode
    description_pointer |= *data++ << 8;
    description_pointer |= *data++ << 16;
    std::string description = decode_string(rom[description_pointer]);
    strcpy(dex_entry->description, description.c_str());
}

// Writes the pokemon's cry data. (base cry, pitch, length)
DLLEXPORT void rom_getmoncrydata(ROM& rom, u8 species, CryData *cry) {
    u8 *data = rom["CryData"] + (u8) (species - 1) * sizeof(CryData);
    memcpy(cry, data, sizeof(CryData));
}

// Writes the 2bpp representation of a pokemons sprite to a 784 size buffer.
// The front flag controlls whether the front sprite or the back sprite is written.
// The sprite can be vertically flipped with the flip flag.
// If the pokemons pic pointer points outside of the ROM address space, garbage data may be
// written by this routine. If the bounding box of the sprite has a width of zero, no data
// will be written since loading those sprites in-game crashes the game.
DLLEXPORT void rom_getmonsprite(ROM& rom, u8 species, bool front, bool flip, u8 *dest) {
    BaseStats stats;
    rom_getmonbasestats(rom, species, &stats);
    
    u8 bank;
    if(species == 0x15) {
        bank = 0x1;
    } else if(species == 0xb6) { 
        bank = 0xb;
    } else if (species < 0x1f) {
        bank = 0x9;
    } else if (species < 0x4a) {
        bank = 0xa;
    } else if (species < 0x74) {
        bank = 0xb;
    } else if (species < 0x99) {
        bank = 0xc;
    } else {
        bank = 0xd;
    }
    
    int pic_pointer = bank << 16 | (front ? stats.front_pic : stats.back_pic);
    pic_decompress(rom[pic_pointer], stats.front_pic_dimensions, dest, front, flip);
}

// Writes raw pcm data of a pokemons cry. The routine will output mono signed 16-bit samples
// at 1048576hz, or downsampled to 48000hz if the flag is set. Note that resampling the
// audio will cause a loss in the quality of high frequency noises.
// Returns the number of samples in the sound.
// If you do not know the number of samples in the sound, pass zero as the destination and
// use the return value to tell you how big the buffer should be.
// Any species <$bf is currently supported.
DLLEXPORT int rom_getmoncry(ROM& rom, u8 species, bool resample, s16 *dest) {
    CryData crydata;
    rom_getmoncrydata(rom, species, &crydata);
    
    u8 pitch = crydata.pitch;
    s8 time_stretch = crydata.length - 0x80;
    
    u8 *cry_header = rom["SFX_Cry00_1"] + crydata.base_cry * 9;
    u8 *pulse1_data = rom[0x20000 | *((u16 *) (cry_header + 1))];
    u8 *pulse2_data = rom[0x20000 | *((u16 *) (cry_header + 4))];
    u8 *noise_data  = rom[0x20000 | *((u16 *) (cry_header + 7))];
    
    return synthesize_cry(pulse1_data, pulse2_data, noise_data, pitch, time_stretch,
                          resample, dest);
}

// Writes raw pcm data of a pokemon yellow pikachu sound clip. The routine will write mono
// signed 16-bit samples at 23301hz, or upsampled to 48000hz if the flag is set.
// Returns the number of samples in the sound.
// If you do not know the number of samples in the sound, pass zero as the destination and
// use the return value to tell you how big the buffer should be.
// Pikachu sound indices range from 0 to 41 (inclusive).
DLLEXPORT int rom_getpikasound(ROM& rom, u8 sound_id, bool resample, s16 *dest) {
    if(rom.game != YELLOW) return 0;
    if(sound_id > 41) return 0;
    
    u8 *table = rom["PikachuCriesPointerTable"] + sound_id * 3;
    u8 bank = *table++;
    u16 address = *((u16 *) table);
    return synthesize_pika_sound(rom[bank << 16 | address], resample, dest);
}