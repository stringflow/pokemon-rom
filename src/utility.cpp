#define MIN(a,b) (a < b ? a : b)
#define MIN3(a,b,c) (MIN(a, MIN(b,c)))
#define MAX(a,b) (a > b ? a : b)
#define MAX3(a,b,c) (MAX(a, MAX(b,c)))
#define CLAMP(value, lower, upper) (MAX(lower, MIN(value, upper)))
#define ARRAY_LENGTH(array) ((sizeof(array) / sizeof(array[0])))

int findByte(u8 *data, u8 value, int stepSize = 1) {
    int count;
    for(count = 0; data[count * stepSize] != value; count++);
    return count;
}

int findByteTerminated(u8 *data, u8 value, u8 listTerminator, int stepSize = 1) {
    for(int i = 0; data[i * stepSize] != listTerminator; i++) {
        if(data[i * stepSize] == value) return i;
    }
    return -1;
}

bool inList(u8 *data, u8 value, u8 listTerminator, int stepSize = 1) {
    return findByteTerminated(data, value, listTerminator, stepSize) != -1;
}

u8 nybbleArray(u8 *arrayStart, u8 index) {
    u8 value = arrayStart[index / 2];
    return index & 1 ? value & 0xf : value >> 4;
}

u8 bitArray(u8 *arrayStart, u8 index) {
    u8 byte = index / 8;
    u8 bit = index % 8;
    return (arrayStart[byte] >> bit) & 1;
}

u8 countSetBits(u8 value) {
    int count = 0;
    while(value) {
        count++;
        value &= value-1;
    }
    return count;
}

u8 nybbleSwap(u8 value) {
    return (value & 0xf) << 4 | (value >> 4);
}

u8 reverseBits(u8 value) {
    value = nybbleSwap(value);
    value = (value & 0x33) << 2 | (value & 0xcc) >> 2;
    value = (value & 0x55) << 1 | (value & 0xaa) >> 1;
    return value;
}

s8 toSigned4(u8 value) {
    return (value & 0x07) * (value & 0x08 ? -1 : 1);
}

s8 toSigned8(u8 value) {
    return (value & 0x7f) * (value & 0x80 ? -1 : 1);
}

int decodeBCD(u8 *data, int count) {
    int result = 0;
    for(int i = 0; i < count; i++) {
        u8 value = data[i];
        result = result * 100 + (value >> 4) * 10 + (value & 0xf);
    }
    return result;
}