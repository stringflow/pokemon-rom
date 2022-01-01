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
    u8 bitCount;
};

struct DecompressionBuffer {
    u8 *bitplane;
    u8 bitShift;
    u8 x;
    u8 y;
    u8 heightPx;
};

u8 readBit(StreamingBuffer *buffer) {
    buffer->bitCount--;
    u8 bit = (*buffer->data >> buffer->bitCount) & 1;
    
    if(buffer->bitCount == 0) {
        buffer->bitCount = 8;
        buffer->data++;
    }
    
    return bit;
}

u8 readBits(StreamingBuffer *buffer, u8 bitCount) {
    u8 result = 0;
    for(int i = 0; i < bitCount; i++) {
        result = result << 1 | readBit(buffer);
    }
    return result;
}

void writePair(DecompressionBuffer *buffer, u8 bitpair) {
    int offset = buffer->x / 8 * buffer->heightPx + buffer->y;
    buffer->bitplane[offset] |= bitpair << (6 - buffer->bitShift);
    
    buffer->y++;
    if(buffer->y >= buffer->heightPx) {
        buffer->y = 0;
        buffer->x += 2;
    }
    buffer->bitShift = buffer-> x & 7;
}

void decompressBitplane(DecompressionBuffer *out, StreamingBuffer *in, u8 *bitplane,
                        int width, int height) {
    int size = width * height * 32;
    
    out->bitplane = bitplane;
    out->bitShift = 0;
    out->x = 0;
    out->y = 0;
    
    int written = 0;
    bool dataPacket = readBit(in);
    while(written < size) {
        if(dataPacket) {
            // NOTE(stringflow): DATA packet
            while(written < size) {
                u8 bitpair = readBits(in, 2);
                if(bitpair == 0) break;
                writePair(out, bitpair);
                written++;
            }
        } else {
            // NOTE(stringflow): RLE packet
            int lengthBits = 0;
            int length = 0;
            do {
                lengthBits = lengthBits << 1 | readBit(in);
                length++;
            } while(lengthBits & 1);
            
            int value = 0;
            for(int i = 0; i < length; i++) {
                value = value << 1 | readBit(in);
            }
            
            int total = lengthBits + 1 + value;
            for(int i = 0; i < total; i++) {
                writePair(out, 0);
                written++;
            }
        }
        
        dataPacket= !dataPacket;
    }
}

void differentialDecode(u8 *bitplane, u8 width, u8 height, bool flip) {
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

void xorBitplanes(u8 *dest, u8 *src, u8 width, u8 height, bool flip) {
    for(int i = 0; i < width * height * 8; i++) {
        u8 srcByte = src[i];
        u8 destByte = dest[i];
        if(flip) {
            destByte = REVERSED_NYBBLES[destByte >> 4] << 4 | REVERSED_NYBBLES[destByte & 0xf];
        }
        dest[i] = srcByte ^ destByte;
    }
}

void padBuffer(u8 *dest, u8 *src, u8 boundingBox) {
    memset(dest, 0, BYTES_PER_PLANE);
    
    int w = boundingBox & 0xf;
    int h = boundingBox >> 4;
    u8 verticalOffset = BUFFER_H - h;
    u8 horizontalOffset = (BUFFER_W + 1 - w) / 2;
    u8 topleftOffset = 8 * (BUFFER_H * horizontalOffset + verticalOffset);
    
    if(h == 0) h = 0x20;
    
    u8 *readptr = src;
    u8 *writeptr = dest + topleftOffset;
    int column = h * 8;
    for(int x = 0; x < w; x++) {
        memcpy(writeptr, readptr, column);
        readptr += column;
        writeptr += BUFFER_H * 8;
    }
}

void scaleBuffer(u8 *dest, u8 *src) {
    u8 *readptr = src + (4*4*8) - 5;
    u8 *writeptr = dest + BYTES_PER_PLANE - 1;
    
    constexpr u8 table[16] = { 
        0x00, 0x03, 0x0c, 0x0f,
        0x30, 0x33, 0x3c, 0x3f,
        0xc0, 0xc3, 0xcc, 0xcf,
        0xf0, 0xf3, 0xfc, 0xff };
    
    for(int column = 0; column < 4; column++) {
        bool multipleColumns = column > 0;
        int pitch = multipleColumns ? BUFFER_W*8-1 : -1;
        for(int i = 0; i < 4*8-4; i++) {
            u8 pixel = *readptr--;
            u8 upperNybble = table[pixel >> 4];
            u8 lowerNybble = table[pixel & 0xf];
            if(multipleColumns) {
                *writeptr-- = lowerNybble;
                *writeptr = lowerNybble;
                writeptr -= pitch;
            }
            *writeptr-- = upperNybble;
            *writeptr = upperNybble;
            writeptr += pitch;
        }
        readptr -= 4;
        if(multipleColumns) writeptr -= BUFFER_H*8;
    }
}

void zipperSprite(u8 *dest, u8 *bitplane0, u8 *bitplane1, bool flip) {
    for(int i = BYTES_PER_PLANE - 1; i >= 0; i--) {
        u8 plane0 = bitplane0[i];
        u8 plane1 = bitplane1[i];
        if(flip) {
            plane0 = nybbleSwap(plane0);
            plane1 = nybbleSwap(plane1);
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
void picDecompress(u8 *data, u8 boundingBox, u8 *dest, bool frontPic, bool flip, bool transpose = true) {
    u8 *buffer = new u8[8192]();
    u8 *bufferA = buffer + 0 * BYTES_PER_PLANE;
    u8 *bufferB = buffer + 1 * BYTES_PER_PLANE;
    u8 *bufferC = buffer + 2 * BYTES_PER_PLANE;
    
    DecompressionBuffer out;
    StreamingBuffer in;
    in.data = data;
    in.bitCount = 8;
    
    u8 width = readBits(&in, 4);
    u8 height = readBits(&in, 4);
    out.heightPx = height * 8;
    
    u8 bitplaneOrder = readBit(&in);
    u8 *bitplane1 = bitplaneOrder ? bufferC : bufferB;
    u8 *bitplane2 = bitplaneOrder ? bufferB : bufferC;
    
    decompressBitplane(&out, &in, bitplane1, width, height);
    
    u8 mode = readBit(&in);
    if(mode == 1) mode = readBit(&in) + 1;
    
    decompressBitplane(&out, &in, bitplane2, width, height);
    
    if(mode == 0) {
        differentialDecode(bitplane2, width, height, flip);
        differentialDecode(bitplane1, width, height, flip);
    } else if(mode == 1) {
        differentialDecode(bitplane1, width, height, flip);
        xorBitplanes(bitplane2, bitplane1, width, height, flip);
    } else if(mode == 2) {
        differentialDecode(bitplane2, width, height, flip);
        differentialDecode(bitplane1, width, height, flip);
        xorBitplanes(bitplane2, bitplane1, width, height, false);
    }
    
    if(frontPic) {
        padBuffer(bufferA, bufferB, boundingBox);
        padBuffer(bufferB, bufferC, boundingBox);
    } else {
        scaleBuffer(bufferA, bufferB);
        scaleBuffer(bufferB, bufferC);
    }
    
    zipperSprite(bufferB, bufferA, bufferB, flip);
    
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
            int newTile = x * BUFFER_W + y;
            memcpy(dest + newTile * 16, bufferB + tile * 16, 16);
        }
    } else {
        memcpy(dest, bufferB, BYTES_PER_PLANE * 2);
    }
    
    delete[] buffer;
}

// TODO(stringflow): picCompress??????????