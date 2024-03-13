
#ifndef GL_H

#include "../lib/window_types.h"
#include "../lib/gl_directx_types.h"
#include "../lib/gl_types.h"
#include "../lib/gl_directx.h"
#include "../lib/font.h"

/*
NOTE simon: Introduction to antialiasing https://www.youtube.com/watch?v=BmT0ZKEzn-Q
*/

/* TODO simon (14/02/24 15:52:50): Handle errors */
stu gl_vertex_format_t gl_vertex_format( u8 position_comporents, u8 color_components, u8 uv_components, u8 index_size ) {
    
    gl_vertex_format_t result = { 0 };
    
    result.positions = math_min( position_comporents, 3 );
    _assert( color_components == 1 || color_components == 4 );
    result.colors = color_components;
    _assert( uv_components == 0 || uv_components == 2 );
    result.uvs = uv_components;
    result.index_size = math_min( index_size, 4 );
    
    return result;
}

stu u8 gl_vertex_format_compute_vertex_size_byte( gl_vertex_format_t vertex_format ) {
    u8 result = vertex_format.positions * sizeof( f32 );
    result += vertex_format.colors * sizeof( f32 );
    result += vertex_format.uvs * sizeof( f32 );
    return result;
}

stu gl_command_buffer_t gl_create_command_buffer( data_t* memory, umm size, umm data_size ) {
    
    gl_command_buffer_t result = { 0 };
    result.command_count = 0;
    result.commands = memory_get_block( memory, size, 0 );
    
    return result;
}

stu void gl_reset_command_buffer( gl_command_buffer_t* buffer ) {
    
    buffer->command_count = 0;
    buffer->commands.used = 0;
}

stu gl_command_t* gl_clear( gl_command_buffer_t* buffer, vec4 color ) {
    
    gl_command_t* result = memory_push_struct( &buffer->commands, gl_command_t );
    _assert( result );
    
    if ( result ) {
        buffer->command_count++;
        result->type = gl_command_type_clear;
        result->clear.color = v4_to_u32_color( color );
    }
    
    return result;
}

stu gl_command_t* gl_viewport( gl_command_buffer_t* buffer, s32 x, s32 y, u32 width, u32 height ) {
    
    gl_command_t* result = memory_push_struct( &buffer->commands, gl_command_t );
    _assert( result );
    
    if ( result ) {
        buffer->command_count++;
        result->type = gl_command_type_viewport;
        result->viewport.x = x;
        result->viewport.y = y;
        result->viewport.width = width;
        result->viewport.height = height;
    }
    
    return result;
}

stu gl_command_t* gl_scissor( gl_command_buffer_t* buffer, b32 enable, s32 x, s32 y, s32 width, s32 height ) {
    
    gl_command_t* result = memory_push_struct( &buffer->commands, gl_command_t );
    _assert( result );
    
    if ( result ) {
        buffer->command_count++;
        result->type = gl_command_type_scissor;
        result->scissor.enable = enable;
        result->scissor.x = math_max( 0, x );
        result->scissor.y = math_max( 0, y );
        result->scissor.width = math_max( 0, width );
        result->scissor.height = math_max( 0, height );
    }
    
    return result;
}

#define gl_scissor_disable( buffer ) gl_scissor( ( buffer ), false, 0, 0, 0, 0 )

stu gl_batch_t* gl_get_batch( gl_t* gl, umm id, b32 allow_unsafe ) {
    
    gl_batch_t* result = 0;
    
    _assert( id < array_count( gl->batches ) );
    
    if ( id < gl->batch_count || allow_unsafe ) {
        result = gl->batches + id;
    }
    
    return result;
}

stu gl_command_t* gl_batch_scissor( gl_batch_t* batch, b32 enable, s32 x, s32 y, s32 width, s32 height ) {
    
    gl_command_t* result = memory_push_struct( &batch->commands, gl_command_t );
    _assert( result );
    
    if ( result ) {
        batch->command_count++;
        result->type = gl_command_type_scissor;
        result->scissor.enable = enable;
        result->scissor.x = math_max( 0, x );
        result->scissor.y = math_max( 0, y );
        result->scissor.width = math_max( 0, width );
        result->scissor.height = math_max( 0, height );
    }
    
    return result;
}

#define gl_batch_scissor_rect( batch, rect ) gl_batch_scissor( ( batch ), true, \
( s32 ) ( ( rect ).left ), \
( s32 ) ( ( rect ).bottom ), \
( s32 ) ( ( rect ).right - ( rect ).left ), \
( s32 ) ( ( rect ).top - ( rect ).bottom ) )

#define gl_batch_scissor_disable( batch ) gl_batch_scissor( ( batch ), false, 0, 0, 0, 0 )

stu void gl_scissor_batches( gl_t* gl, b32 enable, f32 x, f32 y, f32 width, f32 height, u8* batches, umm batch_count ) {
    
    _assert( batch_count < array_count( gl->batches ) );
    
    u8 batch_indicies[ array_count( gl->batches ) ];
    
    if ( batches == 0 ) {
        
        batch_count = gl->batch_count;
        
        for ( umm i = 0; i < batch_count; i++ ) {
            batch_indicies[ i ] = ( u8 ) i;
        }
        
        batches = batch_indicies;
    }
    
    for ( umm i = 0; i < batch_count; i++ ) {
        _assert( batches[ i ] < array_count( gl->batches ) );
        gl_batch_scissor( gl->batches + batches[ i ], enable, ( s32 ) x, ( s32 ) y, ( s32 ) width, ( s32 ) height );
    }
}

#define gl_scissor_batches_disable( gl, batches, batch_count ) gl_scissor_batches( ( gl ), false, 0, 0, 0, 0, ( batches ), ( batch_count ) )
#define gl_scissor_batches_rect( gl, rect, batches, batch_count ) gl_scissor_batches( ( gl ), true, \
( rect ).left, \
( rect ).bottom, \
( rect ).right - ( rect ).left, \
( rect ).top - ( rect ).bottom, \
( batches ), \
( batch_count ) )

stu void* gl_command_get_color_array( gl_command_t* command, gl_vertex_format_t* vertex_format, umm* byte_offset ) {
    
    void* result = 0;
    
    if ( vertex_format->colors ) {
        
        f32* v_array = 0;
        
        if ( command->type == gl_command_type_point ) {
            v_array = command->point.vertex_array;
        } else if ( command->type == gl_command_type_line ) {
            v_array = command->line.vertex_array;
        } else if ( command->type == gl_command_type_triangle_strip || command->type == gl_command_type_triangle_strip_textured ) {
            v_array = command->triangle_strip.vertex_array;
        }
        
        result = cast( void*, v_array + vertex_format->positions );
        
        deref( byte_offset ) = vertex_format->positions * sizeof( f32 ) + vertex_format->colors * sizeof( f32 ) + vertex_format->uvs * sizeof( f32 );
    }
    
    return result;
}

