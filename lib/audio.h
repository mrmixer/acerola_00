
#if !defined( AUDIO_H )

#include "../lib/audio_types.h"

/* https://discord.com/channels/239737791225790464/981439725275611146 wasapi in thread. */
/* https://gist.github.com/mmozeiko/5a5b168e61aff4c1eaec0381da62808f#file-win32_wasapi-h */
/* https://chromium.googlesource.com/external/webrtc/+/HEAD/modules/audio_device/win/core_audio_base_win.cc#335 how they initialize for low lantency. */
/* https://hero.handmade.network/forums/code-discussion/t/6942-day__20__the_tragedy_of_cases */
/* https://dspguru.com/dsp/faqs/multirate/resampling/ */ 
/* Polyphase Resampling */

#include "../lib/threading.h"

typedef enum audio_channel_t {
    
    audio_channel_front_left = 0x1 << 0,
    audio_channel_front_right = 0x1 << 1,
    audio_channel_front_center = 0x1 << 2,
    audio_channel_low_frequency = 0x1 << 3,
    audio_channel_back_left = 0x1 << 4,
    audio_channel_back_right = 0x1 << 5,
    audio_channel_front_left_of_center = 0x1 << 6,
    audio_channel_front_right_of_center = 0x1 << 7,
    audio_channel_back_center = 0x1 << 8,
    audio_channel_side_left = 0x1 << 9,
    audio_channel_side_right = 0x1 << 10,
    audio_channel_top_center = 0x1 << 11,
    audio_channel_top_front_left = 0x1 << 12,
    audio_channel_top_front_center = 0x1 << 13,
    audio_channel_top_front_right = 0x1 << 14,
    audio_channel_top_back_left = 0x1 << 15,
    audio_channel_top_back_center = 0x1 << 16,
    audio_channel_top_back_right = 0x1 << 17,
    
} audio_channel_t;

stu void audio_format_initialize( audio_format_t* format, u32 samples_per_seconds, u16 bits_per_sample, u16 channel_count, u32 channel_mask ) {
    
    format->samples_per_second = samples_per_seconds;
    format->bits_per_sample = bits_per_sample;
    format->channel_count = channel_count;
    format->channel_mask = channel_mask;
    b32 bit_remaining = ( format->bits_per_sample % 8 ) != 0;
    u16 container_size = ( format->bits_per_sample / 8 ) + ( ( bit_remaining ) ? 1 : 0 );
    format->container_size = container_size;
}

#if 0
stu u32 audio_compute_buffer_size( audio_format_t* format, f32 length ) {
    
    u32 length_in_samples = cast( u32, ceil( format->samples_per_second * length ) );
    u32 data_size = format->container_size * format->channel_count * length_in_samples;
    return data_size;
}
#endif

typedef enum audio_command_type_t {
    audio_command_type_none,
    audio_command_type_play,
    audio_command_type_stop,
    audio_command_type_pause,
    audio_command_type_resume,
    audio_command_type_sound_volume,
    audio_command_type_sound_panning,
    audio_command_type_global_volume,
} audio_command_type_t;

stu b32 audio_id_is_valid( audio_id_t id ) {
    b32 result = 0;
    result = id.sound && id.id;
    return result;
}

stu void audio_id_clear( audio_id_t* id ) {
    id->sound = 0;
    id->id = 0;
}

stu void audio_mixer_initialize( audio_mixer_t* mixer, audio_format_t format, u32 max_sound_count ) {
    
    _assert( format.channel_count == 2 );
    
    deref( mixer ) = ( audio_mixer_t ) { 0 };
    mixer->max_sound_count = max_sound_count;
    umm sound_size = max_sound_count * sizeof( audio_sound_t );
    
    mixer->sound_memory = memory_get_virtual( sound_size, 0 );
    mixer->format = format;
    mixer->volume = 1.0f;
    
    umm command_size = max_sound_count * sizeof( audio_command_t );
    mixer->command_memory = memory_get_virtual( command_size, 0 );
    mixer->input_commands = memory_push_array_aligned( &mixer->command_memory, audio_command_t, max_sound_count, sizeof( umm ) );
    mixer->input_read = 0;
    mixer->input_write = 0;
    
    mixer->sound_id = 1; /* NOTE simon (11/10/23 15:07:09): Sound id can't be zero. */
}

