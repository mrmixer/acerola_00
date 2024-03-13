#ifndef GAMEPAD_H

#if !defined( GAMEPAD_GAMEINPUT ) && !defined( GAMEPAD_XINPUT ) && !defined( GAMEPAD_PLAYSTATION_RAW_INPUT )

#define GAMEPAD_GAMEINPUT
#define GAMEPAD_XINPUT
#define GAMEPAD_PLAYSTATION_RAW_INPUT

#endif

#if defined( GAMEPAD_GAMEINPUT )
#include <stdbool.h>
#if !defined( COBJMACROS )
#define COBJMACROS 1
#endif
#include <gameinput.h>
#endif

#if defined( GAMEPAD_XINPUT )
#include <xinput.h>
#endif

#if defined( GAMEPAD_PLAYSTATION_RAW_INPUT )
/* NOTE simon: Needs hid.lib */
#define PS_MAX_DEVICES 4
#include "ps_rawinput.h"
typedef ps_state ps_state_t;
#endif

typedef enum gamepad_button_t {
    
    gamepad_button_menu, /* Start, on the right */
    gamepad_button_view, /* Select, on the left */
    gamepad_button_a,
    gamepad_button_cross = gamepad_button_a,
    gamepad_button_b,
    gamepad_button_circle = gamepad_button_b,
    gamepad_button_x,
    gamepad_button_square = gamepad_button_x,
    gamepad_button_y,
    gamepad_button_triangle = gamepad_button_y,
    gamepad_button_dpad_up,
    gamepad_button_dpad_down,
    gamepad_button_dpad_left,
    gamepad_button_dpad_right,
    gamepad_button_left_shoulder,
    gamepad_button_l1 = gamepad_button_left_shoulder,
    gamepad_button_right_shoulder,
    gamepad_button_r1 = gamepad_button_right_shoulder,
    gamepad_button_left_thumbstick,
    gamepad_button_l3 = gamepad_button_left_thumbstick,
    gamepad_button_right_thumbstick,
    gamepad_button_r3 = gamepad_button_right_thumbstick,
    
    gamepad_button_count
        
} gamepad_button_t;

typedef struct gamepad_t {
    
    union {
        f32 left_trigger;
        f32 l2;
    };
    
    union {
        f32 right_trigger;
        f32 r2;
    };
    
    f32 left_thumbstick_x;
    f32 left_thumbstick_y;
    
    f32 right_thumbstick_x;
    f32 right_thumbstick_y;
    
    u8 buttons[ gamepad_button_count ];
    
} gamepad_t;

#define gamepad_gameinput_first_index 0
#define gamepad_gameinput_last_index 3
#define gamepad_xinput_first_index 4
#define gamepad_xinput_last_index 7
#define gamepad_ps_first_index 8
#define gamepad_ps_last_index 11

typedef struct gamepad_state_t {
    
    gamepad_t gamepads[ 12 ];
    void* devices[ 12 ]; /* NOTE simon: For xinput and playstation this is a bool. For gameinput it's the IGameInputDevice*. */
    
#if defined( GAMEPAD_GAMEINPUT )
    IGameInput* game_input;
    GameInputCallbackToken callback_token;
    u32 gameinput_error;
    HMODULE gameinput_dll;
#endif
    
#if defined( GAMEPAD_XINPUT )
    DWORD ( *XInputGetState )( DWORD UserIndex, XINPUT_STATE* pState );
    u64 time;
    u64 frequency;
    u32 xinput_error;
    HMODULE xinput_dll;
#endif
    
#if defined( GAMEPAD_PLAYSTATION_RAW_INPUT )
    u32 playstation_error;
#endif
    
} gamepad_state_t;

typedef enum gamepad_backend_t {
    gamepad_backend_none = 0,
    gamepad_backend_gameinput = 1 << 0,
    gamepad_backend_xinput = 1 << 1,
    gamepad_backend_playstation_raw_input = 1 << 2,
    
    gamepad_backend_all = gamepad_backend_gameinput | gamepad_backend_xinput | gamepad_backend_playstation_raw_input,
} gamepad_backend_t;

