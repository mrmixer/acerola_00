#if !defined( AUDIO_TYPES_H )

typedef struct audio_format_t {
    
    u32 samples_per_second;
    u16 bits_per_sample;
    u16 channel_count;
    u32 channel_mask;
    u16 container_size;
    
} audio_format_t;

typedef struct audio_sound_t {
    
    u32 current_sample;
    u32 sample_count;
    f32 panning;
    f32 volume;
    b32 paused;
    b32 loops;
    data_t* samples;
    audio_format_t* format;
    u64 id;
    struct audio_sound_t* next;
    
} audio_sound_t;

typedef struct audio_id_t {
    audio_sound_t* sound;
    u64 id;
} audio_id_t;

typedef struct audio_command_t {
    
    u32 type;
    
    union {
        
        struct {
            u32 sample_count;
            data_t* samples;
            audio_format_t* format;
            f32 panning;
            f32 volume;
            b32 loops;
            audio_id_t* id;
        } play;
        
        struct {
            audio_id_t id;
        } stop;
        
        struct {
            audio_id_t id;
        } pause;
        
        struct {
            audio_id_t id;
        } resume;
        
        struct {
            audio_id_t id;
            f32 volume;
        } sound_volume;
        
        struct {
            audio_id_t id;
            f32 panning;
        } sound_panning;
        
        struct {
            f32 volume;
        } global_volume;
    };
    
} audio_command_t;

typedef struct audio_mixer_t {
    
    data_t command_memory;
    data_t sound_memory;
    audio_format_t format;
    f32 volume;
    
    u32 max_sound_count;
    audio_command_t* input_commands;
    u32 input_write;
    u32 input_read;
    audio_sound_t* sounds;
    audio_sound_t* free;
    u64 sound_id;
    
} audio_mixer_t;

#define AUDIO_TYPES_H
#endif