stu void audio_mixer_free( audio_mixer_t* mixer ) {
    
    if ( mixer->sound_memory.bytes ) {
        memory_free_virtual( &mixer->sound_memory );
    }
    
    if ( mixer->command_memory.bytes ) {
        memory_free_virtual( &mixer->command_memory );
    }
    
    deref( mixer ) = ( audio_mixer_t ) { 0 };
}

/* NOTE simon (13/10/23 13:27:31): Only ONE thread is allowed to add input commands. */
/* NOTE simon (13/10/23 13:27:51): Only the audio thread is allowed to read input commands. */

stu b32 audio_queue_command( audio_mixer_t* mixer, audio_command_t command ) {
    
    b32 result = false;
    audio_command_t* queued = 0;
    
    u32 read_index = mixer->input_read;
    u32 write_index = mixer->input_write;
    u32 write_index_incremented = ( write_index + 1 ) % mixer->max_sound_count;
    
    if ( write_index_incremented != read_index ) {
        
        _assert( write_index < mixer->max_sound_count );
        queued = mixer->input_commands + write_index;
        deref( queued ) = command;
        
        atomic_thread_fence( );
        mixer->input_write = ( mixer->input_write + 1 ) % mixer->max_sound_count;
        
        result = true;
    }
    
    return result;
}

stu b32 audio_play( audio_mixer_t* mixer, data_t* samples, audio_format_t* format, u32 sample_count, f32 volume, f32 panning, b32 loops, audio_id_t* id ) {
    
    _assert( format->channel_count == 1 || format->channel_count == 2 );
    
    audio_command_t command = { 0 };
    
    command.type = audio_command_type_play;
    command.play.sample_count = sample_count;
    command.play.samples = samples;
    command.play.format = format;
    command.play.panning = panning;
    command.play.volume = volume;
    command.play.loops = loops;
    command.play.id = id;
    
    if ( id ) {
        id->sound = 0;
        id->id = 0;
    }
    
    b32 result = audio_queue_command( mixer, command );
    
    return result;
}

stu b32 audio_stop( audio_mixer_t* mixer, audio_id_t id ) {
    
    audio_command_t command = { 0 };
    command.type = audio_command_type_stop;
    command.stop.id = id;
    
    b32 result = audio_queue_command( mixer, command );
    
    return result;
}

stu b32 audio_pause( audio_mixer_t* mixer, audio_id_t id ) {
    
    audio_command_t command = { 0 };
    command.type = audio_command_type_pause;
    command.pause.id = id;
    
    b32 result = audio_queue_command( mixer, command );
    
    return result;
}

stu b32 audio_resume( audio_mixer_t* mixer, audio_id_t id ) {
    
    audio_command_t command = { 0 };
    command.type = audio_command_type_resume;
    command.resume.id = id;
    
    b32 result = audio_queue_command( mixer, command );
    
    return result;
}

stu b32 audio_set_volume( audio_mixer_t* mixer, audio_id_t id, f32 volume ) {
    
    audio_command_t command = { 0 };
    command.type = audio_command_type_sound_volume;
    command.sound_volume.id = id;
    command.sound_volume.volume = volume;
    
    b32 result = audio_queue_command( mixer, command );
    
    return result;
}

stu b32 audio_set_panning( audio_mixer_t* mixer, audio_id_t id, f32 panning ) {
    
    audio_command_t command = { 0 };
    command.type = audio_command_type_sound_panning;
    command.sound_panning.id = id;
    command.sound_panning.panning = panning;
    
    b32 result = audio_queue_command( mixer, command );
    
    return result;
}

stu b32 audio_set_global_volume( audio_mixer_t* mixer, f32 volume ) {
    
    audio_command_t command = { 0 };
    command.type = audio_command_type_global_volume;
    command.global_volume.volume = volume;
    
    b32 result = audio_queue_command( mixer, command );
    
    return result;
}

stu f32 audio_s8_to_f32( u8* sample ) {
    
    s8* sample_s8 = cast( s8*, sample );
    f32 result = cast( f32, deref( sample_s8 ) );
    
    if ( result < 0 ) {
        result = result / 0x1p7f;
    } else {
        result = result / ( 0x1p7f - 1 );
    }
    
    return result;
}

stu f32 audio_s16_to_f32( u8* sample ) {
    
    s16* sample_s16 = cast( s16*, sample );
    f32 result = cast( f32, deref( sample_s16 ) );
    
    if ( result < 0 ) {
        result = result / 0x1p15f;
    } else {
        result = result / ( 0x1p15f - 1 );
    }
    
    return result;
}

