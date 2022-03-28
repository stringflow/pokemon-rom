#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <map>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

typedef int(*SymbolLookup)(const char *); 
int symbol_lookup_red(const char *label);
int symbol_lookup_blue(const char *label);
int symbol_lookup_yellow(const char *label);
int symbol_lookup_gold(const char *label);
int symbol_lookup_silver(const char *label);
int symbol_lookup_crystal(const char *label);

#define DLLEXPORT extern "C" __declspec(dllexport)

enum Game {
    NONE,
    RED,
    BLUE,
    YELLOW,
    GOLD,
    SILVER,
    CRYSTAL
};

enum RomLoadResult {
    OK,
    IO_ERROR,
    BAD_FORMAT,
    UNSUPPORTED_GAME
};

struct ROM {
    u8 *contents;
    SymbolLookup symbol_lookup;
    Game game;
    
    u8* operator[](const char *label) {
        return contents + symbol_lookup(label);
    }
    
    u8* operator[](int offset) {
        return contents + offset;
    }
};

#include "utility.cpp"
#include "pic.cpp"
#include "synthesizer.cpp"
#include "text.cpp"
#include "wlastmap.cpp"
#pragma pack(push, 1)
#include "growthrate.cpp"
#include "type.cpp"
#include "pokemon.cpp"
#include "item.cpp"
#include "trainerclass.cpp"
#include "move.cpp"
#include "tileset.cpp"
#include "map.cpp"
#include "collision.cpp"
#pragma pack(pop)

constexpr int BANK_SIZE = 0x4000;
constexpr int NUM_PADDED_BANKS = 0x100;
constexpr int ROM_SIZE = BANK_SIZE * 4 * NUM_PADDED_BANKS;

void pad_rom(u8 *dest, u8 *src, int num_banks) {
    for(int bank = 0; bank < NUM_PADDED_BANKS; bank++) {
        memcpy(dest, src, BANK_SIZE); // home
        dest += BANK_SIZE;
        memcpy(dest, src + (bank % num_banks) * BANK_SIZE, BANK_SIZE); // bank n
        dest += BANK_SIZE;
        memset(dest, 0xff, BANK_SIZE); // vram/sram
        dest += BANK_SIZE;
        memset(dest, 0x00, BANK_SIZE); // wram
        dest += BANK_SIZE;
    }
}

RomLoadResult read_rom_file(u8 *rom, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if(!file) return IO_ERROR;
    
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    u8 *file_contents = new u8[file_size];
    fread(file_contents, 1, file_size, file);
    fclose(file);
    
    pad_rom(rom, file_contents, file_size / BANK_SIZE);
    delete[] file_contents;
    
    return OK;
}

RomLoadResult parse_game_title(ROM& rom) {
    char title[0x11];
    memcpy(title, rom.contents + 0x134, 0x10);
    title[title[0xf] & 0x80 ? 0xf : 0x10] = '\0';
    
    if(strcmp(title, "POKEMON RED") == 0) {
        rom.game = RED;
        rom.symbol_lookup = &symbol_lookup_red;
    } else if(strcmp(title, "POKEMON BLUE") == 0) {
        rom.game = BLUE;
        rom.symbol_lookup = &symbol_lookup_blue;
    } else if(strcmp(title, "POKEMON YELLOW") == 0) {
        rom.game = YELLOW;
        rom.symbol_lookup = &symbol_lookup_yellow;
    } else if(strcmp(title, "POKEMON GOLD") == 0) {
        rom.game = GOLD;
        rom.symbol_lookup = &symbol_lookup_gold;
    } else if(strcmp(title, "POKEMON SILVER") == 0) {
        rom.game = SILVER;
        rom.symbol_lookup = &symbol_lookup_silver;
    } else if(strcmp(title, "POKEMON CRYSTAL") == 0) {
        rom.game = CRYSTAL;
        rom.symbol_lookup = &symbol_lookup_crystal;
    } else {
        return UNSUPPORTED_GAME;
    }
    
    return OK;
}

// Creates a ROM reference.
DLLEXPORT ROM* rom_create() {
    ROM *rom = new ROM();
    rom->contents = new u8[ROM_SIZE];
    rom->game = NONE;
    return rom;
}

// Loads the ROM image. Returns 0 on success, a positive number on error.
DLLEXPORT RomLoadResult rom_load(ROM& rom, const char *romfile) {
    RomLoadResult result = OK;
    if((result = read_rom_file(rom.contents, romfile)) != OK) return result;
    if((result = parse_game_title(rom)) != OK) return result;
    return result;
}

// Frees a ROM reference.
DLLEXPORT void rom_free(ROM *rom) {
    if(rom->contents) delete[] rom->contents;
    if(rom) delete rom;
}