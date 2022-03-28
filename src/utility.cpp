#define MIN(a,b) (a < b ? a : b)
#define MIN3(a,b,c) (MIN(a, MIN(b,c)))
#define MAX(a,b) (a > b ? a : b)
#define MAX3(a,b,c) (MAX(a, MAX(b,c)))
#define CLAMP(value, lower, upper) (MAX(lower, MIN(value, upper)))
#define ARRAY_SIZE(array) ((sizeof(array) / sizeof(array[0])))

int find_byte(u8 *data, u8 value, int step_size = 1) {
    int count;
    for(count = 0; data[count * step_size] != value; count++);
    return count;
}

int find_byte_terminated(u8 *data, u8 value, u8 list_terminator, int step_size = 1) {
    for(int i = 0; data[i * step_size] != list_terminator; i++) {
        if(data[i * step_size] == value) return i;
    }
    return -1;
}

bool in_list(u8 *data, u8 value, u8 list_terminator, int step_size = 1) {
    return find_byte_terminated(data, value, list_terminator, step_size) != -1;
}

u8 nybble_array(u8 *array_start, u8 index) {
    u8 value = array_start[index / 2];
    return index & 1 ? value & 0xf : value >> 4;
}

u8 bit_array(u8 *array_start, u8 index) {
    u8 byte = index / 8;
    u8 bit = index % 8;
    return (array_start[byte] >> bit) & 1;
}

u8 count_set_bits(u8 value) {
    int count = 0;
    while(value) {
        count++;
        value &= value-1;
    }
    return count;
}

u8 nybble_swap(u8 value) {
    return (value & 0xf) << 4 | (value >> 4);
}

u8 reverse_bits(u8 value) {
    value = nybble_swap(value);
    value = (value & 0x33) << 2 | (value & 0xcc) >> 2;
    value = (value & 0x55) << 1 | (value & 0xaa) >> 1;
    return value;
}

s8 to_signed4(u8 value) {
    return (value & 0x07) * (value & 0x08 ? -1 : 1);
}

s8 to_signed8(u8 value) {
    return (value & 0x7f) * (value & 0x80 ? -1 : 1);
}

int decode_bcd(u8 *data, int count) {
    int result = 0;
    for(int i = 0; i < count; i++) {
        u8 value = data[i];
        result = result * 100 + (value >> 4) * 10 + (value & 0xf);
    }
    return result;
}