typedef enum gamepad_error_t {
    
    gamepad_error_none,
    
    gamepad_error_gameinput_cant_load_dll,
    gamepad_error_gameinput_cant_find_GameInputCreate,
    gamepad_error_gameinput_cant_initialize,
    gamepad_error_gameinput_cant_register_device_callback,
    gamepad_error_gameinput_not_initialized,
    gamepad_error_gameinput_cant_get_device_reading,
    
    gamepad_error_xinput_cant_load_dll,
    gamepad_error_xinput_cant_find_XInputGetState,
    gamepad_error_xinput_not_initialized,
    gamepad_error_xinput_cant_get_device_state,
    
    gamepad_error_playstation_not_initialized,
    
} gamepad_error_t;

typedef enum gamepad_key_flag_t {
    
    gamepad_key_flag_none,
    gamepad_key_flag_pressed = 0x1 << 0,
    gamepad_key_flag_just_pressed = 0x1 << 1,
    gamepad_key_flag_just_released = 0x1 << 2,
    
} gamepad_key_flag_t;

void gamepad_update_key( gamepad_t* pad, u8 button_name, u32 pressed ) {
    
    u32 was_pressed = pad->buttons[ button_name ] & gamepad_key_flag_pressed;
    u8 result = pressed ? gamepad_key_flag_pressed : gamepad_key_flag_none;
    result |= ( pressed && !was_pressed ) ? gamepad_key_flag_just_pressed : gamepad_key_flag_none;
    result |= ( !pressed && was_pressed ) ? gamepad_key_flag_just_released : gamepad_key_flag_none;
    pad->buttons[ button_name ] = result;
}

u32 gamepad_is_pressed( gamepad_t* pad, gamepad_button_t button_name ) {
    u32 result = pad && pad->buttons[ button_name ] & gamepad_key_flag_pressed;
    return result;
}

u32 gamepad_just_pressed( gamepad_t* pad, gamepad_button_t button_name ) {
    u32 result = pad && pad->buttons[ button_name ] & gamepad_key_flag_just_pressed;
    return result;
}

u32 gamepad_just_released( gamepad_t* pad, gamepad_button_t button_name ) {
    u32 result = pad && pad->buttons[ button_name ] & gamepad_key_flag_just_released;
    return result;
}

#if defined( GAMEPAD_GAMEINPUT )

void internal_device_callback( GameInputCallbackToken callback_token, void* context, IGameInputDevice* device, uint64_t timestamp, GameInputDeviceStatus current_status, GameInputDeviceStatus previous_status )  {
    
    gamepad_state_t* state = cast( gamepad_state_t*, context );
    
    umm free_index = umm_max;
    
    for ( umm i = gamepad_gameinput_first_index; i < gamepad_gameinput_last_index + 1; i++ ) {
        if ( !state->devices[ i ] ) {
            free_index = i;
            break;
        }
    }
    
    umm gamepad_index = umm_max;
    
    for ( umm i = gamepad_gameinput_first_index; i < gamepad_gameinput_last_index + 1; i++ ) {
        if ( cast( IGameInputDevice*, state->devices[ i ] ) == device ) {
            gamepad_index = i;
            break;
        }
    }
    
    if ( current_status & GameInputDeviceConnected ) {
        
        if ( gamepad_index == umm_max && free_index != umm_max ) {
            state->devices[ free_index ] = cast( void*, device );
            gamepad_t* pad = state->gamepads + free_index;
            deref( pad ) = ( gamepad_t ) { 0 };
        }
        
    } else if ( gamepad_index != umm_max ) {
        
        state->devices[ gamepad_index ] = 0;
        gamepad_t* pad = state->gamepads + gamepad_index;
        deref( pad ) = ( gamepad_t ) { 0 };
    }
}

#endif