stu gl_command_t* gl_batch_points( gl_batch_t* batch, f32* vertex_array, u32 point_count, f32 size, vec4 color ) {
    
    /* TODO simon (13/02/24 12:14:09): Handle errors. */
    gl_command_t* result = memory_push_struct( &batch->commands, gl_command_t );
    _assert( result );
    
    if ( result ) {
        batch->command_count++;
        result->type = gl_command_type_point;
        
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( batch->vertex_format );
        umm array_size = point_count * vertex_size_byte;
        result->point.vertex_array = cast( f32*, memory_push_copy_p( &batch->datas, vertex_array, array_size, 4 ) );
        _assert( result->point.vertex_array );
        
        color = v4_srgb_to_linear_precise( color );
        
        if ( batch->vertex_format.colors == 1 ) {
            
            umm byte_offset = 0;
            u32* c_array = cast( u32*, gl_command_get_color_array( result, &batch->vertex_format, &byte_offset ) );
            
            for ( umm i = 0; i < point_count; i++ ) {
                vec4 srgb = v4_from_u32_color( c_array[ 0 ] );
                vec4 linear = v4_srgb_to_linear_precise( srgb );
                vec4 output_color = v4_hadamard( linear, color );
                c_array[ 0 ] = v4_to_u32_color( output_color );
                c_array = pointer_add_offset( u32, c_array, byte_offset );
            }
            
        } else if ( batch->vertex_format.colors == 4 ) {
            
            umm byte_offset = 0;
            vec4* c_array = cast( vec4*, gl_command_get_color_array( result, &batch->vertex_format, &byte_offset ) );
            
            for ( umm i = 0; i < point_count; i++ ) {
                vec4 linear = v4_srgb_to_linear_precise( c_array[ 0 ] );
                vec4 output_color = v4_hadamard( linear, color );
                c_array[ 0 ] = output_color;
                c_array = pointer_add_offset( vec4, c_array, byte_offset );
            }
        }
        
        result->point.point_count = point_count;
        result->point.size = size;
        result->point.color = color;
    }
    
    return result;
}

stu gl_command_t* gl_batch_lines( gl_batch_t* batch, f32* vertex_array, u32 point_count, vec4 color ) {
    
    gl_command_t* result = memory_push_struct( &batch->commands, gl_command_t );
    _assert( result );
    
    if ( result ) {
        batch->command_count++;
        result->type = gl_command_type_line;
        
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( batch->vertex_format );
        umm array_size = point_count * vertex_size_byte;
        result->line.vertex_array = ( f32* ) memory_push_copy_p( &batch->datas, vertex_array, array_size, 4 );
        _assert( result->line.vertex_array );
        
        color = v4_srgb_to_linear_precise( color );
        
        if ( batch->vertex_format.colors == 1 ) {
            
            umm byte_offset = 0;
            u32* c_array = cast( u32*, gl_command_get_color_array( result, &batch->vertex_format, &byte_offset ) );
            
            for ( umm i = 0; i < point_count; i++ ) {
                vec4 srgb = v4_from_u32_color( c_array[ 0 ] );
                vec4 linear = v4_srgb_to_linear_precise( srgb );
                vec4 output_color = v4_hadamard( linear, color );
                c_array[ 0 ] = v4_to_u32_color( output_color );
                c_array = pointer_add_offset( u32, c_array, byte_offset );
            }
            
        } else if ( batch->vertex_format.colors == 4 ) {
            
            umm byte_offset = 0;
            vec4* c_array = cast( vec4*, gl_command_get_color_array( result, &batch->vertex_format, &byte_offset ) );
            
            for ( umm i = 0; i < point_count; i++ ) {
                vec4 linear = v4_srgb_to_linear_precise( c_array[ 0 ] );
                vec4 output_color = v4_hadamard( linear, color );
                c_array[ 0 ] = output_color;
                c_array = pointer_add_offset( vec4, c_array, byte_offset );
            }
        }
        
        result->line.point_count = point_count;
        result->line.color = color;
    }
    
    return result;
}

stu gl_command_t* gl_batch_rectangle( gl_batch_t* batch, f32* rectangle, f32* uvs, vec4* colors, vec4 color ) {
    
    profiler_event_start( gl_batch_rectangle );
    _assert( rectangle );
    
    gl_command_t* result = memory_push_struct( &batch->commands, gl_command_t );
    _assert( result );
    
    if ( result ) {
        
        batch->command_count++;
        result->type = gl_command_type_triangle_strip;
        
        u32 triangle_count = 2;
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( batch->vertex_format );
        umm array_size = ( 2 + triangle_count ) * vertex_size_byte;
        result->triangle_strip.vertex_array = cast( f32*, memory_push_size( &batch->datas, array_size, 4 ) );
        _assert( result->triangle_strip.vertex_array );
        result->triangle_strip.triangle_count = triangle_count;
        result->triangle_strip.texture = 0;
        
        vec4 dummy_colors[ 4 ] = { white, white, white, white };
        f32 dummy_uvs[ 4 ] = { 0, 0, 1, 1 };
        
        if ( !colors ) {
            colors = dummy_colors;
        }
        
        if ( !uvs ) {
            uvs = dummy_uvs;
        }
        
        b32 has_uv = !!batch->vertex_format.uvs;
        
        {
            f32* destination_positions = result->triangle_strip.vertex_array;
            destination_positions[ 0 ] = rectangle[ 0 ];
            destination_positions[ 1 ] = rectangle[ 3 ];
            destination_positions = pointer_add_offset( f32, destination_positions, vertex_size_byte );
            destination_positions[ 0 ] = rectangle[ 0 ];
            destination_positions[ 1 ] = rectangle[ 1 ];
            destination_positions = pointer_add_offset( f32, destination_positions, vertex_size_byte );
            destination_positions[ 0 ] = rectangle[ 2 ];
            destination_positions[ 1 ] = rectangle[ 3 ];
            destination_positions = pointer_add_offset( f32, destination_positions, vertex_size_byte );
            destination_positions[ 0 ] = rectangle[ 2 ];
            destination_positions[ 1 ] = rectangle[ 1 ];
        }
        
        color = v4_srgb_to_linear_precise( color );
        
        if ( batch->vertex_format.colors == 1 ) {
            
            umm byte_offset = 0;
            u32* c_array = cast( u32*, gl_command_get_color_array( result, &batch->vertex_format, &byte_offset ) );
            
            c_array[ 0 ] = v4_to_u32_color( v4_hadamard( v4_srgb_to_linear_precise( colors[ 2 ] ), color ) );
            c_array = pointer_add_offset( u32, c_array, byte_offset );
            c_array[ 0 ] = v4_to_u32_color( v4_hadamard( v4_srgb_to_linear_precise( colors[ 0 ] ), color ) );
            c_array = pointer_add_offset( u32, c_array, byte_offset );
            c_array[ 0 ] = v4_to_u32_color( v4_hadamard( v4_srgb_to_linear_precise( colors[ 3 ] ), color ) );
            c_array = pointer_add_offset( u32, c_array, byte_offset );
            c_array[ 0 ] = v4_to_u32_color( v4_hadamard( v4_srgb_to_linear_precise( colors[ 1 ] ), color ) );
            
        } else if ( batch->vertex_format.colors == 4 ) {
            
            umm byte_offset = 0;
            vec4* c_array = cast( vec4*, gl_command_get_color_array( result, &batch->vertex_format, &byte_offset ) );
            
            c_array[ 0 ] = v4_hadamard( v4_srgb_to_linear_precise( colors[ 2 ] ), color );
            c_array = pointer_add_offset( vec4, c_array, byte_offset );
            c_array[ 0 ] = v4_hadamard( v4_srgb_to_linear_precise( colors[ 0 ] ), color );
            c_array = pointer_add_offset( vec4, c_array, byte_offset );
            c_array[ 0 ] = v4_hadamard( v4_srgb_to_linear_precise( colors[ 3 ] ), color );
            c_array = pointer_add_offset( vec4, c_array, byte_offset );
            c_array[ 0 ] = v4_hadamard( v4_srgb_to_linear_precise( colors[ 1 ] ), color );
        }
        
        if ( has_uv ) {
            f32* destination_uvs = cast( f32*, result->triangle_strip.vertex_array + 2 );
            destination_uvs += batch->vertex_format.colors;
            destination_uvs[ 0 ] = uvs[ 0 ];
            destination_uvs[ 1 ] = uvs[ 3 ];
            destination_uvs = pointer_add_offset( f32, destination_uvs, vertex_size_byte );
            destination_uvs[ 0 ] = uvs[ 0 ];
            destination_uvs[ 1 ] = uvs[ 1 ];
            destination_uvs = pointer_add_offset( f32, destination_uvs, vertex_size_byte );
            destination_uvs[ 0 ] = uvs[ 2 ];
            destination_uvs[ 1 ] = uvs[ 3 ];
            destination_uvs = pointer_add_offset( f32, destination_uvs, vertex_size_byte );
            destination_uvs[ 0 ] = uvs[ 2 ];
            destination_uvs[ 1 ] = uvs[ 1 ];
        }
        
        result->triangle_strip.color = color;
    }
    
    profiler_event_end( gl_batch_rectangle );
    return result;
}

