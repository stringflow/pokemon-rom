// NOTE(stringflow): Many thanks to the videos by Retro Game Mechanics Explained
// which helped me to implement the decompression algorithm the game uses.
// https://www.youtube.com/c/RetroGameMechanicsExplained

constexpr u8 BUFFER_W = 7;
constexpr u8 BUFFER_H = 7;
constexpr int BYTES_PER_PLANE = BUFFER_W * BUFFER_H * 8;
constexpr u8 REVERSED_NYBBLES[16] = { 
    0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe, 
    0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf };

struct StreamingBuffer {
    u8 *data;
    u8 bit_count;
};

struct DecompressionBuffer {
    u8 *bitplane;
    u8 bitshift;
    u8 x;
    u8 y;
    u8 height_px;
};

u8 read_bit(StreamingBuffer *buffer) {
    buffer->bit_count--;
    u8 bit = (*buffer->data >> buffer->bit_count) & 1;
    
    if(buffer->bit_count == 0) {
        buffer->bit_count = 8;
        buffer->data++;
    }
    
    return bit;
}

u8 read_bits(StreamingBuffer *buffer, u8 bit_count) {
    u8 result = 0;
    for(int i = 0; i < bit_count; i++) {
        result = result << 1 | read_bit(buffer);
    }
    return result;
}

void write_pair(DecompressionBuffer *buffer, u8 bitpair) {
    int offset = buffer->x / 8 * buffer->height_px + buffer->y;
    buffer->bitplane[offset] |= bitpair << (6 - buffer->bitshift);
    
    buffer->y++;
    if(buffer->y >= buffer->height_px) {
        buffer->y = 0;
        buffer->x += 2;
    }
    buffer->bitshift = buffer-> x & 7;
}

void decompress_bitplane(DecompressionBuffer *out, StreamingBuffer *in, u8 *bitplane,
                         int width, int height) {
    int size = width * height * 32;
    
    out->bitplane = bitplane;
    out->bitshift = 0;
    out->x = 0;
    out->y = 0;
    
    int written = 0;
    bool data_packet = read_bit(in);
    while(written < size) {
        if(data_packet) {
            // NOTE(stringflow): DATA packet
            while(written < size) {
                u8 bitpair = read_bits(in, 2);
                if(bitpair == 0) break;
                write_pair(out, bitpair);
                written++;
            }
        } else {
            // NOTE(stringflow): RLE packet
            int length_bits = 0;
            int length = 0;
            do {
                length_bits = length_bits << 1 | read_bit(in);
                length++;
            } while(length_bits & 1);
            
            int value = 0;
            for(int i = 0; i < length; i++) {
                value = value << 1 | read_bit(in);
            }
            
            int total = length_bits + 1 + value;
            for(int i = 0; i < total; i++) {
                write_pair(out, 0);
                written++;
            }
        }
        
        data_packet = !data_packet;
    }
}

void differential_decode(u8 *bitplane, u8 width, u8 height, bool flip) {
    int w = width * 8;
    int h = height * 8;
    for(int y = 0; y < h; y++) {
        u8 prev = 0;
        for(int nx = 0; nx < w / 4; nx++) {
            u8 nybble = 0;
            int i = nx / 2 * h + y;
            for(int px = 0; px < 4; px++) {
                int p = 4 * (nx % 2) + px;
                u8 bit = 1 & (bitplane[i] >> (7 - p));
                if(bit == 1) prev ^= 1;
                nybble = prev | (nybble << 1);
            }
            
            if(flip) nybble = REVERSED_NYBBLES[nybble];
            
            for(int px = 0; px < 4; px++) {
                int p = 4 * (nx % 2) + px;
                u8 bit = 1 & (nybble >> (3 - px));
                bitplane[i] = ((bitplane[i] & ~(1 << (7 - p))) | (bit << (7 - p)));
            }
        }
    }
}

void xor_bitplanes(u8 *dest, u8 *src, u8 width, u8 height, bool flip) {
    for(int i = 0; i < width * height * 8; i++) {
        u8 src_byte = src[i];
        u8 dest_byte = dest[i];
        if(flip) {
            dest_byte = REVERSED_NYBBLES[dest_byte >> 4] << 4 | REVERSED_NYBBLES[dest_byte & 0xf];
        }
        dest[i] = src_byte ^ dest_byte;
    }
}

void pad_buffer(u8 *dest, u8 *src, u8 bounding_box) {
    memset(dest, 0, BYTES_PER_PLANE);
    
    int w = bounding_box & 0xf;
    int h = bounding_box >> 4;
    u8 vertical_offset = BUFFER_H - h;
    u8 horizontal_offset = (BUFFER_W + 1 - w) / 2;
    u8 topleft_offset = 8 * (BUFFER_H * horizontal_offset + vertical_offset);
    
    if(h == 0) h = 0x20;
    
    u8 *readptr = src;
    u8 *writeptr = dest + topleft_offset;
    int column = h * 8;
    for(int x = 0; x < w; x++) {
        memcpy(writeptr, readptr, column);
        readptr += column;
        writeptr += BUFFER_H * 8;
    }
}

