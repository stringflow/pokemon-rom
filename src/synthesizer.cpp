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
    f32 convertion_ratio;
    int output_sample_count;
};

ResamplingInfo get_resampling_info(int sample_count, int input_rate, int output_rate) {
    ResamplingInfo result;
    result.convertion_ratio = (f32) input_rate / output_rate;
    result.output_sample_count = sample_count / result.convertion_ratio;
    return result;
}

void process_audio(f32 *src, int sample_count, ResamplingInfo resampling, s16 *dest) {
    if(resampling.convertion_ratio == 1.0f) {
        // NOTE(stringflow): copy samples
        for(int i = 0; i < resampling.output_sample_count; i++) {
            dest[i] = (s16) (src[i] * 32767.0f);
        }
    } else {
        // TODO(stringflow): implement a more sound resampling algorithm
        // https://programmer.help/blogs/5e87bf7ff41e0.html
        int last_sample = sample_count - 1;
        for(int i = 0; i < resampling.output_sample_count; i++) {
            f32 sample_index = (f32) i * resampling.convertion_ratio;
            int p1 = (int) sample_index;
            f32 coef = sample_index - p1;
            int p2 = p1 == last_sample ? last_sample : p1 + 1;
            f32 sample = (1.0f - coef) * src[p1] + coef * src[p2];
            dest[i] = (s16) (sample * 32767.0f);
        }
    }
}

// NOTE(stringflow): inlines loop commands so that it's more trivial to check if the current
// note is the final note in the command list.
std::vector<u8> preprocess_commands(u8 *data_start) {
    std::vector<u8> dest;
    u8 *data_end = data_start;
    int loop_count = 1;
    while(true) {
        u8 command = *data_end++;
        if(command == 0xff) {
            break;
        } else if(command == 0xfe) {
            loop_count = *data_end;
            break;
        } else if(command == 0xfc) {
            data_end++;
        } else if((command >> 4) == 2) {
            data_end += 3;
        }
    }
    
    for(int i = 0; i < loop_count; i++) {
        for(u8 *data = data_start; data != data_end-1; data++) {
            dest.push_back(*data);
        }
    }
    dest.push_back(0xff);
    
    return dest;
}

int calc_sample_count(int *leftovers, s8 cry_time_stretch, u8 note_length) {
    int subframes = ((cry_time_stretch + 0x100) * (note_length)) + *leftovers;
    *leftovers = subframes & 0xff;
    return GB_SAMPLES_PER_FRAME * (subframes >> 8);
}

s8 sweep_volume(s8 volume, s8 volume_sweep, int index) {
    if(volume_sweep != 0 && (index + 1) % (GB_SAMPLES_PER_FRAME * volume_sweep) == 0) {
        volume += (volume_sweep < 0 ? 1 : -1);
        return CLAMP(volume, 0x0, 0xf);
    } else {
        return volume;
    }
}

f32 sample(u8 bin, s8 volume) {
    return ((2*bin)-1) * (-volume / 16.0);
}

bool calc_duty(u8 duty, f64 percent) {
    switch(duty) {
        case 0: return percent >= 4.0/8.0 && percent < 5.0/8.0;
        case 1: return percent >= 4.0/8.0 && percent < 6.0/8.0;
        case 2: return percent >= 2.0/8.0 && percent < 6.0/8.0;
        case 3: return percent >= 6.0/8.0 || percent < 4.0/8.0;
        default: return false; // NOTE(stringflow): should never be reached
    }
}

void square_wave(std::vector<f32> *dest, int *channel_length, u8 *data_start, u8 cry_pitch, s8 cry_time_stretch) {
    std::vector<u8> data = preprocess_commands(data_start);
    int data_index = 0;
    
    int sample_index = 0;
    int leftovers = 0;
    f64 percent = 0;
    u8 duty = 0;
    u8 command;
    while((command = data[data_index++]) != 0xff) { // NOTE(stringflow): $ff terminator
        if(command == 0xfc) { // NOTE(stringflow): set duty cycle
            duty = data[data_index++];
        } else if((command >> 4) == 2) { // NOTE(stringflow): play pulse note, $20..$2f
            u8 note_length = (command & 0xf) + 1;
            s8 volume = data[data_index] >> 4;
            s8 volume_sweep = to_signed4(data[data_index++]);
            u16 frequency = data[data_index] | (data[data_index+1] << 8);
            data_index += 2;
            
            int period = GB_SAMPLE_RATE * (2048 - ((frequency + cry_pitch) & 0x7ff)) / 131072;
            int sample_count = calc_sample_count(&leftovers, cry_time_stretch, note_length);
            *channel_length += sample_count;
            for(int i = 0; i < 2500000 && (i < sample_count || (data[data_index] == 0xff && volume > 0)); i++) {
                u8 bin = calc_duty(duty & 3, percent);
                f32 sample_value = sample(bin, volume);
                dest->push_back(sample_value);
                percent += 1.0 / (f64) period;
                percent = percent >= 1.0 ? percent - 1.0 : percent;
                sample_index++;
                if(i < sample_count && sample_index % GB_SAMPLES_PER_FRAME == 0) {
                    duty = ((duty & 0x3f) << 2) | ((duty & 0xc0) >> 6);
                }
                
                volume = sweep_volume(volume, volume_sweep, i);
            }
        }
    }
}

