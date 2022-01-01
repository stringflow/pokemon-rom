// NOTE(stringflow): DISCLAIMER! This is not a complete gen 1 synthesizer. There are many
// things that are emulated improperly or chosen to be not implemented at all. This is just
// the bare minimums to get regular pokemon cries outputted. In order to support music
// tracks a lot more work has to be done in here. Glitched cries/music 
// may call unrecognized sound commands, which are extremely difficult to emulate properly.
// A list of some of the functionality that is not emulated:
// - length counters of each audio channel
// - pulse frequency sweeping of the first pulse channel
// - the entirety of the wave channel

constexpr int GB_CLOCK_SPEED = 4194304;
constexpr int GB_SAMPLE_RATE = GB_CLOCK_SPEED / 4;
constexpr int GB_SAMPLES_PER_FRAME = GB_SAMPLE_RATE / 59.7275;
// NOTE(stringflow): Pikachu sound samples are funneled in by the cpu, with there being 180
// cpu cycles between each sample
constexpr int PIKA_SAMPLE_RATE = GB_CLOCK_SPEED / 180;
constexpr int RESAMPLE_RATE = 48000;

struct ResamplingInfo {
    f32 convertionRatio;
    int outputSampleCount;
};

ResamplingInfo getResamplingInfo(int sampleCount, int inputRate, int outputRate) {
    ResamplingInfo result;
    result.convertionRatio = (f32) inputRate / outputRate;
    result.outputSampleCount = sampleCount / result.convertionRatio;
    return result;
}

void processAudio(f32 *src, int sampleCount, ResamplingInfo resampling, s16 *dest) {
    if(resampling.convertionRatio == 1.0f) {
        // NOTE(stringflow): copy samples
        for(int i = 0; i < resampling.outputSampleCount; i++) {
            dest[i] = (s16) (src[i] * 32767.0f);
        }
    } else {
        // TODO(stringflow): implement a more sound resampling algorithm
        // https://programmer.help/blogs/5e87bf7ff41e0.html
        int lastSample = sampleCount - 1;
        for(int i = 0; i < resampling.outputSampleCount; i++) {
            f32 sampleIndex = (f32) i * resampling.convertionRatio;
            int p1 = (int) sampleIndex;
            f32 coef = sampleIndex - p1;
            int p2 = p1 == lastSample ? lastSample : p1 + 1;
            f32 sample = (1.0f - coef) * src[p1] + coef * src[p2];
            dest[i] = (s16) (sample * 32767.0f);
        }
    }
}

// NOTE(stringflow): inlines loop commands so that it's more trivial to check if the current
// note is the final note in the command list.
std::vector<u8> preprocessorCommands(u8 *dataStart) {
    std::vector<u8> dest;
    u8 *dataEnd = dataStart;
    int loopCount = 1;
    while(true) {
        u8 command = *dataEnd++;
        if(command == 0xff) {
            break;
        } else if(command == 0xfe) {
            loopCount = *dataEnd;
            break;
        } else if(command == 0xfc) {
            dataEnd++;
        } else if((command >> 4) == 2) {
            dataEnd += 3;
        }
    }
    
    for(int i = 0; i < loopCount; i++) {
        for(u8 *data = dataStart; data != dataEnd-1; data++) {
            dest.push_back(*data);
        }
    }
    dest.push_back(0xff);
    
    return dest;
}

int calcSampleCount(int *leftovers, s8 cryTimeStretch, u8 noteLength) {
    int subframes = ((cryTimeStretch + 0x100) * (noteLength)) + *leftovers;
    *leftovers = subframes & 0xff;
    return GB_SAMPLES_PER_FRAME * (subframes >> 8);
}

s8 sweepVolume(s8 volume, s8 volumeSweep, int index) {
    if(volumeSweep != 0 && (index + 1) % (GB_SAMPLES_PER_FRAME * volumeSweep) == 0) {
        volume += (volumeSweep < 0 ? 1 : -1);
        return CLAMP(volume, 0x0, 0xf);
    } else {
        return volume;
    }
}

f32 sample(u8 bin, s8 volume) {
    return ((2*bin)-1) * (-volume / 16.0);
}

bool calcDuty(u8 duty, f64 percent) {
    switch(duty) {
        case 0: return percent >= 4.0/8.0 && percent < 5.0/8.0;
        case 1: return percent >= 4.0/8.0 && percent < 6.0/8.0;
        case 2: return percent >= 2.0/8.0 && percent < 6.0/8.0;
        case 3: return percent >= 6.0/8.0 || percent < 4.0/8.0;
        default: return false; // NOTE(stringflow): should never be reached
    }
}

