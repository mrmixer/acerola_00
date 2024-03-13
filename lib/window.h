#ifndef WINDOW_H

#include "../lib/window_types.h"
#include "audio.h"

typedef enum input_message_modifier_t {
    input_message_modifier_none = 0,
    input_message_modifier_control_left = 1 << 0,
    input_message_modifier_control_right = 1 << 1,
    input_message_modifier_alt_left = 1 << 2,
    input_message_modifier_alt_right = 1 << 3,
    input_message_modifier_shift_left = 1 << 4,
    input_message_modifier_shift_right = 1 << 5,
    input_message_modifier_meta_left = 1 << 6,
    input_message_modifier_meta_right = 1 << 7,
} input_message_modifier_t;

typedef enum input_message_type_t {
    input_message_type_none = 0,
    input_message_type_mouse = 1 << 0,
    input_message_type_mouse_move = 1 << 1,
    input_message_type_keyboard = 1 << 2,
} input_message_type_t;

typedef enum input_virtual_key_t {
    
    vk_none,
    
    vk_mouse_left,
    vk_mouse_right,
    vk_mouse_midle,
    vk_mouse_4,
    vk_mouse_5,
    vk_cancel,
    vk_backspace,
    vk_tab,
    vk_clear,
    vk_return,
    vk_shift,
    vk_control,
    vk_alt,
    vk_pause,
    vk_caps_lock,
    vk_kana,
    vk_hangul,
    vk_junja,
    vk_final,
    vk_hanja,
    vk_kanji,
    vk_escape,
    vk_convert,
    vk_non_convert,
    vk_accept,
    vk_mode_change,
    vk_space,
    vk_page_up,
    vk_page_down,
    vk_end,
    vk_home,
    vk_left,
    vk_up,
    vk_right,
    vk_down,
    vk_select,
    vk_print,
    vk_execute,
    vk_print_screen,
    vk_insert,
    vk_delete,
    vk_help,
    
    vk_0,
    vk_1,
    vk_2,
    vk_3,
    vk_4,
    vk_5,
    vk_6,
    vk_7,
    vk_8,
    vk_9,
    
    vk_a,
    vk_b,
    vk_c,
    vk_d,
    vk_e,
    vk_f,
    vk_g,
    vk_h,
    vk_i,
    vk_j,
    vk_k,
    vk_l,
    vk_m,
    vk_n,
    vk_o,
    vk_p,
    vk_q,
    vk_r,
    vk_s,
    vk_t,
    vk_u,
    vk_v,
    vk_w,
    vk_x,
    vk_y,
    vk_z,
    
    vk_left_meta,
    vk_right_meta,
    vk_application,
    vk_sleep,
    vk_numpad_0,
    vk_numpad_1,
    vk_numpad_2,
    vk_numpad_3,
    vk_numpad_4,
    vk_numpad_5,
    vk_numpad_6,
    vk_numpad_7,
    vk_numpad_8,
    vk_numpad_9,
    
    vk_multiply,
    vk_divide,
    vk_add,
    vk_subtract,
    vk_separator,
    vk_decimal,
    
    vk_f1,
    vk_f2,
    vk_f3,
    vk_f4,
    vk_f5,
    vk_f6,
    vk_f7,
    vk_f8,
    vk_f9,
    vk_f10,
    vk_f11,
    vk_f12,
    vk_f13,
    vk_f14,
    vk_f15,
    vk_f16,
    vk_f17,
    vk_f18,
    vk_f19,
    vk_f20,
    vk_f21,
    vk_f22,
    vk_f23,
    vk_f24,
    
    vk_num_lock,
    vk_scroll_lock,
    vk_left_shift,
    vk_right_shift,
    vk_left_control,
    vk_right_control,
    vk_left_alt,
    vk_right_alt,
    
    vk_browser_back,
    vk_browser_forward,
    vk_browser_refresh,
    vk_browser_stop,
    vk_browser_search,
    vk_browser_favorites,
    vk_browser_home,
    vk_volume_mute,
    vk_volume_down,
    vk_volume_up,
    vk_media_next,
    vk_media_previous,
    vk_media_stop,
    vk_media_play,
    vk_launch_mail,
    vk_launch_media,
    vk_launch_application_1,
    vk_launch_application_2,
    
    vk_oem_plus,
    vk_oem_minus,
    vk_oem_comma,
    vk_oem_period,
    vk_oem_1,
    vk_oem_2,
    vk_oem_3,
    vk_oem_4,
    vk_oem_5,
    vk_oem_6,
    vk_oem_7,
    vk_oem_8,
    vk_oem_102,
    vk_oem_clear,
    vk_process_key,
    vk_packet,
    vk_attn,
    vk_crsel,
    vk_exsel,
    vk_ereof,
    vk_play,
    vk_zoom,
    vk_noname,
    vk_pa1,
    
    vk_count
} input_virtual_key_t;

/* NOTE simon: Keyboard input article: https://www.sisyphe.be/donotdelete/keyboard_input_article.txt . */
typedef enum input_scancode_t {
    
    sc_escape = 0x01,
    sc_1 = 0x02,
    sc_2 = 0x03,
    sc_3 = 0x04,
    sc_4 = 0x05,
    sc_5 = 0x06,
    sc_6 = 0x07,
    sc_7 = 0x08,
    sc_8 = 0x09,
    sc_9 = 0x0A,
    sc_0 = 0x0B,
    sc_minus = 0x0C,
    sc_equals = 0x0D,
    sc_backspace = 0x0E,
    sc_tab = 0x0F,
    sc_q = 0x10,
    sc_w = 0x11,
    sc_e = 0x12,
    sc_r = 0x13,
    sc_t = 0x14,
    sc_y = 0x15,
    sc_u = 0x16,
    sc_i = 0x17,
    sc_o = 0x18,
    sc_p = 0x19,
    sc_bracket_left = 0x1A,
    sc_bracket_right = 0x1B,
    sc_enter = 0x1C,
    sc_control_left = 0x1D,
    sc_a = 0x1E,
    sc_s =0x1F,
    sc_d = 0x20,
    sc_f = 0x21,
    sc_g = 0x22,
    sc_h = 0x23,
    sc_j = 0x24,
    sc_k = 0x25,
    sc_l = 0x26,
    sc_semicolon = 0x27,
    sc_apostrophe = 0x28,
    sc_grave = 0x29,
    sc_shift_left = 0x2A,
    sc_backslash = 0x2B,
    sc_z = 0x2C,
    sc_x = 0x2D,
    sc_c = 0x2E,
    sc_v = 0x2F,
    sc_b = 0x30,
    sc_n = 0x31,
    sc_m = 0x32,
    sc_comma = 0x33,
    sc_period = 0x34,
    sc_slash = 0x35,
    sc_shift_right = 0x36,
    sc_numpad_multiply = 0x37,
    sc_alt_left = 0x38,
    sc_space = 0x39,
    sc_caps_lock = 0x3A,
    sc_f1 = 0x3B,
    sc_f2 = 0x3C,
    sc_f3 = 0x3D,
    sc_f4 = 0x3E,
    sc_f5 = 0x3F,
    sc_f6 = 0x40,
    sc_f7 = 0x41,
    sc_f8 = 0x42,
    sc_f9 = 0x43,
    sc_f10 = 0x44,
    sc_num_lock = 0x45,
    sc_scroll_lock = 0x46,
    sc_numpad_7 = 0x47,
    sc_numpad_8 = 0x48,
    sc_numpad_9 = 0x49,
    sc_numpad_minus = 0x4A,
    sc_numpad_4 = 0x4B,
    sc_numpad_5 = 0x4C,
    sc_numpad_6 = 0x4D,
    sc_numpad_plus = 0x4E,
    sc_numpad_1 = 0x4F,
    sc_numpad_2 = 0x50,
    sc_numpad_3 = 0x51,
    sc_numpad_0 = 0x52,
    sc_numpad_period = 0x53,
    sc_alt_print_screen = 0x54, /* Alt + print screen. MapVirtualKeyEx( VK_SNAPSHOT, MAPVK_VK_TO_VSC_EX, 0 ) returns scancode 0x54. */
    sc_bracket_angle = 0x56, /* Key between the left shift and Z. */
    sc_f11 = 0x57,
    sc_f12 = 0x58,
    sc_oem_1 = 0x5a, /* VK_OEM_WSCTRL */
    sc_oem_2 = 0x5b, /* VK_OEM_FINISH */
    sc_oem_3 = 0x5c, /* VK_OEM_JUMP */
    sc_erase_eof = 0x5d,
    sc_oem_4 = 0x5e, /* VK_OEM_BACKTAB */
    sc_oem_5 = 0x5f, /* VK_OEM_AUTO */
    sc_zoom = 0x62,
    sc_help = 0x63,
    sc_f13 = 0x64,
    sc_f14 = 0x65,
    sc_f15 = 0x66,
    sc_f16 = 0x67,
    sc_f17 = 0x68,
    sc_f18 = 0x69,
    sc_f19 = 0x6a,
    sc_f20 = 0x6b,
    sc_f21 = 0x6c,
    sc_f22 = 0x6d,
    sc_f23 = 0x6e,
    sc_oem_6 = 0x6f, /* VK_OEM_PA3 */
    sc_katakana = 0x70,
    sc_oem_7 = 0x71, /* VK_OEM_RESET */
    sc_f24 = 0x76,
    sc_sbcschar = 0x77,
    sc_convert = 0x79,
    sc_nonconvert = 0x7B, /* VK_OEM_PA1 */
    
    sc_media_previous = 0xE010,
    sc_media_next = 0xE019,
    sc_numpad_enter = 0xE01C,
    sc_control_right = 0xE01D,
    sc_volume_mute = 0xE020,
    sc_launch_app2 = 0xE021,
    sc_media_play = 0xE022,
    sc_media_stop = 0xE024,
    sc_volume_down = 0xE02E,
    sc_volume_up = 0xE030,
    sc_browser_home = 0xE032,
    sc_numpad_divide = 0xE035,
    sc_print_screen = 0xE037,
    /*
    sc_printScreen:
    - make: 0xE02A 0xE037
    - break: 0xE0B7 0xE0AA
    - MapVirtualKeyEx( VK_SNAPSHOT, MAPVK_VK_TO_VSC_EX, 0 ) returns scancode 0x54;
    - There is no VK_KEYDOWN with VK_SNAPSHOT.
    */
    sc_alt_right = 0xE038,
    sc_cancel = 0xE046, /* CTRL + Pause */
    sc_home = 0xE047,
    sc_arrow_up = 0xE048,
    sc_page_up = 0xE049,
    sc_arrow_left = 0xE04B,
    sc_arrow_right = 0xE04D,
    sc_end = 0xE04F,
    sc_arrow_down = 0xE050,
    sc_page_down = 0xE051,
    sc_insert = 0xE052,
    sc_delete = 0xE053,
    sc_meta_left = 0xE05B,
    sc_meta_right = 0xE05C,
    sc_application = 0xE05D,
    sc_power = 0xE05E,
    sc_sleep = 0xE05F,
    sc_wake = 0xE063,
    sc_browser_search = 0xE065,
    sc_browser_favorites = 0xE066,
    sc_browser_refresh = 0xE067,
    sc_browser_stop = 0xE068,
    sc_browser_forward = 0xE069,
    sc_browser_back = 0xE06A,
    sc_launch_app1 = 0xE06B,
    sc_launch_email = 0xE06C,
    sc_launch_media = 0xE06D,
    
    sc_pause = 0xE11D45,
    /*
    sc_pause:
    - make: 0xE11D 45 0xE19D C5
    - make in raw input: 0xE11D 0x45
    - break: none
    - No repeat when you hold the key down
    - There are no break so I don't know how the key down/up is expected to work. Raw input sends "keydown" and "keyup" messages, and it appears that the keyup message is sent directly after the keydown message (you can't hold the key down) so depending on when GetMessage or PeekMessage will return messages, you may get both a keydown and keyup message "at the same time". If you use VK messages most of the time you only get keydown messages, but some times you get keyup messages too.
    - when pressed at the same time as one or both control keys, generates a 0xE046 (sc_cancel) and the string for that scancode is "break".
    */
} input_scancode_t;

u32 input_scancode_list[ ] = {
    
    sc_escape,
    sc_1,
    sc_2,
    sc_3,
    sc_4,
    sc_5,
    sc_6,
    sc_7,
    sc_8,
    sc_9,
    sc_0,
    sc_minus,
    sc_equals,
    sc_backspace,
    sc_tab,
    sc_q,
    sc_w,
    sc_e,
    sc_r,
    sc_t,
    sc_y,
    sc_u,
    sc_i,
    sc_o,
    sc_p,
    sc_bracket_left,
    sc_bracket_right,
    sc_enter,
    sc_control_left,
    sc_a,
    sc_s,
    sc_d,
    sc_f,
    sc_g,
    sc_h,
    sc_j,
    sc_k,
    sc_l,
    sc_semicolon,
    sc_apostrophe,
    sc_grave,
    sc_shift_left,
    sc_backslash,
    sc_z,
    sc_x,
    sc_c,
    sc_v,
    sc_b,
    sc_n,
    sc_m,
    sc_comma,
    sc_period,
    sc_slash,
    sc_shift_right,
    sc_numpad_multiply,
    sc_alt_left,
    sc_space,
    sc_caps_lock,
    sc_f1,
    sc_f2,
    sc_f3,
    sc_f4,
    sc_f5,
    sc_f6,
    sc_f7,
    sc_f8,
    sc_f9,
    sc_f10,
    sc_num_lock,
    sc_scroll_lock,
    sc_numpad_7,
    sc_numpad_8,
    sc_numpad_9,
    sc_numpad_minus,
    sc_numpad_4,
    sc_numpad_5,
    sc_numpad_6,
    sc_numpad_plus,
    sc_numpad_1,
    sc_numpad_2,
    sc_numpad_3,
    sc_numpad_0,
    sc_numpad_period,
    sc_alt_print_screen,
    sc_bracket_angle,
    sc_f11,
    sc_f12,
    sc_oem_1,
    sc_oem_2,
    sc_oem_3,
    sc_erase_eof,
    sc_oem_4,
    sc_oem_5,
    sc_zoom,
    sc_help,
    sc_f13,
    sc_f14,
    sc_f15,
    sc_f16,
    sc_f17,
    sc_f18,
    sc_f19,
    sc_f20,
    sc_f21,
    sc_f22,
    sc_f23,
    sc_oem_6,
    sc_katakana,
    sc_oem_7,
    sc_f24,
    sc_sbcschar,
    sc_convert,
    sc_nonconvert,
    
    sc_media_previous,
    sc_media_next,
    sc_numpad_enter,
    sc_control_right,
    sc_volume_mute,
    sc_launch_app2,
    sc_media_play,
    sc_media_stop,
    sc_volume_down,
    sc_volume_up,
    sc_browser_home,
    sc_numpad_divide,
    sc_print_screen,
    sc_alt_right,
    sc_cancel,
    sc_home,
    sc_arrow_up,
    sc_page_up,
    sc_arrow_left,
    sc_arrow_right,
    sc_end,
    sc_arrow_down,
    sc_page_down,
    sc_insert,
    sc_delete,
    sc_meta_left,
    sc_meta_right,
    sc_application,
    sc_power,
    sc_sleep,
    sc_wake,
    sc_browser_search,
    sc_browser_favorites,
    sc_browser_refresh,
    sc_browser_stop,
    sc_browser_forward,
    sc_browser_back,
    sc_launch_app1,
    sc_launch_email,
    sc_launch_media,
    
    sc_pause
};