stu f32 audio_s24_to_f32( u8* sample ) {
    
    s32 sample_s32 = math_s24_to_s32( sample );
    f32 result = cast( f32, sample_s32 );
    
    if ( result < 0 ) {
        result = result / 0x1p23f;
    } else {
        result = result / ( 0x1p23f - 1 );
    }
    
    return result;
}

stu s8 audio_f32_to_s8( f32 sample ) {
    
    s8 result = 0;
    
    if ( sample < 0 ) {
        sample *= 0x1p7f;
    } else {
        sample *= ( 0x1p7f - 1 );
    }
    
    result = cast( s8, sample );
    
    return result;
}

stu s16 audio_f32_to_s16( f32 sample ) {
    
    s16 result = 0;
    
    if ( sample < 0 ) {
        sample *= 0x1p15f;
    } else {
        sample *= ( 0x1p15f - 1 );
    }
    
    result = cast( s16, sample );
    
    return result;
}

stu s32 audio_f32_to_s24( f32 sample ) {
    
    s32 result = 0;
    
    if ( sample < 0 ) {
        sample *= 0x1p23f;
    } else {
        sample *= ( 0x1p23f - 1 );
    }
    
    result = cast( s32, sample );
    
    return result;
}

stu audio_sound_t* internal_audio_get_sound_from_id( audio_id_t id ) {
    
    audio_sound_t* result = 0;
    
    if ( id.sound && id.id && id.sound->id == id.id ) {
        result = id.sound;
    }
    
    return result;
}

