# rom.cpp
### rom_create
`() -> ROM*`  
Creates a ROM reference.
### rom_free
`(ROM *rom) -> void`  
Frees a ROM reference.
### rom_load
`(ROM& rom, const char *romfile, const char *symbolsfile) -> RomLoadResult`  
Loads the ROM image and processes the symbols file. Returns 0 on success, a positive number on error.