u8 input_vk_to_key[ 256 ] = {
#if WINDOWS
    /* 0x00 */ vk_none, /* 0x01 */ vk_mouse_left, /* 0x02 */ vk_mouse_right, /* 0x03 */ vk_cancel, /* 0x04 */ vk_mouse_midle, /* 0x05 */ vk_mouse_4, /* 0x06 */ vk_mouse_5, /* 0x07 */ 0, /* 0x08 */ vk_backspace, /* 0x09 */ vk_tab, /* 0x0a */ 0, /* 0x0b */ 0, /* 0x0c */ vk_clear, /* 0x0d */ vk_return, /* 0x0e */ 0, /* 0x0f */ 0,
    /* 0x10 */ vk_shift, /* 0x11 */ vk_control, /* 0x12 */ vk_alt, /* 0x13 */ vk_pause, /* 0x14 */ vk_caps_lock, /* 0x15 */ vk_kana, /* 0x15 vk_hangul, *//* 0x16 */ 0, /* 0x17 */ vk_junja, /* 0x18 */ vk_final, /* 0x19 */ vk_hanja, /* 0x19 vk_kanji, */ /* 0x1a */ 0, /* 0x1b */ vk_escape, /* 0x1c */ vk_convert, /* 0x1d */ vk_non_convert, /* 0x1e */ vk_accept, /* 0x1f */ vk_mode_change,
    /* 0x20 */ vk_space, /* 0x21 */ vk_page_up, /* 0x22 */ vk_page_down, /* 0x23 */ vk_end, /* 0x24 */ vk_home, /* 0x25 */ vk_left, /* 0x26 */ vk_up, /* 0x27 */ vk_right, /* 0x28 */ vk_down, /* 0x29 */ vk_select, /* 0x2a */ vk_print, /* 0x2b */ vk_execute, /* 0x2c */ vk_print_screen, /* 0x2d */ vk_insert, /* 0x2e */ vk_delete, /* 0x2f */ vk_help,
    /* 0x30 */ vk_0, /* 0x31 */ vk_1, /* 0x32 */ vk_2, /* 0x33 */ vk_3, /* 0x34 */ vk_4, /* 0x35 */ vk_5, /* 0x36 */ vk_6, /* 0x37 */ vk_7, /* 0x38 */ vk_8, /* 0x39 */ vk_9, /* 0x3a */ 0, /* 0x3b */ 0, /* 0x3c */ 0, /* 0x3d */ 0, /* 0x3e */ 0, /* 0x3f */ 0,
    /* 0x40 */ 0, /* 0x41 */ vk_a, /* 0x42 */ vk_b, /* 0x43 */ vk_c, /* 0x44 */ vk_d, /* 0x45 */ vk_e, /* 0x46 */ vk_f, /* 0x47 */ vk_g, /* 0x48 */ vk_h, /* 0x49 */ vk_i, /* 0x4a */ vk_j, /* 0x4b */ vk_k, /* 0x4c */ vk_l, /* 0x4d */ vk_m, /* 0x4e */ vk_n, /* 0x4f */ vk_o,
    /* 0x50 */ vk_p, /* 0x51 */ vk_q, /* 0x52 */ vk_r, /* 0x53 */ vk_s, /* 0x54 */ vk_t, /* 0x55 */ vk_u, /* 0x56 */ vk_v, /* 0x57 */ vk_w, /* 0x58 */ vk_x, /* 0x59 */ vk_y, /* 0x5a */ vk_z, /* 0x5b */ vk_left_meta, /* 0x5c */ vk_right_meta, /* 0x5d */ vk_application, /* 0x5e */ 0,/* 0x5f */ vk_sleep,
    /* 0x60 */ vk_numpad_0, /* 0x61 */ vk_numpad_1, /* 0x62 */ vk_numpad_2, /* 0x63 */ vk_numpad_3, /* 0x64 */ vk_numpad_4, /* 0x65 */ vk_numpad_5,/* 0x66 */ vk_numpad_6, /* 0x67 */ vk_numpad_7, /* 0x68 */ vk_numpad_8, /* 0x69 */ vk_numpad_9, /* 0x6a */ vk_multiply, /* 0x6b */ vk_add, /* 0x6c */ vk_separator, /* 0x6d */ vk_subtract, /* 0x6e */ vk_decimal,/* 0x6f */ vk_divide,
    /* 0x70 */ vk_f1, /* 0x71 */ vk_f2, /* 0x72 */ vk_f3, /* 0x73 */ vk_f4, /* 0x74 */ vk_f5, /* 0x75 */ vk_f6, /* 0x76 */ vk_f7, /* 0x77 */ vk_f8, /* 0x78 */ vk_f9, /* 0x79 */ vk_f10, /* 0x7a */ vk_f11, /* 0x7b */ vk_f12, /* 0x7c */ vk_f13, /* 0x7d */ vk_f14, /* 0x7e */ vk_f15, /* 0x7f */ vk_f16,
    /* 0x80 */ vk_f17, /* 0x81 */ vk_f18, /* 0x82 */ vk_f19, /* 0x83 */ vk_f20, /* 0x84 */ vk_f21, /* 0x85 */ vk_f22, /* 0x86 */ vk_f23, /* 0x87 */ vk_f24, /* 0x88 */ 0, /* 0x89 */ 0, /* 0x8a */ 0, /* 0x8b */ 0, /* 0x8c */ 0, /* 0x8d */ 0, /* 0x8e */ 0, /* 0x8f */ 0,
    /* 0x90 */ vk_num_lock, /* 0x91 */ vk_scroll_lock, /* 0x92 */ 0, /* 0x93 */ 0, /* 0x94 */ 0, /* 0x95 */ 0, /* 0x96 */ 0, /* 0x97 */ 0, /* 0x98 */ 0, /* 0x99 */ 0, /* 0x9a */ 0, /* 0x9b */ 0, /* 0x9c */ 0, /* 0x9d */ 0, /* 0x9e */ 0, /* 0x9f */ 0,
    /* 0xa0 */ vk_left_shift, /* 0xa1 */ vk_right_shift, /* 0xa2 */ vk_left_control, /* 0xa3 */ vk_right_control, /* 0xa4 */ vk_left_alt, /* 0xa5 */ vk_right_alt, /* 0xa6 */ vk_browser_back, /* 0xa7 */ vk_browser_forward, /* 0xa8 */ vk_browser_refresh, /* 0xa9 */ vk_browser_stop, /* 0xaa */ vk_browser_search, /* 0xab */ vk_browser_favorites, /* 0xac */ vk_browser_home, /* 0xad */ vk_volume_mute, /* 0xae */ vk_volume_down, /* 0xaf */ vk_volume_up,
    /* 0xb0 */ vk_media_next, /* 0xb1 */ vk_media_previous, /* 0xb2 */ vk_media_stop, /* 0xb3 */ vk_media_play, /* 0xb4 */ vk_launch_mail, /* 0xb5 */ vk_launch_media, /* 0xb6 */ vk_launch_application_1, /* 0xb7 */ vk_launch_application_2, /* 0xb8 */ 0, /* 0xb9 */ 0, /* 0xba */ vk_oem_1, /* 0xbb */ vk_oem_plus, /* 0xbc */ vk_oem_comma, /* 0xbd */ vk_oem_minus, /* 0xbe */ vk_oem_period, /* 0xbf */ vk_oem_2,
    /* 0xc0 */ vk_oem_3, /* 0xc1 */ 0, /* 0xc2 */ 0, /* 0xc3 */ 0, /* 0xc4 */ 0, /* 0xc5 */ 0, /* 0xc6 */ 0, /* 0xc7 */ 0, /* 0xc8 */ 0, /* 0xc9 */ 0, /* 0xca */ 0, /* 0xcb */ 0, /* 0xcc */ 0, /* 0xcd */ 0, /* 0xce */ 0, /* 0xcf */ 0,
    /* 0xd0 */ 0, /* 0xd1 */ 0, /* 0xd2 */ 0, /* 0xd3 */ 0, /* 0xd4 */ 0, /* 0xd5 */ 0, /* 0xd6 */ 0, /* 0xd7 */ 0, /* 0xd8 */ 0, /* 0xd9 */ 0, /* 0xda */ 0, /* 0xdb */ vk_oem_4, /* 0xdc */ vk_oem_5, /* 0xdd */ vk_oem_6, /* 0xde */ vk_oem_7, /* 0xdf */ vk_oem_8,
    /* 0xe0 */ 0, /* 0xe1 */ 0, /* 0xe2 */ vk_oem_102, /* 0xe3 */ 0, /* 0xe4 */ 0, /* 0xe5 */ vk_process_key, /* 0xe6 */ 0, /* 0xe7 */ vk_packet, /* 0xe8 */ 0, /* 0xe9 */ 0, /* 0xea */ 0, /* 0xeb */ 0, /* 0xec */ 0, /* 0xed */ 0, /* 0xee */ 0, /* 0xef */ 0, /* 0xf0 */ 0, /* 0xf1 */ 0, /* 0xf2 */ 0, /* 0xf3 */ 0, /* 0xf4 */ 0, /* 0xf5 */ 0, /* 0xf6 */ vk_attn, /* 0xf7 */ vk_crsel, /* 0xf8 */ vk_exsel, /* 0xf9 */ vk_ereof, /* 0xfa */ vk_play, /* 0xfb */ vk_zoom, /* 0xfc */ vk_noname, /* 0xfd */ vk_pa1, /* 0xfe */ vk_oem_clear, /* 0xff */ 0
#endif
};

u8 input_key_to_vk[ 256 ] = {
#if WINDOWS
    /* 0x00 */ 0, /* 0x01 */ VK_LBUTTON, /* 0x02 */ VK_RBUTTON, /* 0x03 */ VK_MBUTTON, /* 0x04 */ VK_XBUTTON1, /* 0x05 */ VK_XBUTTON2, /* 0x06 */ VK_CANCEL, /* 0x07 */ VK_BACK, /* 0x08 */ VK_TAB, /* 0x09 */ VK_CLEAR, /* 0x0a */ VK_RETURN, /* 0x0b */ VK_SHIFT, /* 0x0c */ VK_CONTROL, /* 0x0d */ VK_MENU, /* 0x0e */ VK_PAUSE, /* 0x0f */ VK_CAPITAL,
    /* 0x10 */ VK_KANA, /* 0x11 */ VK_HANGUL, /* 0x12 */ VK_JUNJA, /* 0x13 */ VK_FINAL, /* 0x14 */ VK_HANJA, /* 0x15 */ VK_KANJI, /* 0x16 */ VK_ESCAPE, /* 0x17 */ VK_CONVERT,  /* 0x18 */ VK_NONCONVERT, /* 0x19 */ VK_ACCEPT, /* 0x1a */ VK_MODECHANGE, /* 0x1b */ VK_SPACE, /* 0x1c */ VK_PRIOR, /* 0x1d */ VK_NEXT, /* 0x1e */ VK_END, /* 0x1f */ VK_HOME,
    /* 0x20 */ VK_LEFT, /* 0x21 */ VK_UP, /* 0x22 */ VK_RIGHT, /* 0x23 */ VK_DOWN, /* 0x24 */ VK_SELECT, /* 0x25 */ VK_PRINT, /* 0x26 */ VK_EXECUTE, /* 0x27 */ VK_SNAPSHOT, /* 0x28 */ VK_INSERT, /* 0x29 */ VK_DELETE, /* 0x2a */ VK_HELP, /* 0x2b */ '0', /* 0x2c */ '1', /* 0x2d */ '2', /* 0x2e */ '3', /* 0x2f */ '4',
    /* 0x30 */ '5', /* 0x31 */ '6', /* 0x32 */ '7', /* 0x33 */ '8', /* 0x34 */ '9', /* 0x35 */ 'A', /* 0x36 */ 'B', /* 0x37 */ 'C', /* 0x38 */ 'D', /* 0x39 */ 'E', /* 0x3a */ 'F', /* 0x3b */ 'G', /* 0x3c */ 'H', /* 0x3d */ 'I', /* 0x3e */ 'J', /* 0x3f */ 'K',
    /* 0x40 */ 'L', /* 0x41 */ 'M', /* 0x42 */ 'N', /* 0x43 */ 'O', /* 0x44 */ 'P', /* 0x45 */ 'Q', /* 0x46 */ 'R', /* 0x47 */ 'S', /* 0x48 */ 'T', /* 0x49 */ 'U', /* 0x4a */ 'V', /* 0x4b */ 'W', /* 0x4c */ 'X', /* 0x4d */ 'Y', /* 0x4e */ 'Z', /* 0x4f */ VK_LWIN,
    /* 0x50 */ VK_RWIN, /* 0x51 */ VK_APPS, /* 0x52 */ VK_SLEEP, /* 0x53 */ VK_NUMPAD0, /* 0x54 */ VK_NUMPAD1, /* 0x55 */ VK_NUMPAD2, /* 0x56 */ VK_NUMPAD3, /* 0x57 */ VK_NUMPAD4, /* 0x58 */ VK_NUMPAD5, /* 0x59 */ VK_NUMPAD6, /* 0x5a */ VK_NUMPAD7, /* 0x5b */ VK_NUMPAD8, /* 0x5c */ VK_NUMPAD9, /* 0x5d */ VK_MULTIPLY, /* 0x5e */ VK_DIVIDE, /* 0x5f */ VK_ADD,
    /* 0x60 */ VK_SUBTRACT, /* 0x61 */ VK_SEPARATOR, /* 0x62 */ VK_DECIMAL, /* 0x63 */ VK_F1, /* 0x64 */ VK_F2, /* 0x65 */ VK_F3, /* 0x66 */ VK_F4, /* 0x67 */ VK_F5, /* 0x68 */ VK_F6, /* 0x69 */ VK_F7, /* 0x6a */ VK_F8, /* 0x6b */ VK_F9, /* 0x6c */ VK_F10, /* 0x6d */ VK_F11, /* 0x6e */ VK_F12, /* 0x6f */ VK_F13,
    /* 0x70 */ VK_F14, /* 0x71 */ VK_F15, /* 0x72 */ VK_F16, /* 0x73 */ VK_F17, /* 0x74 */ VK_F18, /* 0x75 */ VK_F19, /* 0x76 */ VK_F20, /* 0x77 */ VK_F21, /* 0x78 */ VK_F22, /* 0x79 */ VK_F23, /* 0x7a */ VK_F24, /* 0x7b */ VK_NUMLOCK, /* 0x7c */ VK_SCROLL, /* 0x7d */ VK_LSHIFT, /* 0x7e */ VK_RSHIFT, /* 0x7f */ VK_LCONTROL,
    /* 0x80 */ VK_RCONTROL, /* 0x81 */ VK_LMENU, /* 0x82 */ VK_RMENU, /* 0x83 */ VK_BROWSER_BACK, /* 0x84 */ VK_BROWSER_FORWARD, /* 0x85 */ VK_BROWSER_REFRESH, /* 0x86 */ VK_BROWSER_STOP, /* 0x87 */ VK_BROWSER_SEARCH, /* 0x88 */ VK_BROWSER_FAVORITES, /* 0x89 */ VK_BROWSER_HOME, /* 0x8a */ VK_VOLUME_MUTE, /* 0x8b */ VK_VOLUME_DOWN, /* 0x8c */ VK_VOLUME_UP, /* 0x8d */ VK_MEDIA_NEXT_TRACK, /* 0x8e */ VK_MEDIA_PREV_TRACK, /* 0x8f */ VK_MEDIA_STOP,
    /* 0x90 */ VK_MEDIA_PLAY_PAUSE, /* 0x91 */ VK_LAUNCH_MAIL, /* 0x92 */ VK_LAUNCH_MEDIA_SELECT, /* 0x93 */ VK_LAUNCH_APP1, /* 0x94 */ VK_LAUNCH_APP2, /* 0x95 */ VK_OEM_PLUS, /* 0x96 */ VK_OEM_MINUS, /* 0x97 */ VK_OEM_COMMA, /* 0x98 */ VK_OEM_PERIOD, /* 0x99 */ VK_OEM_1, /* 0x9a */ VK_OEM_2, /* 0x9b */ VK_OEM_3, /* 0x9c */ VK_OEM_4, /* 0x9d */ VK_OEM_5, /* 0x9e */ VK_OEM_6, /* 0x9f */ VK_OEM_7,
    /* 0xa0 */ VK_OEM_8, /* 0xa1 */ VK_OEM_102, /* 0xa2 */ VK_OEM_CLEAR, /* 0xa3 */ VK_PROCESSKEY, /* 0xa4 */ VK_PACKET, /* 0xa5 */ VK_ATTN, /* 0xa6 */ VK_CRSEL, /* 0xa7 */ VK_EXSEL, /* 0xa8 */ VK_EREOF, /* 0xa9 */ VK_PLAY, /* 0xaa */ VK_ZOOM, /* 0xab */ VK_NONAME, /* 0xac */ VK_PA1,
    0
#endif
};

