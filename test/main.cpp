#include "..\src\rom.cpp"
#include <time.h>

typedef int (*RawNameFn)(ROM&, u8, u8*);
typedef void (*NameFn)(ROM&, u8, char*);

static ROM rom;
static int num_passed = 0;
static int num_failed = 0;
static std::string global_test_prefix = "";

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

std::string format_bytes(void *src, int size) {
    std::string ret = "";
    u8 *data = (u8 *) src;
    for(int i = 0; i < size; i++) {
        char buffer[6];
        sprintf(buffer, "$%02x, ", data[i]);
        ret += std::string(buffer);
    }
    return ret.substr(0, ret.length()-2);
}

std::string format_value(int value) {
    char buffer[5];
    sprintf(buffer, "$%04x", value);
    return std::string(buffer);
}

#define TEST(name, expected, got) test_internal(std::string(__FUNCTION__) + "-" + name, expected, got);
void test_internal(std::string name, std::string expected, std::string got) {
    printf("%s%s ... ", global_test_prefix.c_str(), name.c_str());
    if(expected == got) {
        printf("OK\n");
        num_passed++;
    } else {
        printf("FAILED\n  expected=%s\n  got=     %s\n", 
               expected.c_str(), 
               got.c_str());
        num_failed++;
    }
}

void rawname_test(const char *prefix, std::string name, u8 id, int expected_data_pointer, RawNameFn fn) {
    int size = fn(rom, id, nullptr); 
    u8 rawname[size];
    fn(rom, id, rawname);
    test_internal(std::string(prefix) + "-" + name,
                  format_bytes(rom[expected_data_pointer], size),
                  format_bytes(rawname, size));
}

void rawname_test(const char *prefix, std::string name, u8 id, RawNameFn rawnamefn, NameFn namefn) {
    int size = rawnamefn(rom, id, nullptr); 
    u8 rawname[size];
    rawnamefn(rom, id, rawname);
    
    char expected_name[13];
    namefn(rom, id, expected_name);
    
    test_internal(std::string(prefix) + "-" + name,
                  std::string(expected_name), 
                  decode_string(rawname));
}

void name_test(const char *prefix, std::string name, u8 id, std::string expected_name, NameFn fn) {
    char got_name[13];
    fn(rom, id, got_name);
    test_internal(std::string(prefix) + "-" + name,
                  expected_name, 
                  got_name);
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
        RED,
        BLUE,
        YELLOW,
    };
    
    ROM *romptr = rom_create();
    clock_t start = clock();
    for(int i = 0; i < ARRAY_LENGTH(games); i++) {
        int game = games[i];
        
        std::string game_name = "";
        switch(game) {
            case RED: game_name = "red"; break;
            case BLUE: game_name = "blue"; break;
            case YELLOW: game_name = "yellow"; break;
        };
        global_test_prefix = game_name + "-";
        
        char romfile[256];
        char symfile[256];
        sprintf(romfile, "..\\test\\roms\\poke%s.gbc", game_name.c_str());
        sprintf(symfile, "..\\test\\symbols\\poke%s.sym", game_name.c_str());
        RomLoadResult res = rom_load(*romptr, romfile, symfile);
        if(res == OK) {
            rom = *romptr;
            growthrate_tests();
            type_tests();
            pokemon_tests();
            item_tests();
            trainerclass_tests();
            move_tests();
            map_tests();
            tileset_tests();
        } else if(res == IO_ERROR) {
            printf("Unable to test %s: IO Error.\n", game_name.c_str());
        } else if(res == BAD_FORMAT) {
            printf("Unable to test %s: bad format.\n", game_name.c_str());
        } else if(res == UNSUPPORTED_GAME) {
            printf("Unable to test %s: unsupported game.\n", game_name.c_str());
        }
    }
    
    clock_t end = clock();
    f32 elapsed = (f32)(end - start) / (f32) CLOCKS_PER_SEC;
    
    printf("\n");
    int total_tests_ran = num_passed + num_failed;
    printf("%d tests run in %f seconds.\n", total_tests_ran, elapsed);
    printf("%d FAILED (%d tests passed)\n", num_failed, num_passed);
    
    if(total_tests_ran == 0) {
        printf("No tests ran! Please place working ROM images in test/rom, and their corresponding symbol files in test/symbols. (Create the folders if they do not exist yet)\n");
    }
    
    rom_free(romptr);
    return 0;
}