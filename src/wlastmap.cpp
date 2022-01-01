// NOTE(stringflow): keys are (mapId << 8 | warpIndex)
static std::map<u16, u8> rbLastMapDestinations = {
    { 0x2e01, 0x0d },
    { 0x5402, 0x16 },
    { 0x2e00, 0x0d },
    { 0x5403, 0x16 },
    { 0x5400, 0x16 },
    { 0x5401, 0x16 },
    { 0xc401, 0x05 },
    { 0xc400, 0x05 },
    { 0x3401, 0x02 },
    { 0x3400, 0x02 },
    { 0x5500, 0x16 },
    { 0x3403, 0x02 },
    { 0x5501, 0x16 },
    { 0x3402, 0x02 },
    { 0x3701, 0x02 },
    { 0x3700, 0x02 },
    { 0x3c07, 0x0f },
    { 0x3e00, 0x03 },
    { 0x4500, 0x03 },
    { 0x3e01, 0x03 },
    { 0x4501, 0x03 },
    { 0x3e02, 0x03 },
    { 0x4502, 0x03 },
    { 0x2d01, 0x01 },
    { 0x2d00, 0x01 },
    { 0x4000, 0x03 },
    { 0x9800, 0x07 },
    { 0x4001, 0x03 },
    { 0x9801, 0x07 },
    { 0x8e00, 0x04 },
    { 0x8e01, 0x04 },
    { 0x9701, 0x04 },
    { 0x9700, 0x04 },
    { 0x4e00, 0x12 },
    { 0x4e01, 0x12 },
    { 0x3901, 0x02 },
    { 0x3900, 0x02 },
    { 0xab01, 0x08 },
    { 0xb501, 0x0a },
    { 0xab00, 0x08 },
    { 0xb500, 0x0a },
    { 0x9b00, 0x07 },
    { 0x9b01, 0x07 },
    { 0x9501, 0x04 },
    { 0x9500, 0x04 },
    { 0x4201, 0x03 },
    { 0x4200, 0x03 },
    { 0xbd01, 0x17 },
    { 0xbd00, 0x17 },
    { 0x4f01, 0x13 },
    { 0x8002, 0x06 },
    { 0x4f00, 0x13 },
    { 0x8001, 0x06 },
    { 0xb701, 0x0a },
    { 0x4f03, 0x13 },
    { 0x8000, 0x06 },
    { 0xb700, 0x0a },
    { 0x4f02, 0x13 },
    { 0xac01, 0x08 },
    { 0xad01, 0x08 },
    { 0xac00, 0x08 },
    { 0xad00, 0x08 },
    { 0x9901, 0x07 },
    { 0x9900, 0x07 },
    { 0x5703, 0x17 },
    { 0x5702, 0x17 },
    { 0x5701, 0x17 },
    { 0x5700, 0x17 },
    { 0x5e00, 0x05 },
    { 0xa507, 0x08 },
    { 0xc202, 0x22 },
    { 0xa506, 0x08 },
    { 0xc201, 0x22 },
    { 0xa503, 0x08 },
    { 0xa502, 0x08 },
    { 0xa501, 0x08 },
    { 0xa500, 0x08 },
    { 0xaf01, 0x0a },
    { 0xaf00, 0x0a },
    { 0x3601, 0x02 },
    { 0x3600, 0x02 },
    { 0x4101, 0x03 },
    { 0x4100, 0x03 },
    { 0x9d00, 0x07 },
    { 0x9d01, 0x07 },
    { 0xa701, 0x08 },
    { 0xa700, 0x08 },
    { 0x3a00, 0x02 },
    { 0x3a01, 0x02 },
    { 0x4603, 0x10 },
    { 0x4602, 0x10 },
    { 0x4601, 0x10 },
    { 0x4600, 0x10 },
    { 0x5301, 0x15 },
    { 0x5300, 0x15 },
    { 0x5302, 0x15 },
    { 0x8600, 0x06 },
    { 0x8601, 0x06 },
    { 0x4400, 0x0f },
    { 0x4401, 0x0f },
    { 0x3203, 0x0d },
    { 0x3202, 0x0d },
    { 0x5800, 0x24 },
    { 0x5d01, 0x05 },
    { 0x5801, 0x24 },
    { 0x5d00, 0x05 },
    { 0x8b00, 0x06 },
    { 0x8c01, 0x06 },
    { 0x8b01, 0x06 },
    { 0x8c00, 0x06 },
    { 0xa601, 0x08 },
    { 0xa600, 0x08 },
    { 0x5101, 0x15 },
    { 0x5100, 0x15 },
    { 0x9600, 0x04 },
    { 0x9601, 0x04 },
    { 0xe401, 0x03 },
    { 0xe400, 0x03 },
    { 0x2800, 0x00 },
    { 0x2801, 0x00 },
    { 0x4800, 0x10 },
    { 0x4801, 0x10 },
    { 0x5b01, 0x05 },
    { 0x5b00, 0x05 },
    { 0xb300, 0x0a },
    { 0xb301, 0x0a },
    { 0xc003, 0x1f },
    { 0xc002, 0x1f },
    { 0xc001, 0x1f },
    { 0xc000, 0x1f },
    { 0x3f01, 0x03 },
    { 0x3f00, 0x03 },
    { 0x2501, 0x00 },
    { 0x3102, 0x0d },
    { 0x2500, 0x00 },
    { 0x3103, 0x0d },
    { 0x3100, 0x0d },
    { 0x3101, 0x0d },
    { 0xb200, 0x0a },
    { 0xb201, 0x0a },
    { 0x8d00, 0x04 },
    { 0x8d01, 0x04 },
    { 0xeb02, 0x0a },
    { 0x8900, 0x06 },
    { 0x3b01, 0x0f },
    { 0x8901, 0x06 },
    { 0x3b00, 0x0f },
    { 0x5202, 0x15 },
    { 0x5203, 0x15 },
    { 0x5200, 0x15 },
    { 0x5201, 0x15 },
    { 0x9c01, 0x07 },
    { 0x9c00, 0x07 },
    { 0x6c01, 0x22 },
    { 0x6c00, 0x22 },
    { 0xe500, 0x04 },
    { 0xe501, 0x04 },
    { 0x4902, 0x11 },
    { 0x5c00, 0x05 },
    { 0x4903, 0x11 },
    { 0x5c01, 0x05 },
    { 0x4900, 0x11 },
    { 0x4901, 0x11 },
    { 0xbc01, 0x1b },
    { 0xbc00, 0x1b },
    { 0xbe03, 0x1d },
    { 0xbe02, 0x1d },
    { 0xbe01, 0x1d },
    { 0xbe00, 0x1d },
    { 0xa300, 0x05 },
    { 0xa301, 0x05 },
    { 0x2700, 0x00 },
    { 0xb101, 0x0a },
    { 0x2701, 0x00 },
    { 0xb100, 0x0a },
    { 0x3000, 0x0d },
    { 0x3001, 0x0d },
    { 0x2f00, 0x0d },
    { 0x2f01, 0x0d },
    { 0x8701, 0x06 },
    { 0x8700, 0x06 },
    { 0xae00, 0x09 },
    { 0xae01, 0x09 },
    { 0x2c00, 0x01 },
    { 0x2c01, 0x01 },
    { 0xba04, 0x1b },
    { 0xb401, 0x0a },
    { 0xba05, 0x1b },
    { 0xb400, 0x0a },
    { 0x5a00, 0x05 },
    { 0xba06, 0x1b },
    { 0x5a01, 0x05 },
    { 0xba07, 0x1b },
    { 0xba00, 0x1b },
    { 0xba01, 0x1b },
    { 0xba02, 0x1b },
    { 0xba03, 0x1b },
    { 0x2b01, 0x01 },
    { 0x2b00, 0x01 },
    { 0x2901, 0x01 },
    { 0x2900, 0x01 },
    { 0x4700, 0x10 },
    { 0x4a01, 0x11 },
    { 0x4b01, 0x11 },
    { 0x4701, 0x10 },
    { 0x4a00, 0x11 },
    { 0x4b00, 0x11 },
    { 0x4d00, 0x12 },
    { 0x5001, 0x13 },
    { 0x4d01, 0x12 },
    { 0x5000, 0x13 },
    { 0x8501, 0x06 },
    { 0x4301, 0x03 },
    { 0x5900, 0x05 },
    { 0x8500, 0x06 },
    { 0x4300, 0x03 },
    { 0x5901, 0x05 },
    { 0x3801, 0x02 },
    { 0x3800, 0x02 },
    { 0xa400, 0x07 },
    { 0xc102, 0x22 },
    { 0xa401, 0x07 },
    { 0xc103, 0x22 },
    { 0xa402, 0x07 },
    { 0xc100, 0x21 },
    { 0xc101, 0x21 },
    { 0xb801, 0x1a },
    { 0x4c01, 0x12 },
    { 0xb800, 0x1a },
    { 0x4c00, 0x12 },
    { 0xb803, 0x1a },
    { 0x4c03, 0x12 },
    { 0xb802, 0x1a },
    { 0x4c02, 0x12 },
    { 0x2a01, 0x01 },
    { 0x2a00, 0x01 },
    { 0x9a01, 0x07 },
    { 0x9a00, 0x07 },
    { 0xe602, 0x03 },
    { 0xe601, 0x03 },
    { 0xec01, 0xb5 },
    { 0xe600, 0x03 },
    { 0xec00, 0xb5 },
    { 0x7a02, 0x06 },
    { 0x7a03, 0x06 },
    { 0x8a00, 0x06 },
    { 0x7a00, 0x06 },
    { 0x8a01, 0x06 },
    { 0x7a01, 0x06 },
    { 0x9e01, 0x07 },
    { 0xb601, 0x0a },
    { 0x9e00, 0x07 },
    { 0xb600, 0x0a },
};