stu void internal_input_vk_key_check( ) {
#if WINDOWS
#define vk_key_mapping( vk, key ) _assert( input_key_to_vk[ key ] == vk ); _assert( input_vk_to_key[ vk ] == key );
    
    vk_key_mapping( VK_LBUTTON, vk_mouse_left );
    vk_key_mapping( VK_RBUTTON, vk_mouse_right );
    vk_key_mapping( VK_MBUTTON, vk_mouse_midle );
    vk_key_mapping( VK_XBUTTON1, vk_mouse_4 );
    vk_key_mapping( VK_XBUTTON2, vk_mouse_5 );
    vk_key_mapping( VK_CANCEL, vk_cancel );
    vk_key_mapping( VK_BACK, vk_backspace );
    vk_key_mapping( VK_TAB, vk_tab );
    vk_key_mapping( VK_CLEAR, vk_clear );
    vk_key_mapping( VK_RETURN, vk_return );
    vk_key_mapping( VK_SHIFT, vk_shift );
    vk_key_mapping( VK_CONTROL, vk_control );
    vk_key_mapping( VK_MENU, vk_alt );
    vk_key_mapping( VK_PAUSE, vk_pause );
    vk_key_mapping( VK_CAPITAL, vk_caps_lock );
    vk_key_mapping( VK_KANA, vk_kana );
    vk_key_mapping( VK_HANGUL, vk_hangul );
    vk_key_mapping( VK_JUNJA, vk_junja );
    vk_key_mapping( VK_FINAL, vk_final );
    vk_key_mapping( VK_HANJA, vk_hanja );
    vk_key_mapping( VK_KANJI, vk_kanji );
    vk_key_mapping( VK_ESCAPE, vk_escape );
    vk_key_mapping( VK_CONVERT, vk_convert );
    vk_key_mapping( VK_NONCONVERT, vk_non_convert );
    vk_key_mapping( VK_ACCEPT, vk_accept );
    vk_key_mapping( VK_MODECHANGE, vk_mode_change );
    vk_key_mapping( VK_SPACE, vk_space );
    vk_key_mapping( VK_PRIOR, vk_page_up  );
    vk_key_mapping( VK_NEXT, vk_page_down );
    vk_key_mapping( VK_END, vk_end );
    vk_key_mapping( VK_HOME, vk_home );
    vk_key_mapping( VK_LEFT, vk_left );
    vk_key_mapping( VK_UP, vk_up );
    vk_key_mapping( VK_RIGHT, vk_right );
    vk_key_mapping( VK_DOWN, vk_down );
    vk_key_mapping( VK_SELECT, vk_select );
    vk_key_mapping( VK_PRINT, vk_print );
    vk_key_mapping( VK_EXECUTE, vk_execute );
    vk_key_mapping( VK_SNAPSHOT, vk_print_screen );
    vk_key_mapping( VK_INSERT, vk_insert );
    vk_key_mapping( VK_DELETE, vk_delete );
    vk_key_mapping( VK_HELP, vk_help );
    vk_key_mapping( '0', vk_0 );
    vk_key_mapping( '1', vk_1 );
    vk_key_mapping( '2', vk_2 );
    vk_key_mapping( '3', vk_3 );
    vk_key_mapping( '4', vk_4 );
    vk_key_mapping( '5', vk_5 );
    vk_key_mapping( '6', vk_6 );
    vk_key_mapping( '7', vk_7 );
    vk_key_mapping( '8', vk_8 );
    vk_key_mapping( '9', vk_9 );
    vk_key_mapping( 'A', vk_a );
    vk_key_mapping( 'B', vk_b );
    vk_key_mapping( 'C', vk_c );
    vk_key_mapping( 'D', vk_d );
    vk_key_mapping( 'E', vk_e );
    vk_key_mapping( 'F', vk_f );
    vk_key_mapping( 'G', vk_g );
    vk_key_mapping( 'H', vk_h );
    vk_key_mapping( 'I', vk_i );
    vk_key_mapping( 'J', vk_j );
    vk_key_mapping( 'K', vk_k );
    vk_key_mapping( 'L', vk_l );
    vk_key_mapping( 'M', vk_m );
    vk_key_mapping( 'N', vk_n );
    vk_key_mapping( 'O', vk_o );
    vk_key_mapping( 'P', vk_p );
    vk_key_mapping( 'Q', vk_q );
    vk_key_mapping( 'R', vk_r );
    vk_key_mapping( 'S', vk_s );
    vk_key_mapping( 'T', vk_t );
    vk_key_mapping( 'U', vk_u );
    vk_key_mapping( 'V', vk_v );
    vk_key_mapping( 'W', vk_w );
    vk_key_mapping( 'X', vk_x );
    vk_key_mapping( 'Y', vk_y );
    vk_key_mapping( 'Z', vk_z );
    vk_key_mapping( VK_LWIN, vk_left_meta );
    vk_key_mapping( VK_RWIN, vk_right_meta );
    vk_key_mapping( VK_APPS, vk_application );
    vk_key_mapping( VK_SLEEP, vk_sleep );
    vk_key_mapping( VK_NUMPAD0, vk_numpad_0 );
    vk_key_mapping( VK_NUMPAD1, vk_numpad_1 );
    vk_key_mapping( VK_NUMPAD2, vk_numpad_2 );
    vk_key_mapping( VK_NUMPAD3, vk_numpad_3 );
    vk_key_mapping( VK_NUMPAD4, vk_numpad_4 );
    vk_key_mapping( VK_NUMPAD5, vk_numpad_5 );
    vk_key_mapping( VK_NUMPAD6, vk_numpad_6 );
    vk_key_mapping( VK_NUMPAD7, vk_numpad_7 );
    vk_key_mapping( VK_NUMPAD8, vk_numpad_8 );
    vk_key_mapping( VK_NUMPAD9, vk_numpad_9 );
    vk_key_mapping( VK_MULTIPLY, vk_multiply );
    vk_key_mapping( VK_DIVIDE, vk_divide );
    vk_key_mapping( VK_ADD, vk_add );
    vk_key_mapping( VK_SUBTRACT, vk_subtract );
    vk_key_mapping( VK_SEPARATOR, vk_separator );
    vk_key_mapping( VK_DECIMAL, vk_decimal );
    vk_key_mapping( VK_F1, vk_f1 );
    vk_key_mapping( VK_F2, vk_f2 );
    vk_key_mapping( VK_F3, vk_f3 );
    vk_key_mapping( VK_F4, vk_f4 );
    vk_key_mapping( VK_F5, vk_f5 );
    vk_key_mapping( VK_F6, vk_f6 );
    vk_key_mapping( VK_F7, vk_f7 );
    vk_key_mapping( VK_F8, vk_f8 );
    vk_key_mapping( VK_F9, vk_f9 );
    vk_key_mapping( VK_F10, vk_f10 );
    vk_key_mapping( VK_F11, vk_f11 );
    vk_key_mapping( VK_F12, vk_f12 );
    vk_key_mapping( VK_F13, vk_f13 );
    vk_key_mapping( VK_F14, vk_f14 );
    vk_key_mapping( VK_F15, vk_f15 );
    vk_key_mapping( VK_F16, vk_f16 );
    vk_key_mapping( VK_F17, vk_f17 );
    vk_key_mapping( VK_F18, vk_f18 );
    vk_key_mapping( VK_F19, vk_f19 );
    vk_key_mapping( VK_F20, vk_f20 );
    vk_key_mapping( VK_F21, vk_f21 );
    vk_key_mapping( VK_F22, vk_f22 );
    vk_key_mapping( VK_F23, vk_f23 );
    vk_key_mapping( VK_F24, vk_f24 );
    vk_key_mapping( VK_NUMLOCK, vk_num_lock );
    vk_key_mapping( VK_SCROLL, vk_scroll_lock );
    vk_key_mapping( VK_LSHIFT, vk_left_shift );
    vk_key_mapping( VK_RSHIFT, vk_right_shift );
    vk_key_mapping( VK_LCONTROL, vk_left_control );
    vk_key_mapping( VK_RCONTROL, vk_right_control );
    vk_key_mapping( VK_LMENU, vk_left_alt );
    vk_key_mapping( VK_RMENU, vk_right_alt );
    
    vk_key_mapping( VK_BROWSER_BACK, vk_browser_back );
    vk_key_mapping( VK_BROWSER_FORWARD, vk_browser_forward );
    vk_key_mapping( VK_BROWSER_REFRESH, vk_browser_refresh );
    vk_key_mapping( VK_BROWSER_STOP, vk_browser_stop );
    vk_key_mapping( VK_BROWSER_SEARCH, vk_browser_search );
    vk_key_mapping( VK_BROWSER_FAVORITES, vk_browser_favorites );
    vk_key_mapping( VK_BROWSER_HOME, vk_browser_home );
    vk_key_mapping( VK_VOLUME_MUTE, vk_volume_mute );
    vk_key_mapping( VK_VOLUME_DOWN, vk_volume_down );
    vk_key_mapping( VK_VOLUME_UP, vk_volume_up );
    vk_key_mapping( VK_MEDIA_NEXT_TRACK, vk_media_next );
    vk_key_mapping( VK_MEDIA_PREV_TRACK, vk_media_previous );
    vk_key_mapping( VK_MEDIA_STOP, vk_media_stop );
    vk_key_mapping( VK_MEDIA_PLAY_PAUSE, vk_media_play );
    vk_key_mapping( VK_LAUNCH_MAIL, vk_launch_mail );
    vk_key_mapping( VK_LAUNCH_MEDIA_SELECT, vk_launch_media );
    vk_key_mapping( VK_LAUNCH_APP1, vk_launch_application_1 );
    vk_key_mapping( VK_LAUNCH_APP2, vk_launch_application_2 );
    vk_key_mapping( VK_OEM_PLUS, vk_oem_plus );
    vk_key_mapping( VK_OEM_COMMA, vk_oem_comma );
    vk_key_mapping( VK_OEM_MINUS, vk_oem_minus );
    vk_key_mapping( VK_OEM_PERIOD, vk_oem_period );
    vk_key_mapping( VK_OEM_1, vk_oem_1 );
    vk_key_mapping( VK_OEM_2, vk_oem_2 );
    vk_key_mapping( VK_OEM_3, vk_oem_3 );
    vk_key_mapping( VK_OEM_4, vk_oem_4 );
    vk_key_mapping( VK_OEM_5, vk_oem_5 );
    vk_key_mapping( VK_OEM_6, vk_oem_6 );
    vk_key_mapping( VK_OEM_7, vk_oem_7 );
    vk_key_mapping( VK_OEM_8, vk_oem_8 );
    vk_key_mapping( VK_OEM_102, vk_oem_102 );
    vk_key_mapping( VK_OEM_CLEAR, vk_oem_clear );
    vk_key_mapping( VK_PROCESSKEY, vk_process_key );
    vk_key_mapping( VK_PACKET, vk_packet );
    vk_key_mapping( VK_ATTN, vk_attn );
    vk_key_mapping( VK_CRSEL, vk_crsel );
    vk_key_mapping( VK_EXSEL, vk_exsel );
    vk_key_mapping( VK_EREOF, vk_ereof );
    vk_key_mapping( VK_PLAY, vk_play );
    vk_key_mapping( VK_ZOOM, vk_zoom );
    vk_key_mapping( VK_NONAME, vk_noname );
    vk_key_mapping( VK_PA1, vk_pa1 );
    
#undef vk_key_mapping
    
#endif
}

stu umm input_get_scancode_offset( u32 scancode ) {
    
    _assert( scancode );
    
    umm result = scancode;
    umm group_0_end = sc_nonconvert;
    umm group_1_start = sc_media_previous;
    umm group_1_end = sc_launch_media;
    umm group_2_start = sc_pause;
    
    if ( scancode >= group_2_start ) {
        result = group_0_end + 1 + ( group_1_end - group_1_start ) + 1 + ( scancode - group_2_start );
    } else if ( scancode >= group_1_start ) {
        result = group_0_end + 1 + ( scancode - group_1_start );
    }
    
    _assert( result <= 0xff );
    
    return result;
}

enum input_key_state_flag_t {
    
    input_key_state_flag_released = 0,
    input_key_state_flag_pressed = 1 << 0,
    input_key_state_flag_repeated = 1 << 1,
    input_key_state_flag_transition_count_mask = 0xff & ~( input_key_state_flag_pressed | input_key_state_flag_repeated ),
    
} input_key_state_flag_t;

stu u8 input_get_transition_count( window_t* window, u8 key ) {
    u8 result = ( window->keys[ key ] & input_key_state_flag_transition_count_mask ) >> 2;
    return result;
}

stu b32 input_key_is_pressed( window_t* window, u8 key ) {
    b32 result = window->keys[ key ] & input_key_state_flag_pressed;
    return result;
}

stu b32 input_key_just_pressed( window_t* window, u8 key ) {
    b32 result = ( window->keys[ key ] & input_key_state_flag_pressed ) && ( window->keys[ key ] & input_key_state_flag_transition_count_mask );
    return result;
}

stu b32 input_key_just_released( window_t* window, u8 key ) {
    b32 result = !( window->keys[ key ] & input_key_state_flag_pressed ) && ( window->keys[ key ] & input_key_state_flag_transition_count_mask );
    return result;
}

stu b32 input_key_repeated( window_t* window, u8 key ) {
    b32 result = window->keys[ key ] & input_key_state_flag_repeated;
    return result;
}

stu b32 input_key_just_pressed_or_repeated( window_t* window, u8 key ) {
    b32 result = ( window->keys[ key ] & input_key_state_flag_pressed ) && ( window->keys[ key ] & input_key_state_flag_transition_count_mask );
    result = result || window->keys[ key ] & input_key_state_flag_repeated;
    return result;
}

stu b32 input_scancode_is_pressed( window_t* window, u32 scancode ) {
    umm offset = input_get_scancode_offset( scancode );
    b32 result = window->scancodes[ offset ] & input_key_state_flag_pressed;
    return result;
}

stu b32 input_scancode_just_pressed( window_t* window, u32 scancode ) {
    umm offset = input_get_scancode_offset( scancode );
    b32 result = ( window->scancodes[ offset ] & input_key_state_flag_pressed ) && ( window->scancodes[ offset ] & input_key_state_flag_transition_count_mask );
    return result;
}

stu b32 input_scancode_just_released( window_t* window, u32 scancode ) {
    umm offset = input_get_scancode_offset( scancode );
    b32 result = !( window->scancodes[ offset ] & input_key_state_flag_pressed ) && ( window->scancodes[ offset ] & input_key_state_flag_transition_count_mask );
    return result;
}

stu b32 input_scancode_repeated( window_t* window, u32 scancode ) {
    umm offset = input_get_scancode_offset( scancode );
    b32 result = window->scancodes[ offset ] & input_key_state_flag_repeated;
    return result;
}

