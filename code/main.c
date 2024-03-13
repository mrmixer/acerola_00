
#include "icons.h"

#include "../lib/common.h"
#include "../lib/gl.h"
#include "../lib/window.h"
#include "../lib/audio.h"
#include "../lib/wave.h"
#include "../lib/gamepad.h"

audio_format_t g_format = { 0 };
audio_mixer_t g_mixer = { 0 };
gamepad_state_t g_gamepads = { 0 };

#include "game.c"

int main( int argc, char** argv ) {
    
    arguments_parse_command_line( &argc, &argv );
    arguments_initialize( argc, argv );
    
    log_initialize( );
    
    breakable {
        
        u32 window_error = 0;
        
        window_t window = { 0 };
        window_create( &window, string_l( "Redundancy is not enough" ), 1280, 720, s32_max, s32_max, 0, &window_error );
        
        breakable_check( &window_error );
        
        u32 gl_error = 0;
        gl_t gl = gl_make( &window, gl_flag_alpha_blending, &gl_error );
        gl_batch_t* rectangles = gl_add_textured_quad( &gl, &gl_error );
        
        breakable_check( &gl_error );
        
        audio_format_initialize( &g_format, 44100, 16, 2, audio_channel_front_left | audio_channel_front_right );
        audio_mixer_initialize( &g_mixer, g_format, 100 );
        audio_start( &window.platform, &g_mixer, &g_audio_error );
        
        gamepad_start( &g_gamepads, gamepad_backend_all );
        
        while ( window.running ) {
            
            audio_update( &window.platform, &g_audio_error );
            
            window_handle_messages( &window, 0 );
            
            gamepad_update( &g_gamepads );
            
            if ( window.close_requested ) {
                window.running = false;
            }
            
            if ( window.resized ) {
                gl_resize_frame_buffer( &window, &gl_error );
            }
            
            gl_frame_start( &gl, &gl_error );
            gl_clear( &gl.commands, gray_darkest );
            
            game_run( &window, &gl, &gl_error );
            
            gl_render_batches( &gl, &gl_error );
            
            breakable_check( &gl_error );
            
            window_set_cursor( &window, window.platform.cursor_arrow );
            gl_swap_buffers( &window );
        }
        
        gl_texture_free( &game.no );
        gl_texture_free( &game.ship );
        
        for ( umm i = 0; i < array_count( game.directions ); i++ ) {
            gl_texture_free( &game.directions[ i ] );
        }
        
        gl_cleanup( &gl, &gl_error );
        
        gamepad_end( &g_gamepads );
    }
    
    log_to_file( string_l( "acerola_00_log.txt" ) );
    
    return 0;
}