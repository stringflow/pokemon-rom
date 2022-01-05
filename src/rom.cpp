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

RomLoadResult readROMFile(u8 *rom, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if(!file) return IOError;
    
    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    u8 *fileContents = new u8[fileSize];
    fread(fileContents, fileSize, 1, file);
    fclose(file);
    
    padROM(rom, fileContents, fileSize / BANK_SIZE);
    delete[] fileContents;
    
    return Ok;
}

RomLoadResult readSymbolsFile(std::map<std::string, int> *symbols, const char *filename) {
    FILE *file = fopen(filename, "r");
    if(!file) return IOError;
    
    symbols->clear();
    
    int lineLength = 256;
    char line[lineLength];
    
    int bank;
    int addr;
    char label[256];
    while(fgets(line, lineLength, file)) {
        if(line[0] == ';') continue;
        
        if(sscanf(line, "%02x:%04x %s", &bank, &addr, label) == 3) {
            symbols->insert(std::make_pair(std::string(label), bank << 16 | addr));
        } else {
            return BadFormat;
        }
    }
    
    return Ok;
}

RomLoadResult parseGameTitle(Game *game, u8 *rom) {
    char title[0x11];
    memcpy(title, rom + 0x134, 0x10);
    title[title[0xf] & 0x80 ? 0xf : 0x10] = '\0';
    
    if(strcmp(title, "POKEMON RED") == 0) *game = Red;
    else if(strcmp(title, "POKEMON BLUE") == 0) *game = Blue;
    else if(strcmp(title, "POKEMON YELLOW") == 0) *game = Yellow;
    else if(strcmp(title, "POKEMON GOLD") == 0) *game = Gold;
    else if(strcmp(title, "POKEMON SILVER") == 0) *game = Silver;
    else if(strcmp(title, "POKEMON CRYSTAL") == 0) *game = Crystal;
    else return UnsupportedGame;
    
    return Ok;
}

// Creates a ROM reference.
DLLEXPORT ROM* rom_create() {
    ROM *rom = new ROM();
    rom->contents = new u8[ROM_SIZE];
    rom->symbols = std::map<std::string, int>();
    rom->game = None;
    return rom;
}

// Loads the ROM image and processes the symbols file. Returns 0 on success, a positive
// number on error.
DLLEXPORT RomLoadResult rom_load(ROM& rom, const char *romfile, const char *symbolsfile) {
    RomLoadResult result = Ok;
    if((result = readROMFile(rom.contents, romfile)) != Ok) return result;
    if((result = readSymbolsFile(&rom.symbols, symbolsfile)) != Ok) return result;
    if((result = parseGameTitle(&rom.game, rom.contents)) != Ok) return result;
    return result;
}

// Frees a ROM reference.
DLLEXPORT void rom_free(ROM *rom) {
    if(rom->contents) delete[] rom->contents;
    if(rom) delete rom;
}