stu gl_command_t* gl_batch_rectangle_textured( gl_batch_t* batch, f32* rectangle, f32* uvs, vec4* colors, vec4 color, gl_texture_t* texture, gl_sampler_t* sampler ){
    
    gl_command_t* result = gl_batch_rectangle( batch, rectangle, uvs, colors, color );
    
    if ( result ) {
        result->type = gl_command_type_triangle_strip_textured;
        result->triangle_strip.texture = texture;
        result->triangle_strip.sampler = sampler;
    }
    
    return result;
}

stu gl_command_t* gl_batch_triangle_strip( gl_batch_t* batch, f32* vertex_array, u32 triangle_count, vec4 color ) {
    
    gl_command_t* result = memory_push_struct( &batch->commands, gl_command_t );
    _assert( result );
    
    if ( result ) {
        batch->command_count++;
        result->type = gl_command_type_triangle_strip;
        
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( batch->vertex_format );
        umm array_size = ( 2 + triangle_count ) * vertex_size_byte;
        result->triangle_strip.vertex_array = cast( f32*, memory_push_copy_p( &batch->datas, vertex_array, array_size, 4 ) );
        _assert( result->triangle_strip.vertex_array );
        
        color = v4_srgb_to_linear_precise( color );
        
        if ( batch->vertex_format.colors == 1 ) {
            
            umm byte_offset = 0;
            u32* c_array = cast( u32*, gl_command_get_color_array( result, &batch->vertex_format, &byte_offset ) );
            
            for ( umm i = 0; i < triangle_count + 2; i++ ) {
                vec4 srgb = v4_from_u32_color( c_array[ 0 ] );
                vec4 linear = v4_srgb_to_linear_precise( srgb );
                vec4 output_color = v4_hadamard( linear, color );
                c_array[ 0 ] = v4_to_u32_color( output_color );
                c_array = pointer_add_offset( u32, c_array, byte_offset );
            }
            
        } else if ( batch->vertex_format.colors == 4 ) {
            
            umm byte_offset = 0;
            vec4* c_array = cast( vec4*, gl_command_get_color_array( result, &batch->vertex_format, &byte_offset ) );
            
            for ( umm i = 0; i < triangle_count + 2; i++ ) {
                vec4 linear = v4_srgb_to_linear_precise( c_array[ 0 ] );
                vec4 output_color = v4_hadamard( linear, color );
                c_array[ 0 ] = output_color;
                c_array = pointer_add_offset( vec4, c_array, byte_offset );
            }
        }
        
        result->triangle_strip.triangle_count = triangle_count;
        result->triangle_strip.color = color;
    }
    
    return result;
}

stu gl_command_t* gl_batch_triangle_strip_textured( gl_batch_t* batch, f32* vertex_array, u32 triangle_count, vec4 color, gl_texture_t* texture, gl_sampler_t* sampler ) {
    
    gl_command_t* result = memory_push_struct( &batch->commands, gl_command_t );
    _assert( result );
    
    if ( result ) {
        batch->command_count++;
        result->type = gl_command_type_triangle_strip_textured;
        
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( batch->vertex_format );
        umm array_size = ( 2 + triangle_count ) * vertex_size_byte;
        result->triangle_strip.vertex_array = memory_push_copy_p( &batch->datas, vertex_array, array_size, 4 );
        _assert( result->triangle_strip.vertex_array );
        
        color = v4_srgb_to_linear_precise( color );
        
        if ( batch->vertex_format.colors == 1 ) {
            
            umm byte_offset = 0;
            u32* c_array = cast( u32*, gl_command_get_color_array( result, &batch->vertex_format, &byte_offset ) );
            
            for ( umm i = 0; i < triangle_count + 2; i++ ) {
                vec4 srgb = v4_from_u32_color( c_array[ 0 ] );
                vec4 linear = v4_srgb_to_linear_precise( srgb );
                vec4 output_color = v4_hadamard( linear, color );
                c_array[ 0 ] = v4_to_u32_color( output_color );
                c_array = pointer_add_offset( u32, c_array, byte_offset );
            }
            
        } else if ( batch->vertex_format.colors == 4 ) {
            
            umm byte_offset = 0;
            vec4* c_array = cast( vec4*, gl_command_get_color_array( result, &batch->vertex_format, &byte_offset ) );
            
            for ( umm i = 0; i < triangle_count + 2; i++ ) {
                vec4 linear = v4_srgb_to_linear_precise( c_array[ 0 ] );
                vec4 output_color = v4_hadamard( linear, color );
                c_array[ 0 ] = output_color;
                c_array = pointer_add_offset( vec4, c_array, byte_offset );
            }
        }
        
        result->triangle_strip.triangle_count = triangle_count;
        result->triangle_strip.color = color;
        result->triangle_strip.texture = texture;
        result->triangle_strip.sampler = sampler;
    }
    
    return result;
}

stu gl_command_t* gl_batch_text( gl_batch_t* batch, void* string, umm byte_count, f32 x, f32 baseline_y, u32 previous_codepoint, u32 font_index, vec4 color ) {
    
    profiler_event_start( gl_batch_text );
    gl_command_t* result = memory_push_struct( &batch->commands, gl_command_t );
    _assert( result );
    
    if ( !previous_codepoint ) {
        previous_codepoint = 0xffffffff; /* encoding_null_codepoint; */
    }
    
    if ( result ) {
        batch->command_count++;
        result->type = gl_command_type_text;
        
        result->text.position = v3( x, baseline_y, 0 );
        result->text.string = memory_get_block( &batch->datas, byte_count, 1 );
        string_push_length( &result->text.string, ( char* ) string, byte_count );
        
        result->text.outputed_codepoint_count = 0;
        result->text.font_index = font_index;
        result->text.color = v4_srgb_to_linear_precise( color );
        result->text.previous_codepoint = previous_codepoint;
    }
    profiler_event_end( gl_batch_text );
    
    return result;
}

stu void gl_change_rectangle_color( gl_batch_t* batch, gl_command_t* command, vec4* colors, vec4 color ) {
    
    if ( batch->vertex_format.colors ) {
        
        vec4 dummy_colors[ 4 ] = { white, white, white, white };
        
        if ( !colors ) {
            colors = dummy_colors;
        }
        
        color = v4_srgb_to_linear_precise( color );
        
        umm byte_offset = 0;
        
        if ( batch->vertex_format.colors == 1 ) {
            
            u32* c_array = cast( u32*, gl_command_get_color_array( command, &batch->vertex_format, &byte_offset ) );
            c_array[ 0 ] = v4_to_u32_color( v4_hadamard( v4_srgb_to_linear_precise( colors[ 2 ] ), color ) );
            c_array = pointer_add_offset( u32, c_array, byte_offset );
            c_array[ 0 ] = v4_to_u32_color( v4_hadamard( v4_srgb_to_linear_precise( colors[ 0 ] ), color ) );
            c_array = pointer_add_offset( u32, c_array, byte_offset );
            c_array[ 0 ] = v4_to_u32_color( v4_hadamard( v4_srgb_to_linear_precise( colors[ 3 ] ), color ) );
            c_array = pointer_add_offset( u32, c_array, byte_offset );
            c_array[ 0 ] = v4_to_u32_color( v4_hadamard( v4_srgb_to_linear_precise( colors[ 1 ] ), color ) );
            
        } else {
            
            vec4* c_array = cast( vec4*, gl_command_get_color_array( command, &batch->vertex_format, &byte_offset ) );
            c_array[ 0 ] = v4_hadamard( v4_srgb_to_linear_precise( colors[ 2 ] ), color );
            c_array = pointer_add_offset( vec4, c_array, byte_offset );
            c_array[ 0 ] = v4_hadamard( v4_srgb_to_linear_precise( colors[ 0 ] ), color );
            c_array = pointer_add_offset( vec4, c_array, byte_offset );
            c_array[ 0 ] = v4_hadamard( v4_srgb_to_linear_precise( colors[ 3 ] ), color );
            c_array = pointer_add_offset( vec4, c_array, byte_offset );
            c_array[ 0 ] = v4_hadamard( v4_srgb_to_linear_precise( colors[ 1 ] ), color );
        }
        
        command->triangle_strip.color = color;
    }
}