stu b32 input_scancode_just_pressed_or_repeated( window_t* window, u32 scancode ) {
    umm offset = input_get_scancode_offset( scancode );
    b32 result = ( window->scancodes[ offset ] & input_key_state_flag_pressed ) && ( window->scancodes[ offset ] & input_key_state_flag_transition_count_mask );
    result = result || window->scancodes[ offset ] & input_key_state_flag_repeated;
    return result;
}

stu b32 input_message_key_just_pressed( input_message_t* message, u8 key ) {
    b32 result = ( message->virtual_key == key ) && ( message->state == input_key_state_flag_pressed );
    return result;
}

stu b32 input_message_key_just_released( input_message_t* message, u8 key ) {
    b32 result = ( message->virtual_key == key ) && ( message->state == input_key_state_flag_released );
    return result;
}

stu b32 input_message_key_repeated( input_message_t* message, u8 key ) {
    b32 result = ( message->virtual_key == key ) && ( message->state & input_key_state_flag_repeated );
    return result;
}

stu b32 input_message_key_just_pressed_or_repeated( input_message_t* message, u8 key ) {
    b32 result = ( message->virtual_key == key ) && ( message->state & ( input_key_state_flag_pressed | input_key_state_flag_repeated ) );
    return result;
}

stu b32 input_message_scancode_just_pressed( input_message_t* message, u32 scancode ) {
    b32 result = ( message->scancode == scancode ) && ( message->state == input_key_state_flag_pressed );
    return result;
}

stu b32 input_message_scancode_just_released( input_message_t* message, u32 scancode ) {
    b32 result = ( message->scancode == scancode ) && ( message->state == input_key_state_flag_released );
    return result;
}

stu b32 input_message_scancode_repeated( input_message_t* message, u32 scancode ) {
    b32 result = ( message->scancode == scancode ) && ( message->state & input_key_state_flag_repeated );
    return result;
}

stu b32 input_message_scancode_just_pressed_or_repeated( input_message_t* message, u32 scancode ) {
    b32 result = ( message->scancode == scancode ) && ( message->state & ( input_key_state_flag_pressed | input_key_state_flag_repeated ) );
    return result;
}

typedef struct input_message_iterator_t {
    window_t* window;
    umm index;
    input_message_type_t type;
} input_message_iterator_t;

stu input_message_iterator_t input_message_iterator_make( window_t* window, input_message_type_t type ) {
    input_message_iterator_t result = { 0 };
    result.window = window;
    result.index = umm_max;
    result.type = type;
    return result;
}

stu input_message_t* input_message_iterator_next( input_message_iterator_t* iterator ) {
    
    input_message_t* result = 0;
    
    iterator->index++;
    
    while ( iterator->index < iterator->window->message_count ) {
        
        input_message_t* message = iterator->window->messages + iterator->index;
        
        if ( flag_is_any_set_u32( message->type, iterator->type ) ) {
            result = message;
            break;
        }
        
        iterator->index++;
    }
    
    return result;
}

stu input_message_t* input_message_iterator_get( input_message_iterator_t* iterator ) {
    
    input_message_t* result = 0;
    
    if ( iterator->index < iterator->window->message_count ) {
        result = iterator->window->messages + iterator->index;
    }
    
    return result;
}

stu vec2i window_get_mouse_position( window_t* window ) {
    
    vec2i result = { 0 };
    
    breakable {
        
        POINT p;
        
        if ( !GetCursorPos( &p ) ) {
            break;
        }
        
        if ( !ScreenToClient( window->platform.window_handle, &p ) ) {
            break;
        }
        
        result.x = cast( s32, p.x );
        result.y = cast( s32, window->height ) - p.y - 1;
    }
    
    return result;
}

#if WINDOWS

stu void input_clear_keys( window_t* window ) {
    
    for ( umm key = 0; key < vk_count; key++ ) {
        window->keys[ key ] = ( window->keys[ key ] & input_key_state_flag_pressed ) << 2;
    }
    
    for ( umm scancode = 0; scancode < array_count( window->scancodes ); scancode++ ) {
        window->scancodes[ scancode ] = ( window->scancodes[ scancode ] & input_key_state_flag_pressed ) << 2;
    }
    
    window->codepoint_count = 0;
    window->codepoints_utf8_total_codeunit_count = 0;
    
    vec2i mouse = window_get_mouse_position( window );
    window->mouse_x = mouse.x;
    window->mouse_y = mouse.y;
}
#endif

#ifdef WINDOWS

// MARK windows

stu u32 input_key_to_scan_code( u32 key ) {
    /* TODO simon: make sure this works with all scancodes. */
    u32 result = MapVirtualKeyEx( input_key_to_vk[ key ], MAPVK_VK_TO_VSC_EX, 0 );
    
    if ( key == vk_home || key == vk_end || key == vk_insert || key == vk_delete || key == vk_page_up || key == vk_page_down
        || key == vk_up || key == vk_down || key == vk_left || key == vk_right ) {
        
        result += 0xe000;
    }
    
    return result;
}

stu u32 input_scan_code_to_key( u32 scancode ) {
    
    u32 result;
    
    if ( scancode == 0x45 ){
        scancode = 0xE045;
    } else if ( scancode == 0xE11D45 ) {
        scancode = 0x45;
    }
    
    result = MapVirtualKeyEx( scancode, MAPVK_VSC_TO_VK_EX, 0 );
    result = input_vk_to_key[ result ];
    return result;
}

stu void input_clear_key( window_t* window, u8 key ) {
    
    _assert( !"This hasn't been tested (not even ran) once" );
    window->keys[ key ] = ( window->keys[ key ] & input_key_state_flag_pressed ) << 2;
    u32 scancode = input_key_to_scan_code( key );
    umm offset = input_get_scancode_offset( scancode );
    window->scancodes[ offset ] = ( window->scancodes[ offset ] & input_key_state_flag_pressed ) << 2;
}

stu u32 input_get_scancode_name( u32 scancode, char* buffer, u32 maxLength ) {
    
    u32 result = 0;
    u32 extended = scancode & 0xffff00;
    u32 lParam = 0;
    
    if ( extended ) {
        
        if ( extended == 0xE11D00 ) {
            lParam = 0x45 << 16;
        } else {
            lParam = ( 0x100 | ( scancode & 0xff ) ) << 16;
        }
        
    } else {
        
        lParam = scancode << 16;
        
        if ( scancode == 0x45 ) {
            lParam |= ( 0x1 << 24 );
        }
    }
    
    result = GetKeyNameText( lParam, buffer, maxLength );
    return result;
}

stu void internal_window_set_vk_or_sc_state( u8* key_state, b32 pressed, b32 repeated, umm transition_count ) {
    
    ( *key_state ) = pressed ? input_key_state_flag_pressed : 0;
    ( *key_state ) |= repeated ? input_key_state_flag_repeated : 0;
    ( *key_state ) |= ( ( u8 ) transition_count ) << 2;
}

stu void input_sync_keys( window_t* window ) {
    
    profiler_event_start( input_sync_keys );
    
    vec2i mouse = window_get_mouse_position( window );
    window->mouse_x = mouse.x;
    window->mouse_y = mouse.y;
    
    window->keys[ vk_none ] = 0;
    
    b32 inside_client_rect =
        window->mouse_x >= 0
        && window->mouse_x < ( s32 ) window->width
        && window->mouse_y >= 0
        && window->mouse_y < ( s32 ) window->height;
    
    if ( inside_client_rect ) {
        
        for ( umm index = vk_mouse_left; index < vk_cancel; index++ ) {
            u16 key_state = GetAsyncKeyState( input_key_to_vk[ index ] );
            b32 pressed = ( key_state & ( 0x1 << 15 ) ) > 0;
            internal_window_set_vk_or_sc_state( window->keys + index, pressed, 0, 0 );
        }
    } else {
        
        for ( umm index = vk_mouse_left; index < vk_cancel; index++ ) {
            window->keys[ index ] = 0;
        }
    }
    
    for ( umm index = vk_cancel; index < vk_pa1; index++ ) {
        u16 key_state = GetAsyncKeyState( input_key_to_vk[ index ] );
        b32 pressed = ( key_state & ( 0x1 << 15 ) ) > 0;
        internal_window_set_vk_or_sc_state( window->keys + index, pressed, 0, 0 );
    }
    
    for ( umm index = 0; index < array_count( input_scancode_list ); index++ ) {
        
        u32 scancode = input_scancode_list[ index ];
        umm offset;
        u32 vk;
        u16 key_state;
        
        if ( scancode == 0x45 ) {
            scancode = 0xE045;
        } else if ( scancode == 0xE11D45 ) {
            scancode = 0x45;
        }
        
        offset = input_get_scancode_offset( scancode );
        vk = MapVirtualKeyEx( scancode, MAPVK_VSC_TO_VK_EX, 0 );
        key_state = GetAsyncKeyState( vk );
        
        b32 pressed = ( key_state & ( 0x1 << 15 ) ) > 0;
        internal_window_set_vk_or_sc_state( window->scancodes + offset, pressed, 0, 0 );
    }
    
    window->codepoint_count = 0;
    window->codepoints_utf8_total_codeunit_count = 0;
    profiler_event_end( input_sync_keys );
}

/* NOTE simon: How do I switch a window between normal and fullscreen ?
https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
https://devblogs.microsoft.com/oldnewthing/20050505-04/?p=35703
*/
/* NOTE simon: IsIconic to know if a window is minimized. */

stu LRESULT CALLBACK window_proc( HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam ) {
    
    profiler_event_start( window_proc );
    
    LRESULT result = 0;
    window_t* window = cast( window_t*, GetWindowLongPtrW( window_handle, 0 ) );
    /* TODO simon: when the window style is popup, we get here before the state variable is set. Find why or a solution. */
    
    switch ( message ) {
        
        // NOTE simon: Not caught by PeekMessage.
        
        case WM_DESTROY :
        case WM_CLOSE : {
            if ( window ) {
                window->close_requested = true;
            }
        } break;
        
        // NOTE simon: WM_SIZE and WM_MOVE will not be called if we don't call DefWIndowProc with WM_WINDOWPOSCHANGED. WM_SIZE and  WM_MOVE will be called a startup only.
        /* TODO simon: When resizing the window, we need to redraw from here, as GetMessage will not return. */
        case WM_WINDOWPOSCHANGED :
        case WM_MOVE :
        case WM_SIZE : {
            
            if ( window ) {
                
                RECT client_rect = { 0 };
                GetClientRect( window_handle, &client_rect );
                window->width = client_rect.right;
                window->height = client_rect.bottom;
                window->resized = 2;
                
                POINT p;
                p.x = 0;
                p.y = window->height;
                
                /* NOTE simon: Those are virtual screen coordinates. */
                ClientToScreen( window_handle, &p );
                
                HMONITOR monitor = MonitorFromWindow( window_handle, MONITOR_DEFAULTTOPRIMARY );
                MONITORINFO monitor_info = { .cbSize = sizeof( MONITORINFO ) };
                b32 got_monitor_info = GetMonitorInfoW( monitor, &monitor_info );
                
                window->x = p.x - monitor_info.rcMonitor.left;
                window->y = monitor_info.rcMonitor.bottom - p.y;
            }
            
        } break;
        
        case WM_SYSCOMMAND: {
            
            // NOTE simon: Remove beeping sound when ALT + some key is pressed.
            if ( wParam == SC_KEYMENU ) {
                result = 0;
            } else {
                result = DefWindowProcW( window_handle, message, wParam, lParam );
            }
        } break;
        
        case WM_SETFOCUS: {
            
            if ( window ) {
                /* TODO simon: Generate input_messages ? */
                input_sync_keys( window );
                window->has_focus = true;
            }
            
            result = 0;
            
        } break;
        
        case WM_KILLFOCUS: {
            
            if ( window ) {
                /* TODO simon: Generate input_messages ? */
                input_clear_keys( window );
                window->has_focus = false;
            }
            
            result = 0;
            
        } break;
        
        case WM_SETCURSOR: {
            
            if ( LOWORD( lParam ) == HTCLIENT ) {
                result = 1;
            } else {
                result = DefWindowProcW( window_handle, message, wParam, lParam );
            }
            
        } break;
        
        case WM_NCCALCSIZE: {
            
            if ( !window || !window->fullscreen ) {
                result = DefWindowProcW( window_handle, message, wParam, lParam );
            } else {
                RECT* window_rect = cast( RECT*, lParam );
                _assert( window->fullscreen ) ;
                u32 rect_width = window_rect->right - window_rect->left;
                u32 rect_height = window_rect->bottom - window_rect->top;
                _assert( window->width == rect_width );
                _assert( window->height == rect_height );
            }
            
        } break;
        
        case WM_NCHITTEST: {
            
            if ( !window || !window->fullscreen ) {
                result = DefWindowProcW( window_handle, message, wParam, lParam );
            } else {
                result = HTCLIENT;
            }
            
        } break;
        
        // NOTE simon: End of not caught by PeekMessage.
        
        default : {
            result = DefWindowProcW( window_handle, message, wParam, lParam );
        } break;
    }
    
    profiler_event_end( window_proc );
    
    return result;
}

stu void window_toggle_fullscreen( window_t* window ) {
    
    /* NOTE simon (07/02/24 16:12:07): Don't call IDXGISwapChain_SetFullscreenState because that's for exclusive fullscreen. */
    
    HMONITOR monitor = MonitorFromWindow( window->platform.window_handle, MONITOR_DEFAULTTOPRIMARY );
    MONITORINFO monitor_info = { .cbSize = sizeof( MONITORINFO ) };
    b32 got_monitor_info = GetMonitorInfoW( monitor, &monitor_info );
    
    if ( got_monitor_info ) {
        
        b32 was_fullscreen = window->fullscreen;
        /* NOTE simon: This need to be toggled before SetWindowPos because it need to be correct when WM_NCCALCSIZE is processed. */
        window->fullscreen = !window->fullscreen;
        
        s32 virtual_screen_height = GetSystemMetrics( SM_CYVIRTUALSCREEN );
        
        if ( was_fullscreen ) {
            
            RECT window_rect;
            
            window->x = window->previous_x;
            window->y = window->previous_y;
            window->width = window->previous_width;
            window->height = window->previous_height;
            
            window_rect.left = monitor_info.rcMonitor.left + window->x;
            window_rect.bottom = monitor_info.rcMonitor.bottom - window->y;
            
            window_rect.right = window_rect.left + window->width;
            window_rect.top = window_rect.bottom - window->height;
            
            AdjustWindowRect( &window_rect, WS_OVERLAPPEDWINDOW, 0 );
            
            // SetWindowPos( window->platform.window_handle, 0, window_rect.left, window_rect.top, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, 0 );
            
            WINDOWPLACEMENT placement = { 0 };
            placement.length = sizeof( WINDOWPLACEMENT );
            placement.showCmd = ( window->maximized ) ? SW_MAXIMIZE : SW_NORMAL;
            placement.rcNormalPosition = window_rect;
            
            SetWindowPlacement( window->platform.window_handle, &placement );
            
        } else {
            
            WINDOWPLACEMENT placement = { 0 };
            GetWindowPlacement( window->platform.window_handle, &placement );
            
            if ( placement.showCmd == SW_MAXIMIZE ) {
                window->maximized = true;
            } else {
                window->maximized = false;
                window->previous_x = window->x;
                window->previous_y = window->y;
                window->previous_width = window->width;
                window->previous_height = window->height;
            }
            
            RECT window_rect = monitor_info.rcMonitor;
            
            window->x = 0;
            window->y = 0;
            window->width = window_rect.right - window_rect.left;
            window->height = window_rect.bottom - window_rect.top;
            
            placement.showCmd = SW_NORMAL;
            placement.rcNormalPosition = window_rect;
            
            SetWindowPlacement( window->platform.window_handle, &placement );
            
            // SetWindowPos( window->platform.window_handle, 0, window_rect.left, window_rect.top, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, 0 );
        }
    }
}

