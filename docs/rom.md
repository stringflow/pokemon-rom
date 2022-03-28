# rom.cpp
### rom_create
`() -> ROM*`  
Creates a ROM reference.
### rom_free
`(ROM *rom) -> void`  
Frees a ROM reference.
### rom_load
`(ROM& rom, const char *romfile) -> RomLoadResult`  
Loads the ROM image. Returns 0 on success, a positive number on error.