stu gl_command_t* gl_push_batch( gl_command_buffer_t* commands, gl_batch_t* batch ) {
    
    gl_command_t* result = memory_push_struct( &commands->commands, gl_command_t );
    _assert( result );
    
    if ( result ) {
        commands->command_count++;
        result->type = gl_command_type_batch;
        result->batch.batch = batch;
    }
    
    return result;
}

stu void gl_batch_frame_init( gl_batch_t* batch ) {
    
    batch->command_count = 0;
    batch->commands.used = 0;
    batch->datas.used = 0;
    batch->vertex_used = 0;
    batch->vertex_index = 0;
    batch->index_used = 0;
}

stu font_t* gl_batch_get_font( gl_batch_t* batch, u32 font_id ) {
    
    _assert( batch );
    _assert( batch->type == gl_command_type_text );
    _assert( batch->collection );
    _assert( batch->collection->fonts );
    _assert( batch->collection->font_count > font_id );
    
    font_t* result = 0;
    
    if ( batch->type == gl_command_type_text && batch->collection && batch->collection->fonts && batch->collection->font_count > font_id ) {
        result = batch->collection->fonts + font_id;
    }
    
    return result;
}

// MARK render

stu void internal_gl_push_index( void* i_array, umm* i_array_index, umm value, umm index_size ) {
    
    if ( index_size == 1 ) {
        ( ( u8* ) i_array )[ *i_array_index ] = ( u8 ) value;
    } else if ( index_size == 2 ) {
        ( ( u16* ) i_array )[ *i_array_index ] = ( u16 ) value;
    } else {
        ( ( u32* ) i_array )[ *i_array_index ] = ( u32 ) value;
    }
    
    ( *i_array_index )++;
}

stu void internal_gl_push_primitive_restart( void* i_array, umm* i_array_index, umm index_size ) {
    
    if ( index_size == 1 ) {
        ( ( u8* ) i_array )[ *i_array_index ] = gl_primitive_restart_index_u8;
    } else if ( index_size == 2 ) {
        ( ( u16* ) i_array )[ *i_array_index ] = gl_primitive_restart_index_u16;
    } else {
        ( ( u32* ) i_array )[ *i_array_index ] = gl_primitive_restart_index_u32;
    }
    
    ( *i_array_index )++;
}

stu u32 gl_text_to_vertex_array( void* text, umm byte_count, f32 x, f32 y, u32 previous_codepoint, font_collection_t* collection, u32 font_index, f32* v_array, u32* i_array, umm index, vec4 color, gl_vertex_format_t vertex_format ) {
    
    u32 color_u32 = v4_to_u32_color( color );
    
    u8 index_size = vertex_format.index_size;
    umm vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( vertex_format );
    
    f32 sheet_width = ( f32 ) collection->atlas_width;
    f32 sheet_height = ( f32 ) collection->atlas_height;
    
    font_t* font = collection->fonts + font_index;
    f32 scale = font->scale;
    f32 line_start_x = x;
    
    codepoint_metrics_t* previous_codepoint_metrics = 0;
    
    if ( previous_codepoint != 0xffffffff /* encoding_null_codepoint */ ) {
        previous_codepoint_metrics = font_get_codepoint_metrics( font, previous_codepoint );
    }
    
    umm index_offset = 0;
    u32 outputed_codepoint_count = 0;
    u8* current = ( u8* ) text;
    u8* end = current + byte_count;
    
    while ( current < end ) {
        
        u32 codepoint = deref( current );
        u8 codeunit_count = 0;
        
        if ( codepoint == '\r' ) {
            
            codeunit_count = 1;
            
            if ( current + 1 >= end || deref( current + 1 ) != '\n' ) {
                x = line_start_x;
                y += ( font->descent - font->line_gap - font->ascent ) * scale;
                previous_codepoint_metrics = 0;
            }
            
        } else if ( codepoint == '\n' ) {
            
            codeunit_count = 1;
            x = line_start_x;
            y += ( font->descent - font->line_gap - font->ascent ) * scale;
            previous_codepoint_metrics = 0;
            
        } else {
            
            u32 error = 0;
            utf8_decode( current, end, &codepoint, &codeunit_count, &error );
            
            if ( is_error( &error ) ) {
                codepoint = ucs_replacement_codepoint;
                codeunit_count = 1;
            }
            
            outputed_codepoint_count++;
            
            f32 width_to_add = 0;
            codepoint_metrics_t* codepoint_metrics = font_get_codepoint_metrics( font, codepoint );
            
            if ( !previous_codepoint_metrics ) {
                width_to_add -= font_first_character_offset( font, codepoint_metrics );
            } else {
                width_to_add += font_get_kerning( font, previous_codepoint_metrics, codepoint ) * font->scale;
            }
            
            previous_codepoint_metrics = codepoint_metrics;
            
            umm v_array_index = 0;
            u32* c_array = cast( u32*, v_array );
            
            v_array[ v_array_index++ ] = x + codepoint_metrics->x0 + width_to_add;
            v_array[ v_array_index++ ] = y + codepoint_metrics->y0;
            
            if ( vertex_format.colors == 1 ) {
                c_array[ v_array_index++ ] = color_u32;
            } else {
                v_array[ v_array_index++ ] = color.r;
                v_array[ v_array_index++ ] = color.g;
                v_array[ v_array_index++ ] = color.b;
                v_array[ v_array_index++ ] = color.a;
            }
            
            v_array[ v_array_index++ ] = ( f32 ) codepoint_metrics->u0 / sheet_width;
            v_array[ v_array_index++ ] = ( f32 ) codepoint_metrics->v0 / sheet_height;
            
            /* -------------------- */
            
            v_array[ v_array_index++ ] = x + codepoint_metrics->x1 + width_to_add;
            v_array[ v_array_index++ ] = y + codepoint_metrics->y0;
            
            if ( vertex_format.colors == 1 ) {
                c_array[ v_array_index++ ] = color_u32;
            } else {
                v_array[ v_array_index++ ] = color.r;
                v_array[ v_array_index++ ] = color.g;
                v_array[ v_array_index++ ] = color.b;
                v_array[ v_array_index++ ] = color.a;
            }
            
            v_array[ v_array_index++ ] = ( f32 ) codepoint_metrics->u1 / sheet_width;
            v_array[ v_array_index++ ] = ( f32 ) codepoint_metrics->v0 / sheet_height;
            
            /* -------------------- */
            
            v_array[ v_array_index++ ] = x + codepoint_metrics->x0 + width_to_add;
            v_array[ v_array_index++ ] = y + codepoint_metrics->y1;
            
            if ( vertex_format.colors == 1 ) {
                c_array[ v_array_index++ ] = color_u32;
            } else {
                v_array[ v_array_index++ ] = color.r;
                v_array[ v_array_index++ ] = color.g;
                v_array[ v_array_index++ ] = color.b;
                v_array[ v_array_index++ ] = color.a;
            }
            
            v_array[ v_array_index++ ] = ( f32 ) codepoint_metrics->u0 / sheet_width;
            v_array[ v_array_index++ ] = ( f32 ) codepoint_metrics->v1 / sheet_height;
            
            /* -------------------- */
            
            v_array[ v_array_index++ ] = x + codepoint_metrics->x1 + width_to_add;
            v_array[ v_array_index++ ] = y + codepoint_metrics->y1;
            
            if ( vertex_format.colors == 1 ) {
                c_array[ v_array_index++ ] = color_u32;
            } else {
                v_array[ v_array_index++ ] = color.r;
                v_array[ v_array_index++ ] = color.g;
                v_array[ v_array_index++ ] = color.b;
                v_array[ v_array_index++ ] = color.a;
            }
            
            v_array[ v_array_index++ ] = ( f32 ) codepoint_metrics->u1 / sheet_width;
            v_array[ v_array_index++ ] = ( f32 ) codepoint_metrics->v1 / sheet_height;
            
            internal_gl_push_index( i_array, &index_offset, index + 0, index_size );
            internal_gl_push_index( i_array, &index_offset, index + 1, index_size );
            internal_gl_push_index( i_array, &index_offset, index + 2, index_size );
            internal_gl_push_index( i_array, &index_offset, index + 3, index_size );
            internal_gl_push_primitive_restart( i_array, &index_offset, index_size );
            
            index += 4;
            v_array = pointer_add_offset( f32, v_array, vertex_size_byte * 4 );
            
            width_to_add += codepoint_metrics->advance;
            x += width_to_add;
        }
        
        current += codeunit_count;
    }
    
    return outputed_codepoint_count;
}