typedef enum window_create_flag_t {
    window_create_flag_none,
    window_create_flag_fullscreen = 1 << 0,
    window_create_flag_maximized = 1 << 1,
} window_create_flag_t;

typedef enum window_error_t {
    
    window_error_none,
    
    window_error_window_is_null,
    window_error_register_class_failed,
    
} window_error_t;

stu void window_create( window_t* window, data_t window_name, u32 width, u32 height, s32 x, s32 y, u32 flags, u32* error ) {
    
    profiler_event_start( window_create );
    
    if ( flag_is_set_u32( flags, window_create_flag_fullscreen ) ) {
        flag_clear_u32( &flags, window_create_flag_maximized );
    }
    
    breakable {
        
        breakable_check( error );
        
        if ( !window ) {
            log_l( "[ERROR] Window pointer is null.\n" );
            set_error( error, window_error_window_is_null );
            break;
        }
        
        window->platform.executable_instance = GetModuleHandleW( 0 );
        window->platform.cursor_arrow = LoadCursorW( 0, cast( LPCWSTR, IDC_ARROW ) );
        window->platform.cursor_hand = LoadCursorW( 0, cast( LPCWSTR, IDC_HAND ) );
        window->platform.cursor_text = LoadCursorW( 0, cast( LPCWSTR, IDC_IBEAM ) );
        window->platform.cursor_size_ns = LoadCursorW( 0, cast( LPCWSTR, IDC_SIZENS ) );
        window->platform.cursor_size_we = LoadCursorW( 0, cast( LPCWSTR, IDC_SIZEWE ) );
        
        window->double_click_delay_ms = GetDoubleClickTime( );
        /* NOTE simon: if the retrieved value is WHEEL_PAGESCROLL (u32_max) we should scroll a page instead of a number of line. */
        SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &window->mouse_wheel_line_scroll_count, 0 );
        /* NOTE simon: SystemParametersInfo( SPI_GETCARETWIDTH, 0, 0, 0 ); Get the size of text cursor. */
        window->cursor_blink_delay_ms = GetCaretBlinkTime( );
        
        if ( window->cursor_blink_delay_ms == INFINITE ) {
            window->cursor_blink_delay_ms = u32_max;
        }
        
        /* NOTE simon: Double click rectangle, default to 4*4. Does it mean 2 on each side ? */
        window->double_click_width = GetSystemMetrics( SM_CXDOUBLECLK );
        window->double_click_height = GetSystemMetrics( SM_CYDOUBLECLK );
        
        WNDCLASSW window_class = { 0 };
        window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        window_class.lpfnWndProc = window_proc;
        window_class.hInstance = window->platform.executable_instance;
        window_class.lpszClassName = L"simple_window";
        window_class.hCursor = window->platform.cursor_arrow;
        
#if defined( application_icon )
        window_class.hIcon = LoadIconW( window_class.hInstance, ( LPCWSTR ) application_icon );
#endif
        
        window_class.cbWndExtra = sizeof( window_t* );
        
        memory_get_on_stack( window_name_utf16, 2048 );
        utf8_to_utf16( window_name, &window_name_utf16, &g_ucs_error );
        memory_push_u16( &window_name_utf16, 0, 1 );
        
        ATOM atom = RegisterClassW( &window_class );
        
        if ( !atom ) {
            log_l( "[ERROR] RegisterClass failed.\n" );
            set_error( error, window_error_register_class_failed );
            break;
        }
        
        u32 window_style = WS_OVERLAPPEDWINDOW;
        /* NOTE simon: add WS_EX_TOPMOST as the first argument to have always on top window. */
        u32 ex_style = 0;
        
        HWND window_handle = CreateWindowExW( ex_style, window_class.lpszClassName, cast( LPCWSTR, window_name_utf16.bytes ), window_style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, window_class.hInstance, 0
                                             );
        
        SetWindowLongPtrW( window_handle, 0, cast( LONG_PTR, window ) );
        
        HMONITOR monitor = MonitorFromWindow( window_handle, MONITOR_DEFAULTTOPRIMARY );
        MONITORINFO monitor_info = { .cbSize = sizeof( MONITORINFO ) };
        b32 got_monitor_info = GetMonitorInfoW( monitor, &monitor_info );
        s32 virtual_screen_height = GetSystemMetrics( SM_CYVIRTUALSCREEN );
        
        s32 monitor_left = monitor_info.rcMonitor.left;
        s32 monitor_right = monitor_info.rcMonitor.right;
        s32 monitor_bottom = monitor_info.rcMonitor.bottom;
        s32 monitor_top = monitor_info.rcMonitor.top;
        
        s32 monitor_width = monitor_right - monitor_left;
        s32 monitor_height = monitor_bottom - monitor_top;
        
        if ( x == s32_max ) {
            x = ( monitor_width - width ) / 2;
        }
        
        if ( y == s32_max ) {
            y = ( monitor_height - height ) / 2;
        }
        
        window->previous_x = x;
        window->previous_y = y;
        window->previous_width = width;
        window->previous_height = height;
        
        RECT window_rect = { 0 };
        
        if ( flag_is_set_u32( flags, window_create_flag_fullscreen ) ) {
            
            window->fullscreen = true;
            window->x = 0;
            window->y = 0;
            window->width = monitor_width;
            window->height = monitor_height;
            
            window_rect.left = monitor_left;
            window_rect.right = monitor_right;
            window_rect.top = monitor_top;
            window_rect.bottom = monitor_bottom;
            
        } else {
            
            window_rect.left = monitor_left + x;
            window_rect.bottom = monitor_bottom - y;
            
            window_rect.right = window_rect.left + width;
            window_rect.top = window_rect.bottom - height;
            
            AdjustWindowRect( &window_rect, window_style, 0 );
            
            /* NOTE simon: window contains the client rect information.
                    Do not use window_rect here since it contains window rect information. */
            window->fullscreen = false;
            window->x = x;
            window->y = y;
            window->width = width;
            window->height = height;
        }
        
        window->platform.window_handle = window_handle;
        window->close_requested = false;
        window->running = true;
        
        SetWindowPos( window_handle, 0, window_rect.left, window_rect.top, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, 0 /*SWP_NOMOVE */ );
        
        int show_window_flag = SW_SHOW;
        
        if ( flag_is_set_u32( flags, window_create_flag_maximized ) ) {
            show_window_flag = SW_MAXIMIZE;
        }
        
        ShowWindow( window_handle, show_window_flag );
        
#if defined( GAMEPAD_H ) && defined( GAMEPAD_PLAYSTATION_RAW_INPUT )
        
        if ( !ps_init( window_handle ) ) {
            log_l( "[WARNING][GAMEPAD] Failed to initialize PlayStation raw input.\n" );
        }
#endif
        
        RAWINPUTDEVICE raw_input_devices[ ] = {
#if 1
            {
                /* NOTE simon: keyboard. */
                .usUsagePage = 0x01,
                .usUsage = 0x06,
                .dwFlags = 0, /* RIDEV_NOLEGACY;   // adds HID keyboard and also ignores legacy keyboard messages */
                /* RIDEV_NOHOTKEYS: disable windows logo key hotkeys */
                .hwndTarget = 0,
            },
#endif
#if 0
            /* NOTE simon: this was previously used to capture mouse movement when the mouse is outside the window.
            I use SetCapture and RleaseCapture instead now.*/
            {
                /* NOTE simon: mouse. */
                .usUsagePage = 0x01,
                .usUsage = 0x02,
                .dwFlags = 0,
                .hwndTarget = 0,
            }
#endif
        };
        
        b32 registered = RegisterRawInputDevices( raw_input_devices, array_count( raw_input_devices ), sizeof( raw_input_devices[ 0 ] ) );
        
        if ( !registered ) {
            log_l( "[WARNING] Failed to register raw input devices.\n" );
        }
    }
    
    log_to_file( string_l( "log.txt" ) );
    
    profiler_event_end( window_create );
}

stu void window_set_top_most( window_t* window, b32 top_most ) {
    
    HWND position = HWND_TOPMOST;
    
    if ( !top_most ) {
        position = HWND_NOTOPMOST;
    }
    
    SetWindowPos( window->platform.window_handle, position, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE );
}

stu b32 window_has_timer( window_t* window ) {
    b32 result = ( window->platform.timer_id != 0 );
    return result;
}

stu void window_timer_start( window_t* window, u32 interval_ms ) {
    _assert( window->platform.timer_id == 0 );
    window->platform.timer_id = SetTimer( 0, 0, interval_ms, 0 );
    /* debug_string_timestamp_l( "Timer start.\n" ); */
}

stu void window_timer_end( window_t* window ) {
    _assert( window->platform.timer_id != 0 );
    KillTimer( 0, window->platform.timer_id );
    window->platform.timer_id = 0;
    /* debug_timestamp_l( "Timer end.\n" ); */
}

typedef enum window_handle_messages_flag_t {
    whm_none = 0,
    whm_wait_for_message = 1 << 0,
    whm_wait_skip_mouse_move = 1 << 1,
} window_handle_messages_flag_t;

stu void internal_input_message_update_modifiers( input_message_t* message ) {
    
    if ( GetKeyState( VK_LCONTROL ) < 0 ) {
        message->modifiers |= input_message_modifier_control_left;
    }
    
    if ( GetKeyState( VK_RCONTROL ) < 0 ) {
        message->modifiers |= input_message_modifier_control_right;
    }
    
    if ( GetKeyState( VK_LMENU ) < 0 ) {
        message->modifiers |= input_message_modifier_alt_left;
    }
    
    if ( GetKeyState( VK_RMENU ) < 0 ) {
        message->modifiers |= input_message_modifier_alt_right;
    }
    
    if ( GetKeyState( VK_LSHIFT ) < 0 ) {
        message->modifiers |= input_message_modifier_shift_left;
    }
    
    if ( GetKeyState( VK_RSHIFT ) < 0 ) {
        message->modifiers |= input_message_modifier_shift_right;
    }
    
    if ( GetKeyState( VK_LWIN ) < 0 ) {
        message->modifiers |= input_message_modifier_meta_left;
    }
    
    if ( GetKeyState( VK_RWIN ) < 0 ) {
        message->modifiers |= input_message_modifier_meta_right;
    }
}

stu void internal_input_message_print( data_t* out_buffer, input_message_t* message ) {
    
    if ( message->type == input_message_type_keyboard ) {
        memory_push_copy_l( out_buffer, "keyboard\n" );
    } else if ( message->type == input_message_type_mouse ) {
        memory_push_copy_l( out_buffer, "mouse\n" );
    } else if ( message->type == input_message_type_mouse_move ) {
        memory_push_copy_l( out_buffer, "mouse move\n" );
    } else {
        _assert( !"No type ?" );
    }
    
    memory_push_copy_l( out_buffer, "vk: " );
    string_push_u64( out_buffer, message->virtual_key, 0, 0, string_format_flag_hexadecimal ); 
    memory_push_copy_l( out_buffer, "\nsc: " );
    string_push_u64( out_buffer, message->scancode, 0, 0, string_format_flag_hexadecimal );
    
    memory_push_copy_l( out_buffer, "\nstate: " );
    
    if ( message->state == input_key_state_flag_released ) {
        memory_push_copy_l( out_buffer, "released" );
    } else {
        
        if ( message->state & input_key_state_flag_pressed ) {
            memory_push_copy_l( out_buffer, "pressed " );
        }
        
        if ( message->state & input_key_state_flag_repeated ) {
            memory_push_copy_l( out_buffer, "repeated" );
        }
    }
    
    if ( message->type == input_message_type_mouse || message->type == input_message_type_mouse_move ) {
        
        memory_push_copy_l( out_buffer, "\nMouse (x, y): " );
        string_push_s64( out_buffer, message->mouse_x, 0, 0, 0 );
        memory_push_copy_l( out_buffer, ", " );
        string_push_s64( out_buffer, message->mouse_y, 0, 0, 0 );
        memory_push_copy_l( out_buffer, "\n" );
        
        memory_push_copy_l( out_buffer, "\nMouse scroll(x, y): " );
        string_push_f64( out_buffer, message->scroll_x, 0, 0, 0, 0  );
        memory_push_copy_l( out_buffer, ", " );
        string_push_f64( out_buffer, message->scroll_y, 0, 0, 0, 0 );
        memory_push_copy_l( out_buffer, "\n" );
    }
    
    memory_push_copy_l( out_buffer, "\nModifiers:\n" );
    
    if ( message->modifiers & input_message_modifier_control_left ) {
        memory_push_copy_l( out_buffer, "control left\n" );
    }
    
    if ( message->modifiers & input_message_modifier_control_right ) {
        memory_push_copy_l( out_buffer, "control right\n" );
    }
    
    if ( message->modifiers & input_message_modifier_alt_left ) {
        memory_push_copy_l( out_buffer, "alt left\n" );
    }
    
    if ( message->modifiers & input_message_modifier_alt_right ) {
        memory_push_copy_l( out_buffer, "alt right\n" );
    }
    
    if ( message->modifiers & input_message_modifier_shift_left ) {
        memory_push_copy_l( out_buffer, "shift left\n" );
    }
    
    if ( message->modifiers & input_message_modifier_shift_right ) {
        memory_push_copy_l( out_buffer, "shift right\n" );
    }
    
    if ( message->modifiers & input_message_modifier_meta_left ) {
        memory_push_copy_l( out_buffer, "meta left\n" );
    }
    
    if ( message->modifiers & input_message_modifier_meta_right ) {
        memory_push_copy_l( out_buffer, "meta right\n" );
    }
    
    if ( message->codeunit_count ) {
        memory_push_copy_l( out_buffer, "codepoint: " );
        string_push_u64( out_buffer, message->codepoint, 0, 0, string_format_flag_hexadecimal );
        memory_push_copy_l( out_buffer, "\nutf8: " );
        memory_push_copy_p( out_buffer, message->codepoint_utf8, message->codeunit_count, 1 );
    }
    
    memory_push_copy_l( out_buffer, "\n\n" );
    
    string_zero_terminate( *out_buffer );
}

/* NOTE simon: SetCapture and ReleaseCapture explanation here:
https://docs.microsoft.com/en-us/windows/win32/learnwin32/mouse-movement
It allows to receive mouse messages when the mouse goes outside the window and there was a button pressed. */
stu void internal_window_mouse_button_down( window_t* window, u8 virtual_key, MSG message ) {
    
    u8 value = input_get_transition_count( window, virtual_key );
    internal_window_set_vk_or_sc_state( window->keys + virtual_key, 1, 0, value + 1 );
    SetCapture( window->platform.window_handle );
    
    window->mouse_x = ( ( s32 ) ( s16 ) ( message.lParam & 0xffff ) );
    window->mouse_y = window->height - ( ( s32 ) ( s16 ) ( ( message.lParam >> 16 ) & 0xffff ) ) - 1;
    
    input_message_t* input_message = window->messages + window->message_count;
    _assert( window->message_count < array_count( window->messages ) );
    memory_zero_struct( input_message );
    
    input_message->type = input_message_type_mouse;
    input_message->virtual_key = virtual_key;
    input_message->state = input_key_state_flag_pressed;
    
    internal_input_message_update_modifiers( input_message );
    
    input_message->mouse_x = window->mouse_x;
    input_message->mouse_y = window->mouse_y;
    input_message->timestamp_ms = ( u32 ) message.time;
    
    window->message_count++;
}

