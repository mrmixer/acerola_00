
#ifndef PROFILER_IDS_H

#ifdef PROFILER_ON

#include <stdint.h>

typedef enum profiler_timeline_id_t {
    profiler_timeline_id_worker_thread,
    profiler_timeline_id_count
} profiler_timeline_id_t;

typedef enum profiler_event_id_t {
    profiler_event_id_profiler_collapse,
    profiler_event_id_profiler_not_measured,
    profiler_event_id_char_is_any_u32,
    profiler_event_id_date_to_string,
    profiler_event_id_font_build_atlas,
    profiler_event_id_font_build_collection,
    profiler_event_id_gl_batch_rectangle,
    profiler_event_id_gl_batch_text,
    profiler_event_id_gl_swap_buffers,
    profiler_event_id_gl_render,
    profiler_event_id_gl_update_texture_content,
    profiler_event_id_window_create,
    profiler_event_id_window_swap_buffers,
    profiler_event_id_http_file_open,
    profiler_event_id_http_file_get,
    profiler_event_id_image_resize,
    profiler_event_id_image_to_ascii_render,
    profiler_event_id_extra_step,
    profiler_event_id_layout_text,
    profiler_event_id_layout_get_single_line_size,
    profiler_event_id_layout_get_byte_at_position,
    profiler_event_id_layout_fit_single_line,
    profiler_event_id_layout_string_p,
    profiler_event_id_layout_end,
    profiler_event_id_layout_single_line,
    profiler_event_id_layout_pass_only_size,
    profiler_event_id_layout_pass_end,
    profiler_event_id_layout_pass_start,
    profiler_event_id_text_layout_next_rect,
    profiler_event_id_text_layout_next_rect_2,
    profiler_event_id_ui_compute_axis_sizes,
    profiler_event_id_ui_render_rect_clip_string,
    profiler_event_id_ui_render_rect_clip_string_over,
    profiler_event_id_ui_render_rect_clip_string_visible,
    profiler_event_id_ui_compute_size,
    profiler_event_id_ui_render,
    profiler_event_id_ui_position_rect,
    profiler_event_id_ui_render_rect,
    profiler_event_id_ui_compute_size_popup,
    profiler_event_id_ui_position_rect_popup,
    profiler_event_id_ui_render_rect_popup,
    profiler_event_id_thread_do_work,
    profiler_event_id_thread_user_function,
    profiler_event_id_thread_queue_completed_signal,
    profiler_event_id_thread_loop,
    profiler_event_id_thread_wait_for_work,
    profiler_event_id_thread_queue_terminated_signal,
    profiler_event_id_thread_queue_work,
    profiler_event_id_thread_queue_signal,
    profiler_event_id_thread_complete_queue,
    profiler_event_id_thread_wait_for_completion,
    profiler_event_id_thread_terminate,
    profiler_event_id_thread_terminate_signal,
    profiler_event_id_thread_wait_for_termination,
    profiler_event_id_pair_active_set_cursor,
    profiler_event_id_pair_hot_set_cursor,
    profiler_event_id_input_sync_keys,
    profiler_event_id_window_proc,
    profiler_event_id_window_handle_messages,
    profiler_event_id_setup,
    profiler_event_id_GetMessage,
    profiler_event_id_PeekMessage,
    profiler_event_id_loop,
    profiler_event_id_default_translate,
    profiler_event_id_default_dispatch,
    profiler_event_id_count
} profiler_event_id_t;

#ifdef PROFILER_IMPLEMENTATION

const char* profiler_timeline_id_strings[ profiler_timeline_id_count ] = {
    "worker_thread",
};

uint16_t profiler_timeline_id_string_lengths[ profiler_timeline_id_count ] = {
    13, 
};

uint16_t profiler_max_timeline_id_string_length = 13;

const char* profiler_event_id_strings[ profiler_event_id_count ] = {
    "profiler_collapse",
    "profiler_not_measured",
    "char_is_any_u32",
    "date_to_string",
    "font_build_atlas",
    "font_build_collection",
    "gl_batch_rectangle",
    "gl_batch_text",
    "gl_swap_buffers",
    "gl_render",
    "gl_update_texture_content",
    "window_create",
    "window_swap_buffers",
    "http_file_open",
    "http_file_get",
    "image_resize",
    "image_to_ascii_render",
    "extra_step",
    "layout_text",
    "layout_get_single_line_size",
    "layout_get_byte_at_position",
    "layout_fit_single_line",
    "layout_string_p",
    "layout_end",
    "layout_single_line",
    "layout_pass_only_size",
    "layout_pass_end",
    "layout_pass_start",
    "text_layout_next_rect",
    "text_layout_next_rect_2",
    "ui_compute_axis_sizes",
    "ui_render_rect_clip_string",
    "ui_render_rect_clip_string_over",
    "ui_render_rect_clip_string_visible",
    "ui_compute_size",
    "ui_render",
    "ui_position_rect",
    "ui_render_rect",
    "ui_compute_size_popup",
    "ui_position_rect_popup",
    "ui_render_rect_popup",
    "thread_do_work",
    "thread_user_function",
    "thread_queue_completed_signal",
    "thread_loop",
    "thread_wait_for_work",
    "thread_queue_terminated_signal",
    "thread_queue_work",
    "thread_queue_signal",
    "thread_complete_queue",
    "thread_wait_for_completion",
    "thread_terminate",
    "thread_terminate_signal",
    "thread_wait_for_termination",
    "pair_active_set_cursor",
    "pair_hot_set_cursor",
    "input_sync_keys",
    "window_proc",
    "window_handle_messages",
    "setup",
    "GetMessage",
    "PeekMessage",
    "loop",
    "default_translate",
    "default_dispatch",
};

uint16_t profiler_event_id_string_lengths[ profiler_event_id_count ] = {
    17,
    21,
    15, 14, 16, 21, 18, 13, 15, 9, 25, 13, 19, 14, 13, 12, 21, 10, 11, 27, 27,
    22, 15, 10, 18, 21, 15, 17, 21, 23, 21, 26, 31, 34, 15, 9, 16, 14, 21, 22, 20,
    14, 20, 29, 11, 20, 30, 17, 19, 21, 26, 16, 23, 27, 22, 19, 15, 11, 22, 5,
    10, 11, 4, 17, 16, 
};

uint16_t profiler_max_event_id_string_length = 34;

#else /* PROFILER_IMPLEMENTATION */ 

extern const char* profiler_timeline_id_strings[ profiler_timeline_id_count ];
extern uint16_t profiler_timeline_id_string_lengths[ profiler_timeline_id_count ];
extern uint16_t profiler_max_timeline_id_string_length;
extern const char* profiler_event_id_strings[ profiler_event_id_count ];
extern uint16_t profiler_event_id_string_lengths[ profiler_event_id_count ];
extern uint16_t profiler_max_event_id_string_length;

#endif /* PROFILER_IMPLEMENTATION */ 

#endif /* PROFILER_ON */

#define PROFILER_IDS_H
#endif