stu gl_t gl_make( window_t* window, u32 flags, u32* error ) {
    
    gl_t result = { 0 };
    
    gl_initialize( window, flags, error );
    
    if ( no_error( error ) ) {
        
        result.window = window;
        data_t memory = memory_get_virtual( mebioctet( 40 ), 0 );
        // result.commands = gl_create_command_buffer( &memory, kibioctet( 512 ), kibioctet( 512 ) );
        result.commands = gl_create_command_buffer( &memory, mebioctet( 20 ), mebioctet( 20 ) );
        matrix_create_screen_space( result.matrix, window->width, window->height );
    }
    
    return result;
}

stu void gl_render_batches( gl_t* gl, u32* error ) {
    
    for ( umm i = 0; i < gl->batch_count; i++ ) {
        gl_push_batch( &gl->commands, gl->batches + i );
    }
    
    gl_render( gl, &gl->commands, error );
}

stu void gl_frame_start( gl_t* gl, u32* error ) {
    
    breakable {
        
        breakable_check( error );
        
        window_t* window = gl->window;
        
        gl_platform_frame_start( gl, error );
        
        matrix_create_screen_space( gl->matrix, window->width, window->height );
        
        gl_reset_command_buffer( &gl->commands );
        
        for ( umm i = 0; i < gl->batch_count; i++ ) {
            gl_batch_t* batch = gl->batches + i;
            gl_batch_frame_init( batch );
            
            /* NOTE simon (06/02/24 13:55:54): This might become an issue. */
            memory_free( &batch->uniform );
            memory_push_copy_p( &batch->uniform, gl->matrix, sizeof( float ) * 16, 8 );
        }
        
        gl_viewport( &gl->commands, 0, 0, window->width, window->height );
        gl_scissor( &gl->commands, false, 0, 0, window->width, window->height );
    }
}

stu gl_batch_t gl_batch_create_point( gl_t* gl, data_t* memory, umm point_count, umm command_count, gl_vertex_format_t vertex_format, gl_shader_t* shader, f32* matrix, u32* error ) {
    
    gl_batch_t batch = { 0 };
    
    breakable {
        
        breakable_check( error );
        
        u8 position_component_count = vertex_format.positions;
        _assert( !vertex_format.uvs );
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( vertex_format );
        u8 index_size_byte = vertex_format.index_size;
        
        batch.type = gl_command_type_point;
        batch.vertex_format = vertex_format;
        batch.v_buffer_size = cast( u32, vertex_size_byte * point_count * ( ( position_component_count == 2 ) ? 4 : 8 ) );
        batch.i_buffer_size = cast( u32, index_size_byte * point_count * ( ( position_component_count == 2 ) ? 5 : 15 ) );
        batch.shader = shader;
        batch.matrix = matrix;
        
        batch.vertex_buffer = gl_buffer_create( gl, gl_buffer_type_vertex, batch.v_buffer_size, error );
        batch.index_buffer = gl_buffer_create( gl, gl_buffer_type_index, batch.i_buffer_size, error );
        
        breakable_check( error );
        
        umm size = sizeof( gl_command_t );
        batch.uniform = memory_get_block( memory, kibioctet( 1 ), 16 );
        batch.commands = memory_get_block( memory, size * command_count, 8 );
        batch.datas = memory_get_block( memory, vertex_size_byte * point_count, 4 );
    }
    
    if ( is_error( error ) ) {
        
        if ( batch.vertex_buffer.pointer ) {
            gl_buffer_free( gl, &batch.vertex_buffer );
        }
        
        if ( batch.index_buffer.pointer ) {
            gl_buffer_free( gl, &batch.index_buffer );
        }
        
        batch = ( gl_batch_t ) { 0 };
    }
    
    return batch;
}

stu gl_batch_t gl_batch_create_line( gl_t* gl, data_t* memory, umm point_count, umm command_count, gl_vertex_format_t vertex_format, gl_shader_t* shader, f32* matrix, u32* error ) {
    
    gl_batch_t batch = { 0 };
    
    breakable {
        
        breakable_check( error );
        
        _assert( !vertex_format.uvs );
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( vertex_format );
        u8 index_size_byte = vertex_format.index_size;
        
        batch.type = gl_command_type_line;
        batch.vertex_format = vertex_format;
        batch.v_buffer_size = cast( u32, vertex_size_byte * point_count );
        batch.i_buffer_size = cast( u32, index_size_byte * ( point_count + command_count ) );
        batch.shader = shader;
        batch.matrix = matrix;
        
        batch.vertex_buffer = gl_buffer_create( gl, gl_buffer_type_vertex, batch.v_buffer_size, error );
        batch.index_buffer = gl_buffer_create( gl, gl_buffer_type_index, batch.i_buffer_size, error );
        
        breakable_check( error );
        
        umm size = sizeof( gl_command_t );
        batch.uniform = memory_get_block( memory, kibioctet( 1 ), 16 );
        batch.commands = memory_get_block( memory, size * command_count, 8 );
        batch.datas = memory_get_block( memory, vertex_size_byte * point_count, 4 );
    }
    
    if ( is_error( error ) ) {
        
        if ( batch.vertex_buffer.pointer ) {
            gl_buffer_free( gl, &batch.vertex_buffer );
        }
        
        if ( batch.index_buffer.pointer ) {
            gl_buffer_free( gl, &batch.index_buffer );
        }
        
        batch = ( gl_batch_t ) { 0 };
    }
    
    return batch;
}

