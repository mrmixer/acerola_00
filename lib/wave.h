
#if !defined( WAVE_H )

#include "../lib/audio.h"

typedef struct wave_chunk_header_t {
    u8 id[ 4 ];
    u32 size;
} wave_chunk_header_t;

typedef struct wave_format_chunk_t {
    
    u16 tag;
    u16 channels;
    u32 samples_per_sec;
    u32 avg_bytes_per_sec;
    u16 block_align;
    u16 bits_per_sample;
} wave_format_chunk_t;

typedef struct wave_format_chunk_extensible_t {
    
    wave_format_chunk_t format;
    u16 size;
    u16 valid_bits_per_sample;
    u32 channel_mask;
    u8 sub_format[ 16 ];
} wave_format_chunk_extensible_t;

typedef struct wave_fact_chunk_t {
    u32 total_samples_per_channel;
} wave_fact_chunk_t;

// NOTE simon: to avoid unnecessary includes ( mmreg.h )

#define wave_WAVE_FORMAT_PCM 0x0001
#define wave_WAVE_FORMAT_IEEE_FLOAT 0x0003
#define wave_WAVE_FORMAT_ALAW 0x0006
#define wave_WAVE_FORMAT_MULAW 0x0007
#define wave_WAVE_FORMAT_EXTENSIBLE 0xFFFE

#define wave_WAVE_EXTENSIBLE_GUID( type, guid ) \
*( ( u16* ) ( guid ) ) = type; \
( guid )[ 2 ] = 0x00;\
( guid )[ 3 ] = 0x00;\
( guid )[ 4 ] = 0x00;\
( guid )[ 5 ] = 0x00;\
( guid )[ 6 ] = 0x10;\
( guid )[ 7 ] = 0x00;\
( guid )[ 8 ] = 0x80;\
( guid )[ 9 ] = 0x00;\
( guid )[ 10 ] = 0x00;\
( guid )[ 11 ] = 0xAA;\
( guid )[ 12 ] = 0x00;\
( guid )[ 13 ] = 0x38;\
( guid )[ 14 ] = 0x9B;\
( guid )[ 15 ] = 0x71;

stu void wave_save( audio_format_t sound_format, f32 length, u8* memory, data_t file_name ) {
    
    u32 length_in_samples = cast( u32, ceil( sound_format.samples_per_second * length ) );
    u32 header_size = sizeof( wave_chunk_header_t );
    b32 bit_remaining = ( sound_format.bits_per_sample % 8 ) != 0;
    u32 container_size = ( sound_format.bits_per_sample / 8 ) + ( ( bit_remaining ) ? 1 : 0 );
    u32 data_size = container_size * sound_format.channel_count * length_in_samples;
    u32 padding = data_size % 2;
    
    u16 wave_format = wave_WAVE_FORMAT_PCM;
    u32 format_size = sizeof( wave_format_chunk_t );
    u32 fact_size = 0;
    
    if ( sound_format.bits_per_sample > 16 || bit_remaining || sound_format.channel_count > 2 || sound_format.channel_mask > 0 ) {
        
        wave_format = wave_WAVE_FORMAT_EXTENSIBLE;
        format_size = sizeof( wave_format_chunk_extensible_t );
        fact_size = header_size + sizeof( wave_fact_chunk_t );
    }
    
    u32 riff_size = sizeof( "WAVE" ) - 1 + header_size + format_size + fact_size + header_size + data_size + padding;
    
    data_t file = memory_get_virtual( header_size + riff_size, 0 );
    // data_t riff = string_l( "RIFF" );
    // string_push( &file, riff.bytes );
    memory_push_copy_l( &file, "RIFF" );
    memory_push_u32( &file, riff_size, 1 ); /* TODO simon: Verify that 1 is the correct alignment here. */
    // data_t wave = string_l( "WAVE" );
    // string_push( &file, wave.bytes );
    memory_push_copy_l( &file, "WAVE" );
    
    // data_t fmt = string_l( "fmt " );
    // string_push( &file, fmt.bytes );
    memory_push_copy_l( &file, "fmt " );
    memory_push_u32( &file, format_size, 1 ); /* TODO simon: Verify that 1 is the correct alignment here. */
    wave_format_chunk_t* format = 0;
    
    if ( wave_format == wave_WAVE_FORMAT_PCM ) {
        
        format = memory_push_struct_aligned( &file, wave_format_chunk_t, 1 );
        format->tag = wave_WAVE_FORMAT_PCM;
        
    } else if ( wave_format == wave_WAVE_FORMAT_EXTENSIBLE ) {
        
        wave_format_chunk_extensible_t* extensible = memory_push_struct_aligned( &file, wave_format_chunk_extensible_t, 1 );
        extensible->format.tag = wave_WAVE_FORMAT_EXTENSIBLE;
        extensible->size = 22;
        extensible->valid_bits_per_sample = sound_format.bits_per_sample;
        extensible->channel_mask = sound_format.channel_mask;
        
        u16* sub_format = cast( u16*, extensible->sub_format );
        wave_WAVE_EXTENSIBLE_GUID( wave_WAVE_FORMAT_PCM, sub_format );
        format = cast( wave_format_chunk_t*, extensible );
        
    } else {
        
        debug_break( );
    }
    
    format->channels = sound_format.channel_count;
    format->samples_per_sec = sound_format.samples_per_second;
    format->avg_bytes_per_sec = sound_format.channel_count * container_size * sound_format.samples_per_second;
    format->block_align = cast( u16, sound_format.channel_count * container_size );
    format->bits_per_sample = cast( u16, container_size * 8 );
    
    if ( wave_format != wave_WAVE_FORMAT_PCM ) {
        // data_t fact = string_l( "fact" );
        // string_push( &file, fact.bytes );
        memory_push_copy_l( &file, "fact" );
        memory_push_u32( &file, 4, 1 ); /* TODO simon: Verify that 1 is the correct alignment here. */
        memory_push_u32( &file, length_in_samples, 1 ); /* TODO simon: Verify that 1 is the correct alignment here. */
    }
    
    // data_t data_string = string_l( "data" );
    // string_push( &file, data_string.bytes );
    memory_push_copy_l( &file, "data" );
    memory_push_u32( &file, data_size, 1 ); /* TODO simon: Verify that 1 is the correct alignment here. */
    
    u32 error = file_error_none;
    file_handle_t f = file_open( file_name, file_mode_write, true, &error );
    file_write_handle( f, 0, file.used, file.bytes, &error );
    file_write_handle( f, file.used, data_size, memory, &error );
    
    if ( padding ) {
        u8 pad = 0;
        file_write_handle( f, file.used + data_size, 1, &pad, &error );
    }
    
    file_close( f, &error );
    
    memory_free_virtual( &file );
}

