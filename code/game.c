
#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

audio_wave_t audio_woosh = { 0 };
audio_wave_t audio_ship = { 0 };
audio_wave_t audio_hit = { 0 };
audio_wave_t audio_music = { 0 };
audio_wave_t audio_flare = { 0 };
audio_wave_t audio_bad = { 0 };
audio_wave_t audio_good = { 0 };

typedef enum state_t {
    
    state_none,
    
    state_title,
    state_manual,
    state_flare,
    state_phase_start,
    state_turn_start,
    state_turn_display,
    state_turn_feedback,
    state_turn_ship_move,
    state_turn_danger_1,
    state_turn_danger_2,
    state_turn_danger_3,
    state_turn_danger_4,
    state_turn_ship_move_back,
    state_count,
    state_end,
    
} state_t;

/* NOTE simon (03/03/24 23:41:06): State at index 0 must stay with a duration of zero so that we get a state changed on the first update called. */

typedef struct state_machine_t {
    u8 value;
    u8 state_changed_manually;
    f32 timer;
    f32 durations[ 255 ];
} state_machine_t;

void state_machine_set_duration( state_machine_t* machine, umm state, f32 duration ) {
    
    if ( state ) {
        machine->durations[ state ] = duration;
    }
}

u8 state_machine_set_state( state_machine_t* machine, umm state ) {
    u8 result = machine->value;
    machine->value = cast( u8, state );
    machine->timer = 0;
    machine->state_changed_manually = true;
    return result;
}

b32 state_machine_update( state_machine_t* machine, f32 delta_time ) {
    
    b32 state_changed = false;
    machine->timer += delta_time;
    
    if ( machine->value == 0 ) {
        
        state_changed = true;
        machine->value++;
        machine->timer = 0;
        
    } else if ( machine->state_changed_manually ) {
        
        state_changed = true;
        machine->timer = 0;
        
    } else if ( machine->timer > machine->durations[ machine->value ] ) {
        
        state_changed = true;
        machine->timer -= machine->durations[ machine->value ];
        machine->value++;
    }
    
    machine->state_changed_manually = false;
    
    return state_changed;
}

f32 state_machine_get_factor( state_machine_t* machine ) {
    
    f32 result = 0;
    
    if ( machine->durations[ machine->value ] ) {
        result = machine->timer / machine->durations[ machine->value ];
        result = math_clamp( 0.0f, result, 1.0f );
    }
    
    return result;
}

typedef struct world_dimensions_t {
    f32 width;
    f32 height;
    f32 screen_width;
    f32 screen_height;
} world_dimensions_t;

world_dimensions_t world_get_dimensions_x( f32 reference_width, f32 reference_height, f32 world_dimension_x, f32 actual_width, f32 actual_height ) {
    
    world_dimensions_t result = { 0 };
    
    f32 reference_factor = reference_height / reference_width;
    f32 world_dimension_y = world_dimension_x * reference_factor;
    f32 factor = actual_height / actual_width;
    
    result.width = world_dimension_x;
    result.height = world_dimension_y;
    
    b32 y_fits = factor > reference_factor;
    
    if ( y_fits ) {
        result.screen_width = world_dimension_x;
        result.screen_height = world_dimension_x * factor;
    } else {
        result.screen_width = world_dimension_y * ( actual_width / actual_height );
        result.screen_height = world_dimension_y;
    }
    
    return result;
}

typedef enum direction_t {
    direction_left,
    direction_up,
    direction_down,
    direction_right,
} direction_t;

typedef struct game_t {
    
    b32 initialized;
    
    gl_batch_t* rectangles;
    u8 computers[ 5 ];
    u8 computers_error_on[ 5 ][ 4 ];
    u8 computers_error_is[ 5 ][ 4 ];
    u8 rock_free;
    u8 phase;
    u8 turn;
    u8 danger_order[ 3 ];
    
    vec2 ship_p;
    
    u64 time;
    u64 frequency;
    f32 delta_time;
    f32 timer;
    
    state_machine_t state;
    
    f32 world_width;
    
    random_64_t rng;
    random_64_t frame_rng;
    
    gl_texture_t no;
    gl_texture_t ship;
    gl_texture_t directions[ 4 ];
    gl_texture_t progress_frame;
    gl_texture_t time_bar;
    gl_texture_t warning;
    gl_texture_t stars[ 7 ];
    gl_texture_t rocks[ 4 ];
    gl_texture_t manual;
    gl_texture_t flares[ 10 ];
    gl_texture_t title;
    gl_texture_t end;
    
    b32 hit_played;
    audio_id_t music_id;
    b32 music_muted;
    data_t audio_memory;
    
} game_t;

game_t game = { 0 };

typedef union matrix_t {
    
    union {
        f32 _11, _12, _13, _14;
        f32 _21, _22, _23, _24;
        f32 _31, _32, _33, _34;
        f32 _41, _42, _43, _44;
    };
    union {
        vec4 _1;
        vec4 _2;
        vec4 _3;
        vec4 _4;
    };
    vec4 v[ 4 ];
    f32 e[ 16 ];
    
} matrix_t;

void matrix_identity_4( f32* matrix ) {
    umm row = 4;
    matrix[ 0 * row + 0 ] = 1; matrix[ 0 * row + 1 ] = 0; matrix[ 0 * row + 2 ] = 0; matrix[ 0 * row + 3 ] = 0;
    matrix[ 1 * row + 0 ] = 0; matrix[ 1 * row + 1 ] = 1; matrix[ 1 * row + 2 ] = 0; matrix[ 1 * row + 3 ] = 0;
    matrix[ 2 * row + 0 ] = 0; matrix[ 2 * row + 1 ] = 0; matrix[ 2 * row + 2 ] = 1; matrix[ 2 * row + 3 ] = 0;
    matrix[ 3 * row + 0 ] = 0; matrix[ 3 * row + 1 ] = 0; matrix[ 3 * row + 2 ] = 0; matrix[ 3 * row + 3 ] = 1;
}