stu gl_batch_t gl_batch_create_triangle_strip( gl_t* gl, data_t* memory, umm triangle_count, umm command_count, gl_vertex_format_t vertex_format, gl_shader_t* shader, f32* matrix, u32* error ) {
    
    gl_batch_t batch = { 0 };
    
    breakable {
        
        breakable_check( error );
        
        umm vertex_count = triangle_count + ( 2 * command_count );
        
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( vertex_format );
        u8 index_size_byte = vertex_format.index_size;
        
        batch.type = gl_command_type_triangle_strip;
        batch.vertex_format = vertex_format;
        batch.v_buffer_size = cast( u32, vertex_size_byte * vertex_count );
        batch.i_buffer_size = cast( u32, ( vertex_count + command_count ) * index_size_byte );
        batch.shader = shader;
        batch.matrix = matrix;
        
        batch.vertex_buffer = gl_buffer_create( gl, gl_buffer_type_vertex, batch.v_buffer_size, error );
        batch.index_buffer = gl_buffer_create( gl, gl_buffer_type_index, batch.i_buffer_size, error );
        
        breakable_check( error );
        
        umm size = sizeof( gl_command_t );
        batch.uniform = memory_get_block( memory, kibioctet( 1 ), 16 );
        batch.commands = memory_get_block( memory, size * command_count, 8 );
        batch.datas = memory_get_block( memory, vertex_size_byte * vertex_count, 4 );
    }
    
    if ( is_error( error ) ) {
        
        if ( batch.vertex_buffer.pointer ) {
            gl_buffer_free( gl, &batch.vertex_buffer );
        }
        
        if ( batch.index_buffer.pointer ) {
            gl_buffer_free( gl, &batch.index_buffer );
        }
        
        batch = ( gl_batch_t ) { 0 };
    }
    
    return batch;
}

stu gl_batch_t gl_batch_create_triangle_strip_textured( gl_t* gl, data_t* memory, umm triangle_count, umm command_count, gl_vertex_format_t vertex_format, gl_shader_t* shader, f32* matrix, u32* error ) {
    
    gl_batch_t batch = { 0 };
    
    breakable {
        
        breakable_check( error );
        
        umm vertex_count = triangle_count + ( 2 * command_count );
        
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( vertex_format );
        u8 index_size_byte = vertex_format.index_size;
        
        batch.type = gl_command_type_triangle_strip_textured;
        batch.vertex_format = vertex_format;
        batch.v_buffer_size = cast( u32, vertex_size_byte * vertex_count );
        batch.i_buffer_size = cast( u32, ( vertex_count + command_count ) * index_size_byte );
        batch.shader = shader;
        batch.matrix = matrix;
        
        batch.vertex_buffer = gl_buffer_create( gl, gl_buffer_type_vertex, batch.v_buffer_size, error );
        batch.index_buffer = gl_buffer_create( gl, gl_buffer_type_index, batch.i_buffer_size, error );
        
        breakable_check( error );
        
        umm size = sizeof( gl_command_t );
        batch.uniform = memory_get_block( memory, kibioctet( 1 ), 16 );
        batch.commands = memory_get_block( memory, size * command_count, 8 );
        batch.datas = memory_get_block( memory, vertex_size_byte * vertex_count, 4 );
    }
    
    if ( is_error( error ) ) {
        
        if ( batch.vertex_buffer.pointer ) {
            gl_buffer_free( gl, &batch.vertex_buffer );
        }
        
        if ( batch.index_buffer.pointer ) {
            gl_buffer_free( gl, &batch.index_buffer );
        }
        
        batch = ( gl_batch_t ) { 0 };
    }
    
    return batch;
}

stu gl_batch_t gl_batch_create_text( gl_t* gl, data_t* memory, umm one_byte_character_count, umm command_count, gl_vertex_format_t vertex_format, gl_shader_t* shader, f32* matrix, font_collection_t* collection, u32* error ) {
    
    gl_batch_t batch = { 0 };
    
    breakable {
        
        breakable_check( error );
        
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( vertex_format );
        u8 index_size_byte = vertex_format.index_size;
        
        batch.type = gl_command_type_text;
        batch.vertex_format = vertex_format;
        batch.v_buffer_size = cast( u32, vertex_size_byte * 4 * one_byte_character_count );
        batch.i_buffer_size = cast( u32, one_byte_character_count * 5 * index_size_byte );
        batch.shader = shader;
        batch.matrix = matrix;
        
        batch.vertex_buffer = gl_buffer_create( gl, gl_buffer_type_vertex, batch.v_buffer_size, error );
        batch.index_buffer = gl_buffer_create( gl, gl_buffer_type_index, batch.i_buffer_size, error );
        
        breakable_check( error );
        
        umm size = sizeof( gl_command_t );
        batch.commands = memory_get_block( memory, size * command_count, 8 );
        batch.datas = memory_get_block( memory, one_byte_character_count, 1 );
        batch.uniform = memory_get_block( memory, kibioctet( 1 ), 16 );
        batch.collection = collection;
    }
    
    if ( is_error( error ) ) {
        
        if ( batch.vertex_buffer.pointer ) {
            gl_buffer_free( gl, &batch.vertex_buffer );
        }
        
        if ( batch.index_buffer.pointer ) {
            gl_buffer_free( gl, &batch.index_buffer );
        }
        
        batch = ( gl_batch_t ) { 0 };
    }
    
    return batch;
}

stu gl_batch_t* gl_add_point_extended( gl_t* gl, umm point_count, umm command_count, gl_vertex_format_t* vertex_format, gl_shader_t* shader, f32* matrix, u32* error ) {
    
    gl_batch_t* result = 0;
    
    _assert( gl->batch_count < array_count( gl->batches ) );
    
    breakable {
        
        breakable_check( error );
        
        if ( gl->batch_count >= array_count( gl->batches ) ) {
            log_l( "[ERROR] Batch creation failed.\n" );
            set_error( error, 1 );
            break;
        }
        
        u8 batch_index = gl->batch_count;
        result = gl->batches + gl->batch_count;
        gl->batch_count++;
        
        gl_vertex_format_t vertex_format_local = gl_vertex_format( 2, 4, 0, 2 );
        
        if ( vertex_format ) {
            vertex_format_local = deref( vertex_format );
        }
        
        if ( !shader ) {
            
            if ( !gl->shaders[ batch_index ].valid ) {
                gl->shaders[ batch_index ] = gl_shader_create( gl, &vertex_format_local, memory_string_array( vs_screen_space_color ), memory_string_array( fs_default_vertex_color ), error );
            }
            
            shader = gl->shaders + batch_index;
        }
        
        breakable_check( error );
        
        if ( !matrix ) {
            matrix = gl->matrix;
        }
        
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( vertex_format_local );
        umm uniform_size = kibioctet( 1 );
        
        umm memory_size = ( sizeof( gl_command_t ) * command_count ) + ( vertex_size_byte * point_count ) + uniform_size;
        data_t memory = memory_get_virtual( memory_size, 0 );
        
        if ( !memory.bytes ) {
            log_l( "[ERROR] Batch creation out of memory.\n" );
            set_error( error, 2 );
            break;
        }
        
        deref( result ) = gl_batch_create_point( gl, &memory, point_count, command_count, vertex_format_local, shader, matrix, error );
        
        breakable_check( error );
    }
    
    if ( is_error( error ) ) {
        
        gl->batch_count--;
        
        if ( result ) {
            deref( result ) = ( gl_batch_t ) { 0 };
        }
        
        result = 0;
    }
    
    return result;
}

stu gl_batch_t* gl_add_point( gl_t* gl, u32* error ) {
    gl_batch_t* result = gl_add_point_extended( gl, 100, 100, 0, 0, 0, error );
    return result;
}

