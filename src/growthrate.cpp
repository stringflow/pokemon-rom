struct GrowthRate {
    u8 cubic;
    u8 quadradic;
    u8 linear;
    u8 constant;
};

// Returns the total xp needed to reach the desired level.
DLLEXPORT int rom_calcxp(ROM& rom, u8 growth_rate_id, int level) {
    GrowthRate func = ((GrowthRate *) rom["GrowthRateTable"])[growth_rate_id];
    // NOTE(stringflow): xp calculation is a cubic function
    return (int) floor((f32) (func.cubic >> 4) / (func.cubic & 0xf) * level * level * level +
                       to_signed8(func.quadradic) * level * level +
                       func.linear * level -
                       func.constant) & 0xffffff;
}

// Returns the relative xp needed for the next level.
DLLEXPORT int rom_calcxptolevelup(ROM& rom, u8 growth_rate_id, int level) {
    return (rom_calcxp(rom, growth_rate_id, level + 1) - 
            rom_calcxp(rom, growth_rate_id, level));
}