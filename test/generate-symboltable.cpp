#define ARRAY_SIZE(array) ((sizeof(array) / sizeof(array[0])))
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    const char *file_header = R"(// NOTE(stringflow): generated code
constexpr unsigned long long hash(const char *src, int off = 0) {
    return !src[off] ? 5381 : (hash(src, off+1)*33) ^ src[off]; 
}
)";
    
    FILE *out = fopen("..\\sym\\symbol-table.cpp", "w");
    fprintf(out, file_header);
    
    for(int i = 1; i < argc; i++) {
        char *game_name = argv[i];
        char file_path[256];
        sprintf(file_path, "..\\test\\symfiles\\poke%s.sym", argv[i]);
        
        fprintf(out, "\nint symbol_lookup_%s(const char *label) {\n", game_name);
        fprintf(out, "    switch(hash(label)) {\n");
        
        FILE *in = fopen(file_path, "r");
        char line[256];
        char label[256];
        int bank;
        int addr;
        while(fgets(line, ARRAY_SIZE(line), in)) {
            if(*line == ';') continue;
            
            line[strlen(line) - 1] = 0;
            
            sscanf(line, "%02x:%04x %s", &bank, &addr, label);
            fprintf(out, "    case hash(\"%s\"): return 0x%08x;\n", 
                    label, bank << 16 | addr);
        }
        fclose(in);
        
        fprintf(out, "        default: return 0;\n");
        fprintf(out, "    };\n");
        fprintf(out, "}\n");
    }
    
    fclose(out);
}