stu data_t wave_load( audio_format_t* format, u32* length_in_samples, data_t* memory, data_t file_name ) {
    
    data_t result = { 0 };
    data_t file = file_read_entire( file_name, 0, &g_file_error );
    
    u8* chunk_id = memory_seek_p( file.bytes, memory_end( file ), "WAVE", 4 );
    
    if ( chunk_id < memory_end( file ) )  {
        
        chunk_id += 12;
        wave_format_chunk_extensible_t* wave_format = cast( wave_format_chunk_extensible_t*, chunk_id );
        format->samples_per_second = wave_format->format.samples_per_sec;
        format->channel_count = wave_format->format.channels;
        format->channel_mask = 0;
        u32 container_size = wave_format->format.bits_per_sample / 8;
        
        if ( wave_format->format.tag == wave_WAVE_FORMAT_PCM ) {
            
            format->bits_per_sample = wave_format->format.bits_per_sample;
            
        } else if ( wave_format->format.tag == wave_WAVE_FORMAT_EXTENSIBLE ) {
            
            format->bits_per_sample = wave_format->valid_bits_per_sample;
            format->channel_mask = wave_format->channel_mask;
            
        } else {
            
            debug_break( );
        }
        
        b32 bit_remaining = ( format->bits_per_sample % 8 ) != 0;
        format->container_size = ( format->bits_per_sample / 8 ) + ( ( bit_remaining ) ? 1 : 0 );
        
        chunk_id = memory_seek_p( chunk_id, memory_end( file ), "data", 4 );
        
        if ( chunk_id < memory_end( file ) )  {
            
            chunk_id += 4;
            u32 data_size = deref( cast( u32*, chunk_id ) );
            deref( length_in_samples ) = data_size / ( format->channel_count * container_size );
            chunk_id += 4;
            
            _assert( memory->reserved - memory->used > data_size );
            
            result = memory_get_block( memory, data_size, 4 );
            
            if ( result.reserved ) {
                memory_push_copy_p( &result, chunk_id, data_size, 1 );
            }
        }
    }
    
    file_free_memory( &file );
    
    return result;
}

typedef struct audio_wave_t {
    
    audio_format_t format;
    u32 sample_count;
    data_t samples;
    
} audio_wave_t;

stu audio_wave_t audio_wave_load( data_t filename, data_t* memory ) {
    
    audio_wave_t result = { 0 };
    result.samples = wave_load( &result.format, &result.sample_count, memory, filename );
    
    return result;
}

stu b32 audio_wave_play( audio_mixer_t* mixer, audio_wave_t* audio, f32 volume, f32 panning, b32 loops, audio_id_t* id ) {
    
    b32 result = audio_play( mixer, &audio->samples, &audio->format, audio->sample_count, volume, panning, loops, id );
    return result;
}

#define WAVE_H

#endif //WAVE_H