stu void internal_window_mouse_button_up( window_t* window, u8 virtual_key, MSG message ) {
    
    u8 value = input_get_transition_count( window, virtual_key );
    internal_window_set_vk_or_sc_state( window->keys + virtual_key, 0, 0, value + 1 );
    ReleaseCapture( );
    
    window->mouse_x = ( ( s32 ) ( s16 ) ( message.lParam & 0xffff ) );
    window->mouse_y = window->height - ( ( s32 ) ( s16 ) ( ( message.lParam >> 16 ) & 0xffff ) ) - 1;
    
    input_message_t* input_message = window->messages + window->message_count;
    _assert( window->message_count < array_count( window->messages ) );
    memory_zero_struct( input_message );
    
    input_message->type = input_message_type_mouse;
    input_message->virtual_key = virtual_key;
    input_message->state = input_key_state_flag_released;
    
    internal_input_message_update_modifiers( input_message );
    
    input_message->mouse_x = window->mouse_x;
    input_message->mouse_y = window->mouse_y;
    input_message->timestamp_ms = ( u32 ) message.time;
    
    window->message_count++;
}

/* NOTE simon: After looking why my menu seems a little tiny bit laggy, I recorded Windows 7 file explorer.
It was obvious that they predict mouse movement for highlighting items. For example if I move the mouse
over the button under the location bar in a quick movement, the recording clearly showed that the buttons
were highlighted before the mouse got over them. Thats is I think why they feel more reactive than my
system, which highlight them with a frame of lag.

I did a simila recording on Windows 10 and there, the UI just lags like mine. */

stu void window_handle_messages( window_t* window, u32 whm_flags ) {
    
    profiler_event_start( window_handle_messages );
    
    profiler_event_start( setup );
    /* TODO simon (25/01/24 13:04:35): When I call toggle fullscreen I think this resets the flag to false before the user code cand handle it. */
    window->resized = ( window->resized == 2 ) ? 1 : 0 ;
    window->codepoint_count = 0;
    window->codepoints_utf8_total_codeunit_count = 0;
    window->scroll_x = 0;
    window->scroll_y = 0;
    
    memory_zero_array( window->messages, input_message_t, array_count( window->messages ) );
    window->message_count = 0;
    
    for ( umm key = 0; key < vk_count; key++ ) {
        window->keys[ key ] &= input_key_state_flag_pressed;
    }
    
    for ( umm scancode = 0; scancode < array_count( window->scancodes ); scancode++ ) {
        window->scancodes[ scancode ] &= input_key_state_flag_pressed;
    }
    
    b32 pause_scancode_read = false;
    
    MSG message;
    b32 has_message = false;
    
    profiler_event_end( setup );
    
    if ( whm_flags & whm_wait_for_message ) {
        
        profiler_event_start( GetMessage );
        
        has_message = GetMessageW( &message, 0, 0, 0 );
        
        while ( ( whm_flags & whm_wait_skip_mouse_move ) && ( message.message == WM_MOUSEMOVE ) ) {
            TranslateMessage( &message );
            DispatchMessageW( &message );
            has_message = GetMessageW( &message, 0, 0, 0 );
        }
        
        profiler_event_end( GetMessage );
        
    } else {
        profiler_event_start( PeekMessage );
        has_message = PeekMessageW( &message, 0, 0, 0, PM_REMOVE );
        profiler_event_end( PeekMessage );
    }
    
    profiler_event_start( loop );
    
    vec2i mouse = window_get_mouse_position( window );
    window->mouse_x = mouse.x;
    window->mouse_y = mouse.y;
    
    while ( has_message ) {
        
        switch ( message.message ) {
            
            case WM_MOUSEMOVE: {
                
                window->mouse_x = ( ( s32 ) ( s16 ) ( message.lParam & 0xffff ) );
                window->mouse_y = window->height - ( ( s32 ) ( s16 ) ( ( message.lParam >> 16 ) & 0xffff ) ) - 1;
                
#if 0
                log_l( "mouse_move: " );
                log_u64( message.time );
                log_l( "\n   x: " );
                log_s64( window->mouse_x );
                log_l( "\n   y: " );
                log_s64( window->mouse_y );
                log_l( "\n" );
#endif
                
                /* NOTE simon: I'm not sure why, but sometimes I get a lot of mouse move message.
They are several repeats of a similar message e.g. 5 message with same x and y, than 8 message with similar x and y... in a single frame.
So I decided to collapse successive mouse move message even if the coordinates aren't the same. */
                input_message_t* previous_message = 0;
                
                if ( window->message_count ) {
                    previous_message = window->messages + ( window->message_count - 1 );
                }
                
                input_message_t* input_message = 0;
                
                if ( previous_message && previous_message->type == input_message_type_mouse_move ) {
                    input_message = previous_message;
                } else if ( window->message_count < array_count( window->messages ) ) {
                    input_message = window->messages + window->message_count;
                    window->message_count++;
                } else {
                    log_l( "window_handle_messages: dropped mouse move message.\n" );
                }
                
                if ( input_message ) {
                    memory_zero_struct( input_message );
                    
                    input_message->type = input_message_type_mouse_move;
                    internal_input_message_update_modifiers( input_message );
                    
                    input_message->mouse_x = window->mouse_x;
                    input_message->mouse_y = window->mouse_y;
                    input_message->timestamp_ms = ( u32 ) message.time;
                }
                
            } break;
            
            /* NOTE simon: WM_QUIT only happens if PostQuitMessage is called. */
            case WM_QUIT : {
                window->close_requested = true;
            } break;
            
            
            
            case WM_LBUTTONDOWN: {
                internal_window_mouse_button_down( window, vk_mouse_left, message );
            } break;
            
            case WM_LBUTTONUP: {
                internal_window_mouse_button_up( window, vk_mouse_left, message );
            } break;
            
            
            
            case WM_RBUTTONDOWN: {
                internal_window_mouse_button_down( window, vk_mouse_right, message );
            } break;
            
            case WM_RBUTTONUP: {
                internal_window_mouse_button_up( window, vk_mouse_right, message );
            } break;
            
            
            
            case WM_MBUTTONDOWN: {
                internal_window_mouse_button_down( window, vk_mouse_midle, message );
            } break;
            
            case WM_MBUTTONUP: {
                internal_window_mouse_button_up( window, vk_mouse_midle, message );
            } break;
            
            
            
            case WM_XBUTTONDOWN: {
                u8 virtual_key = ( u8 ) ( vk_mouse_midle + HIWORD( message.wParam ) );
                internal_window_mouse_button_down( window, virtual_key, message );
            } break;
            
            case WM_XBUTTONUP: {
                u8 virtual_key = ( u8 ) ( vk_mouse_midle + HIWORD( message.wParam ) );
                internal_window_mouse_button_up( window, virtual_key, message );
            } break;
            
            
            
            case WM_SYSKEYDOWN :
            case WM_KEYDOWN :
            case WM_SYSKEYUP :
            case WM_KEYUP: {
                
#if 1
                /* TODO simon: remove this when it's clear that this is always true. */
                u32 time = cast( u32, GetMessageTime( ) );
                _assert( message.time == time );
#endif
                /* TODO simon: Should I just ditch this and only use raw input ? */
                u32 key = input_vk_to_key[ message.wParam ];
                
                u8 pressed = ( u8 ) ( ( ~message.lParam ) >> 31 ) & 0x1;
                u8 repeated = ( ( window->keys[ key ] & input_key_state_flag_pressed ) == input_key_state_flag_pressed ) && pressed;
                u8 transition = ( window->keys[ key ] & input_key_state_flag_pressed ) != pressed;
                u8 value = window->keys[ key ] & input_key_state_flag_transition_count_mask;
                
                b32 extended = ( message.lParam >> 24 ) & 0x1;
                u32 scancode = ( message.lParam >> 16 ) & 0xff;
                
                if ( extended ) {
                    
                    /* Num lock is extended in the lparam but not in the scancode list. */
                    if ( scancode != 0x45 ) {
                        scancode |= 0xE000;
                    }
                    
                } else {
                    
                    /* And Pause in not extended when it should be 0xE11D45. */
                    if ( scancode == 0x45 ) {
                        scancode = 0xE11D45;
                    } else if ( scancode == 0x54 ) { /* ALT + print screen */
                        scancode = 0xE037;
                    }
                }
                
                /* We need to force the transition because print screen doesn't sends keydown messages. */
                if ( scancode == 0xE037 ) {
                    transition = 1;
                }
                
                value += ( transition << 2 );
                value |= pressed;
                value |= ( repeated << 1 );
                window->keys[ key ] = value;
                
                /* NOTE simon: scancodes are set with raw input. */
                /* window->scancodes[ window_get_scancode_offset( scancode ) ] = value; */
                
                if ( message.wParam == VK_SHIFT ) {
                    
                    key = input_vk_to_key[ ( scancode == sc_shift_left ) ? VK_LSHIFT : VK_RSHIFT ];
                    window->keys[ key ] = value;
                    
                } else if ( message.wParam == VK_CONTROL ) {
                    
                    key = input_vk_to_key[ ( extended ) ? VK_RCONTROL : VK_LCONTROL ];
                    window->keys[ key ] = value;
                    
                } else if ( message.wParam == VK_MENU ) {
                    
                    key = input_vk_to_key[ ( extended ) ? VK_RMENU : VK_LMENU ];
                    window->keys[ key ] = value;
                }
                
#if 0
                memory_get_on_stack( test, kibioctet( 1 ) );
                memory_push_copy_l( &test, "vk: " );
                string_push_u64( &test, scancode, 0, 0, string_format_flag_hexadecimal );
                memory_push_u8( &test, '\n' );
                string_zero_terminate( test );
                OutputDebugStringA( ( LPCSTR ) test.bytes );
#endif
                
                if ( message.message == WM_SYSKEYDOWN && message.wParam == VK_F4 ) {
                    window->close_requested = true;
                }
                
                /* NOTE simon: Input messages are handle in here because there are subtle
differences between this and raw input (as well as differences between different versions of windows)
when using Alt Gr (different scancodes) and the behavior of the virtual key is
what I decided to use. */
                /* NOTE simon: when switching from a layout with Alt Gr key to one without it on windows 10,
a left control up message is sent without a corresponding WM_INPUT message before (or after). */
                /* NOTE simon: on windows 7, using the Alt Gr key sends a "left control" first without the corresponding
WM_INPUT message before (or after). */
                
                _assert( window->message_count < array_count( window->messages ) );
                input_message_t* input_message = window->messages + window->message_count;
                memory_zero_struct( input_message );
                window->message_count++;
                
                input_message->type = input_message_type_keyboard;
                input_message->virtual_key = ( u8 ) key;
                input_message->scancode = scancode;
                input_message->state = ( pressed | ( repeated << 1 ) );
                input_message->timestamp_ms = ( u32 ) message.time;
                
                internal_input_message_update_modifiers( input_message );
                
                TranslateMessage( &message );
                
            } break;
            
            case WM_INPUT: {
                
#if 0
                log_l( "input: " );
                log_u64( message.time );
                log_l( "\n" );
#endif
                /* NOTE simon: I doesn't seem necessary to DispatchMessageW for GameInput or XInput to work. */
                
#if defined( GAMEPAD_PLAYSTATION_RAW_INPUT )
                if ( !ps_input( message.lParam ) ) {
#endif
                    
                    /* NOTE simon: Raw inputs are processed before virtual key messages. */
                    RAWINPUT raw_input;
                    u32 raw_input_size = sizeof( raw_input );
                    GetRawInputData( ( HRAWINPUT ) message.lParam, RID_INPUT, &raw_input, &raw_input_size, sizeof( RAWINPUTHEADER ) );
                    
                    if ( raw_input.header.dwType == RIM_TYPEKEYBOARD ) {
                        
                        b32 ignore = false;
                        u32 scancode = raw_input.data.keyboard.MakeCode;
                        u16 flags = raw_input.data.keyboard.Flags;
                        
                        if ( scancode ) {
                            
                            _assert( scancode <= 0xff );
                            
                            if ( ( flags & RI_KEY_E0 ) != 0 ) {
                                scancode |= 0xE000;
                            } else if ( ( flags & RI_KEY_E1 ) != 0 ) {
                                scancode |= 0xE100;
                            }
                            
                            if ( pause_scancode_read ) {
                                
                                if ( scancode == 0x45 ) {
                                    scancode = 0xE11D45;
                                }
                                
                                pause_scancode_read = false;
                                
                            } else if ( scancode == 0xE11D ) {
                                
                                pause_scancode_read = true;
                                
                            } else if ( scancode == 0x54 ) {
                                /* 0x54 is generated when print screen and one or both alt are pressed. */
                                scancode = 0xE037;
                            }
                            
                            if ( scancode == 0xE11D || scancode == 0xE02A || scancode == 0xE0AA || scancode == 0xE0B6 || scancode == 0xE036 ) {
                                ignore = true;
                            }
                            
                        } else {
                            /* NOTE simon: when switching from a virtual machine to your program,
                            a numlock key press can be sent to your application with a scancode of 0.
                            Speculation: since it doesn't come from a hardware keypress, there is no
                            scancode in the raw input struct.
                            Hard to reproduce but quickly switching between the vm and the application
                            using the mouse on the taskbar icon seem to be best way to reproduce. */
                            
                            /* NOTE simon: There is another way for a "no scancode" message to arrive.
    Some times message with reserved VKey can arrive. The range 0xc1 to 0xd7
    (both value included). It happened once when I clicked in the window, so I guess
    it could be related to mouse input (not much information though).
    This page contains the list of reserved VKey.
    https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    */
                            
                            u16 vk = raw_input.data.keyboard.VKey; 
                            
                            if ( vk == 0x0a || vk == 0x0b || vk == 0x5e || vk == 0xb8 || vk == 0xb9 || ( vk >= 0xc1 && vk <= 0xd7 ) || vk == 0xe0 || vk == 0xfc ) {
                                ignore = true;
                            } else {
                                
                                u32 key = input_vk_to_key[ raw_input.data.keyboard.VKey ];
                                scancode = input_key_to_scan_code( key );
                                _assert( scancode && scancode < 0xe11d45 );
                            }
                        }
                        
                        u8 pressed = 0;
                        
                        if ( ( flags & RI_KEY_BREAK ) == 0 ) {
                            pressed = 1;
                        }
                        
                        if ( !ignore ) {
                            
#if 0
                            memory_get_on_stack( test, kibioctet( 1 ) );
                            memory_push_copy_l( &test, "sc: " );
                            string_push_u64( &test, scancode, 0, 0, string_format_flag_hexadecimal );
                            memory_push_u8( &test, '\n' );
                            string_zero_terminate( test );
                            OutputDebugStringA( ( LPCSTR ) test.bytes );
#endif
                            
                            umm offset = input_get_scancode_offset( scancode );
                            u8 value = window->scancodes[ offset ] & input_key_state_flag_transition_count_mask;
                            u8 repeated = ( ( window->scancodes[ offset ] & input_key_state_flag_pressed ) == input_key_state_flag_pressed ) && pressed;
                            u8 transition = ( window->scancodes[ offset ] & input_key_state_flag_pressed ) != pressed;
                            value += ( transition << 2 );
                            value |= pressed;
                            value |= ( repeated << 1 );
                            window->scancodes[ offset ] = value;
                        }
                        
                    } else if ( raw_input.header.dwType == RIM_TYPEMOUSE ) {
                        /* NOTE simon: we use raw input to get mouse movement messages when the mouse is outside the window and the window has the focus. */
                    } else {
                        /* NOTE simon: RIM_TYPEHID */
                        TranslateMessage( &message );
                        DispatchMessageW( &message );
                    }
                    
#if defined( GAMEPAD_PLAYSTATION_RAW_INPUT )
                } else {
                    /* NOTE simon: PlayStation controller has been processed. */
                }
#endif
                
            } break;
            
            /* NOTE simon: WM_MOUSEHWHEEL and WM_MOUSEWHEEL can be recieved when the mouse is outside the client rect (in the window shadow ?).
The position in lParam is in screen space instead of client space like in the mouse down message.
*/
            
            case WM_MOUSEHWHEEL: {
                
                s16 delta = GET_WHEEL_DELTA_WPARAM( message.wParam );
                window->scroll_x = delta / ( f32 ) WHEEL_DELTA;
                
                input_message_t* input_message = window->messages + window->message_count;
                _assert( window->message_count < array_count( window->messages ) );
                memory_zero_struct( input_message );
                
                input_message->type = input_message_type_mouse;
                internal_input_message_update_modifiers( input_message );
                
                POINT p;
                p.x = ( s16 ) ( message.lParam & 0xffff );
                p.y = ( s16 ) ( ( message.lParam >> 16 ) & 0xffff );
                
                if ( ScreenToClient( window->platform.window_handle, &p ) ) {
                    input_message->mouse_x = p.x;
                    input_message->mouse_y = cast( s32, window->height ) - p.y - 1;
                } else {
                    input_message->mouse_x = 0;
                    input_message->mouse_y = 0;
                }
                
                input_message->scroll_x = window->scroll_x;
                input_message->scroll_y = 0;
                
                input_message->timestamp_ms = ( u32 ) message.time;
                
                window->message_count++;
                
            } break;
            
            case WM_MOUSEWHEEL: {
                
                s16 delta = GET_WHEEL_DELTA_WPARAM( message.wParam );
                window->scroll_y =  delta / ( f32 ) WHEEL_DELTA;
                
                input_message_t* input_message = window->messages + window->message_count;
                _assert( window->message_count < array_count( window->messages ) );
                memory_zero_struct( input_message );
                
                input_message->type = input_message_type_mouse;
                internal_input_message_update_modifiers( input_message );
                
                POINT p;
                p.x = ( s16 ) ( message.lParam & 0xffff );
                p.y = ( s16 ) ( ( message.lParam >> 16 ) & 0xffff );
                
                if ( ScreenToClient( window->platform.window_handle, &p ) ) {
                    input_message->mouse_x = p.x;
                    input_message->mouse_y = cast( s32, window->height ) - p.y - 1;
                } else {
                    input_message->mouse_x = 0;
                    input_message->mouse_y = 0;
                }
                
                input_message->scroll_x = 0;
                input_message->scroll_y = window->scroll_y;
                
                input_message->timestamp_ms = ( u32 ) message.time;
                
                window->message_count++;
                
            } break;
            
            case WM_CHAR: {
                
                u32 c = ( u32 ) message.wParam;
                
                /* NOTE simon: Decode utf16. */
                u16 hi_surrogate_start = 0xD800;
                u16 lo_surrogate_start = 0xDC00;
                u16 surrogate_end = 0xDFFF;
                
                static u16 high_surrogate = 0;
                
                if ( c >= hi_surrogate_start && c < lo_surrogate_start ) {
                    
                    high_surrogate = ( u16 ) c;
                    
                } else {
                    
                    if ( c >= lo_surrogate_start && c <= surrogate_end ) {
                        
                        u16 low_surrogate = ( u16 ) c;
                        c = ( high_surrogate - hi_surrogate_start ) << 10;
                        c |= ( low_surrogate - lo_surrogate_start );
                        c += 0x10000;
                        
                        high_surrogate = 0;
                    }
                    
                    u32 codepoint = c;
                    u8 codeunits[ 4 ] = { 0 };
                    u8 codeunit_count = 0;
                    
                    u32 error = 0;
                    utf8_encode( codeunits, codeunits + 4, codepoint, &codeunit_count, &error );
                    _assert( no_error( &error ) );
                    
                    if ( window->codepoint_count < array_count( window->codepoints ) ) {
                        
                        window->codepoints[ window->codepoint_count ] = codepoint;
                        
                        u8* current = window->codepoints_utf8 + window->codepoints_utf8_total_codeunit_count;
                        _assert( window->codepoints_utf8_total_codeunit_count + codeunit_count < array_count( window->codepoints_utf8 ) );
                        memory_copy_p( current, codeunits, codeunit_count );
                        
                        window->codepoints_utf8_codeunit_count[ window->codepoint_count ] = codeunit_count;
                        window->codepoints_utf8_total_codeunit_count += codeunit_count;
                        window->codepoint_count++;
                    }
                    
                    _assert( window->message_count >= 1 );
                    input_message_t* input_message = window->messages + ( window->message_count - 1 );
                    _assert( input_message->type == input_message_type_keyboard );
                    _assert( input_message->codeunit_count == 0 );
                    _assert( input_message->virtual_key != 0 );
                    _assert( input_message->scancode != 0 );
                    _assert( input_message->state == input_key_state_flag_pressed || input_message->state == ( input_key_state_flag_pressed | input_key_state_flag_repeated ) );
                    input_message->codepoint = codepoint;
                    memory_copy_p( input_message->codepoint_utf8, codeunits, codeunit_count );
                    input_message->codeunit_count = codeunit_count;
                    input_message->timestamp_ms = ( u32 ) message.time;
                }
                
            } break;
            
#if defined( GAMEPAD_PLAYSTATION_RAW_INPUT )
            
            case WM_INPUT_DEVICE_CHANGE: {
                
                s32 pad_index = ps_update( message.wParam, message.lParam );
                
                if ( !pad_index ) {
                    TranslateMessage( &message );
                    DispatchMessageW( &message );
                }
                
            } break;
#endif
            
            case WM_USER: {
                /* NOTE simon: This can be used to wakeup the application from a thread. */
            } break;
            
            case WM_TIMER: {
                /* NOTE simon: Periodicaly wake up the application. Use window_start_timer and window_end_timer. */
                
                /* NOTE simon: auto-generated message on demand:
https://devblogs.microsoft.com/oldnewthing/20191108-00/?p=103080
https://devblogs.microsoft.com/oldnewthing/20160624-00/?p=93745
https://devblogs.microsoft.com/oldnewthing/20130523-00/?p=4273
*/
                
                /* NOTE simon: We need to dispatch timer we are not interested in because at least one of the timers
that exist without us asking has a the KillTimer function as it time out function. Otherwise we will get
WM_TIMER message continuously. */
                
                if ( message.wParam != window->platform.timer_id ) {
                    TranslateMessage( &message );
                    DispatchMessageW( &message );
                }
                
            } break;
            
            default : {
                profiler_event_start( default_translate );
                TranslateMessage( &message );
                profiler_event_end( default_translate );
                profiler_event_start( default_dispatch );
                DispatchMessageW( &message );
                profiler_event_end( default_dispatch );
            } break;
        }
        profiler_event_start( PeekMessage );
        has_message = PeekMessageW( &message, 0, 0, 0, PM_REMOVE );
        profiler_event_end( PeekMessage );
    }
    
    profiler_event_end( loop );
    
    profiler_event_end( window_handle_messages );
}