void matrix_orthographic_x_4( f32 aspect_width, f32 aspect_height, f32 reference_x, f32 near_plane, f32 far_plane, f32* matrix ) {
    
    umm row = 4;
    f32 two_over_x = 2 / reference_x;
    f32 ratio = aspect_width / aspect_height;
    f32 reference_z = far_plane - near_plane;
    f32 one_over_z = 1 / reference_z;
    
    matrix_identity_4( matrix );
    
    matrix[ 0 * row + 0 ] = two_over_x;
    matrix[ 1 * row + 1 ] = ratio * two_over_x;
    matrix[ 2 * row + 2 ] = one_over_z;
    matrix[ 3 * row + 2 ] = -near_plane * one_over_z;
}

void computer_clear_all( ) {
    for ( umm computer_index = 0; computer_index < 5; computer_index++ ) {
        for ( umm error_index = 0; error_index < 4; error_index++ ) {
            game.computers_error_on[ computer_index ][ error_index ] = 5;
        }
    }
}

void computer_clear_errors( u8 computer_index ) {
    for ( umm error_index = 0; error_index < 4; error_index++ ) {
        game.computers_error_on[ computer_index ][ error_index ] = 5;
    }
}

void computer_set_error( umm computer_index, umm error_index, u8 on, u8 is ) {
    game.computers_error_on[ computer_index ][ error_index ] = on;
    game.computers_error_is[ computer_index ][ error_index ] = is;
}

gl_texture_t load_texture( gl_t* gl, data_t file_name, u32* gl_error ) {
    
    clear_error( &g_file_error );
    data_t file = file_read_entire( file_name, 0, &g_file_error );
    s32 x, y;
    u8* data = stbi_load_from_memory( file.bytes, cast( s32, file.used ), &x, &y, 0, 4 );
    gl_texture_t result = gl_texture_create( gl, data, x, y, 4, 0, gl_error );
    return result;
}

void array_shuffle_u8( u8* array, umm count ) {
    
    for ( umm to = 0; to < count; to++ ) {
        umm from = random_64_get_range( &game.rng, 0, count );
        swap_type( array[ to ], array[ from ], u8 );
    }
}

