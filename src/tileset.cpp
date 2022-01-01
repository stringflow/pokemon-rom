struct Tileset {
    u8 bank;
    u16 blockPointer;
    u16 gfxPointer;
    u16 collisionPointer;
    u8 counterTiles[3];
    u8 grassTile;
    u8 animations;
};

// Writes the tileset's header data.
DLLEXPORT void rom_gettileset(ROM& rom, u8 tilesetId, Tileset *tileset) {
    u8 *data = rom["Tilesets"] + tilesetId * sizeof(Tileset);
    memcpy(tileset, data, sizeof(Tileset));
}