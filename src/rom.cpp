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

#define DLLEXPORT extern "C" __declspec(dllexport)

enum Game {
    None,
    Red,
    Blue,
    Yellow,
    Gold,
    Silver,
    Crystal
};

enum RomLoadResult {
    Ok,
    IOError,
    BadFormat,
    UnsupportedGame
};

struct ROM {
    u8 *contents;
    std::map<std::string, int> symbols;
    Game game;
    
    u8* operator[](std::string label) {
        return contents + symbols[label];
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

void padROM(u8 *dest, u8 *src, int numBanks) {
    for(int bank = 0; bank < NUM_PADDED_BANKS; bank++) {
        memcpy(dest, src, BANK_SIZE); // home
        dest += BANK_SIZE;
        memcpy(dest, src + (bank % numBanks) * BANK_SIZE, BANK_SIZE); // bank n
        dest += BANK_SIZE;
        memset(dest, 0xff, BANK_SIZE); // vram/sram
        dest += BANK_SIZE;
        memset(dest, 0x00, BANK_SIZE); // wram
        dest += BANK_SIZE;
    }
}

u8* readROMFile(const char *filename) {
    FILE *file = fopen(filename, "rb");
    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    u8 *fileContents = new u8[fileSize];
    fread(fileContents, fileSize, 1, file);
    fclose(file);
    
    u8 *rom = new u8[ROM_SIZE];
    padROM(rom, fileContents, fileSize / BANK_SIZE);
    delete[] fileContents;
    
    return rom;
}

std::map<std::string, int> readSymbolsFile(const char *filename) {
    std::map<std::string, int> result;
    
    FILE *file = fopen(filename, "r");
    int lineLength = 256;
    char line[lineLength];
    while(fgets(line, lineLength, file)) {
        if(line[0] == ';') continue;
        line[strlen(line)-1] = 0;
        
        char *label;
        u8 bank = strtol(line, NULL, 16);
        u16 addr = strtol(line+3, &label, 16);
        
        result[std::string(label+1)] = bank << 16 | addr;
    }
    
    return result;
}

Game parseGameTitle(u8 *rom) {
    char title[0x11];
    memcpy(title, rom + 0x134, 0x10);
    title[title[0xf] & 0x80 ? 0xf : 0x10] = '\0';
    
    if(strcmp(title, "POKEMON RED") == 0) return Red;
    else if(strcmp(title, "POKEMON BLUE") == 0) return Blue;
    else if(strcmp(title, "POKEMON YELLOW") == 0) return Yellow;
    else if(strcmp(title, "POKEMON GOLD") == 0) return Gold;
    else if(strcmp(title, "POKEMON SILVER") == 0) return Silver;
    else if(strcmp(title, "POKEMON CRYSTAL") == 0) return Crystal;
    else return None;
}

// TODO(stringflow): Implement error handling
//                  ( 0) success
//                  (-1) file does not exist
//                  (-2) bad format
//                  (-3) unsupported game/platform
DLLEXPORT ROM* rom_load(const char *romfile, const char *symbolsfile) {
    ROM *result = new ROM();
    result->contents = readROMFile(romfile);
    result->symbols = readSymbolsFile(symbolsfile);
    result->game = parseGameTitle(result->contents);
    
    return result;
}

DLLEXPORT void rom_free(ROM *rom) {
    if(rom->contents) delete[] rom->contents;
    if(rom) delete rom;
}