void gamepad_start( gamepad_state_t* state, gamepad_backend_t backends ) {
    
    _assert( state );
    
    deref( state ) = ( gamepad_state_t ) { 0 };
    
#if defined( GAMEPAD_GAMEINPUT )
    
    breakable {
        
        if ( !flag_is_set_u32( backends, gamepad_backend_gameinput ) ) {
            break;
        }
        
        state->gameinput_dll = LoadLibraryW( L"gameinput.dll" );
        
        if ( !state->gameinput_dll ) {
            set_error( &state->gameinput_error, gamepad_error_gameinput_cant_load_dll );
            break;
        }
        
        typedef HRESULT GameInputCreate_t( IGameInput** game_input );
        GameInputCreate_t* GameInputCreate = cast( GameInputCreate_t*, GetProcAddress( state->gameinput_dll, "GameInputCreate" ) );
        
        if ( !GameInputCreate ) {
            set_error( &state->gameinput_error, gamepad_error_gameinput_cant_find_GameInputCreate );
            break;
        }
        
        HRESULT hr = GameInputCreate( &state->game_input );
        
        if ( !SUCCEEDED( hr ) ) {
            set_error( &state->gameinput_error, gamepad_error_gameinput_cant_initialize );
            break;
        }
        
        /* NOTE simon: Use GameInputBlockingEnumeration or GameInputAsyncEnumeration to get call for pads that are already connected. */
        hr = IGameInput_RegisterDeviceCallback( state->game_input, 0, GameInputKindGamepad, GameInputDeviceConnected, GameInputBlockingEnumeration, state, internal_device_callback, &state->callback_token );
        
        if ( !SUCCEEDED( hr ) ) {
            set_error( &state->gameinput_error, gamepad_error_gameinput_cant_register_device_callback );
            break;
        }
    }
#endif
    
#if defined( GAMEPAD_XINPUT )
    
    breakable {
        
        if ( !flag_is_set_u32( backends, gamepad_backend_xinput ) ) {
            break;
        }
        
        state->xinput_dll = LoadLibraryW( L"Xinput1_4.dll" );
        
        if ( !state->xinput_dll ) {
            state->xinput_dll = LoadLibraryW( L"Xinput9_1_0.dll" );
        }
        
        if ( !state->xinput_dll ) {
            state->xinput_dll = LoadLibraryW( L"Xinput1_3.dll" );
        }
        
        if ( !state->xinput_dll ) {
            state->xinput_dll = LoadLibraryW( L"Xinputuap.dll" );
        }
        
        if ( !state->xinput_dll ) {
            set_error( &state->xinput_error, gamepad_error_xinput_cant_load_dll );
            break;
        }
        
        typedef DWORD XInputGetState_t( DWORD UserIndex, XINPUT_STATE* pState );
        state->XInputGetState = ( XInputGetState_t* ) GetProcAddress( state->xinput_dll, "XInputGetState" );
        
        if ( !state->XInputGetState ) {
            set_error( &state->xinput_error, gamepad_error_xinput_cant_find_XInputGetState );
            break;
        }
        
        perf_get_frequency( &state->frequency );
    }
    
#endif
    
#if defined( GAMEPAD_PLAYSTATION_RAW_INPUT )
    
    breakable {
        
        if ( !flag_is_set_u32( backends, gamepad_backend_playstation_raw_input ) ) {
            set_error( &state->playstation_error, gamepad_error_playstation_not_initialized );
            break;
        }
        
    }
    
#endif
}

void gamepad_end( gamepad_state_t* state ) {
    
#if defined( GAMEPAD_GAMEINPUT )
    
    if ( state->callback_token ) {
        IGameInput_UnregisterCallback( state->game_input, state->callback_token, 5000 );
        state->callback_token = 0;
    }
    
    if ( state->game_input ) {
        IGameInput_Release( state->game_input );
        state->game_input = 0;
    }
    
    if ( state->gameinput_dll ) {
        FreeLibrary( state->gameinput_dll );
        state->gameinput_dll = 0;
    }
    
    if ( state->xinput_dll ) {
        FreeLibrary( state->xinput_dll );
        state->xinput_dll = 0;
    }
    
#endif
}

