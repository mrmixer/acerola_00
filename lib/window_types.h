#if !defined( WINDOW_TYPES_H )

#include "../lib/custom_system_header.h"
#include "../lib/audio_types.h"
#include "../lib/gl_directx_types.h"

typedef struct platform_t {
    
#if WINDOWS
    
    HWND window_handle;
    HINSTANCE executable_instance;
    
    HCURSOR previous_cursor;
    
    HCURSOR cursor_arrow;
    HCURSOR cursor_hand;
    HCURSOR cursor_text;
    HCURSOR cursor_size_ns;
    HCURSOR cursor_size_we;
    
    struct csh_IMMDeviceEnumerator* audio_enumerator;
    struct csh_IAudioClient* audio_client;
    struct csh_IAudioRenderClient* audio_render_client;
    HANDLE audio_client_event;
    HANDLE audio_thread;
    HANDLE audio_thread_stopped;
    volatile b32 audio_stop_thread;
    audio_mixer_t* mixer;
    
    UINT_PTR timer_id;
    
#elif LINUX
    
    Display* display;
    Window window;
    GLXContext render_context;
    
#else
# error Unsuported platform.
#endif
} platform_t;

typedef struct input_message_t {
    
    u8 type;
    u8 virtual_key;
    u8 state; /* NOTE simon: Pressed, realeased...*/
    u8 modifiers;
    u32 scancode;
    u32 timestamp_ms;
    
    union {
        struct {
            u32 codepoint;
            u8 codepoint_utf8[ 4 ];
            u8 codeunit_count;
        };
        struct {
            s32 mouse_x;
            s32 mouse_y;
            f32 scroll_x;
            f32 scroll_y;
        };
    };
    
} input_message_t;

typedef struct window_t {
    
    b32 running;
    b32 close_requested;
    u32 has_focus;
    b32 fullscreen;
    b32 maximized;/* NOTE simon (31/01/24 17:47:14): Do not rely on this flag, it's only updated when we go to fullscreen. */
    
    s32 x; /* NOTE simon: Relative to the "main" monitor the window is in. */
    s32 y; /* NOTE simon: Relative to the "main" monitor the window is in. */
    u32 width;
    u32 height;
    s32 previous_x; /* NOTE simon: Relative to the "main" monitor the window is in. */
    s32 previous_y; /* NOTE simon: Relative to the "main" monitor the window is in. */
    u32 previous_width;
    u32 previous_height;
    
    b32 resized;
    
    u8 keys[ 256 ];
    u8 scancodes[ 256 ];
    u32 codepoints[ 32 ];
    u32 codepoint_count;
    u8 codepoints_utf8[ 32 * 4 ];
    u8 codepoints_utf8_codeunit_count[ 32 ];
    u8 codepoints_utf8_total_codeunit_count;
    s32 mouse_x;
    s32 mouse_y;
    f32 scroll_x;
    f32 scroll_y;
    
    input_message_t messages[ 32 ];
    u32 message_count;
    platform_t platform;
    gl_platform_t gl_platform;
    
    /* NOTE simon: carets https://docs.microsoft.com/en-us/windows/win32/menurc/carets */
    u32 double_click_delay_ms;
    u32 mouse_wheel_line_scroll_count;
    u32 cursor_blink_delay_ms;
    s32 double_click_width;
    s32 double_click_height;
    
} window_t;

#define WINDOW_TYPES_H
#endif
