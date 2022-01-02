void growthrate(std::string name, u8 growthRateId, int level, int expectedXp) {
    TEST(name,
         formatValue(expectedXp),
         formatValue(rom_calcxp(rom, growthRateId, level)));
}

void growthrateTests() {
    growthrate("mediumfast",           0, 50, 125000);
    growthrate("mediumslow",           3, 50, 117360);
    growthrate("mediumslow-underflow", 3,  1, 16777162);
    growthrate("fast",                 4, 50, 100000);
    growthrate("slow",                 5, 50, 156250);
}