stu gl_batch_t* gl_add_line_extended( gl_t* gl, umm point_count, umm command_count, gl_vertex_format_t* vertex_format, gl_shader_t* shader, f32* matrix, u32* error ) {
    
    gl_batch_t* result = 0;
    
    _assert( gl->batch_count < array_count( gl->batches ) );
    
    breakable {
        
        breakable_check( error );
        
        if ( gl->batch_count >= array_count( gl->batches ) ) {
            log_l( "[ERROR] Batch creation failed.\n" );
            set_error( error, 1 );
            break;
        }
        
        u8 batch_index = gl->batch_count;
        result = gl->batches + gl->batch_count;
        gl->batch_count++;
        
        gl_vertex_format_t vertex_format_local = gl_vertex_format( 2, 4, 0, 2 );
        
        if ( vertex_format ) {
            vertex_format_local = deref( vertex_format );
        }
        
        if ( !shader ) {
            
            if ( !gl->shaders[ batch_index ].valid ) {
                gl->shaders[ batch_index ] = gl_shader_create( gl, &vertex_format_local, memory_string_array( vs_screen_space_color ), memory_string_array( fs_default_vertex_color ), error );
            }
            
            shader = gl->shaders + batch_index;
        }
        
        breakable_check( error );
        
        if ( !matrix ) {
            matrix = gl->matrix;
        }
        
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( vertex_format_local );
        umm uniform_size = kibioctet( 1 );
        
        umm memory_size = ( sizeof( gl_command_t ) * command_count ) + ( vertex_size_byte * point_count ) + uniform_size;
        data_t memory = memory_get_virtual( memory_size, 0 );
        
        if ( !memory.bytes ) {
            log_l( "[ERROR] Batch creation out of memory.\n" );
            set_error( error, 2 );
            break;
        }
        
        deref( result ) = gl_batch_create_line( gl, &memory, point_count, command_count, vertex_format_local, shader, matrix, error );
        
        breakable_check( error );
    }
    
    if ( is_error( error ) ) {
        
        gl->batch_count--;
        
        if ( result ) {
            deref( result ) = ( gl_batch_t ) { 0 };
        }
        
        result = 0;
    }
    
    return result;
}

stu gl_batch_t* gl_add_line( gl_t* gl, u32* error ) {
    gl_batch_t* result = gl_add_line_extended( gl, 100, 100, 0, 0, 0, error );
    return result;
}

stu gl_batch_t* gl_add_rectangle_extended( gl_t* gl, umm rectangle_count, umm command_count, gl_vertex_format_t* vertex_format, gl_shader_t* shader, f32* matrix, u32* error ) {
    
    gl_batch_t* result = 0;
    
    _assert( gl->batch_count < array_count( gl->batches ) );
    
    breakable {
        
        breakable_check( error );
        
        if ( gl->batch_count >= array_count( gl->batches ) ) {
            log_l( "[ERROR] Batch creation failed.\n" );
            set_error( error, 1 );
            break;
        }
        
        u8 batch_index = gl->batch_count;
        result = gl->batches + gl->batch_count;
        gl->batch_count++;
        
        gl_vertex_format_t vertex_format_local = gl_vertex_format( 2, 4, 0, 2 );
        
        if ( vertex_format ) {
            vertex_format_local = deref( vertex_format );
        }
        
        if ( !shader ) {
            
            if ( !gl->shaders[ batch_index ].valid ) {
                gl->shaders[ batch_index ] = gl_shader_create( gl, &vertex_format_local, memory_string_array( vs_screen_space_color ), memory_string_array( fs_default_vertex_color ), error );
            }
            
            shader = gl->shaders + batch_index;
        }
        
        breakable_check( error );
        
        if ( !matrix ) {
            matrix = gl->matrix;
        }
        
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( vertex_format_local );
        umm triangle_count = rectangle_count * 2;
        umm vertex_count = triangle_count + ( 2 * command_count );
        umm uniform_size = kibioctet( 1 );
        
        umm memory_size = ( sizeof( gl_command_t ) * command_count ) + ( vertex_size_byte * vertex_count ) + uniform_size;
        data_t memory = memory_get_virtual( memory_size, 0 );
        
        if ( !memory.bytes ) {
            log_l( "[ERROR] Batch creation out of memory.\n" );
            set_error( error, 2 );
            break;
        }
        
        deref( result ) = gl_batch_create_triangle_strip( gl, &memory, triangle_count, command_count, vertex_format_local, shader, matrix, error );
        
        breakable_check( error );
    }
    
    if ( is_error( error ) ) {
        
        gl->batch_count--;
        
        if ( result ) {
            deref( result ) = ( gl_batch_t ) { 0 };
        }
        
        result = 0;
    }
    
    return result;
}

stu gl_batch_t* gl_add_rectangle( gl_t* gl, u32* error ) {
    gl_batch_t* result = gl_add_rectangle_extended( gl, 1000, 1000, 0, 0, 0, error );
    return result;
}

typedef enum default_font_t {
    default_font_monospaced,
    default_font_proportional,
} default_font_t;

stu gl_batch_t* gl_add_text_extended( gl_t* gl, umm letter_count, umm command_count, gl_vertex_format_t* vertex_format, gl_shader_t* shader, f32* matrix, font_collection_t* collection, umm font_size, gl_sampler_t* sampler, u32* error ) {
    
    gl_batch_t* result = 0;
    
    _assert( gl->batch_count < array_count( gl->batches ) );
    
    breakable {
        
        breakable_check( error );
        
        if ( gl->batch_count >= array_count( gl->batches ) ) {
            log_l( "[ERROR] Batch creation failed.\n" );
            set_error( error, 1 );
            break;
        }
        
        u8 batch_index = gl->batch_count;
        result = gl->batches + gl->batch_count;
        gl->batch_count++;
        
        gl_vertex_format_t vertex_format_local = gl_vertex_format( 2, 4, 2, 4 );
        
        if ( vertex_format ) {
            vertex_format_local = deref( vertex_format );
        }
        
        if ( !shader ) {
            
            if ( !gl->shaders[ batch_index ].valid ) {
                gl->shaders[ batch_index ] = gl_shader_create( gl, &vertex_format_local, memory_string_array( vs_screen_space_texture_color ), memory_string_array( fs_default_u8_color ), error );
            }
            
            shader = gl->shaders + batch_index;
        }
        
        breakable_check( error );
        
        if ( !matrix ) {
            matrix = gl->matrix;
        }
        
        if ( !collection ) {
            
            if ( !font_size ) {
                font_size = 14;
            }
            
            /* NOTE simon: This could be free after font_build_collection, but we keep it around in case we want to generate again with different codepoints. */
            /* TODO simon (31/01/24 15:36:23): We might be leaking the collection if we create several text batch. Unless the collection for the second
             text batch uses the collection from the first batch. It might be a  better  API to force the user to create the default collection and pass it down. */
            collection = font_create_default_collection( font_size, font_codepoint_set, array_count( font_codepoint_set ) );
            font_build_collection( collection );
        }
        
        /* NOTE simon: *4 because a codepoint can be 4 codeunits in utf8. */
        umm maximum_text_byte_count = letter_count * 4;
        umm uniform_size = kibioctet( 1 );
        
        umm memory_size = ( sizeof( gl_command_t ) * command_count ) + maximum_text_byte_count + uniform_size; 
        data_t memory = memory_get_virtual( memory_size, 0 );
        
        if ( !memory.bytes ) {
            log_l( "[ERROR] Batch creation out of memory.\n" );
            set_error( error, 2 );
            break;
        }
        
        deref( result ) = gl_batch_create_text( gl, &memory, maximum_text_byte_count, command_count, vertex_format_local, shader, matrix, collection, error );
        
        breakable_check( error );
        
        if ( !sampler ) {
            sampler = gl_sampler_get( gl, gl_sampler_type_linear );
        }
        
        result->sampler = sampler;
    }
    
    if ( is_error( error ) ) {
        
        gl->batch_count--;
        
        if ( result ) {
            deref( result ) = ( gl_batch_t ) { 0 };
        }
        
        result = 0;
    }
    
    return result;
}

stu gl_batch_t* gl_add_text( gl_t* gl, u32* error ) {
    gl_batch_t* result = gl_add_text_extended( gl, 10000, 1000, 0, 0, 0, 0, 0, gl_sampler_get( gl, gl_sampler_type_linear ), error );
    return result;
}