/* TODO simon: Prevent going over the maximum value for samples. Did I mean Amplitude ? */
/* TODO simon: use a format that is float directly since the mixer use float at the moment as an intermediate step ? */
stu void audio_mixer_render( audio_mixer_t* mixer, u8* buffer, u32 sample_count ) {
    
    u32 write_index = mixer->input_write;
    u32 read_index = mixer->input_read;
    
    while ( read_index != write_index ) {
        
        audio_command_t* command = mixer->input_commands + read_index;
        
        switch ( command->type ) {
            
            case audio_command_type_play: {
                
                audio_sound_t* sound = 0;
                
                if ( mixer->sound_id < u64_max ) {
                    
                    if ( mixer->free ) {
                        sound = mixer->free;
                        mixer->free = sound->next;
                    }
                    
                    if ( !sound ) {
                        sound = memory_push_struct_aligned( &mixer->sound_memory, audio_sound_t, sizeof( umm ) );
                    }
                    
                    if ( sound ) {
                        
                        deref( sound ) = ( audio_sound_t ) { 0 };
                        sound->samples = command->play.samples;
                        sound->format = command->play.format;
                        sound->volume = command->play.volume;
                        sound->panning = command->play.panning;
                        sound->loops = command->play.loops;
                        sound->sample_count = command->play.sample_count;
                        sound->id = mixer->sound_id;
                        
                        if ( command->play.id ) {
                            command->play.id->sound = sound;
                            command->play.id->id = sound->id;
                        }
                        
                        mixer->sound_id++;
                        sound->next = mixer->sounds;
                        mixer->sounds = sound;
                        
                    } else {
                        log_l( "Couldn't play sound.\n" );
                    }
                    
                } else {
                    log_l( "How is it possible to run out of 64 bit sound ids.\n" );
                }
                
            } break;
            
            case audio_command_type_stop: {
                
                /* NOTE simon (11/10/23 14:58:14): I don't remove the sound from the list because I want to avoid traversing the list several times. I let the render loop do it. */
                audio_sound_t* sound = internal_audio_get_sound_from_id( command->stop.id );
                
                if ( sound ) {
                    sound->current_sample = sound->sample_count;
                }
                
            } break;
            
            case audio_command_type_pause: {
                
                audio_sound_t* sound = internal_audio_get_sound_from_id( command->pause.id );
                
                if ( sound ) {
                    sound->paused = true;
                }
                
            } break;
            
            case audio_command_type_resume: {
                
                audio_sound_t* sound = internal_audio_get_sound_from_id( command->resume.id );
                
                if ( sound ) {
                    sound->paused = false;
                }
                
            } break;
            
            case audio_command_type_sound_volume: {
                
                audio_sound_t* sound = internal_audio_get_sound_from_id( command->sound_volume.id );
                
                if ( sound ) {
                    sound->volume = command->sound_volume.volume;
                }
                
            } break;
            
            case audio_command_type_sound_panning: {
                
                audio_sound_t* sound = internal_audio_get_sound_from_id( command->sound_panning.id );
                
                if ( sound ) {
                    sound->panning = command->sound_panning.panning;
                }
                
            } break;
            
            case audio_command_type_global_volume: {
                mixer->volume = command->global_volume.volume;
            } break;
        }
        
        read_index = ( read_index + 1 ) % mixer->max_sound_count;
    }
    
    mixer->input_read = read_index;
    
    u32 mixer_container_size = mixer->format.container_size;
    u32 mixer_bytes_per_sample = mixer->format.channel_count * mixer_container_size;
    memory_zero( buffer, sample_count * mixer_bytes_per_sample  );
    
    audio_sound_t** previous = &mixer->sounds;
    audio_sound_t* sound = mixer->sounds;
    
    while ( sound ) {
        
        u8* destination = buffer;
        
        u32 left_to_write = sound->sample_count - sound->current_sample;
        u32 samples_to_write = sample_count;
        
        if ( !sound->loops ) {
            if ( left_to_write < sample_count ) {
                samples_to_write = left_to_write;
            }
        }
        
        u32 sound_bytes_per_sample = sound->format->container_size * sound->format->channel_count;
        s8* source = cast( s8*, sound->samples->bytes + ( sound->current_sample * sound_bytes_per_sample ) );
        
        if ( sound->paused ) {
            samples_to_write = 0;
        }
        
        for ( umm sample_index = 0; sample_index < samples_to_write; sample_index++ )  {
            
            if ( sound->loops ) {
                
                if ( sound->current_sample == sound->sample_count ) {
                    source = cast( s8*, sound->samples->bytes );
                    sound->current_sample = 0;
                    left_to_write = sound->sample_count;
                }
            }
            
            _assert( sound->current_sample < sound->sample_count );
            
            f32 left_sample = 0;
            f32 right_sample = 0;
            
            u32 sound_container_size = sound->format->container_size;
            
            if ( sound->format->channel_count == 1 ) {
                
                if ( sound_container_size == 1 ) {
                    _assert( !"I think 8bits wave file encode things in a special way." );
                    left_sample = right_sample = audio_s8_to_f32( cast( u8*, source ) );
                    source++;
                } else if ( sound_container_size == 2 ) {
                    left_sample = right_sample = audio_s16_to_f32( cast( u8*, source ) );
                    source += 2;
                } else if ( sound_container_size == 3 ) {
                    left_sample = right_sample = audio_s24_to_f32( cast( u8*, source ) );
                    source += 3;
                } else {
                    _assert( sound_container_size == 4 );
                    _assert( !"I think this should be float directly." );
                    left_sample = right_sample = cast( f32, deref( source ) );
                    source += 4;
                }
                
                right_sample = left_sample;
                
            } else if ( sound->format->channel_count == 2 ) {
                
                if ( sound_container_size == 1 ) {
                    _assert( !"I think 8bits wave file encode things in a special way." );
                    left_sample = audio_s8_to_f32( cast( u8*, source ) );
                    source++;
                    right_sample = audio_s8_to_f32( cast( u8*, source ) );
                    source++;
                } else if ( sound_container_size == 2 ) {
                    left_sample = audio_s16_to_f32( cast( u8*, source ) );
                    source += 2;
                    right_sample = audio_s16_to_f32( cast( u8*, source ) );
                    source += 2;
                } else if ( sound_container_size == 3 ) {
                    left_sample = audio_s24_to_f32( cast( u8*, source ) );
                    source += 3;
                    right_sample = audio_s24_to_f32( cast( u8*, source ) );
                    source += 3;
                } else {
                    _assert( sound_container_size == 4 );
                    _assert( !"I think this should be float directly." );
                    left_sample = cast( f32, deref( source ) );
                    source += 4;
                    right_sample = cast( f32, deref( source ) );
                    source += 4;
                }
            }
            
            left_sample *= mixer->volume * sound->volume * sound->panning;
            right_sample *= mixer->volume * sound->volume * ( 1.0f - sound->panning );
            
            left_sample = math_clamp( -1.0f, left_sample, 1.0f );
            right_sample = math_clamp( -1.0f, right_sample, 1.0f );
            
            if ( mixer_container_size == 1 ) {
                
                s8 left_sample_s8 = audio_f32_to_s8( left_sample );
                s8 right_sample_s8 = audio_f32_to_s8( right_sample );
                deref( destination ) += left_sample_s8;
                destination++;
                deref( destination ) += right_sample_s8;
                destination++;
                
            } else if ( mixer_container_size == 2 ) {
                
                s16 left_sample_s16 = audio_f32_to_s16( left_sample );
                s16 right_sample_s16 = audio_f32_to_s16( right_sample );
                s16* destination_s16 = cast( s16*, destination );
                deref( destination_s16 ) += left_sample_s16;
                destination += 2;
                destination_s16 += 1;
                deref( destination_s16 ) += right_sample_s16;
                destination += 2;
                
            } else if ( mixer_container_size == 3 ) {
                
                s32 left_sample_s32 = audio_f32_to_s24( left_sample );
                s32 right_sample_s32 = audio_f32_to_s24( right_sample );
                s32 left_destination_s32 = math_s24_to_s32( destination );
                s32 right_destination_s32 = math_s24_to_s32( destination + 3 );
                left_sample_s32 += left_destination_s32;
                right_sample_s32 += right_destination_s32;
                
                s8* left_bytes = cast( s8*, &left_sample_s32 );
                s8* right_bytes = cast( s8*, &right_sample_s32 );
                
                destination[ 0 ] = left_bytes[ 0 ];
                destination[ 1 ] = left_bytes[ 1 ];
                destination[ 2 ] = left_bytes[ 2 ];
                destination += 3;
                destination[ 0 ] = right_bytes[ 0 ];
                destination[ 1 ] = right_bytes[ 1 ];
                destination[ 2 ] = right_bytes[ 2 ];
                destination += 3;
                
            } else if ( mixer_container_size == 4 ) {
                
                f32* destination_f32 = cast( f32*, destination );
                deref( destination_f32 ) += left_sample;
                destination += 4;
                destination_f32 += 1;
                deref( destination_f32 ) += right_sample;
                destination += 4;
            }
            
            sound->current_sample += 1;
        }
        
        if ( !sound->loops && sound->current_sample >= sound->sample_count ) {
            
            deref( previous ) = sound->next;
            deref( sound ) = ( audio_sound_t ) { 0 };
            sound->next = mixer->free;
            mixer->free = sound;
            sound = deref( previous );
            
        } else {
            
            previous = &sound->next;
            sound = sound->next;
        } 
    }
}