void gamepad_update( gamepad_state_t* state ) {
    
#if defined( GAMEPAD_GAMEINPUT )
    
    breakable {
        
        if ( is_error( &state->gameinput_error ) && deref( &state->gameinput_error ) != gamepad_error_gameinput_cant_get_device_reading ) {
            break;
        }
        
        state->gameinput_error = 0;
        
        if ( !state->game_input ) {
            set_error( &state->gameinput_error, gamepad_error_gameinput_not_initialized );
            break;
        }
        
        for ( umm i = gamepad_gameinput_first_index; i < gamepad_gameinput_last_index + 1; i++ ) {
            
            if ( state->devices[ i ] ) {
                
                IGameInputReading* reading;
                HRESULT hr = IGameInput_GetCurrentReading( state->game_input, GameInputKindGamepad, state->devices[ i ], &reading );
                
                if ( SUCCEEDED( hr ) ) {
                    
                    GameInputGamepadState pad_state;
                    IGameInputReading_GetGamepadState( reading, &pad_state );
                    IGameInputReading_Release( reading );
                    
                    gamepad_t* pad = state->gamepads + i;
                    gamepad_update_key( pad, gamepad_button_menu, pad_state.buttons & GameInputGamepadMenu );
                    gamepad_update_key( pad, gamepad_button_view, pad_state.buttons & GameInputGamepadView );
                    gamepad_update_key( pad, gamepad_button_a, pad_state.buttons & GameInputGamepadA );
                    gamepad_update_key( pad, gamepad_button_b, pad_state.buttons & GameInputGamepadB );
                    gamepad_update_key( pad, gamepad_button_x, pad_state.buttons & GameInputGamepadX );
                    gamepad_update_key( pad, gamepad_button_y, pad_state.buttons & GameInputGamepadY );
                    gamepad_update_key( pad, gamepad_button_dpad_up, pad_state.buttons & GameInputGamepadDPadUp );
                    gamepad_update_key( pad, gamepad_button_dpad_down, pad_state.buttons & GameInputGamepadDPadDown );
                    gamepad_update_key( pad, gamepad_button_dpad_left, pad_state.buttons & GameInputGamepadDPadLeft );
                    gamepad_update_key( pad, gamepad_button_dpad_right, pad_state.buttons & GameInputGamepadDPadRight );
                    gamepad_update_key( pad, gamepad_button_left_shoulder, pad_state.buttons & GameInputGamepadLeftShoulder );
                    gamepad_update_key( pad, gamepad_button_right_shoulder, pad_state.buttons & GameInputGamepadRightShoulder );
                    gamepad_update_key( pad, gamepad_button_left_thumbstick, pad_state.buttons & GameInputGamepadLeftThumbstick );
                    gamepad_update_key( pad, gamepad_button_right_thumbstick, pad_state.buttons & GameInputGamepadRightThumbstick );
                    pad->left_trigger = pad_state.leftTrigger;
                    pad->right_trigger = pad_state.rightTrigger;
                    
                    {
                        f32 x = pad_state.leftThumbstickX;
                        f32 y = pad_state.leftThumbstickY;
                        f32 magnitude = math_sqrtf( x * x + y * y );
                        
                        if ( magnitude ) {
                            x /= magnitude;
                            y /= magnitude;
                            magnitude = math_clamp( 0, magnitude, 1 );
                            pad->left_thumbstick_x = x * magnitude;
                            pad->left_thumbstick_y = y * magnitude;
                        } else {
                            pad->left_thumbstick_x = 0;
                            pad->left_thumbstick_y = 0;
                        }
                    }
                    
                    {
                        f32 x = pad_state.rightThumbstickX;
                        f32 y = pad_state.rightThumbstickY;
                        f32 magnitude = math_sqrtf( x * x + y * y );
                        
                        if ( magnitude ) {
                            x /= magnitude;
                            y /= magnitude;
                            magnitude = math_clamp( 0, magnitude, 1 );
                            pad->right_thumbstick_x = x * magnitude;
                            pad->right_thumbstick_y = y * magnitude;
                        } else {
                            pad->right_thumbstick_x = 0;
                            pad->right_thumbstick_y = 0;
                        }
                    }
                    
                } else {
                    
                    gamepad_t* pad = state->gamepads + i;
                    deref( pad ) = ( gamepad_t ) { 0 };
                    set_error( &state->gameinput_error, gamepad_error_gameinput_cant_get_device_reading );
                }
            }
        }
    }
#endif
    
#if defined( GAMEPAD_XINPUT )
    
    breakable {
        
        if ( is_error( &state->xinput_error ) && state->xinput_error != gamepad_error_xinput_cant_get_device_state ) {
            break;
        }
        
        if ( !state->XInputGetState ) {
            set_error( &state->xinput_error, gamepad_error_xinput_not_initialized );
            break;
        }
        
        b32 check_for_new_controller = false;
        
        u64 time;
        perf_get_time( &time );
        
        if ( state->time == 0 ) {
            check_for_new_controller = true;
            state->time = time;
        } else {
            
            f32 delta = cast( f32, cast( f64, time - state->time ) / cast( f64, state->frequency ) );
            
            if ( delta >= 3.0f ) {
                check_for_new_controller = true;
                state->time = time;
            }
        }
        
        for ( u32 i = gamepad_xinput_first_index; i < gamepad_xinput_last_index + 1; i++ ) {
            
            if ( state->devices[ i ] || check_for_new_controller ) {
                
                XINPUT_STATE pad_state;
                DWORD result = state->XInputGetState( i - gamepad_xinput_first_index, &pad_state );
                
                if ( result == ERROR_SUCCESS ) {
                    
                    state->devices[ i ] = cast( void*, true );
                    
                    gamepad_t* pad = state->gamepads + i;
                    gamepad_update_key( pad, gamepad_button_menu, pad_state.Gamepad.wButtons & XINPUT_GAMEPAD_START );
                    gamepad_update_key( pad, gamepad_button_view, pad_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK );
                    gamepad_update_key( pad, gamepad_button_a, pad_state.Gamepad.wButtons & XINPUT_GAMEPAD_A );
                    gamepad_update_key( pad, gamepad_button_b, pad_state.Gamepad.wButtons & XINPUT_GAMEPAD_B );
                    gamepad_update_key( pad, gamepad_button_x, pad_state.Gamepad.wButtons & XINPUT_GAMEPAD_X );
                    gamepad_update_key( pad, gamepad_button_y, pad_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y );
                    gamepad_update_key( pad, gamepad_button_dpad_up, pad_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP );
                    gamepad_update_key( pad, gamepad_button_dpad_down, pad_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN );
                    gamepad_update_key( pad, gamepad_button_dpad_left, pad_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT );
                    gamepad_update_key( pad, gamepad_button_dpad_right, pad_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT );
                    gamepad_update_key( pad, gamepad_button_left_shoulder, pad_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER );
                    gamepad_update_key( pad, gamepad_button_right_shoulder, pad_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER );
                    gamepad_update_key( pad, gamepad_button_left_thumbstick, pad_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB );
                    gamepad_update_key( pad, gamepad_button_right_thumbstick, pad_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB );
                    
                    pad->left_trigger = pad_state.Gamepad.bLeftTrigger / 255.0f;
                    pad->right_trigger = pad_state.Gamepad.bRightTrigger / 255.0f;
                    
                    {
                        f32 x = cast( f32, pad_state.Gamepad.sThumbLX ) + 32768;
                        f32 y = cast( f32, pad_state.Gamepad.sThumbLY ) + 32768;
                        x = ( x / 65535 ) * 2 - 1;
                        y = ( y / 65535 ) * 2 - 1;
                        f32 magnitude = math_sqrtf( ( x * x ) + ( y * y ) );
                        
                        if ( magnitude ) {
                            x /= magnitude;
                            y /= magnitude;
                            magnitude = math_clamp( 0, magnitude, 1 );
                            pad->left_thumbstick_x = x * magnitude;
                            pad->left_thumbstick_y = y * magnitude;
                        } else {
                            pad->left_thumbstick_x = 0;
                            pad->left_thumbstick_y = 0;
                        }
                    }
                    
                    {
                        f32 x = cast( f32, pad_state.Gamepad.sThumbRX ) + 32768;
                        f32 y = cast( f32, pad_state.Gamepad.sThumbRY ) + 32768;
                        x = ( x / 65535 ) * 2 - 1;
                        y = ( y / 65535 ) * 2 - 1;
                        f32 magnitude = math_sqrtf( ( x * x ) + ( y * y ) );
                        
                        if ( magnitude ) {
                            x /= magnitude;
                            y /= magnitude;
                            magnitude = math_clamp( 0, magnitude, 1 );
                            pad->right_thumbstick_x = x * magnitude;
                            pad->right_thumbstick_y = y * magnitude;
                        } else {
                            pad->right_thumbstick_x = 0;
                            pad->right_thumbstick_y = 0;
                        }
                    }
                    
                } else if ( result == ERROR_DEVICE_NOT_CONNECTED ) {
                    
                    state->devices[ i ] = 0;
                    gamepad_t* pad = state->gamepads + i;
                    deref( pad ) = ( gamepad_t ) { 0 };
                    
                } else {
                    
                    set_error( &state->xinput_error, gamepad_error_xinput_cant_get_device_state );
                }
            }
        }
    }
#endif
    
#if defined( GAMEPAD_PLAYSTATION_RAW_INPUT )
    
    breakable {
        
        for ( s32 i = gamepad_ps_first_index, j = 1; i < gamepad_ps_last_index + 1; i++, j++ ) {
            
            const ps_state_t* pad_state = ps_get( j );
            
            if ( pad_state ) {
                
                state->devices[ i ] = cast( void*, true );
                
                gamepad_t* pad = state->gamepads + i;
                gamepad_update_key( pad, gamepad_button_menu, pad_state->buttons & PS_BUTTON_OPTIONS );
                gamepad_update_key( pad, gamepad_button_view, pad_state->buttons & PS_BUTTON_SHARE );
                gamepad_update_key( pad, gamepad_button_a, pad_state->buttons & PS_BUTTON_X );
                gamepad_update_key( pad, gamepad_button_b, pad_state->buttons & PS_BUTTON_O );
                gamepad_update_key( pad, gamepad_button_x, pad_state->buttons & PS_BUTTON_S );
                gamepad_update_key( pad, gamepad_button_y, pad_state->buttons & PS_BUTTON_T );
                gamepad_update_key( pad, gamepad_button_dpad_up, pad_state->buttons & PS_DPAD_UP );
                gamepad_update_key( pad, gamepad_button_dpad_down, pad_state->buttons & PS_DPAD_DOWN );
                gamepad_update_key( pad, gamepad_button_dpad_left, pad_state->buttons & PS_DPAD_LEFT );
                gamepad_update_key( pad, gamepad_button_dpad_right, pad_state->buttons & PS_DPAD_RIGHT );
                gamepad_update_key( pad, gamepad_button_left_shoulder, pad_state->buttons & PS_BUTTON_L1 );
                gamepad_update_key( pad, gamepad_button_right_shoulder, pad_state->buttons & PS_BUTTON_R1 );
                gamepad_update_key( pad, gamepad_button_left_thumbstick, pad_state->buttons & PS_BUTTON_L3 );
                gamepad_update_key( pad, gamepad_button_right_thumbstick, pad_state->buttons & PS_BUTTON_R3 );
                
                pad->left_trigger = pad_state->l2 / 255.0f;
                pad->right_trigger = pad_state->r2 / 255.0f;
                
                {
                    f32 x = pad_state->left[ 0 ] + 128.0f;
                    f32 y = pad_state->left[ 1 ] + 128.0f;
                    
                    x = ( x / 255 ) * 2 - 1;
                    y = ( y / 255 ) * 2 - 1;
                    
                    f32 magnitude = math_sqrtf( ( x * x ) + ( y * y ) );
                    
                    if ( magnitude ) {
                        x /= magnitude;
                        y /= magnitude;
                        magnitude = math_clamp( 0, magnitude, 1 );
                        pad->left_thumbstick_x = x * magnitude;
                        pad->left_thumbstick_y = y * magnitude;
                        pad->left_thumbstick_y *= -1;
                    } else {
                        pad->left_thumbstick_x = 0;
                        pad->left_thumbstick_y = 0;
                    }
                }
                
                {
                    f32 x = pad_state->right[ 0 ] + 128.0f;
                    f32 y = pad_state->right[ 1 ] + 128.0f;
                    
                    x = ( x / 255 ) * 2 - 1;
                    y = ( y / 255 ) * 2 - 1;
                    
                    f32 magnitude = math_sqrtf( ( x * x ) + ( y * y ) );
                    
                    if ( magnitude ) {
                        x /= magnitude;
                        y /= magnitude;
                        magnitude = math_clamp( 0, magnitude, 1 );
                        pad->right_thumbstick_x = x * magnitude;
                        pad->right_thumbstick_y = y * magnitude;
                        pad->right_thumbstick_y *= -1;
                    } else {
                        pad->right_thumbstick_x = 0;
                        pad->right_thumbstick_y = 0;
                    }
                }
                
            } else {
                
                state->devices[ i ] = false;
                gamepad_t* pad = state->gamepads + i;
                deref( pad ) = ( gamepad_t ) { 0 };
            }
        }
#endif
    }
}

#define GAMEPAD_H
#endif