stu gl_batch_t* gl_add_textured_quad_extended( gl_t* gl, umm quad_count, umm command_count, gl_vertex_format_t* vertex_format, gl_shader_t* shader, f32* matrix, gl_sampler_t* sampler, u32* error ) {
    
    gl_batch_t* result = 0;
    
    _assert( gl->batch_count < array_count( gl->batches ) );
    
    breakable {
        
        breakable_check( error );
        
        if ( gl->batch_count >= array_count( gl->batches ) ) {
            log_l( "[ERROR] Batch creation failed.\n" );
            set_error( error, 1 );
            break;
        }
        
        u8 batch_index = gl->batch_count;
        result = gl->batches + gl->batch_count;
        gl->batch_count++;
        
        gl_vertex_format_t vertex_format_local = gl_vertex_format( 2, 4, 2, 2 );
        
        if ( vertex_format ) {
            vertex_format_local = deref( vertex_format );
        }
        
        if ( !shader ) {
            
            if ( !gl->shaders[ batch_index ].valid ) {
                gl->shaders[ batch_index ] = gl_shader_create( gl, &vertex_format_local, memory_string_array( vs_screen_space_texture_color ), memory_string_array( fs_textured_color ), error );
            }
            
            shader = gl->shaders + batch_index;
        }
        
        breakable_check( error );
        
        if ( !matrix ) {
            matrix = gl->matrix;
        }
        
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( vertex_format_local );
        umm triangle_count = quad_count * 2;
        umm vertex_count = triangle_count + ( 2 * command_count );
        umm uniform_size = kibioctet( 1 );
        
        umm memory_size = ( sizeof( gl_command_t ) * command_count ) + ( vertex_size_byte * vertex_count ) + uniform_size;
        data_t memory = memory_get_virtual( memory_size, 0 );
        
        if ( !memory.bytes ) {
            log_l( "[ERROR] Batch creation out of memory.\n" );
            set_error( error, 2 );
            break;
        }
        
        deref( result ) = gl_batch_create_triangle_strip_textured( gl, &memory, quad_count, command_count, vertex_format_local, shader, matrix, error );
        
        breakable_check( error );
        
        if ( !sampler ) {
            sampler = gl_sampler_get( gl, gl_sampler_type_linear );
        }
        
        result->sampler = sampler;
    }
    
    if ( is_error( error ) ) {
        
        gl->batch_count--;
        
        if ( result ) {
            deref( result ) = ( gl_batch_t ) { 0 };
        }
        
        result = 0;
    }
    
    return result;
}

stu gl_batch_t* gl_add_textured_quad( gl_t* gl, u32* error ) {
    gl_batch_t* result = gl_add_textured_quad_extended( gl, 1000, 1000, 0, 0, 0, gl_sampler_get( gl, gl_sampler_type_linear ), error );
    return result;
}

stu gl_batch_t* gl_add_triangle_strip_extended( gl_t* gl, umm triangle_count, umm command_count, gl_vertex_format_t* vertex_format, gl_shader_t* shader, f32* matrix, gl_sampler_t* sampler, u32* error ) {
    
    gl_batch_t* result = 0;
    
    _assert( gl->batch_count < array_count( gl->batches ) );
    
    breakable {
        
        breakable_check( error );
        
        if ( gl->batch_count >= array_count( gl->batches ) ) {
            log_l( "[ERROR] Batch creation failed.\n" );
            set_error( error, 1 );
            break;
        }
        
        u8 batch_index = gl->batch_count;
        result = gl->batches + gl->batch_count;
        gl->batch_count++;
        
        gl_vertex_format_t vertex_format_local = gl_vertex_format( 2, 4, 0, 2 );
        
        if ( vertex_format ) {
            vertex_format_local = deref( vertex_format );
        }
        
        if ( !shader ) {
            
            if ( !gl->shaders[ batch_index ].valid ) {
                gl->shaders[ batch_index ] = gl_shader_create( gl, &vertex_format_local, memory_string_array( vs_screen_space_color ), memory_string_array( fs_default_vertex_color ), error );
            }
            
            shader = gl->shaders + batch_index;
        }
        
        breakable_check( error );
        
        if ( !matrix ) {
            matrix = gl->matrix;
        }
        
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( vertex_format_local );
        umm vertex_count = triangle_count + ( 2 * command_count );
        umm uniform_size = kibioctet( 1 );
        
        umm memory_size = ( sizeof( gl_command_t ) * command_count ) + ( vertex_count * vertex_size_byte ) + uniform_size;
        data_t memory = memory_get_virtual( memory_size, 0 );
        
        if ( !memory.bytes ) {
            log_l( "[ERROR] Batch creation out of memory.\n" );
            set_error( error, 2 );
            break;
        }
        
        deref( result ) = gl_batch_create_triangle_strip( gl, &memory, triangle_count, command_count, vertex_format_local, shader, matrix, error );
        
        breakable_check( error );
        
        if ( !sampler ) {
            sampler = gl_sampler_get( gl, gl_sampler_type_linear );
        }
        
        result->sampler = sampler;
    }
    
    return result;
}

stu gl_batch_t* gl_add_triangle_strip( gl_t* gl, u32* error ) {
    gl_batch_t* result = gl_add_triangle_strip_extended( gl, 100, 100, 0, 0, 0, gl_sampler_get( gl, gl_sampler_type_point ), error );
    return result;
}

stu gl_batch_t* gl_add_atlas_extended( gl_t* gl, umm triangle_count, umm command_count, gl_vertex_format_t* vertex_format, gl_shader_t* shader, f32* matrix, gl_texture_t* texture, gl_sampler_t* sampler, u32* error ) {
    
    gl_batch_t* result = 0;
    
    _assert( gl->batch_count < array_count( gl->batches ) );
    
    breakable {
        
        breakable_check( error );
        
        if ( gl->batch_count >= array_count( gl->batches ) ) {
            log_l( "[ERROR] Batch creation failed.\n" );
            set_error( error, 1 );
            break;
        }
        
        u8 batch_index = gl->batch_count;
        result = gl->batches + gl->batch_count;
        gl->batch_count++;
        
        gl_vertex_format_t vertex_format_local = gl_vertex_format( 2, 4, 2, 2 );
        
        if ( vertex_format ) {
            vertex_format_local = deref( vertex_format );
        }
        
        if ( !shader ) {
            
            if ( !gl->shaders[ batch_index ].valid ) {
                gl->shaders[ batch_index ] = gl_shader_create( gl, &vertex_format_local, memory_string_array( vs_screen_space_texture_color ), memory_string_array( fs_textured_color ), error );
            }
            
            shader = gl->shaders + batch_index;
        }
        
        breakable_check( error );
        
        if ( !matrix ) {
            matrix = gl->matrix;
        }
        
        u8 vertex_size_byte = gl_vertex_format_compute_vertex_size_byte( vertex_format_local );
        umm vertex_count = triangle_count + ( 2 * command_count );
        umm uniform_size = kibioctet( 1 );
        
        umm memory_size = ( sizeof( gl_command_t ) * command_count ) + ( vertex_count * vertex_size_byte ) + uniform_size;
        data_t memory = memory_get_virtual( memory_size, 0 );
        
        if ( !memory.bytes ) {
            log_l( "[ERROR] Batch creation out of memory.\n" );
            set_error( error, 2 );
            break;
        }
        
        deref( result ) = gl_batch_create_triangle_strip( gl, &memory, triangle_count, command_count, vertex_format_local, shader, matrix, error );
        
        breakable_check( error );
        
        result->texture = texture;
        
        if ( !sampler ) {
            sampler = gl_sampler_get( gl, gl_sampler_type_linear );
        }
        
        result->sampler = sampler;
    }
    
    if ( is_error( error ) ) {
        
        gl->batch_count--;
        
        if ( result ) {
            deref( result ) = ( gl_batch_t ) { 0 };
        }
        
        result = 0;
    }
    
    return result;
}

stu gl_batch_t* gl_add_atlas( gl_t* gl, gl_texture_t* texture, u32* error ) {
    gl_batch_t* result = gl_add_atlas_extended( gl, 100, 100, 0, 0, 0, texture, gl_sampler_get( gl, gl_sampler_type_point ), error );
    return result;
}

#define GL_H
#endif