void squareWave(std::vector<f32> *dest, int *channelLength, u8 *dataStart, u8 cryPitch, s8 cryTimeStretch) {
    std::vector<u8> data = preprocessorCommands(dataStart);
    int dataIndex = 0;
    
    int sampleIndex = 0;
    int leftovers = 0;
    f64 percent = 0;
    u8 duty = 0;
    u8 command;
    while((command = data[dataIndex++]) != 0xff) { // NOTE(stringflow): $ff terminator
        if(command == 0xfc) { // NOTE(stringflow): set duty cycle
            duty = data[dataIndex++];
        } else if((command >> 4) == 2) { // NOTE(stringflow): play pulse note, $20..$2f
            u8 noteLength = (command & 0xf) + 1;
            s8 volume = data[dataIndex] >> 4;
            s8 volumeSweep = toSigned4(data[dataIndex++]);
            u16 frequency = data[dataIndex] | (data[dataIndex+1] << 8);
            dataIndex += 2;
            
            int period = GB_SAMPLE_RATE * (2048 - ((frequency + cryPitch) & 0x7ff)) / 131072;
            int sampleCount = calcSampleCount(&leftovers, cryTimeStretch, noteLength);
            *channelLength += sampleCount;
            for(int i = 0; i < 2500000 && (i < sampleCount || (data[dataIndex] == 0xff && volume > 0)); i++) {
                u8 bin = calcDuty(duty & 3, percent);
                f32 sampleValue = sample(bin, volume);
                dest->push_back(sampleValue);
                percent += 1.0 / (f64) period;
                percent = percent >= 1.0 ? percent - 1.0 : percent;
                sampleIndex++;
                if(i < sampleCount && sampleIndex % GB_SAMPLES_PER_FRAME == 0) {
                    duty = ((duty & 0x3f) << 2) | ((duty & 0xc0) >> 6);
                }
                
                volume = sweepVolume(volume, volumeSweep, i);
            }
        }
    }
}

void noise(std::vector<f32> *dest, u8 *dataStart, u8 cryPitch, s8 cryTimeStretch, int cutoff) {
    std::vector<u8> data = preprocessorCommands(dataStart);
    int dataIndex = 0;
    
    int sampleIndex = 0;
    int leftovers = 0;
    u8 command;
    while((command = data[dataIndex++]) != 0xff) {
        u8 noteLength = (command & 0xf) + 1;
        s8 volume = data[dataIndex] >> 4;
        s8 volumeSweep = toSigned4(data[dataIndex++]);
        u8 params = data[dataIndex++];
        
        params += sampleIndex >= cutoff ? 0 : cryPitch;
        u8 shift = (params >> 4) & 0xf;
        shift = shift > 0xd ? shift & 0xd : shift;
        u8 divider = params & 0x7;
        bool width = params & 0x8;
        u16 lfsr = 0x7fff;
        int lfsrSampleRate = (divider == 0 ? 1 : 2*divider) * (1 << (shift + 1));
        
        int sampleCount = calcSampleCount(&leftovers, cryTimeStretch, noteLength);
        for(int i = 0; i < 2500000 && (i < sampleCount || (data[dataIndex] == 0xff && volume > 0)); i++) {
            u8 bit0 = lfsr & 1;
            f32 sampleValue = sample(bit0 ^ 1, volume);
            dest->push_back(sampleValue);
            sampleIndex++;
            
            if(sampleIndex % lfsrSampleRate == 0) {
                u8 bit1 = (lfsr >> 1) & 1;
                lfsr = (lfsr >> 1) | ((bit0 ^ bit1) << 14);
                if(width) lfsr = (lfsr >> 1) | ((bit0 ^ bit1) << 6);
            }
            
            volume = sweepVolume(volume, volumeSweep, i);
        }
    }
}

int synthesizeCry(u8 *pulse1Data, u8 *pulse2Data, u8 *noiseData, u8 pitch, s8 timeStretch, bool resample, s16 *dest) {
    std::vector<f32> pulse1Channel;
    std::vector<f32> pulse2Channel;
    std::vector<f32> noiseChannel;
    int pulseLen1 = 0;
    int pulseLen2 = 0;
    squareWave(&pulse1Channel, &pulseLen1, pulse1Data, pitch, timeStretch);
    squareWave(&pulse2Channel, &pulseLen2, pulse2Data, pitch, timeStretch);
    int cutoff = (pulseLen1 > pulseLen2 ? pulseLen1 : pulseLen2) - GB_SAMPLES_PER_FRAME;
    noise(&noiseChannel, noiseData, pitch, 0, cutoff);
    
    int sampleCount = MAX3(pulse1Channel.size(), pulse2Channel.size(), noiseChannel.size());
    ResamplingInfo resampling = getResamplingInfo(sampleCount, GB_SAMPLE_RATE, resample ? RESAMPLE_RATE : GB_SAMPLE_RATE);
    
    if(dest) {
        f32 *samples = new f32[sampleCount]();
        
        for(int i = 0; i < (int) pulse1Channel.size(); i++) {
            samples[i] += pulse1Channel[i] / 3.0f;
        }
        for(int i = 0; i < (int) pulse2Channel.size(); i++) {
            samples[i] += pulse2Channel[i] / 3.0f;
        }
        for(int i = 0; i < (int) noiseChannel.size(); i++) {
            samples[i] += noiseChannel[i] / 3.0f;
        }
        
        processAudio(samples, sampleCount, resampling, dest);
        
        delete[] samples;
    }
    
    return resampling.outputSampleCount;
}

int synthesizePikaSound(u8 *cryData, bool resample, s16 *dest) {
    u16 byteCount = *((u16 *) cryData);
    cryData += 2;
    int sampleCount = byteCount * 8;
    ResamplingInfo resampling = getResamplingInfo(sampleCount, PIKA_SAMPLE_RATE, resample ? RESAMPLE_RATE : PIKA_SAMPLE_RATE);
    if(dest) {
        float *samples = new float[sampleCount];
        
        for(int byte = 0; byte < byteCount; byte++) {
            u8 value = *cryData++;
            for(int bit = 0; bit < 8; bit++, value <<= 1) {
                samples[byte * 8 + bit] = value & 0x80 ? 1.0f : -1.0f;
            }
        }
        
        processAudio(samples, sampleCount, resampling, dest);
        delete[] samples;
    }
    
    return resampling.outputSampleCount;
}