void scale_buffer(u8 *dest, u8 *src) {
    u8 *readptr = src + (4*4*8) - 5;
    u8 *writeptr = dest + BYTES_PER_PLANE - 1;
    
    constexpr u8 table[16] = { 
        0x00, 0x03, 0x0c, 0x0f,
        0x30, 0x33, 0x3c, 0x3f,
        0xc0, 0xc3, 0xcc, 0xcf,
        0xf0, 0xf3, 0xfc, 0xff };
    
    for(int column = 0; column < 4; column++) {
        bool multiple_columns = column > 0;
        int pitch = multiple_columns ? BUFFER_W*8-1 : -1;
        for(int i = 0; i < 4*8-4; i++) {
            u8 pixel = *readptr--;
            u8 upper_nybble = table[pixel >> 4];
            u8 lower_nybble = table[pixel & 0xf];
            if(multiple_columns) {
                *writeptr-- = lower_nybble;
                *writeptr = lower_nybble;
                writeptr -= pitch;
            }
            *writeptr-- = upper_nybble;
            *writeptr = upper_nybble;
            writeptr += pitch;
        }
        readptr -= 4;
        if(multiple_columns) writeptr -= BUFFER_H*8;
    }
}

void zipper_sprite(u8 *dest, u8 *bitplane0, u8 *bitplane1, bool flip) {
    for(int i = BYTES_PER_PLANE - 1; i >= 0; i--) {
        u8 plane0 = bitplane0[i];
        u8 plane1 = bitplane1[i];
        if(flip) {
            plane0 = nybble_swap(plane0);
            plane1 = nybble_swap(plane1);
        }
        dest[i * 2 + 0] = plane0;
        dest[i * 2 + 1] = plane1;
    }
}

// problem species: 0xc3
//                  0xcb (?)
//                  0xcf
//                  0xd4
//                  0xdc (?)
//                  0xe3 (?)
//                  0xe7 (?)
void pic_decompress(u8 *data, u8 bounding_box, u8 *dest, bool front_pic, bool flip, bool transpose = true) {
    u8 *buffer = new u8[8192]();
    u8 *bufferA = buffer + 0 * BYTES_PER_PLANE;
    u8 *bufferB = buffer + 1 * BYTES_PER_PLANE;
    u8 *bufferC = buffer + 2 * BYTES_PER_PLANE;
    
    DecompressionBuffer out;
    StreamingBuffer in;
    in.data = data;
    in.bit_count = 8;
    
    u8 width = read_bits(&in, 4);
    u8 height = read_bits(&in, 4);
    out.height_px = height * 8;
    
    u8 bitplane_order = read_bit(&in);
    u8 *bitplane1 = bitplane_order ? bufferC : bufferB;
    u8 *bitplane2 = bitplane_order ? bufferB : bufferC;
    
    decompress_bitplane(&out, &in, bitplane1, width, height);
    
    u8 mode = read_bit(&in);
    if(mode == 1) mode = read_bit(&in) + 1;
    
    decompress_bitplane(&out, &in, bitplane2, width, height);
    
    if(mode == 0) {
        differential_decode(bitplane2, width, height, flip);
        differential_decode(bitplane1, width, height, flip);
    } else if(mode == 1) {
        differential_decode(bitplane1, width, height, flip);
        xor_bitplanes(bitplane2, bitplane1, width, height, flip);
    } else if(mode == 2) {
        differential_decode(bitplane2, width, height, flip);
        differential_decode(bitplane1, width, height, flip);
        xor_bitplanes(bitplane2, bitplane1, width, height, false);
    }
    
    if(front_pic) {
        pad_buffer(bufferA, bufferB, bounding_box);
        pad_buffer(bufferB, bufferC, bounding_box);
    } else {
        scale_buffer(bufferA, bufferB);
        scale_buffer(bufferB, bufferC);
    }
    
    zipper_sprite(bufferB, bufferA, bufferB, flip);
    
    if(flip) {
        u8 temp[BUFFER_W * 16];
        for(int column = 0; column < BUFFER_H / 2; column++) {
            u8 *src = bufferB + column * BUFFER_W * 16;
            u8 *dest = bufferB + (BUFFER_H - column - 1) * BUFFER_W * 16;
            memcpy(temp, dest, BUFFER_W * 16);
            memcpy(dest, src, BUFFER_W * 16);
            memcpy(src, temp, BUFFER_W * 16);
        }
    }
    
    if(transpose) {
        for(int tile = 0; tile < BUFFER_W * BUFFER_H; tile++) {
            u8 x = tile % BUFFER_W;
            u8 y = tile / BUFFER_H;
            int new_tile = x * BUFFER_W + y;
            memcpy(dest + new_tile * 16, bufferB + tile * 16, 16);
        }
    } else {
        memcpy(dest, bufferB, BYTES_PER_PLANE * 2);
    }
    
    delete[] buffer;
}

// TODO(stringflow): picCompress??????????