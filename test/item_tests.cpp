void itemrawname(std::string name, u8 itemId, int expectedDataPointer) {
    rawnameTest(__FUNCTION__, name, itemId, expectedDataPointer, rom_getitemrawname);
}

void itemrawname(std::string name, u8 itemId) {
    rawnameTest(__FUNCTION__, name, itemId, rom_getitemrawname, rom_getitemname);
}

void itemname(std::string name, u8 itemId, std::string expectedName) {
    nameTest(__FUNCTION__, name, itemId, expectedName, rom_getitemname);
}

void itempointer(std::string name, u8 itemId, u16 expectedPointer) {
    u16 gotPointer = rom_getitemexecutionpointer(rom, itemId);
    TEST(name,
         formatValue(expectedPointer),
         formatValue(gotPointer));
}

void itempointer(std::string name, u8 itemId, std::string expectedPointerName) {
    itempointer(name, itemId, (u16) rom.symbols[expectedPointerName]);
}

void keyitem(std::string name, u8 itemId, bool expectedKeyItem) {
    TEST(name,
         formatValue(expectedKeyItem),
         formatValue(rom_iskeyitem(rom, itemId)));
}

void guarddrink(std::string name, u8 itemId, bool expectedGuardDrink) {
    TEST(name,
         formatValue(expectedGuardDrink),
         formatValue(rom_isguarddrink(rom, itemId)));
}

void itemprice(std::string name, u8 itemId, int expectedPrice) {
    int gotPrice = rom_getitemprice(rom, itemId);
    TEST(name,
         formatValue(expectedPrice),
         formatValue(gotPrice));
}

void itemTests() {
    itemrawname("masterball", 0x01, rom.game == Yellow ? 0x145b7 : 0x1472b);
    itemrawname("oaksparcel", 0x46, rom.game == Yellow ? 0x1486d : 0x149e1);
    itemrawname("hex00", 0x00,      rom.game == Yellow ? 0x17789 : 0x17b22);
    itemrawname("hex62", 0x62,      rom.game == Yellow ? 0x1491e : 0x14a92);
    itemrawname("hex9e", 0x9e,      rom.game == Yellow ? 0x16460 : 0x16701);
    itemrawname("hexc3", 0xc3,      rom.game == Yellow ? 0x16d38 : 0x16fb6);
    itemrawname("hm01", 0xc4);
    itemrawname("hm05", 0xc8);
    itemrawname("tm01", 0xc9);
    itemrawname("tm55", 0xff);
    
    itemname("masterball", 0x01, "MASTER BALL");
    itemname("oaksparcel", 0x46, "OAK's PARCEL");
    itemname("hm01", 0xc4, "HM01");
    itemname("hm05", 0xc8, "HM05");
    itemname("tm01", 0xc9, "TM01");
    itemname("tm55", 0xff, "TM55");
    itemname("hex00", 0x00, "hex00");
    itemname("hex62", 0x62, "hex62");
    itemname("hexc3", 0xc3, "hexc3");
    
    itempointer("masterball", 0x01, "ItemUseBall");
    itempointer("surfboard", 0x07, "ItemUseSurfboard");
    itempointer("hex00", 0x00, rom.game == Yellow ? 0xfe06 : 0x01d1);
    itempointer("hex5d", 0x5d, rom.game == Yellow ? 0x04fe : 0xd163);
    itempointer("hm01", 0xc4, "ItemUseTMHM");
    itempointer("tm55", 0xff, "ItemUseTMHM");
    
    guarddrink("masterball", 0x01, false);
    guarddrink("freshwater", 0x3c, true);
    guarddrink("sodapop", 0x3d, true);
    guarddrink("lemonade", 0x3e, true);
    guarddrink("hex00", 0x00, false);
    guarddrink("hexff", 0xff, false);
    
    keyitem("masterball", 0x01, false);
    keyitem("ultraball", 0x02, false);
    keyitem("townmap", 0x05, true);
    keyitem("rarecandy", 0x28, false);
    keyitem("goldteeth", 0x40, true);
    
    itemprice("masterball", 0x01, 0);
    itemprice("ultraball", 0x02, 1200);
    itemprice("nugget", 0x31, 10000);
    itemprice("freshwater", 0x3c, 200);
    itemprice("sodapop", 0x3d, 300);
    itemprice("lemonade", 0x3e, 350);
    itemprice("hex00", 0x00, 829850);
    itemprice("hm01", 0xc4, 0);
    itemprice("hm05", 0xc8, 0);
    itemprice("tm01", 0xc9, 3000);
    itemprice("tm02", 0xca, 2000);
    itemprice("tm03", 0xcb, 2000);
    itemprice("tm50", 0xfa, 2000);
    itemprice("tm55", 0xff, rom.game == Yellow ? 4000 : 0);
}