static std::map<u16, u8> yLastMapDestinations = {
    { 0x2e01, 0x0d },
    { 0x5402, 0x16 },
    { 0x2e00, 0x0d },
    { 0x5403, 0x16 },
    { 0x5400, 0x16 },
    { 0x5401, 0x16 },
    { 0xc401, 0x05 },
    { 0xc400, 0x05 },
    { 0x3401, 0x02 },
    { 0x3400, 0x02 },
    { 0x5500, 0x16 },
    { 0x3403, 0x02 },
    { 0x5501, 0x16 },
    { 0x3402, 0x02 },
    { 0x3701, 0x02 },
    { 0x3700, 0x02 },
    { 0x3c07, 0x0f },
    { 0x3e00, 0x03 },
    { 0x4500, 0x03 },
    { 0x3e01, 0x03 },
    { 0x4501, 0x03 },
    { 0x3e02, 0x03 },
    { 0x4502, 0x03 },
    { 0x2d01, 0x01 },
    { 0x2d00, 0x01 },
    { 0x4000, 0x03 },
    { 0x9800, 0x07 },
    { 0x4001, 0x03 },
    { 0x9801, 0x07 },
    { 0x8e00, 0x04 },
    { 0x8e01, 0x04 },
    { 0x9701, 0x04 },
    { 0x9700, 0x04 },
    { 0x4e00, 0x12 },
    { 0x4e01, 0x12 },
    { 0x3901, 0x02 },
    { 0x3900, 0x02 },
    { 0xab01, 0x08 },
    { 0xb501, 0x0a },
    { 0xab00, 0x08 },
    { 0xb500, 0x0a },
    { 0x9b00, 0x07 },
    { 0x9b01, 0x07 },
    { 0x9501, 0x04 },
    { 0x9500, 0x04 },
    { 0x4201, 0x03 },
    { 0x4200, 0x03 },
    { 0xbd01, 0x17 },
    { 0xbd00, 0x17 },
    { 0x4f01, 0x13 },
    { 0x8002, 0x06 },
    { 0x4f00, 0x13 },
    { 0x8001, 0x06 },
    { 0xb701, 0x0a },
    { 0x4f03, 0x13 },
    { 0x8000, 0x06 },
    { 0xb700, 0x0a },
    { 0x4f02, 0x13 },
    { 0xac01, 0x08 },
    { 0xad01, 0x08 },
    { 0xac00, 0x08 },
    { 0xad00, 0x08 },
    { 0x9901, 0x07 },
    { 0x9900, 0x07 },
    { 0x5703, 0x17 },
    { 0x5702, 0x17 },
    { 0x5701, 0x17 },
    { 0x5700, 0x17 },
    { 0x5e00, 0x05 },
    { 0xa507, 0x08 },
    { 0xc202, 0x22 },
    { 0xa506, 0x08 },
    { 0xc201, 0x22 },
    { 0xa503, 0x08 },
    { 0xa502, 0x08 },
    { 0xa501, 0x08 },
    { 0xa500, 0x08 },
    { 0xaf01, 0x0a },
    { 0xaf00, 0x0a },
    { 0x3601, 0x02 },
    { 0x3600, 0x02 },
    { 0x4101, 0x03 },
    { 0x4100, 0x03 },
    { 0x9d00, 0x07 },
    { 0x9d01, 0x07 },
    { 0xa701, 0x08 },
    { 0xa700, 0x08 },
    { 0x3a00, 0x02 },
    { 0x3a01, 0x02 },
    { 0x4603, 0x10 },
    { 0x4602, 0x10 },
    { 0x4601, 0x10 },
    { 0x4600, 0x10 },
    { 0x5301, 0x15 },
    { 0x5300, 0x15 },
    { 0x5302, 0x15 },
    { 0x8600, 0x06 },
    { 0x8601, 0x06 },
    { 0x4400, 0x0f },
    { 0x4401, 0x0f },
    { 0x3203, 0x0d },
    { 0x3202, 0x0d },
    { 0x5800, 0x24 },
    { 0x5d01, 0x05 },
    { 0x5801, 0x24 },
    { 0x5d00, 0x05 },
    { 0x8b00, 0x06 },
    { 0x8c01, 0x06 },
    { 0x8b01, 0x06 },
    { 0x8c00, 0x06 },
    { 0xa601, 0x08 },
    { 0xa600, 0x08 },
    { 0x5101, 0x15 },
    { 0x5100, 0x15 },
    { 0x9600, 0x04 },
    { 0x9601, 0x04 },
    { 0xe401, 0x03 },
    { 0xe400, 0x03 },
    { 0x2800, 0x00 },
    { 0x2801, 0x00 },
    { 0x4800, 0x10 },
    { 0x4801, 0x10 },
    { 0x5b01, 0x05 },
    { 0x5b00, 0x05 },
    { 0xb300, 0x0a },
    { 0xb301, 0x0a },
    { 0xc003, 0x1f },
    { 0xc002, 0x1f },
    { 0xc001, 0x1f },
    { 0xc000, 0x1f },
    { 0x3f01, 0x03 },
    { 0x3f00, 0x03 },
    { 0x2501, 0x00 },
    { 0x3102, 0x0d },
    { 0x2500, 0x00 },
    { 0x3103, 0x0d },
    { 0x3100, 0x0d },
    { 0x3101, 0x0d },
    { 0xb200, 0x0a },
    { 0xb201, 0x0a },
    { 0x8d00, 0x04 },
    { 0x8d01, 0x04 },
    { 0xeb02, 0x0a },
    { 0x8900, 0x06 },
    { 0x3b01, 0x0f },
    { 0x8901, 0x06 },
    { 0x3b00, 0x0f },
    { 0x5202, 0x15 },
    { 0x5203, 0x15 },
    { 0x5200, 0x15 },
    { 0x5201, 0x15 },
    { 0x9c01, 0x07 },
    { 0x9c00, 0x07 },
    { 0x6c01, 0x22 },
    { 0x6c00, 0x22 },
    { 0xe500, 0x04 },
    { 0xe501, 0x04 },
    { 0x4902, 0x11 },
    { 0x5c00, 0x05 },
    { 0x4903, 0x11 },
    { 0x5c01, 0x05 },
    { 0x4900, 0x11 },
    { 0x4901, 0x11 },
    { 0xbc01, 0x1b },
    { 0xbc00, 0x1b },
    { 0xbe03, 0x1d },
    { 0xbe02, 0x1d },
    { 0xbe01, 0x1d },
    { 0xbe00, 0x1d },
    { 0xa300, 0x05 },
    { 0xa301, 0x05 },
    { 0x2700, 0x00 },
    { 0xb101, 0x0a },
    { 0x2701, 0x00 },
    { 0xb100, 0x0a },
    { 0x3000, 0x0d },
    { 0x3001, 0x0d },
    { 0x2f00, 0x0d },
    { 0x2f01, 0x0d },
    { 0x8701, 0x06 },
    { 0x8700, 0x06 },
    { 0xae00, 0x09 },
    { 0xae01, 0x09 },
    { 0x2c00, 0x01 },
    { 0x2c01, 0x01 },
    { 0xba04, 0x1b },
    { 0xb401, 0x0a },
    { 0xba05, 0x1b },
    { 0xb400, 0x0a },
    { 0x5a00, 0x05 },
    { 0xba06, 0x1b },
    { 0x5a01, 0x05 },
    { 0xba07, 0x1b },
    { 0xba00, 0x1b },
    { 0xba01, 0x1b },
    { 0xba02, 0x1b },
    { 0xba03, 0x1b },
    { 0x2b01, 0x01 },
    { 0x2b00, 0x01 },
    { 0x2901, 0x01 },
    { 0x2900, 0x01 },
    { 0x4700, 0x10 },
    { 0x4a01, 0x11 },
    { 0x4b01, 0x11 },
    { 0x4701, 0x10 },
    { 0x4a00, 0x11 },
    { 0x4b00, 0x11 },
    { 0x4d00, 0x12 },
    { 0x5001, 0x13 },
    { 0x4d01, 0x12 },
    { 0x5000, 0x13 },
    { 0x8501, 0x06 },
    { 0x4301, 0x03 },
    { 0x5900, 0x05 },
    { 0x8500, 0x06 },
    { 0x4300, 0x03 },
    { 0x5901, 0x05 },
    { 0x3801, 0x02 },
    { 0x3800, 0x02 },
    { 0xa400, 0x07 },
    { 0xc102, 0x22 },
    { 0xa401, 0x07 },
    { 0xc103, 0x22 },
    { 0xa402, 0x07 },
    { 0xc100, 0x21 },
    { 0xc101, 0x21 },
    { 0xb801, 0x1a },
    { 0x4c01, 0x12 },
    { 0xb800, 0x1a },
    { 0x4c00, 0x12 },
    { 0xb803, 0x1a },
    { 0x4c03, 0x12 },
    { 0xb802, 0x1a },
    { 0x4c02, 0x12 },
    { 0x2a01, 0x01 },
    { 0x2a00, 0x01 },
    { 0x9a01, 0x07 },
    { 0x9a00, 0x07 },
    { 0xe602, 0x03 },
    { 0xe601, 0x03 },
    { 0xec01, 0xb5 },
    { 0xe600, 0x03 },
    { 0xec00, 0xb5 },
    { 0x7a02, 0x06 },
    { 0x7a03, 0x06 },
    { 0x8a00, 0x06 },
    { 0x7a00, 0x06 },
    { 0x8a01, 0x06 },
    { 0x7a01, 0x06 },
    { 0x9e01, 0x07 },
    { 0xb601, 0x0a },
    { 0x9e00, 0x07 },
    { 0xb600, 0x0a },
    { 0xf800, 0x1e },
    { 0xf801, 0x1e },
};