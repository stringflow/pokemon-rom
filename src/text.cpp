constexpr u8 HM01 = 0xc4;
constexpr u8 TM01 = 0xc9;
constexpr int MACHINE_NAME_SIZE = 5; 

struct MachineName {
    u8 prefix[2];
    u8 number;
};

struct Charmap {
    char charset[256*4];
    char *map[256];
};

static Charmap charmap = []() {
    const char delimiter = '|';
    Charmap result;
    
    strcpy(result.charset,
           "#|#|#|#|#|#|#|#|#|#|#|#|#|#|#| |"
           "*|#|#|#|#|#|#|#|#|#|#|#|#|#|#|#|"
           "#|#|#|#|#|#|#|#|#|#|#|#|#|#|#|#|"
           "#|#|#|#|#|#|#|#|#|#|#|#|#|#|#|#|"
           "#|#|#|#|#|#|#|#|#|#|#|#|#|#|#|#|"
           "#|#|#|#|#|#|#|#|#| |PKMN|_|_|#| |#|"
           "*|_|<player>|<rival>|POKé|_|……|_|_|<target>|<attacker>|PC|TM|TRAINER|ROCKET|.|"
           "#|#|#|#|#|#|#|#|#|#|#|#|#|#|#|#|"
           "#|#|#|#|#|#|#|#|#|#|#|#|#|#|#| |"
           "A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|"
           "Q|R|S|T|U|V|W|X|Y|Z|(|)|:|;|[|]|"
           "a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|"
           "q|r|s|t|u|v|w|x|y|z|é|'d|'l|'s|'t|'v|"
           " | | | | | | | | | | | | | | | |"
           " | | | | | | | | | | | | | | | |"
           "'|PK|MN|-|'r|'m|?|!|.|ァ|ゥ|ェ|>|>|V|♂|"
           "$|x|.|/|,|♀|0|1|2|3|4|5|6|7|8|9");
    
    int delimiter_count = 0;
    for(const char *at = result.charset; *at; at++) {
        if(*at == delimiter) {
            delimiter_count++;
        }
    }
    
    int current_byte = 255 - delimiter_count;
    char *at = result.charset;
    while(*at) {
        char *end = at;
        while(*end && *end != delimiter) end++;
        *end = 0;
        result.map[current_byte++] = at;
        at = end+1;
    }
    
    return result;
} ();

void decode_string(char *dest, u8 *data, int max_length = 65535) {
    u8 character;
    for(int i = 0; (character = data[i]) != 0x50 && i < max_length; i++) {
        for(char *at = charmap.map[character]; *at; at++) {
            *dest++ = *at;
        }
    }
    
    *dest = 0;
}

void hex_string(char *dest, u8 value) {
    sprintf(dest, "hex%02x", value);
}

bool is_machine(u8 index) {
    return index >= HM01;
}

bool is_technical_machine(u8 index) {
    return index >= TM01;
}

bool is_hidden_machine(u8 index) {
    return is_machine(index) && !is_technical_machine(index);
}

u8 encode_digit(int digit) {
    return 0xf6 + digit;
}

// NOTE(stringflow): BUG: This applies to all names instead of just items
MachineName get_machine_name(u8 index) {
    MachineName result = {};
    u8 offset = 0;
    if(is_technical_machine(index+1)) {
        // NOTE(stringflow): the machine is a technical machine
        result.prefix[0] = 0x93; // T
        offset = TM01;
    } else {
        // NOTE(stringflow): the machine is a hidden machine
        result.prefix[0] = 0x87; // H
        offset = HM01;
    }
    
    result.prefix[1] = 0x8c;
    result.number = index - offset + 2;
    
    return result;
}

int get_list_element_offset(u8 *list_start, u8 index) {
    u8 *list = list_start;
    for(int i = 0; i < index; i++) {
        list += find_byte(list, 0x50) + 1;
    }
    return list - list_start;
}

// TODO(stringflow): should this write into a char * directly?
void get_list_element(char *dest, u8 *list_start, u8 index) {
    if(is_machine(index+1)) {
        MachineName machine_name = get_machine_name(index);
        char prefix[ARRAY_SIZE(machine_name.prefix)];
        decode_string(prefix, machine_name.prefix, ARRAY_SIZE(machine_name.prefix));
        sprintf(dest, "%s%02d", prefix, machine_name.number);
    } else {
        int offset = get_list_element_offset(list_start, index);
        decode_string(dest, list_start + offset);
    }
}

int get_list_element(u8 *dest, u8 *list_start, u8 index) {
    if(is_machine(index+1)) {
        if(dest) {
            MachineName machine_name = get_machine_name(index);
            memcpy(dest, machine_name.prefix, 2);
            dest[2] = encode_digit(machine_name.number / 10);
            dest[3] = encode_digit(machine_name.number % 10);
            dest[4] = 0x50;
        }
        return MACHINE_NAME_SIZE;
    } else {
        u8 *list = list_start + get_list_element_offset(list_start, index);
        int size = find_byte(list, 0x50) + 1;
        if(dest) memcpy(dest, list, size);
        return size;
    }
}