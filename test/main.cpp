#include "..\src\rom.cpp"
#include <time.h>

typedef int (*RawNameFn)(ROM&, u8, u8*);
typedef void (*NameFn)(ROM&, u8, char*);

static ROM rom;
static int numPassed = 0;
static int numFailed = 0;
static std::string globalTestPrefix = "";

u32 crc32(u8 *data, int count) {
    u32 crc = 0xffffffff;
    for(int i = 0; i < count; i++) {
        crc ^= data[i];
        for(int j = 0; j < 8; j++) {
            u32 t = ~((crc & 1) - 1);
            crc = (crc >> 1) ^ (0xedb88320 & t);
        }
    }
    
    return ~crc;
}

std::string formatBytes(void *src, int size) {
    std::string ret = "";
    u8 *data = (u8 *) src;
    for(int i = 0; i < size; i++) {
        char buffer[6];
        sprintf(buffer, "$%02x, ", data[i]);
        ret += std::string(buffer);
    }
    return ret.substr(0, ret.length()-2);
}

std::string formatValue(int value) {
    char buffer[5];
    sprintf(buffer, "$%04x", value);
    return std::string(buffer);
}

#define TEST(name, expected, got) testInternal(std::string(__FUNCTION__) + "-" + name, expected, got);
void testInternal(std::string name, std::string expected, std::string got) {
    printf("%s%s ... ", globalTestPrefix.c_str(), name.c_str());
    if(expected == got) {
        printf("OK\n");
        numPassed++;
    } else {
        printf("FAILED\n  expected=%s\n  got=     %s\n", 
               expected.c_str(), 
               got.c_str());
        numFailed++;
    }
}

void rawnameTest(const char *prefix, std::string name, u8 id, int expectedDataPointer, RawNameFn fn) {
    int size = fn(rom, id, nullptr); 
    u8 rawname[size];
    fn(rom, id, rawname);
    testInternal(std::string(prefix) + "-" + name,
                 formatBytes(rom[expectedDataPointer], size),
                 formatBytes(rawname, size));
}

void rawnameTest(const char *prefix, std::string name, u8 id, RawNameFn rawnameFn, NameFn nameFn) {
    int size = rawnameFn(rom, id, nullptr); 
    u8 rawname[size];
    rawnameFn(rom, id, rawname);
    
    char expectedName[13];
    nameFn(rom, id, expectedName);
    
    testInternal(std::string(prefix) + "-" + name,
                 std::string(expectedName), 
                 decodeString(rawname));
}

void nameTest(const char *prefix, std::string name, u8 id, std::string expectedName, NameFn fn) {
    char gotName[13];
    fn(rom, id, gotName);
    testInternal(std::string(prefix) + "-" + name,
                 expectedName, 
                 gotName);
}

#include "growthrate_tests.cpp"
#include "type_tests.cpp"
#include "pokemon_tests.cpp"
#include "item_tests.cpp"
#include "trainerclass_tests.cpp"
#include "move_tests.cpp"
#include "map_tests.cpp"
#include "tileset_tests.cpp"

int main() {
    int games[] = { 
        Red,
        Blue,
        Yellow,
    };
    
    clock_t start = clock();
    for(int i = 0; i < ARRAY_LENGTH(games); i++) {
        int game = games[i];
        std::string gameName = "";
        switch(game) {
            case Red: gameName = "red"; break;
            case Blue: gameName = "blue"; break;
            case Yellow: gameName = "yellow"; break;
        };
        
        globalTestPrefix = gameName + "-";
        
        char romfile[256];
        char symfile[256];
        sprintf(romfile, "..\\test\\roms\\poke%s.gbc", gameName.c_str());
        sprintf(symfile, "..\\test\\symbols\\poke%s.sym", gameName.c_str());
        ROM *romptr = rom_load(romfile, symfile);
        rom = *romptr;
        growthrateTests();
        typeTests();
        pokemonTests();
        itemTests();
        trainerclassTests();
        moveTests();
        mapTests();
        tilesetTests();
        rom_free(romptr);
    }
    
    clock_t end = clock();
    f32 elapsed = (f32)(end - start) / (f32) CLOCKS_PER_SEC;
    
    printf("\n");
    printf("%d tests run in %f seconds.\n", numPassed + numFailed, elapsed);
    printf("%d FAILED (%d tests passed)\n", numFailed, numPassed);
    
    return 0;
}