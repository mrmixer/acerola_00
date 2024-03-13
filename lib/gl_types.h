
#if !defined( GL_TYPES_H )

#include "../lib/font.h"

typedef enum gl_flag_t {
    gl_flag_none = 0x0,
    gl_flag_depth_testing = 0x1 << 0,
    gl_flag_backface_culling = 0x1 << 1,
    gl_flag_alpha_blending = 0x1 << 2,
} gl_flags_t;

typedef enum gl_texture_flag_t {
    
    gl_texture_flag_none = 0 << 0,
    
    gl_texture_flag_inverse_input_format = 1 << 0,
    gl_texture_flag_generate_mipmaps = 1 << 1,
    
} gl_texture_flag_t;

typedef enum gl_sampler_type_t {
    
    gl_sampler_type_point,
    gl_sampler_type_linear,
    gl_sampler_type_anisotropic_4,
    gl_sampler_type_anisotropic_8,
    gl_sampler_type_anisotropic_16,
    
    gl_sampler_type_count
} gl_sampler_type_t;

_ctassert( gl_sampler_type_count <= array_count( cast( window_t*, 0 )->gl_platform.samplers ) );

typedef enum gl_command_type_t {
    
    gl_command_type_none,
    
    gl_command_type_clear,
    gl_command_type_viewport,
    gl_command_type_point,
    gl_command_type_line,
    gl_command_type_triangle_strip,
    gl_command_type_triangle_strip_textured,
    gl_command_type_text,
    gl_command_type_batch,
    gl_command_type_scissor,
    
    gl_command_type_count
        
} gl_command_type_t;

typedef struct gl_vertex_format_t {
    u8 positions;
    u8 colors;
    u8 uvs;
    u8 index_size;
} gl_vertex_format_t;

typedef struct gl_batch_t {
    
    gl_command_type_t type;
    u32 command_count;
    data_t commands;
    data_t datas;
    gl_buffer_t batch_constants_buffer;
    gl_buffer_t vertex_buffer;
    u32 v_buffer_size;
    u32 vertex_used;
    u32 vertex_index;
    gl_buffer_t index_buffer;
    u32 i_buffer_size;
    u32 index_used;
    /* NOTE simon (08/02/24 14:35:48): If we want to have per command uniform buffer we could use this. I'm not doing it now because it doesn't make sens for my current use (1 draw call for a lot of rectangle; we handle per rectangle color in vertex color ). */
    // gl_buffer_t command_constants_buffer;
    gl_shader_t* shader;
    f32* matrix;
    gl_vertex_format_t vertex_format;
    font_collection_t* collection;
    gl_sampler_t* sampler;
    gl_texture_t* texture;
    data_t uniform;
    
} gl_batch_t;

typedef struct gl_command_t {
    
    gl_command_type_t type;
    
    union {
        
        struct {
            u32 color;
        } clear;
        
        struct {
            s32 x, y;
            u32 width, height;
        } viewport;
        
        struct {
            b32 enable;
            s32 x, y, width, height;
        } scissor;
        
        struct {
            f32* vertex_array;
            u32 point_count;
            f32 size;
            vec4 color;
        } point;
        
        struct {
            f32* vertex_array;
            u32 point_count;
            vec4 color;
        } line;
        
        struct {
            f32* vertex_array;
            u32 triangle_count;
            vec4 color;
            gl_texture_t* texture;
            gl_sampler_t* sampler;
        } triangle_strip;
        
        struct {
            vec3 position;
            u32 font_index;
            data_t string;
            vec4 color;
            u32 outputed_codepoint_count;
            u32 previous_codepoint;
        } text;
        
        struct {
            gl_batch_t* batch;
        } batch;
    };
} gl_command_t;

#define gl_command_t_alignment 8

typedef struct gl_command_buffer_t {
    
    u32 command_count;
    data_t commands;
    
} gl_command_buffer_t;

typedef struct gl_t {
    
    window_t* window;
    gl_command_buffer_t commands;
    gl_batch_t batches[ 16 ];
    gl_shader_t shaders[ 16 ];
    f32 matrix[ 16 ];
    u8 batch_count;
    
} gl_t;

#define GL_TYPES_H
#endif