/* MARK resampling */

/*
- Decimation: To decrease the sampling rate
- Interpolation: To increase the sampling rate
- Resampling:To combine decimation and interpolation in order to change the sampling rate by a fractional value that can be expressed as a ratio. For example, to resample by a factor of 1.5, you just interpolate by a factor of 3 then decimate by a factor of 2 (to change the sampling rate by a factor of 3/2=1.5.)

# Decimation
https://dspguru.com/dsp/faqs/multirate/decimation/
The decimation factor is simply the ratio of the input rate to the output rate. It is usually symbolized by “M”, so input rate / output rate=M.
Decimation involves throwing away samples, so you can only decimate by integer factors

This combined operation of (low-pass) filtering and downsampling is called decimation.
The low-pass filter needs to remove frequency that don't obey the nyquist theorem in the output from the input signal ( if output is 10khz, you need to remove any signal that is more than 10/2khz.

https://gist.github.com/kbob/045978eb044be88fe568
//   https://christianfloisand.wordpress.com/2012/12/05/audio-resampling-part-1/
//   https://christianfloisand.wordpress.com/2013/01/28/audio-resampling-part-2/
//   http://www.dspguide.com/ch16.htm
//   http://en.wikipedia.org/wiki/Window_function#Blackman_windows

https://wiki.hydrogenaudio.org/index.php?title=Category:Signal_Processing
https://wiki.hydrogenaudio.org/index.php?title=Resampling
https://ccrma.stanford.edu/~jos/resample/

https://ccrma.stanford.edu/~jos/filters/Simplest_Lowpass_Filter.html

https://www.keil.com/pack/doc/cmsis/DSP/html/group__FIR__decimate.html
https://www.mathworks.com/help/dsp/ref/dsp.firdecimator-system-object.html#bsfxw05_sep_btkr5tg-1

https://dspguru.com/dsp/faqs/multirate/resampling/
*/



#define AUDIO_H

#endif