void game_run( window_t* window, gl_t* gl, u32* gl_error ) {
    
    if ( !game.initialized ) {
        
        game = ( game_t ) { 0 };
        
        game.rectangles = gl->batches + 0;
        game.initialized = true;
        
        perf_get_frequency( &game.frequency );
        
        game.world_width = 10;
        
#if defined( DEBUG )
        game.rng = random_64_make( ( u128_t ) { 0 }, ( u128_t ) { 0 } );
#else
        game.rng = random_64_make( random_seed_get_u128( ), random_seed_get_u128( ) );
#endif
        
        stbi_set_flip_vertically_on_load( true );
        
        game.no = load_texture( gl, string_l( "assets\\white.tga" ), gl_error );
        game.ship = load_texture( gl, string_l( "assets\\ship.tga" ), gl_error );
        game.directions[ 0 ] = load_texture( gl, string_l( "assets\\left.tga" ), gl_error );
        game.directions[ 1 ] = load_texture( gl, string_l( "assets\\up.tga" ), gl_error );
        game.directions[ 2 ] = load_texture( gl, string_l( "assets\\down.tga" ), gl_error );
        game.directions[ 3 ] = load_texture( gl, string_l( "assets\\right.tga" ), gl_error );
        game.progress_frame = load_texture( gl, string_l( "assets\\progress_frame.tga" ), gl_error );
        game.stars[ 0 ] = load_texture( gl, string_l( "assets\\stars_01.tga" ), gl_error );
        game.stars[ 1 ] = load_texture( gl, string_l( "assets\\stars_02.tga" ), gl_error );
        game.stars[ 2 ] = load_texture( gl, string_l( "assets\\stars_03.tga" ), gl_error );
        game.stars[ 3 ] = load_texture( gl, string_l( "assets\\stars_04.tga" ), gl_error );
        game.stars[ 4 ] = load_texture( gl, string_l( "assets\\stars_05.tga" ), gl_error );
        game.stars[ 5 ] = game.stars[ 4 ];
        game.stars[ 6 ] = game.stars[ 4 ];
        game.time_bar = load_texture( gl, string_l( "assets\\timer.tga" ), gl_error );
        game.warning = load_texture( gl, string_l( "assets\\warning_1.tga" ), gl_error );
        game.rocks[ 0 ] = load_texture( gl, string_l( "assets\\debris_01.tga" ), gl_error );
        game.rocks[ 1 ] = load_texture( gl, string_l( "assets\\debris_02.tga" ), gl_error );
        game.rocks[ 2 ] = load_texture( gl, string_l( "assets\\debris_03.tga" ), gl_error );
        game.rocks[ 3 ] = load_texture( gl, string_l( "assets\\debris_02.tga" ), gl_error );
        game.manual = load_texture( gl, string_l( "assets\\manual.tga" ), gl_error );
        
        game.flares[ 0 ] = load_texture( gl, string_l( "assets\\flare_00.tga" ), gl_error );
        game.flares[ 1 ] = load_texture( gl, string_l( "assets\\flare_01.tga" ), gl_error );
        game.flares[ 2 ] = load_texture( gl, string_l( "assets\\flare_02.tga" ), gl_error );
        game.flares[ 3 ] = load_texture( gl, string_l( "assets\\flare_03.tga" ), gl_error );
        game.flares[ 4 ] = load_texture( gl, string_l( "assets\\flare_04.tga" ), gl_error );
        game.flares[ 5 ] = load_texture( gl, string_l( "assets\\flare_05.tga" ), gl_error );
        game.flares[ 6 ] = load_texture( gl, string_l( "assets\\flare_06.tga" ), gl_error );
        game.flares[ 7 ] = load_texture( gl, string_l( "assets\\flare_07.tga" ), gl_error );
        game.flares[ 8 ] = load_texture( gl, string_l( "assets\\flare_08.tga" ), gl_error );
        game.flares[ 9 ] = load_texture( gl, string_l( "assets\\flare_09.tga" ), gl_error );
        
        game.title = load_texture( gl, string_l( "assets\\title.tga" ), gl_error );
        game.end = load_texture( gl, string_l( "assets\\end.tga" ), gl_error );
        
        state_machine_set_duration( &game.state, state_title, f32_max );
        state_machine_set_duration( &game.state, state_manual, f32_max );
        state_machine_set_duration( &game.state, state_flare, 1 );
        state_machine_set_duration( &game.state, state_phase_start, 0 );
        state_machine_set_duration( &game.state, state_turn_start, 0 );
        state_machine_set_duration( &game.state, state_turn_display, 20 );
        state_machine_set_duration( &game.state, state_turn_feedback, 2.0f );
        state_machine_set_duration( &game.state, state_turn_ship_move, 0.5f );
        state_machine_set_duration( &game.state, state_turn_danger_1, 0.75f );
        state_machine_set_duration( &game.state, state_turn_danger_2, 0.5f );
        state_machine_set_duration( &game.state, state_turn_danger_3, 0.5f );
        state_machine_set_duration( &game.state, state_turn_danger_4, 0.5f );
        state_machine_set_duration( &game.state, state_turn_ship_move_back, 0.5f );
        state_machine_set_duration( &game.state, state_end, f32_max );
        
        computer_clear_all( );
        
        game.audio_memory = memory_get_virtual( mebioctet( 100 ), 0 );
        
        audio_woosh = audio_wave_load( string_l( "assets\\woosh.wav" ), &game.audio_memory );
        audio_ship = audio_wave_load( string_l( "assets\\ship_00.wav" ), &game.audio_memory );
        audio_hit = audio_wave_load( string_l( "assets\\hit_01.wav" ), &game.audio_memory );
        audio_music = audio_wave_load( string_l( "assets\\best.wav" ), &game.audio_memory );
        audio_flare = audio_wave_load( string_l( "assets\\flare.wav" ), &game.audio_memory );
        audio_bad = audio_wave_load( string_l( "assets\\bad.wav" ), &game.audio_memory );
        audio_good = audio_wave_load( string_l( "assets\\good.wav" ), &game.audio_memory );
        
        audio_wave_play( &g_mixer, &audio_music, 2.0f, 0.5f, true, &game.music_id );
    }
    
    game.frame_rng = random_64_make( ( u128_t ) { 0 }, ( u128_t ) { 0 } );
    
    u64 new_time = 0;
    perf_get_time( &new_time );
    
    game.delta_time = 1/60.0f;
    
    if ( game.time ) {
        u64 difference = new_time - game.time;
        game.delta_time = cast( f32, cast( f64, difference ) / cast( f64, game.frequency ) );
    }
    
    game.time = new_time;
    game.timer += game.delta_time;
    
    if ( game.timer > 60*60 ) {
        game.timer -= 60*60;
    }
    
    world_dimensions_t dimensions = world_get_dimensions_x( 16.0f, 9.0f, game.world_width, cast( f32, window->width ), cast( f32, window->height ) );
    
    gamepad_t* pad = 0;
    
    for ( umm i = 0; i < array_count( g_gamepads.devices ); i++ ) {
        if ( g_gamepads.devices[ i ] ) {
            pad = g_gamepads.gamepads + i;
            break;
        }
    }
    
    if ( input_key_just_pressed( window, vk_f ) || gamepad_just_pressed( pad, gamepad_button_y ) ) {
        window_toggle_fullscreen( window );
    }
    
    if ( input_key_just_pressed( window, vk_s ) || gamepad_just_pressed( pad, gamepad_button_x ) ) {
        
        if ( g_mixer.volume ) {
            audio_set_global_volume( &g_mixer, 0.0f );
        } else {
            audio_set_global_volume( &g_mixer, 1.0f );
        }
    }
    
    if ( input_key_just_pressed( window, vk_m ) || gamepad_just_pressed( pad, gamepad_button_b ) ) {
        
        if ( !game.music_muted ) {
            audio_set_volume( &g_mixer, game.music_id, 0.0f );
        } else {
            audio_set_volume( &g_mixer, game.music_id, 2.0f );
        }
        
        game.music_muted = !game.music_muted;
    }
    
    if ( input_key_just_pressed( window, vk_escape ) || gamepad_just_pressed( pad, gamepad_button_view ) ) {
        window->running = false;
    }
    
    b32 up = input_key_just_pressed( window, vk_up ) || gamepad_just_pressed( pad, gamepad_button_dpad_up );
    b32 down = input_key_just_pressed( window, vk_down ) || gamepad_just_pressed( pad, gamepad_button_dpad_down );
    b32 left = input_key_just_pressed( window, vk_left ) || gamepad_just_pressed( pad, gamepad_button_dpad_left );
    b32 right = input_key_just_pressed( window, vk_right ) || gamepad_just_pressed( pad, gamepad_button_dpad_right );
    
    gl_sampler_t* sampler = gl_sampler_get( gl, gl_sampler_type_point );
    f32 pixel_size = 4;
    f32 pixel_to_unit = 1 / 32.0f;
    f32 unit_to_pixel = 32;
    
    b32 state_changed = state_machine_update( &game.state, game.delta_time );
    
    if ( game.state.value == state_phase_start ) {
        
        if ( state_changed ) {
            
            _assert( game.turn < 5 );
            
            if ( game.phase >= 5 ) {
                game.phase = 0;
            }
            
            umm count = 0;
            u8 indices[ 5 ] = { 0 };
            
            while ( count != 5 ) {
                
                u8 rn = cast( u8, random_64_get_range( &game.rng, 0, 5 ) );
                b32 found = false;
                
                for ( umm i = 0; i < count; i++ ) {
                    if ( indices[ i ] == rn ) {
                        found = true;
                        break;
                    }
                }
                
                if ( !found ) {
                    indices[ count ] = rn;
                    count++;
                }
            }
            
            if ( game.phase == 0 ) {
                
                computer_clear_all( );
                
                state_machine_set_duration( &game.state, state_turn_danger_1, 0.75f );
                state_machine_set_duration( &game.state, state_turn_danger_2, 0.5f );
                state_machine_set_duration( &game.state, state_turn_danger_3, 0.5f );
                state_machine_set_duration( &game.state, state_turn_danger_4, 0.5f );
                
            } else if ( game.phase == 1 ) {
                
                computer_set_error( 0, 0, direction_up, direction_left );
                computer_set_error( 0, 1, direction_right, direction_left );
                computer_set_error( 0, 2, direction_down, direction_left );
                computer_set_error( 0, 3, direction_left, direction_left );
                
                for ( umm computer_index = 1; computer_index < 5; computer_index++ ) {
                    computer_set_error( computer_index, 0, direction_up, direction_down );
                    computer_set_error( computer_index, 1, direction_right, direction_left );
                    computer_set_error( computer_index, 2, direction_down, direction_up );
                    computer_set_error( computer_index, 3, direction_left, direction_right );
                }
                
                state_machine_set_duration( &game.state, state_turn_danger_1, 0.5f );
                state_machine_set_duration( &game.state, state_turn_danger_2, 0.25f );
                state_machine_set_duration( &game.state, state_turn_danger_3, 0.25f );
                state_machine_set_duration( &game.state, state_turn_danger_4, 0.25f );
                
            } else if ( game.phase == 2 ) {
                
                computer_set_error( indices[ 0 ], 0, direction_up, direction_right );
                computer_set_error( indices[ 0 ], 1, direction_right, direction_right );
                computer_set_error( indices[ 0 ], 2, direction_down, direction_right );
                computer_set_error( indices[ 0 ], 3, direction_left, direction_right );
                
                computer_set_error( indices[ 1 ], 0, direction_up, direction_up );
                computer_set_error( indices[ 1 ], 1, direction_right, direction_up );
                computer_set_error( indices[ 1 ], 2, direction_down, direction_up );
                computer_set_error( indices[ 1 ], 3, direction_left, direction_up );
                
                computer_set_error( indices[ 2 ], 0, direction_up, direction_down );
                computer_set_error( indices[ 2 ], 1, direction_down, direction_up );
                computer_set_error( indices[ 2 ], 2, direction_left, direction_left );
                computer_set_error( indices[ 2 ], 3, direction_right, direction_right );
                
                computer_set_error( indices[ 3 ], 0, direction_up, direction_down );
                computer_set_error( indices[ 3 ], 1, direction_down, direction_up );
                computer_set_error( indices[ 3 ], 2, direction_left, direction_left );
                computer_set_error( indices[ 3 ], 3, direction_right, direction_right );
                
                computer_set_error( indices[ 4 ], 0, direction_up, direction_down );
                computer_set_error( indices[ 4 ], 1, direction_down, direction_up );
                computer_set_error( indices[ 4 ], 2, direction_left, direction_left );
                computer_set_error( indices[ 4 ], 3, direction_right, direction_right );
                
                state_machine_set_duration( &game.state, state_turn_danger_1, 0.5f );
                state_machine_set_duration( &game.state, state_turn_danger_2, 0.25f );
                state_machine_set_duration( &game.state, state_turn_danger_3, 0.25f );
                state_machine_set_duration( &game.state, state_turn_danger_4, 0.25f );
                
            } else if ( game.phase == 3 ) {
                
                computer_clear_all( );
                
                state_machine_set_duration( &game.state, state_turn_danger_1, 0.5f );
                state_machine_set_duration( &game.state, state_turn_danger_2, 0.25f );
                state_machine_set_duration( &game.state, state_turn_danger_3, 0.25f );
                state_machine_set_duration( &game.state, state_turn_danger_4, 0.25f );
                
            } else if ( game.phase == 4 ) {
                
                computer_set_error( indices[ 0 ], 0, direction_up, direction_right );
                computer_set_error( indices[ 0 ], 1, direction_right, direction_right );
                computer_set_error( indices[ 0 ], 2, direction_down, direction_right );
                computer_set_error( indices[ 0 ], 3, direction_left, direction_right );
                
                computer_set_error( indices[ 1 ], 0, direction_up, direction_right );
                computer_set_error( indices[ 1 ], 1, direction_right, direction_down );
                computer_set_error( indices[ 1 ], 2, direction_down, direction_left );
                computer_set_error( indices[ 1 ], 3, direction_left, direction_up );
                
                computer_set_error( indices[ 2 ], 0, direction_up, direction_right );
                computer_set_error( indices[ 2 ], 1, direction_right, direction_down );
                computer_set_error( indices[ 2 ], 2, direction_down, direction_left );
                computer_set_error( indices[ 2 ], 3, direction_left, direction_up );
                
                computer_set_error( indices[ 3 ], 0, direction_up, direction_left );
                computer_set_error( indices[ 3 ], 1, direction_right, direction_left );
                computer_set_error( indices[ 3 ], 2, direction_down, direction_left );
                computer_set_error( indices[ 3 ], 3, direction_left, direction_left );
                
                computer_set_error( indices[ 4 ], 0, direction_up, direction_down );
                computer_set_error( indices[ 4 ], 1, direction_right, direction_down );
                computer_set_error( indices[ 4 ], 2, direction_down, direction_down );
                computer_set_error( indices[ 4 ], 3, direction_left, direction_down );
                
                state_machine_set_duration( &game.state, state_turn_danger_1, 0.5f );
                state_machine_set_duration( &game.state, state_turn_danger_2, 0.25f );
                state_machine_set_duration( &game.state, state_turn_danger_3, 0.25f );
                state_machine_set_duration( &game.state, state_turn_danger_4, 0.25f );
            }
        }
        
        state_machine_set_duration( &game.state, state_turn_display, 20 );
        state_machine_set_state( &game.state, state_turn_start );
    }
    
    if ( game.state.value == state_turn_start ) {
        
        if ( state_changed ) {
            
            game.rock_free = cast( u8, random_64_get_range( &game.rng, 0, 4 ) );
            game.hit_played = false;
            
            if ( game.phase == 3 ) {
                
                u8 numbers[ 5 ] = { 0, 1, 2, 3, 5 };
                
                while ( numbers[ game.rock_free ] == game.rock_free ) {
                    numbers[ game.rock_free ] = cast( u8, random_64_get_range( &game.rng, 0, 4 ) );
                }
                
                numbers[ 4 ] = game.rock_free;
                
                while ( numbers[ 4 ] == game.rock_free ) {
                    numbers[ 4 ] = cast( u8, random_64_get_range( &game.rng, 0, 4 ) );
                }
                
                array_shuffle_u8( numbers, array_count( numbers ) );
                
                for ( umm i = 0; i < array_count( numbers ); i++ ) {
                    game.computers[ i ] = numbers[ i ];
                }
                
            } else {
                
                for ( umm i = 0; i < array_count( game.computers ); i++ ) {
                    
                    game.computers[ i ] = game.rock_free;
                    
                    for ( umm error = 0; error < 4; error++ ) {
                        if ( game.computers_error_on[ i ][ error ] == game.computers[ i ] ) {
                            game.computers[ i ] = game.computers_error_is[ i ][ error ];
                            break;
                        }
                    }
                }
            }
            
            game.ship_p.x = 0;
            game.ship_p.y = 0;
            
            if ( game.turn == 0 ) {
                state_machine_set_duration( &game.state, state_turn_display, 15 );
            } else if ( game.turn == 1 ) {
                state_machine_set_duration( &game.state, state_turn_display, 10 );
            } else if ( game.turn == 2 ) {
                state_machine_set_duration( &game.state, state_turn_display, 5 );
            } else if ( game.turn == 3 ) {
                state_machine_set_duration( &game.state, state_turn_display, 2.5f );
            } else if(  game.turn == 4 ) {
                state_machine_set_duration( &game.state, state_turn_display, 2.0f );
            }
        }
    }
    
    f32 background_right = math_ceilf( dimensions.screen_width * 0.5f );
    f32 background_left = -background_right;
    f32 background_top = math_ceilf( dimensions.screen_height * 0.5f );
    f32 background_bottom = -background_top;
    
    vec4 colors[ 7 ] = { white, gray, { 0.24f, 0.84f, 0.94f, 1.0f }, { 0.93f, 0.46f, 0.28f, 1.0f }, white, white, white };
    
    for ( f32 y = background_bottom; y < background_top; y++ ) {
        
        for ( f32 x = background_left; x < background_right; x++ ) {
            
            u64 index = random_64_get_range( &game.frame_rng, 0, array_count( game.stars ) );
            gl_texture_t* texture = game.stars + index;
            vec4 color = colors[ index ];
            
            rect2_t rect = r2_min_size( x, y, 1, 1 );
            gl_batch_rectangle_textured( game.rectangles, rect.e, 0, 0, color, texture, sampler );
        }
    }
    
    f32 min_intensity = 0.5f;
    f32 intensity_left = 1.0f - min_intensity;
    
    for ( umm star = 0; star < 50; star++ ) {
        
        f32 x = random_64_get_f32( &game.frame_rng );
        f32 y = random_64_get_f32( &game.frame_rng );
        
        x = ( x - 0.5f ) * dimensions.screen_width;
        y = ( y - 0.5f ) * dimensions.screen_height;
        
        umm n = star;
        
        if ( n < 10 ) {
            n = 10;
        }
        
        f32 period = ( 15.0f / n ) * math_tau;
        f32 timer_factor_01 = ( sinf( game.timer * period ) + 1.0f ) * 0.5f;
        f32 col = min_intensity + intensity_left * timer_factor_01;
        vec4 color = v4( col, col, col, 1 );
        
        rect2_t rect = r2_min_size( x, y, 1 * pixel_to_unit, 1 * pixel_to_unit );
        gl_batch_rectangle_textured( game.rectangles, rect.e, 0, 0, color, &game.no, sampler );
    }
    
    if ( game.state.value == state_turn_feedback ) {
        if ( up || down || left || right )  {
            state_machine_set_state( &game.state, state_turn_ship_move );
        }
    }
    
    if ( game.state.value == state_turn_display ) {
        
        f32 computer_y = -2;
        f32 computer_x = -( ( game.world_width - 5 ) / 2 );
        f32 offset_x = computer_x / -2.0f;
        
        for ( umm i = 0; i < array_count( game.computers ); i++ ) {
            
            rect2_t rect = r2_center_size( computer_x, computer_y, 0.5f, 0.5f  );
            computer_x += offset_x;
            
            gl_batch_rectangle_textured( game.rectangles, rect.e, 0, 0, white, game.directions + game.computers[ i ], gl_sampler_get( gl, gl_sampler_type_point ) );
        }
        
        if ( left ) {
            
            if ( game.rock_free == 0 ) {
                audio_wave_play( &g_mixer, &audio_good, 1, 0.5f, false, 0  );
            } else {
                audio_wave_play( &g_mixer, &audio_bad, 1, 0.5f, false, 0 );
            }
            
            game.ship_p.x = -2;
            state_machine_set_state( &game.state, state_turn_feedback );
            
        } else if ( up ) {
            
            if ( game.rock_free == 1 ) {
                audio_wave_play( &g_mixer, &audio_good, 1, 0.5f, false, 0 );
            } else {
                audio_wave_play( &g_mixer, &audio_bad, 1, 0.5f, false, 0 );
            }
            
            game.ship_p.x = -1;
            state_machine_set_state( &game.state, state_turn_feedback );
            
        } else if ( down ) {
            
            if ( game.rock_free == 2 ) {
                audio_wave_play( &g_mixer, &audio_good, 1, 0.5f, false, 0 );
            } else {
                audio_wave_play( &g_mixer, &audio_bad, 1, 0.5f, false, 0 );
            }
            
            game.ship_p.x = 1;
            state_machine_set_state( &game.state, state_turn_feedback );
            
        } else if ( right ) {
            
            if ( game.rock_free == 3 ) {
                audio_wave_play( &g_mixer, &audio_good, 1, 0.5f, false, 0 );
            } else {
                audio_wave_play( &g_mixer, &audio_bad, 1, 0.5f, false, 0 );
            }
            
            game.ship_p.x = 2;
            state_machine_set_state( &game.state, state_turn_feedback );
        }
    }
    
    f32 ship_x = game.ship_p.x;
    
    if ( game.state.value == state_turn_ship_move && ship_x ) {
        
        if ( state_changed ) {
            audio_wave_play( &g_mixer, &audio_ship, 0.75f, 0.5f, false, 0 );
        }
        
        f32 sign = math_sign( ship_x ) * -1;
        ship_x = interpolation_cubic( 0, sign * 0.75f, ship_x * 1.2f, ship_x, state_machine_get_factor( &game.state ) );
        
    } else if ( game.state.value == state_turn_ship_move_back && ship_x ) {
        
        if ( state_changed ) {
            audio_wave_play( &g_mixer, &audio_ship, 0.75f, 0.5f, false, 0 );
        }
        
        f32 sign = math_sign( ship_x ) * -1;
        ship_x = interpolation_cubic( ship_x, ship_x * 1.2f, sign * 0.5f, 0, state_machine_get_factor( &game.state ) );
    }
    
    f32 rock_x[ 4 ] = { -2, -1, 1, 2 };
    
    if ( game.state.value == state_turn_danger_1 ) {
        
        if ( state_changed ) {
            audio_wave_play( &g_mixer, &audio_woosh, 0.15f, 0.5f, false, 0 );
        }
        
        f32 start_y = ( dimensions.screen_height * 0.5f ) + 1.0f;
        f32 end_y = -start_y;
        f32 factor = state_machine_get_factor( &game.state );
        f32 y = math_lerpf( start_y, factor, end_y );
        
        u32 frame = 0;
        
        if ( factor > 3.0f / 4.0f ) {
            frame = 3;
        } else if ( factor > 2.0f / 4.0f ) {
            frame = 2;
        } else if ( factor > 1.0f / 4.0f ) {
            frame = 1;
        } else {
            frame = 0;
        }
        
        rect2_t rock = r2_center_size( 0, y, 0.5f, 1.0f );
        gl_batch_rectangle_textured( game.rectangles, rock.e, 0, 0, white, &game.rocks[ frame ], sampler );
        
        if ( !game.hit_played && game.ship_p.x == 0.0f && factor > 0.5f ) {
            audio_wave_play( &g_mixer, &audio_hit, 0.5f, 0.5f, false, 0 );
            game.hit_played = true;
            
            if ( game.turn ) {
                game.turn--;
            }
        }
        
    } else if ( game.state.value == state_turn_danger_2 ) {
        
        if ( state_changed ) {
            
            umm count = 0;
            
            for ( umm i = 0; i < 4; i++ ) {
                
                if ( i == game.rock_free ) {
                    continue;
                }
                
                game.danger_order[ count ] = cast( u8, i );
                count++;
            }
            
            array_shuffle_u8( game.danger_order, array_count( game.danger_order ) );
        }
        
        if ( state_changed ) {
            audio_wave_play( &g_mixer, &audio_woosh, 0.15f, 0.5f, false, 0 );
        }
        
        f32 start_y = ( dimensions.screen_height * 0.5f ) + 1.0f;
        f32 end_y = -start_y;
        f32 factor = state_machine_get_factor( &game.state );
        f32 y = math_lerpf( start_y, factor, end_y );
        
        u32 frame = 0;
        
        if ( factor > 3.0f / 4.0f ) {
            frame = 3;
        } else if ( factor > 2.0f / 4.0f ) {
            frame = 2;
        } else if ( factor > 1.0f / 4.0f ) {
            frame = 1;
        } else {
            frame = 0;
        }
        
        rect2_t rock = r2_center_size( rock_x[ game.danger_order[ 0 ] ], y, 0.5f, 1.0f );
        gl_batch_rectangle_textured( game.rectangles, rock.e, 0, 0, white, &game.rocks[ frame ], sampler );
        
        if ( !game.hit_played && game.ship_p.x == rock_x[ game.danger_order[ 0 ] ] && factor > 0.5f ) {
            audio_wave_play( &g_mixer, &audio_hit, 0.5f, 0.5f, false, 0 );
            game.hit_played = true;
            
            if ( game.turn ) {
                game.turn--;
            }
        }
        
    } else if ( game.state.value == state_turn_danger_3 ) {
        
        if ( state_changed ) {
            audio_wave_play( &g_mixer, &audio_woosh, 0.15f, 0.5f, false, 0 );
        }
        
        f32 start_y = ( dimensions.screen_height * 0.5f ) + 1.0f;
        f32 end_y = -start_y;
        f32 factor = state_machine_get_factor( &game.state );
        f32 y = math_lerpf( start_y, factor, end_y );
        
        u32 frame = 0;
        
        if ( factor > 3.0f / 4.0f ) {
            frame = 3;
        } else if ( factor > 2.0f / 4.0f ) {
            frame = 2;
        } else if ( factor > 1.0f / 4.0f ) {
            frame = 1;
        } else {
            frame = 0;
        }
        
        rect2_t rock = r2_center_size( rock_x[ game.danger_order[ 1 ] ], y, 0.5f, 1.0f );
        gl_batch_rectangle_textured( game.rectangles, rock.e, 0, 0, white, &game.rocks[ frame ], sampler );
        
        if ( !game.hit_played && game.ship_p.x == rock_x[ game.danger_order[ 1 ] ] && factor > 0.5f ) {
            audio_wave_play( &g_mixer, &audio_hit, 0.5f, 0.5f, false, 0 );
            game.hit_played = true;
            
            if ( game.turn ) {
                game.turn--;
            }
        }
        
    } else if ( game.state.value == state_turn_danger_4 ) {
        
        if ( state_changed ) {
            audio_wave_play( &g_mixer, &audio_woosh, 0.15f, 0.5f, false, 0 );
        }
        
        f32 start_y = ( dimensions.screen_height * 0.5f ) + 1.0f;
        f32 end_y = -start_y;
        f32 factor = state_machine_get_factor( &game.state );
        f32 y = math_lerpf( start_y, factor, end_y );
        
        u32 frame = 0;
        
        if ( factor > 3.0f / 4.0f ) {
            frame = 3;
        } else if ( factor > 2.0f / 4.0f ) {
            frame = 2;
        } else if ( factor > 1.0f / 4.0f ) {
            frame = 1;
        } else {
            frame = 0;
        }
        
        rect2_t rock = r2_center_size( rock_x[ game.danger_order[ 2 ] ], y, 0.5f, 1.0f );
        gl_batch_rectangle_textured( game.rectangles, rock.e, 0, 0, white, &game.rocks[ frame ], sampler );
        
        if ( !game.hit_played && game.ship_p.x == rock_x[ game.danger_order[ 2 ] ] && factor > 0.5f ) {
            audio_wave_play( &g_mixer, &audio_hit, 0.5f, 0.5f, false, 0 );
            game.hit_played = true;
            
            if ( game.turn ) {
                game.turn--;
            }
        }
    }
    
    if ( game.state.value == state_turn_ship_move_back ) {
        
        if ( state_changed ) {
            
            if ( game.ship_p.x == rock_x[ game.rock_free ] ) {
                game.turn++;
            }
        }
    }
    
    if ( game.state.value == state_count ) {
        
        if ( state_changed ) {
            
            game.ship_p.x = 0;
            ship_x = game.ship_p.x;
            
            if ( game.turn == 5 ) {
                game.phase++;
                game.turn = 0;
                
                if ( game.phase == 5 ) {
                    state_machine_set_state( &game.state, state_end );
                } else {
                    state_machine_set_state( &game.state, state_flare );
                }
                
            } else {
                state_machine_set_state( &game.state, state_turn_start );
            }
        }
    }
    
    f32 progress_x = ( dimensions.width * 0.5f ) - ( game.progress_frame.width * pixel_to_unit ) - ( 8 * pixel_to_unit );
    f32 progress_y = -( game.progress_frame.height * 0.5f * pixel_to_unit );
    f32 progress_width = game.progress_frame.width * pixel_to_unit;
    f32 progress_height = game.progress_frame.height * pixel_to_unit;
    
    rect2_t progress_frame = r2_min_size( progress_x, progress_y, progress_width, progress_height );
    gl_batch_rectangle_textured( game.rectangles, progress_frame.e, 0, 0, white, &game.progress_frame, sampler );
    
    rect2_t progress_bars = progress_frame;
    progress_bars.left += 3 * pixel_to_unit;
    progress_bars.right -= 3 * pixel_to_unit;
    progress_bars.bottom += 3 * pixel_to_unit;
    
    for ( umm phase = 0; phase < game.phase; phase++ ) {
        
        for ( umm turn = 0; turn < 5; turn++ ) {
            
            progress_bars.top = progress_bars.bottom + 5 * pixel_to_unit;
            gl_batch_rectangle_textured( game.rectangles, progress_bars.e, 0, 0, white, &game.no, sampler );
            progress_bars.bottom = progress_bars.top + 1 * pixel_to_unit;
        }
    }
    
    for ( umm turn = 0; turn < game.turn; turn++ ) {
        
        progress_bars.top = progress_bars.bottom + 5 * pixel_to_unit;
        gl_batch_rectangle_textured( game.rectangles, progress_bars.e, 0, 0, white, &game.no, sampler );
        progress_bars.bottom = progress_bars.top + 1 * pixel_to_unit;
    }
    
    f32 time_bar_x = -( ( dimensions.width * 0.5f ) - ( 8 * pixel_to_unit ) );
    f32 time_bar_y = -( game.time_bar.height * 0.5f * pixel_to_unit );
    f32 time_bar_width = game.time_bar.width * pixel_to_unit;
    f32 time_bar_height = game.time_bar.height * pixel_to_unit;
    
    rect2_t time_bar = r2_min_size( time_bar_x, time_bar_y, time_bar_width, time_bar_height );
    gl_batch_rectangle_textured( game.rectangles, time_bar.e, 0, 0, white, &game.time_bar, sampler );
    
    time_bar.bottom += 3 * pixel_to_unit;
    time_bar.left += 3 * pixel_to_unit;
    time_bar.right -= 3 * pixel_to_unit;
    f32 state_factor = state_machine_get_factor( &game.state );
    
    if ( game.state.value == state_turn_display ) {
        f32 max_height = game.time_bar.height - 6.0f;
        f32 height = max_height * state_factor;
        time_bar.top = time_bar.bottom + height * pixel_to_unit;
    } else {
        time_bar.top = time_bar.bottom;
    }
    
    gl_batch_rectangle_textured( game.rectangles, time_bar.e, 0, 0, red, &game.no, sampler );
    
    if ( game.state.value == state_turn_display ) {
        
        rect2_t warning = r2_min_size( time_bar.right + 4 * pixel_to_unit, 0, game.warning.width * pixel_to_unit, game.warning.height * pixel_to_unit );
        
        if ( state_factor > 0.50f ) {
            gl_batch_rectangle_textured( game.rectangles, warning.e, 0, 0, white, &game.warning, sampler );
        }
        
        warning = r2_min_size( time_bar.right + 4 * pixel_to_unit, 25 * pixel_to_unit, game.warning.width * pixel_to_unit, game.warning.height * pixel_to_unit );
        
        if ( state_factor > 0.625f ) {
            gl_batch_rectangle_textured( game.rectangles, warning.e, 0, 0, orange, &game.warning, sampler );
        }
        
        warning = r2_min_size( time_bar.right + 4 * pixel_to_unit, 50 * pixel_to_unit, game.warning.width * pixel_to_unit, game.warning.height * pixel_to_unit );
        
        if ( state_factor > 0.75f ) {
            gl_batch_rectangle_textured( game.rectangles, warning.e, 0, 0, red, &game.warning, sampler );
        }
        
        {
            gl_texture_t* direction = game.directions + 0;
            rect2_t positions = r2_center_size( -2, 0, direction->width * pixel_to_unit, direction->height * pixel_to_unit );
            gl_batch_rectangle_textured( game.rectangles, positions.e, 0, 0, gray_darker, direction, sampler );
        }
        
        {
            gl_texture_t* direction = game.directions + 1;
            rect2_t positions = r2_center_size( -1, 0, direction->width * pixel_to_unit, direction->height * pixel_to_unit );
            gl_batch_rectangle_textured( game.rectangles, positions.e, 0, 0, gray_darker, direction, sampler );
        }
        
        {
            gl_texture_t* direction = game.directions + 2;
            rect2_t positions = r2_center_size( 1, 0, direction->width * pixel_to_unit, direction->height * pixel_to_unit );
            gl_batch_rectangle_textured( game.rectangles, positions.e, 0, 0, gray_darker, direction, sampler );
        }
        
        {
            gl_texture_t* direction = game.directions + 3;
            rect2_t positions = r2_center_size( 2, 0, direction->width * pixel_to_unit, direction->height * pixel_to_unit );
            gl_batch_rectangle_textured( game.rectangles, positions.e, 0, 0, gray_darker, direction, sampler );
        }
    }
    
    if ( game.state.value == state_turn_feedback ) {
        
        f32 computer_y = -2;
        f32 computer_x = -( ( game.world_width - 5 ) / 2 );
        f32 offset_x = computer_x / -2.0f;
        
        for ( umm i = 0; i < array_count( game.computers ); i++ ) {
            
            rect2_t rect = r2_center_size( computer_x, computer_y, 0.5f, 0.5f  );
            computer_x += offset_x;
            
            gl_batch_rectangle_textured( game.rectangles, rect.e, 0, 0, white, game.directions + game.computers[ i ], gl_sampler_get( gl, gl_sampler_type_point ) );
        }
        
        ship_x = 0;
        
        {
            vec4 color = gray_darker;
            
            if ( game.rock_free == 0 ) {
                
                if ( game.ship_p.x == -2 ) {
                    color = green;
                } else {
                    color = red;
                }
            }
            
            gl_texture_t* direction = game.directions + 0;
            rect2_t positions = r2_center_size( -2, 0, direction->width * pixel_to_unit, direction->height * pixel_to_unit );
            gl_batch_rectangle_textured( game.rectangles, positions.e, 0, 0, color, direction, sampler );
        }
        
        {
            vec4 color = gray_darker;
            
            if ( game.rock_free == 1 ) {
                
                if ( game.ship_p.x == -1 ) {
                    color = green;
                } else {
                    color = red;
                }
            }
            
            gl_texture_t* direction = game.directions + 1;
            rect2_t positions = r2_center_size( -1, 0, direction->width * pixel_to_unit, direction->height * pixel_to_unit );
            gl_batch_rectangle_textured( game.rectangles, positions.e, 0, 0, color, direction, sampler );
        }
        
        {
            vec4 color = gray_darker;
            if ( game.rock_free == 2 ) {
                if ( game.ship_p.x == 1 ) {
                    color = green;
                } else {
                    color = red;
                }
            }
            
            gl_texture_t* direction = game.directions + 2;
            rect2_t positions = r2_center_size( 1, 0, direction->width * pixel_to_unit, direction->height * pixel_to_unit );
            gl_batch_rectangle_textured( game.rectangles, positions.e, 0, 0, color, direction, sampler );
        }
        
        {
            vec4 color = gray_darker;
            if ( game.rock_free == 3 ) {
                if ( game.ship_p.x == 2 ) {
                    color = green;
                } else {
                    color = red;
                }
            }
            
            gl_texture_t* direction = game.directions + 3;
            rect2_t positions = r2_center_size( 2, 0, direction->width * pixel_to_unit, direction->height * pixel_to_unit );
            gl_batch_rectangle_textured( game.rectangles, positions.e, 0, 0, color, direction, sampler );
        }
    }
    
    if ( game.state.value == state_manual ) {
        
        if ( up || down || left || right ) {
            state_machine_set_state( &game.state, state_phase_start );
        }
        
        rect2_t rect = r2_center_size( 0, 0, game.manual.width * pixel_to_unit, game.manual.height * pixel_to_unit );
        gl_batch_rectangle_textured( game.rectangles, rect.e, 0, 0, white, &game.manual, sampler );
        
    } else if ( game.state.value != state_flare ) {
        
        rect2_t ship = r2_center_size_v( v2( ship_x, game.ship_p.y ), v2( 0.5f, 0.5f ) );
        gl_batch_rectangle_textured( game.rectangles, ship.e, 0, 0, white, &game.ship, sampler );
    }
    
    matrix_t matrix = { 0 };
    matrix_orthographic_x_4( cast( f32, window->width ), cast( f32, window->height ), dimensions.screen_width, 0, 10, matrix.e );
    
    if ( game.state.value == state_flare ) {
        
        if( state_changed ) {
            audio_wave_play( &g_mixer, &audio_flare, 1.0f, 0.5f, false, 0 );
        }
        
        f32 factor = state_machine_get_factor( &game.state );
        umm index = cast( umm, math_floorf( factor * 10.0f ) );
        
        rect2_t flare = r2_center_size( 0, 0, dimensions.width, dimensions.height );
        gl_batch_rectangle_textured( game.rectangles, flare.e, 0, 0, white, &game.flares[ index ], sampler );
    }
    
    if ( game.state.value == state_title ) {
        
        if ( up || down || left || right ) {
            state_machine_set_state( &game.state, state_manual );
        }
        
        rect2_t title = r2_center_size( 0, 0, dimensions.width, dimensions.height );
        gl_batch_rectangle_textured( game.rectangles, title.e, 0, 0, white, &game.title, sampler );
    }
    
    if ( game.state.value == state_end ) {
        rect2_t end = r2_center_size( 0, 0, dimensions.width, dimensions.height );
        gl_batch_rectangle_textured( game.rectangles, end.e, 0, 0, white, &game.end, sampler );
    }
    
    memory_free( &game.rectangles->uniform );
    memory_push_copy_p( &game.rectangles->uniform, &matrix, sizeof( matrix ), 1 );
}