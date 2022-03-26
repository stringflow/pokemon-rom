void growthrate(std::string name, u8 growth_rate_id, int level, int expected_xp) {
    TEST(name,
         format_value(expected_xp),
         format_value(rom_calcxp(rom, growth_rate_id, level)));
}

void growthrate_tests() {
    growthrate("mediumfast",           0, 50, 125000);
    growthrate("mediumslow",           3, 50, 117360);
    growthrate("mediumslow-underflow", 3,  1, 16777162);
    growthrate("fast",                 4, 50, 100000);
    growthrate("slow",                 5, 50, 156250);
}