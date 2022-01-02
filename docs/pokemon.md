# pokemon.cpp
### rom_getmonbasestats
`(ROM &rom, u8 species, BaseStats *stats) -> void`  
Writes the pokemon's basestats.
### rom_getmoncry
`(ROM& rom, u8 species, bool resample, s16 *dest) -> int`  
Writes raw pcm data of a pokemons cry. The routine will output mono signed 16-bit samples at 1048576hz, or downsampled to 48000hz if the flag is set. Note that resampling the audio will cause a loss in the quality of high frequency noises.  
Returns the number of samples in the sound.  
If you do not know the number of samples in the sound, pass zero as the destination and use the return value to tell you how big the buffer should be.  
Any species <$bf is currently supported.
### rom_getmoncrydata
`(ROM& rom, u8 species, CryData *cry) -> void`  
Writes the pokemon's cry data. (base cry, pitch, length)
### rom_getmondexentry
`(ROM& rom, u8 species, DexEntry *dexentry) -> void`  
Writes the dex entry of a pokemon. Requesting the dex entry of glitched pokemon will currently result in undefined behavior.
### rom_getmonicon
`(ROM& rom, u8 species) -> u8`  
Returns the icon a pokemon will use in the party screen.
### rom_getmonname
`(ROM& rom, u8 species, char *dest) -> void`  
Writes pokemon's decoded name to a 11 size character buffer. The $50 terminator is not written. For glitched pokemon, a string of format 'hexXX' is outputted instead with the exception of MISSINGNO.
### rom_getmonrawname
`(ROM& rom, u8 species, u8 *dest) -> int`  
Writes pokemon's raw name to a 10 size byte buffer. It will only include the $50 terminator for names of <10 length.
### rom_getmonsprite
`(ROM& rom, u8 species, bool front, bool flip, u8 *dest) -> void`  
Writes the 2bpp representation of a pokemons sprite to a 784 size buffer.  
The front flag controlls whether the front sprite or the back sprite is written.  
The sprite can be vertically flipped with the flip flag.  
If the pokemons pic pointer points outside of the ROM address space, garbage data may be written by this routine. If the bounding box of the sprite has a width of zero, no data will be written since loading those sprites in-game crashes the game.
### rom_getpikasound
`(ROM& rom, u8 soundId, bool resample, s16 *dest) -> int`  
Writes raw pcm data of a pokemon yellow pikachu sound clip. The routine will write mono signed 16-bit samples at 23301hz, or upsampled to 48000hz if the flag is set.  
Returns the number of samples in the sound.  
If you do not know the number of samples in the sound, pass zero as the destination and use the return value to tell you how big the buffer should be.  
Pikachu sound indices range from 0 to 41 (inclusive).
### rom_getpokedexnumber
`(ROM& rom, u8 species) -> u8`  
Returns the pokedex number of a pokemon.