void noise(std::vector<f32> *dest, u8 *data_start, u8 cry_pitch, s8 cry_time_stretch, int cutoff) {
    std::vector<u8> data = preprocess_commands(data_start);
    int data_index = 0;
    
    int sample_index = 0;
    int leftovers = 0;
    u8 command;
    while((command = data[data_index++]) != 0xff) {
        u8 note_length = (command & 0xf) + 1;
        s8 volume = data[data_index] >> 4;
        s8 volume_sweep = to_signed4(data[data_index++]);
        u8 params = data[data_index++];
        
        params += sample_index >= cutoff ? 0 : cry_pitch;
        u8 shift = (params >> 4) & 0xf;
        shift = shift > 0xd ? shift & 0xd : shift;
        u8 divider = params & 0x7;
        bool width = params & 0x8;
        u16 lfsr = 0x7fff;
        int lfsr_sample_rate = (divider == 0 ? 1 : 2*divider) * (1 << (shift + 1));
        
        int sample_count = calc_sample_count(&leftovers, cry_time_stretch, note_length);
        for(int i = 0; i < 2500000 && (i < sample_count || (data[data_index] == 0xff && volume > 0)); i++) {
            u8 bit0 = lfsr & 1;
            f32 sample_value = sample(bit0 ^ 1, volume);
            dest->push_back(sample_value);
            sample_index++;
            
            if(sample_index % lfsr_sample_rate == 0) {
                u8 bit1 = (lfsr >> 1) & 1;
                lfsr = (lfsr >> 1) | ((bit0 ^ bit1) << 14);
                if(width) lfsr = (lfsr >> 1) | ((bit0 ^ bit1) << 6);
            }
            
            volume = sweep_volume(volume, volume_sweep, i);
        }
    }
}

int synthesize_cry(u8 *pulse1_data, u8 *pulse2_data, u8 *noise_data, u8 pitch, s8 time_stretch, bool resample, s16 *dest) {
    std::vector<f32> pulse1_channel;
    std::vector<f32> pulse2_channel;
    std::vector<f32> noise_channel;
    int pulse_len1 = 0;
    int pulse_len2 = 0;
    square_wave(&pulse1_channel, &pulse_len1, pulse1_data, pitch, time_stretch);
    square_wave(&pulse2_channel, &pulse_len2, pulse2_data, pitch, time_stretch);
    int cutoff = MAX(pulse_len1, pulse_len2) - GB_SAMPLES_PER_FRAME;
    noise(&noise_channel, noise_data, pitch, 0, cutoff);
    
    int sample_count = MAX3(pulse1_channel.size(), pulse2_channel.size(), noise_channel.size());
    ResamplingInfo resampling = get_resampling_info(sample_count, GB_SAMPLE_RATE, resample ? RESAMPLE_RATE : GB_SAMPLE_RATE);
    
    if(dest) {
        f32 *samples = new f32[sample_count]();
        
        for(int i = 0; i < (int) pulse1_channel.size(); i++) {
            samples[i] += pulse1_channel[i] / 3.0f;
        }
        for(int i = 0; i < (int) pulse2_channel.size(); i++) {
            samples[i] += pulse2_channel[i] / 3.0f;
        }
        for(int i = 0; i < (int) noise_channel.size(); i++) {
            samples[i] += noise_channel[i] / 3.0f;
        }
        
        process_audio(samples, sample_count, resampling, dest);
        
        delete[] samples;
    }
    
    return resampling.output_sample_count;
}

int synthesize_pika_sound(u8 *cry_data, bool resample, s16 *dest) {
    u16 byte_count = *((u16 *) cry_data);
    cry_data += 2;
    int sample_count = byte_count * 8;
    ResamplingInfo resampling = get_resampling_info(sample_count, PIKA_SAMPLE_RATE, resample ? RESAMPLE_RATE : PIKA_SAMPLE_RATE);
    if(dest) {
        float *samples = new float[sample_count];
        
        for(int byte = 0; byte < byte_count; byte++) {
            u8 value = *cry_data++;
            for(int bit = 0; bit < 8; bit++, value <<= 1) {
                samples[byte * 8 + bit] = value & 0x80 ? 1.0f : -1.0f;
            }
        }
        
        process_audio(samples, sample_count, resampling, dest);
        delete[] samples;
    }
    
    return resampling.output_sample_count;
}