stu void window_push_mouse_move_message( window_t* window ) {
    
    input_message_t* input_message = window->messages + window->message_count;
    _assert( window->message_count < array_count( window->messages ) );
    input_message->type = input_message_type_mouse_move;
    input_message->virtual_key = 0;
    input_message->scancode = 0;
    input_message->state = 0;
    
    internal_input_message_update_modifiers( input_message );
    
    input_message->mouse_x = window->mouse_x;
    input_message->mouse_y = window->mouse_y;
    input_message->timestamp_ms = 0;
    
    window->message_count++;
}

stu void window_set_cursor( window_t* window, HCURSOR cursor ) {
    
    if ( window->mouse_x >= 0 && window->mouse_x < cast( s32, window->width ) && window->mouse_y >= 0 && window->mouse_y < cast( s32, window->height ) ) {
        _assert( cursor );
        window->platform.previous_cursor = SetCursor( cursor );
    }
}

// MARK audio

/* Core Audio APIs on msdn: https://msdn.microsoft.com/en-us/library/windows/desktop/dd316599(v=vs.85).aspx */
// #include <initguid.h>
#include "../lib/custom_system_header.h"

#define SOUND_ENABLED 1

#if SOUND_ENABLED
#define DEBUG_WASAPI 0

#ifndef window_WAVE_FORMAT_PCM
#define window_WAVE_FORMAT_PCM 0x0001
#endif

#ifndef window_WAVE_FORMAT_IEEE_FLOAT
#define window_WAVE_FORMAT_IEEE_FLOAT 0x0003
#endif

#ifndef window_WAVE_FORMAT_EXTENSIBLE
#define window_WAVE_FORMAT_EXTENSIBLE 0xFFFE
#endif

#define AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM 0x80000000

#define guid_wave_extensible( name, data_1 ) csh_define_guid_2( name, data_1, 0x0000, 0x0010, 0x8000, 0x00aa00389b71 )
guid_wave_extensible( guid_pcm, window_WAVE_FORMAT_PCM ); /* KSDATAFORMAT_SUBTYPE_PCM */

csh_WAVEFORMATEXTENSIBLE g_device_mix_format = { 0 };

#if 1

typedef enum audio_error_t {
    audio_error_failed_to_load_ole32,
    audio_error_failed_to_find_CoCreateInstance,
    audio_error_failed_to_find_CoTaskMemFree,
    audio_error_failed_to_create_device_enumerator,
    audio_error_failed_to_get_the_default_audio_endpoint,
    audio_error_failed_to_active_device,
    audio_error_failed_to_get_the_mix_format,
    audio_error_failed_to_initialize_audio_client,
    audio_error_failed_to_get_the_buffer_size,
    audio_error_failed_to_get_the_render_client_service,
    audio_error_failed_to_create_audio_client_event,
    audio_error_failed_to_set_audio_client_event_handle,
    audio_error_failed_to_create_thread_stopped_event,
    audio_error_failed_to_create_audio_thread,
    audio_error_failed_to_get_the_buffer,
    audio_error_failed_to_release_the_buffer,
    audio_error_failed_to_get_padding,
    audio_error_failed_to_start,
    audio_error_failed_to_stop
} audio_error_t;

const char* audio_error_strings[ ] = {
    "audio_error_failed_to_load_ole32",
    "audio_error_failed_to_find_CoCreateInstance",
    "audio_error_failed_to_find_CoTaskMemFree",
    "audio_error_failed_to_create_device_enumerator",
    "audio_error_failed_to_get_the_default_audio_endpoint",
    "audio_error_failed_to_active_device",
    "audio_error_failed_to_get_the_mix_format",
    "audio_error_failed_to_initialize_audio_client",
    "audio_error_failed_to_get_the_buffer_size",
    "audio_error_failed_to_get_the_render_client_service",
    "audio_error_failed_to_create_audio_client_event",
    "audio_error_failed_to_set_audio_client_event_handle",
    "audio_error_failed_to_create_thread_stopped_event",
    "audio_error_failed_to_create_audio_thread",
    "audio_error_failed_to_get_the_buffer",
    "audio_error_failed_to_release_the_buffer",
    "audio_error_failed_to_get_padding",
    "audio_error_failed_to_start",
    "audio_error_failed_to_stop"
};

uint16_t audio_error_string_lengths[ ] = {
    32, 43, 40, 46, 52, 35, 40, 45, 41, 51,
    47, 51, 49, 41, 36, 40, 33, 27, 26
};

uint16_t audio_error_max_string_length = 52;

#else

/*
#include "meta_enums.h"

_meta_enum( audio_error,
          failed_to_load_ole32,
          failed_to_find_CoCreateInstance,
          failed_to_find_CoTaskMemFree,
          failed_to_create_device_enumerator,
          failed_to_get_the_default_audio_endpoint,
          failed_to_active_device,
          failed_to_get_the_mix_format,
          failed_to_initialize_audio_client,
          failed_to_get_the_buffer_size,
          failed_to_get_the_render_client_service,
          failed_to_create_audio_client_event,
          failed_to_set_audio_client_event_handle,
          failed_to_create_thread_stopped_event,
          failed_to_create_audio_thread,
          failed_to_get_the_buffer,
          failed_to_release_the_buffer,
          failed_to_get_padding,
          failed_to_start,
          failed_to_stop
          );
*/

#endif

b32 g_audio_restart = false;

HRESULT OnDeviceStateChanged_( csh_IMMNotificationClient* This, LPCWSTR pwstrDeviceId, DWORD dwNewState ) {
    return S_OK;
}

HRESULT OnDeviceAdded_( csh_IMMNotificationClient* This, LPCWSTR pwstrDeviceId ) {
    return S_OK;
}

HRESULT OnDeviceRemoved_( csh_IMMNotificationClient* This, LPCWSTR pwstrDeviceId ) {
    return S_OK;
}

HRESULT OnDefaultDeviceChanged_( csh_IMMNotificationClient* This, csh_EDataFlow flow, csh_ERole role, LPCWSTR pwstrDefaultDeviceId ) {
    
    g_audio_restart = true;
    
    return S_OK;
}

HRESULT OnPropertyValueChanged_( csh_IMMNotificationClient* This, LPCWSTR pwstrDeviceId, const csh_PROPERTYKEY key ) {
    return S_OK;
}

csh_IMMNotificationClientVtbl g_audio_notifications_vtbl = {
    .OnDeviceStateChanged = OnDeviceStateChanged_,
    .OnDeviceAdded = OnDeviceAdded_,
    .OnDeviceRemoved = OnDeviceRemoved_,
    .OnDefaultDeviceChanged = OnDefaultDeviceChanged_,
    .OnPropertyValueChanged = OnPropertyValueChanged_
};
csh_IMMNotificationClient g_audio_notifications = { &g_audio_notifications_vtbl };

#include <avrt.h>

DWORD audio_thread( void* thread_data ) {
    
    platform_t* platform = cast( platform_t*, thread_data );
    
    DWORD task_index = 0;
    HANDLE task_handle = AvSetMmThreadCharacteristicsW( L"Pro Audio", &task_index );
    
    if ( !task_handle ) {
        log_l( "Couldn't set the audio thread characteristic to \"Pro Audio\".\n" );
    }
    
    breakable {
        
        REFERENCE_TIME default_period;
        REFERENCE_TIME minimum_period;
        HRESULT test = csh_IAudioClient_GetDevicePeriod( platform->audio_client, &default_period, &minimum_period );
        
        if ( test != S_OK ) {
            log_l( "Couldn't get the audio device period.\n" );
            break;
        }
        
        u64 samples_per_second = platform->mixer->format.samples_per_second;
        u64 period_to_second = 10000000; /* NOTE simon (06/10/23 15:28:41): period is expressed in "100 nanoseconds" unit*/
        
        u32 samples_to_fill = cast( u32, ( cast( f64, default_period ) / period_to_second ) * samples_per_second );
        
        HRESULT success = csh_IAudioClient_Start( platform->audio_client );
        
        if ( success != S_OK ) {
            log_l( "Couldn't start the audio client.\n" );
            break;
        }
        
        while ( WaitForSingleObject( platform->audio_client_event, INFINITE ) == WAIT_OBJECT_0  ) {
            
            if ( platform->audio_stop_thread ) {
                break;
            }
            
            u32 pending_samples;
            success = csh_IAudioClient_GetCurrentPadding( platform->audio_client, &pending_samples );
            
            if ( success != S_OK ) {
                log_l( "Couldn't retrieve the audio padding.\n" );
                break;
            }
            
            u32 to_fill = samples_to_fill;
            
            if ( pending_samples < samples_to_fill ) {
                to_fill = samples_to_fill - pending_samples;
            }
            
#if DEBUG_WASAPI
            memory_get_on_stack( message, kibioctet( 1 ) );
            memory_push_copy_l( &message, "audio_thread: " );
            string_push_u64( &message, to_fill );
            memory_push_u8( &message, "\n" );
            debug_d( message );
#endif
            
            u8* buffer;
            success = csh_IAudioRenderClient_GetBuffer( platform->audio_render_client, to_fill, &buffer );
            
            if ( success != S_OK ) {
                log_l( "Couldn't get the audio buffer.\n" );
                break;
            }
            
            audio_mixer_render( platform->mixer, buffer, to_fill );
            
            success = csh_IAudioRenderClient_ReleaseBuffer( platform->audio_render_client, to_fill, 0 /*csh_AUDCLNT_BUFFERFLAGS_SILENT */ );
            
            if ( success != S_OK ) {
                log_l( "Couldn't release the audio buffer.\n" );
                break;
            }
        }
        
        success = csh_IAudioClient_Stop( platform->audio_client );
        
        if ( success != S_OK ) {
            log_l( "Couldn't stop the audio client.\n" );
            break;
        }
    }
    
    if ( task_handle ) {
        AvRevertMmThreadCharacteristics( task_handle );
    }
    
    thread_event_signal( &platform->audio_thread_stopped );
    
    return 0;
}

u32 g_audio_error = 0;

stu void audio_end( platform_t* platform, u32* error ) {
    
    /* NOTE simon (13/10/23 17:22:18): Release functions that are "derived" from IUnknown return the reference count left for the object. We don't want to compare them to S_OK */
    
    if ( platform->audio_enumerator ) {
        
        HRESULT success = csh_IMMDeviceEnumerator_UnregisterEndpointNotificationCallback( platform->audio_enumerator, &g_audio_notifications );
        
        if ( success != S_OK ) {
            log_l( "[WARNING][Audio] Couldn't unregister audio notification callbacks.\n" );
        }
        
        csh_IMMDeviceEnumerator_Release( platform->audio_enumerator );
        platform->audio_enumerator = 0;
    }
    
    if ( platform->audio_thread ) {
        platform->audio_stop_thread = true;
        /* NOTE simon (13/10/23 16:29:55): Do I need a memory fence here ? */
        WaitForSingleObject( platform->audio_thread_stopped, INFINITE );
        platform->audio_stop_thread = false;
        platform->audio_thread = 0;
    }
    
    if ( platform->audio_thread_stopped ) {
        CloseHandle( platform->audio_thread_stopped );
        platform->audio_thread_stopped = 0;
    }
    
    if ( platform->audio_client_event ) {
        CloseHandle( platform->audio_client_event );
        platform->audio_client_event = 0;
    }
    
    if ( platform->audio_render_client ) {
        csh_IAudioRenderClient_Release( platform->audio_render_client );
        platform->audio_render_client = 0;
    }
    
    if ( platform->audio_client ) {
        csh_IAudioClient_Release( platform->audio_client );
        platform->audio_client = 0;
    }
}

stu void audio_start( platform_t* platform, audio_mixer_t* mixer, u32* error ) {
    
    /* NOTE simon: Kernel streaming (KS) refers to the Microsoft-provided services that support kernel-mode processing of streamed data.
https://docs.microsoft.com/en-us/windows-hardware/drivers/stream/kernel-streaming
*/
    
    /* NOTE simon:
https://docs.microsoft.com/en-us/windows/win32/coreaudio/user-mode-audio-components

"In exclusive mode, the client can choose to open the stream in any audio format that the endpoint device supports. In shared mode, the client must open the stream in the mix format that is currently in use by the audio engine (or a format that is similar to the mix format). The audio engine's input streams and the output mix from the engine are all in this format.

In Windows 7, a new feature called low-latence mode has been added for streams in share mode. In this mode, the audio engine runs in pull mode, in which there a significant reduction in latency. This is very useful for communication applications that require low audio stream latency for faster streaming."

https://docs.microsoft.com/en-us/windows/win32/coreaudio/device-formats

"An application that uses WASAPI to manage shared-mode streams can rely on the audio engine to perform only limited format conversions. The audio engine can convert between a standard PCM sample size used by the application and the floating-point samples that the engine uses for its internal processing. However, the format for an application stream typically must have the same number of channels and the same sample rate as the stream format used by the device."

https://docs.microsoft.com/en-us/windows/win32/coreaudio/representing-formats-for-iec-61937-transmissions
*/
    
    /* NOTE simon: https://handmade.network/forums/t/8622/p/29139
If you want to do manual resampling you can do it with Media Foundation api using CLSID_CResamplerMediaObject object. wcap code uses it to resample captured wasapi audio for audio encoding to mp4.
    */
    
    /* NOTE simon: Raymond Chen on COM interface https://devblogs.microsoft.com/oldnewthing/20200909-00/?p=104198 */
    
    _assert( platform->audio_enumerator == 0 );
    _assert( platform->audio_client == 0 );
    _assert( platform->audio_render_client == 0 );
    _assert( platform->audio_client_event == 0 );
    _assert( platform->audio_thread == 0 );
    _assert( platform->audio_thread_stopped == 0 );
    _assert( platform->audio_stop_thread == 0 );
    
    csh_IMMDeviceEnumerator* enumerator = 0;
    csh_IMMDevice* device = 0;
    
    breakable {
        
        breakable_check( error );
        
        HMODULE ole_dll = LoadLibraryW( L"Ole32.dll" ); // LPUNKNOWN pUnkOuter
        
        if ( !ole_dll ) {
            set_error( error, audio_error_failed_to_load_ole32 );
            break;
        }
        
        struct IUnknown;
        typedef HRESULT CoCreateInstance_t( const IID* rclsid, struct IUnknown* pUnkOuter, DWORD dwClsContext, const IID* riid, LPVOID *ppv );
        CoCreateInstance_t* CoCreateInstance = ( CoCreateInstance_t* ) GetProcAddress( ole_dll, "CoCreateInstance" );
        
        if ( !CoCreateInstance ) {
            set_error( error, audio_error_failed_to_find_CoCreateInstance );
            break;
        }
        
        typedef void CoTaskMemFree_t( void* pv );
        CoTaskMemFree_t* CoTaskMemFree = ( CoTaskMemFree_t* ) GetProcAddress( ole_dll, "CoTaskMemFree" );
        
        if ( !CoTaskMemFree ) {
            set_error( error, audio_error_failed_to_find_CoTaskMemFree );
            break;
        }
        
        HRESULT success = CoCreateInstance( ( const IID * ) &csh_CLSID_MMDeviceEnumerator, NULL, csh_CLSCTX_ALL, ( const IID* ) &csh_IID_IMMDeviceEnumerator, ( void** ) &enumerator );
        
        if ( success != S_OK ) {
            set_error( error, audio_error_failed_to_create_device_enumerator );
            break;
        }
        
        platform->audio_enumerator = enumerator;
        
        FreeLibrary( ole_dll );
        
        success = csh_IMMDeviceEnumerator_GetDefaultAudioEndpoint( enumerator, csh_eRender, csh_eConsole, &device );
        
        if ( success != S_OK ) {
            set_error( error, audio_error_failed_to_get_the_default_audio_endpoint );
            break;
        }
        
        success = csh_IMMDevice_Activate( device, ( const IID* ) &csh_IID_IAudioClient, csh_CLSCTX_ALL, NULL, ( void** ) &platform->audio_client );
        
        if ( success != S_OK ) {
            set_error( error, audio_error_failed_to_active_device );
            break;
        }
        
        csh_WAVEFORMATEXTENSIBLE* device_mix_format;
        success = csh_IAudioClient_GetMixFormat( platform->audio_client, ( csh_WAVEFORMATEX** ) &device_mix_format );
        
        if ( success != S_OK ) {
            set_error( error, audio_error_failed_to_get_the_mix_format );
            break;
        }
        
        g_device_mix_format = deref( device_mix_format );
        CoTaskMemFree( device_mix_format );
        
        csh_WAVEFORMATEXTENSIBLE buffer_format;
        
        buffer_format.Format.wFormatTag = window_WAVE_FORMAT_EXTENSIBLE;
        buffer_format.Format.nChannels = mixer->format.channel_count;
        buffer_format.Format.nSamplesPerSec = mixer->format.samples_per_second;
        buffer_format.Format.wBitsPerSample = mixer->format.container_size * 8;
        buffer_format.Format.nBlockAlign = mixer->format.channel_count * mixer->format.container_size;
        buffer_format.Format.nAvgBytesPerSec = buffer_format.Format.nSamplesPerSec * buffer_format.Format.nBlockAlign;
        buffer_format.Format.cbSize = sizeof( buffer_format );
        
        buffer_format.Samples.wValidBitsPerSample = mixer->format.bits_per_sample;
        buffer_format.dwChannelMask = mixer->format.channel_mask;
        
        buffer_format.SubFormat = csh_guid_pcm;
        
        /* NOTE simon (11/10/23 16:27:02):
        1 reference_time = 100 nanoseconds = 1 sec / 10 000 000
        10000000 is 1 seconde.
        */
        u64 buffer_duration = 10000000;
        
        success = csh_IAudioClient_Initialize( platform->audio_client,
                                              csh_AUDCLNT_SHAREMODE_SHARED,
                                              csh_AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | csh_AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY | csh_AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                                              buffer_duration, 0, &buffer_format.Format, 0 );
        
        if ( success != S_OK ) {
            set_error( error, audio_error_failed_to_initialize_audio_client );
            break;
        }
        
#if 0
        u32 buffer_size_in_samples = 0;
        success = csh_IAudioClient_GetBufferSize( platform->audio_client, &buffer_size_in_samples );
        
        if ( success != S_OK ) {
            set_error( error, audio_error_failed_to_get_the_buffer_size );
            break;
        }
#endif
        
        success = csh_IAudioClient_GetService( platform->audio_client, ( const IID* ) &csh_IID_IAudioRenderClient, ( void** ) &platform->audio_render_client );
        
        if ( success != S_OK ) {
            set_error( error, audio_error_failed_to_get_the_render_client_service );
            break;
        }
        
        platform->mixer = mixer;
        platform->audio_client_event = CreateEventW( 0, 0, 0, 0 );
        
        if ( !platform->audio_client_event ) {
            set_error( error, audio_error_failed_to_create_audio_client_event );
            break;
        }
        
        success = csh_IAudioClient_SetEventHandle( platform->audio_client, platform->audio_client_event );
        
        if ( success != S_OK ) {
            set_error( error, audio_error_failed_to_set_audio_client_event_handle );
            break;
        }
        
        platform->audio_thread_stopped = CreateEventW( 0, 0, 0, 0 );
        
        if ( !platform->audio_thread_stopped ) {
            set_error( error, audio_error_failed_to_create_thread_stopped_event );
            break;
        }
        
        platform->audio_thread = CreateThread( 0, 0, audio_thread, platform, 0, 0 );
        
        if ( !platform->audio_thread ) {
            set_error( error, audio_error_failed_to_create_audio_thread );
            break;
        }
        
        SetThreadDescription( platform->audio_thread, L"Audio thread" );
        
        success = csh_IMMDeviceEnumerator_RegisterEndpointNotificationCallback( enumerator, &g_audio_notifications );
        
        if ( success != S_OK ) {
            log_l( "[WARNING][Audio] Couldn't register audio notification callbacks.\n" );
        }
    }
    
    if ( is_error( error ) ) {
        audio_end( platform, error );
    }
    
    if ( device ) {
        csh_IMMDevice_Release( device );
    }
}

stu void audio_update( platform_t* platform, u32* error ) {
    
    if ( g_audio_restart ) {
        audio_end( platform, error );
        g_audio_restart = false;
        audio_start( platform, platform->mixer, error );
    }
}

#endif

#elif defined LINUX

stu void window_create( window_state_t* state, void* window_name, u32 width, u32 height, s32 x, s32 y, b32 title_bar ) {
    
    state->platform.display = XOpenDisplay( 0 );
    
    if ( state->platform.display ){
        
        Window desktop = DefaultRootWindow( state->platform.display );
        GLint attributes[ ] = {
            GLX_RGBA,
            GLX_DEPTH_SIZE, 24,
            GLX_DOUBLEBUFFER,
            None
        };
        XVisualInfo* info = glXChooseVisual( state->platform.display, 0, attributes );
        
        if ( info ){
            Colormap colormap = XCreateColormap( state->platform.display, desktop, info->visual, AllocNone );
            XSetWindowAttributes window_attributes = {
                .colormap = colormap,
                .event_mask = ExposureMask | KeyPressMask,
            };
            state->platform.window = XCreateWindow( state->platform.display, desktop, 0, 0, width, height, 0, info->depth, InputOutput, info->visual, CWColormap | CWEventMask, &window_attributes );
            XMapWindow( state->platform.display, state->platform.window );
            XStoreName( state->platform.display, state->platform.window, window_name );
            
            state->platform.render_context = glXCreateContext( state->platform.display, info, 0, GL_TRUE );
            glXMakeCurrent( state->platform.display, state->platform.window, state->platform.render_context );
            
            char* version = ( char* ) glGetString( GL_VERSION );
            log_s( log_name_gl, version );
            state->close_requested = false;
            state->running = true;
            
        } else {
            log_l( log_name_gl, "Failed to choose visual.\n" );
        }
        
    } else {
        log_l( log_name_gl, "Failed to open the display.\n" );
    }
    
    if ( arguments_find_s( "-gfx_log", 0 ) ) {
        log_write_to_file( log_name_gl, string_arg_l( "linux_opengl.log" ) );
    }
}

stu void window_handle_messages( window_state_t* state, u32 whm_flags ) {
    
    state->resized = false;
    
    while ( 1 ) {
        
        XEvent event;
        XNextEvent( state->platform.display, &event );
        
        if ( event.type == Expose ) {
            
            XWindowAttributes window_attributes;
            XGetWindowAttributes( state->platform.display, state->platform.window, &window_attributes );
            state->width = window_attributes.width;
            state->height = window_attributes.height;
            break;
            
        } else if ( event.type == KeyPress ) {
            
            /* TODO simon: handle messages. */
            XKeyPressedEvent* e = ( XKeyPressedEvent* ) &event;
            
            if ( e->keycode == XKeysymToKeycode( state->platform.display, XK_Left ) ) {
                state->close_requested = true;
            }
            
            break;
            
        } else if ( event.type == ConfigureNotify ) {
            
            XConfigureEvent* e = ( XConfigureEvent* ) &event;
            state->width = e->width;
            state->height = e->height;
            state->resized = true;
            break;
        }
    }
}

stu void window_clean_up( window_state_t* state ) {
    
    glXMakeCurrent( state->platform.display, None, 0 );
    glXDestroyContext( state->platform.display, state->platform.render_context );
    XDestroyWindow( state->platform.display, state->platform.window );
    XCloseDisplay( state->platform.display );
}

stu void window_destroy( platform_specific_t* platform ) {
    
    glXMakeCurrent( platform->display, None, 0 );
    glXDestroyContext( platform->display, platform->render_context );
    XDestroyWindow( platform->display, platform->window );
    XCloseDisplay( platform->display );
}

stu void window_swap_buffers( window_state_t* state ) {
    glXSwapBuffers( state->platform.display, state->platform.window );
}

stu b32 sound_create( platform_specific_t* platform, sound_format_t* format, u32* out_buffer_size_in_samples ) {
    _assert( !"Not implemented." );
    return 0;
}

stu void sound_fill_buffer( platform_specific_t* platform, sound_format_t* format, u32 sampleCount, void* source ) {
    _assert( !"Not implemented." );
}

stu u32 sound_get_sample_count_to_add( platform_specific_t* platform, u32 samples_per_second, f32 delta_time ) {
    _assert( !"Not implemented." );
    return 0;
}

stu void sound_update( platform_specific_t* platform, sound_format_t* format, u32 sample_count_to_add, void* samples ) {
    _assert( !"Not implemented." );
}

stu void sound_start( platform_specific_t* platform ) {
    _assert( !"Not implemented." );
}

stu void sound_stop( platform_specific_t* platform ) {
    _assert( !"Not implemented." );
}

#else
#error Unsuported platform.
#endif

#define WINDOW_H

#endif
