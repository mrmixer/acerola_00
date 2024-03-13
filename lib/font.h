
#ifndef FONT_H

#define DEBUG_FONT_BUILD_ATLAS 0

#if DEBUG_FONT_BUILD_ATLAS
#include "../lib/tga.h"
#endif

#define STBRP_ASSERT( x ) _assert( x )
#define STB_RECT_PACK_IMPLEMENTATION
#include "../lib/stb_rect_pack.h"

#define STBTT_assert( x ) _assert( x )
#define STB_TRUETYPE_IMPLEMENTATION
#include "../lib/stb_truetype.h"

// NOTE simon: ISO/IEC_8859-15 + 0xfffd + 0x2026.
// NOTE simon: U+FFFD is the codepoint for the replacement character.
// NOTE simon: U+2026 is horizontal elipsis.
u32 font_codepoint_set[ ] = {
    
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa5, 0xa7, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb5, 0xb6, 0xb7, 0xb9, 0xba, 0xbb, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff,
    
    0x20ac, 0x160, 0x161, 0x17d, 0x17e, 0x152, 0x153, 0x178, 0xfffd, 0x2026
};

u32 font_replacement_codepoint = 0xfffd;
u32 font_replacement_codepoint_index = array_count( font_codepoint_set ) - 2;

typedef struct codepoint_metrics_t {
    
    u16 u0, v0, u1, v1;
    f32 x0, y0, x1, y1;
    f32 advance;
    u32 kerning_count;
    u32* kerning;
} codepoint_metrics_t;

#define codepoint_metrics_t_alignment 4

typedef struct font_t {
    
    stbtt_fontinfo info;
    data_t file;
    u32* codepoint_set;
    codepoint_metrics_t* cp_metrics;
    s32* kerning;
    u32 codepoint_count;
    f32 size;
    f32 scale;
    s32 ascent;
    s32 descent;
    s32 line_gap;
    u8 cache[ 256 ];
    u32 cache_offset;
    u32 first_index_out_of_cache;
    b32 is_monospaced;
    u32 replacement_codepoint_index;
    
} font_t;

#define font_t_alignment 8

typedef struct font_collection_t {
    
    data_t memory;
    data_t atlas;
    font_t* fonts;
    u16 atlas_width;
    u16 atlas_height;
    u8 font_count;
    umm memory_usage_after_font_are_added;
    
} font_collection_t;

#define font_collection_t_alignment sizeof( umm )

typedef struct font_metrics_t {
    font_t* font;
    u32 name;
    f32 line_height;
    f32 baseline;
    f32 top_to_baseline;
} font_metrics_t;

stu void font_build_atlas( font_collection_t* collection ) {
    
    profiler_event_start( font_build_atlas );
    
    collection->atlas_width = 256;
    collection->atlas_height = 256;
    
    b32 success = false;
    
    while ( !success ) {
        
        collection->atlas.reserved = collection->atlas.used = collection->atlas_width * collection->atlas_height;
        collection->atlas.raw = memory_push_size( &collection->memory, collection->atlas.used, 1 );
        
        stbtt_pack_context context;
        stbtt_PackBegin( &context, collection->atlas.bytes, collection->atlas_width, collection->atlas_height, 0, 1, 0 );
        success = true;
        
        for ( umm font_index = 0; success && font_index < collection->font_count; font_index++ ) {
            
            font_t* f = collection->fonts + font_index;
            
            stbtt_packedchar char_datas[ 1024 ]; /* TODO simon: change this. */
            _assert( f->codepoint_count < array_count( char_datas ) );
            stbtt_pack_range range = { ( f32 ) f->size, 0, ( int* ) f->codepoint_set, ( int ) f->codepoint_count, 0, 0, 0 };
            range.chardata_for_range = char_datas;
            
            stbtt_PackSetOversampling( &context, 2, 2 );
            success = stbtt_PackFontRanges( &context, f->info.data, 0, &range, 1 );
            
            if ( success ) {
                
                for ( umm codepoint_index = 0; codepoint_index < f->codepoint_count; codepoint_index++ ) {
                    
                    stbtt_packedchar* packed_char = char_datas + codepoint_index;
                    codepoint_metrics_t* cm = f->cp_metrics + codepoint_index;
                    
                    cm->x0 = packed_char->xoff;
                    cm->y0 = -packed_char->yoff2;
                    cm->x1 = packed_char->xoff2;
                    cm->y1 = -packed_char->yoff;
                    
                    u16 height = packed_char->y1 - packed_char->y0;
                    
                    cm->u0 = packed_char->x0;
                    cm->v0 = collection->atlas_height - packed_char->y0 - height;
                    cm->u1 = packed_char->x1;
                    cm->v1 = cm->v0 + height;
                    cm->advance = packed_char->xadvance;
                }
            }
        }
        
        stbtt_PackEnd( &context );
        
        if ( !success ) {
            
            memory_pop_size( &collection->memory, collection->atlas.used );
            collection->atlas.reserved = collection->atlas.used = 0;
            
            if ( collection->atlas_width <= collection->atlas_height ) {
                collection->atlas_width *= 2;
            } else {
                collection->atlas_height *= 2;
            }
        }
    }
    
    u8* line_up = collection->atlas.bytes + collection->atlas_width * ( collection->atlas_height - 1 );
    u8* line_down = collection->atlas.bytes;
    
    for ( umm y = 0; y < collection->atlas_height / 2; y++ ) {
        
        u32* word_up = ( u32* ) line_up;
        u32* word_down = ( u32* ) line_down;
        
        for ( umm x = 0; x < collection->atlas_width / sizeof( u32 ); x++ ) {
            
            u32 temp = deref( word_up );
            deref( word_up ) = deref( word_down );
            deref( word_down ) = temp;
            word_up++;
            word_down++;
        }
        
        line_up -= collection->atlas_width;
        line_down += collection->atlas_width;
    }
    
    profiler_event_end( font_build_atlas );
    
#if DEBUG_FONT_BUILD_ATLAS
    u8 header[ 18 ] = { 0 };
    tga_set_image_type( header, tga_image_type_black_and_white );
    tga_set_width( header, ( u16 ) collection->atlas_width );
    tga_set_height( header, ( u16 ) collection->atlas_height );
    tga_set_pixel_depth( header, 8 );
    u32 error = 0;
    file_handle_t h = file_open( string_l( "atlas.tga" ), file_mode_write, true, &error );
    file_write_handle( h, 0, 18, header, &error );
    file_write_handle( h, 18, collection->atlas.used, collection->atlas.bytes, &error );
    file_close( h, &error );
#endif
    
}

stu font_t* font_collection_get_font( font_collection_t* collection, umm font_id ) {
    font_t* font = collection->fonts + font_id;
    return font;
}

/* NOTE simon: codepoint set must contain the replacement codepoint 0xfffd */
stu void font_add_font( font_collection_t* collection, data_t file, f32 size, u32* codepoint_set, u32 codepoint_count, b32 is_monospaced ) {
    
    _assert( collection );
    _assert( file.bytes );
    _assert( codepoint_set );
    
    font_t* f = memory_push_struct( &collection->memory, font_t );
    
    if ( !collection->fonts ) {
        collection->fonts = f;
    }
    
    collection->font_count++;
    f->file = file;
    
    f->codepoint_count = codepoint_count;
    f->codepoint_set = codepoint_set;
    f->size = size;
    f->is_monospaced = is_monospaced;
    f->replacement_codepoint_index = 0xffffffff;
    
    s32 offset = stbtt_GetFontOffsetForIndex( file.bytes, 0 );
    stbtt_InitFont( &f->info, file.bytes, offset );
    f->scale = stbtt_ScaleForPixelHeight( &f->info, size );
    stbtt_GetFontVMetrics( &f->info, &f->ascent, &f->descent, &f->line_gap );
    
    collection->memory_usage_after_font_are_added = collection->memory.used;
}

stu void font_set_codepoints( font_collection_t* collection, u32* codepoint_set, u32 codepoint_count ) {
    
    for ( umm font_index = 0; font_index < collection->font_count; font_index++ ) {
        font_t* f = collection->fonts + font_index;
        f->codepoint_count = codepoint_count;
        f->codepoint_set = codepoint_set;
    }
}

stu void font_build_cache( font_t* font ) {
    
    for ( umm i = 0; i < font->codepoint_count - 1; i++ ) {
        
        for ( umm j = i; j < font->codepoint_count; j++ ) {
            
            if ( font->codepoint_set[ i ] > font->codepoint_set[ j ] ) {
                swap_type( font->codepoint_set[ i ], font->codepoint_set[ j ], u32 );
            }
        }
    }
    
    memory_zero( font->cache, sizeof( font->cache ) );
    
    font->cache_offset = font->codepoint_set[ 0 ];
    font->first_index_out_of_cache = 0;
    
    for ( umm i = 0; i < font->codepoint_count; i++ ) {
        
        umm index = font->codepoint_set[ i ] - font->cache_offset;
        
        if ( index < array_count( font->cache ) ) {
            font->cache[ index ] = ( u8 ) i;
        } else {
            font->first_index_out_of_cache = ( u32 ) i;
            break;
        }
    }
}

stu void font_build_collection( font_collection_t* collection ) {
    
    profiler_event_start( font_build_collection );
    
    /* NOTE simon: If we wanted to recompute this position instead of saving it, we would have a
    possible problem as we have to different way the memory is used, (default vs rss_reader). In the
    default case, the font_collection_t struct is allocated in the font_collection_t->memory itself,
    while the rss_reader doesn't do that. */
    collection->memory.used = collection->memory_usage_after_font_are_added;
    
    for ( umm font_index = 0; font_index < collection->font_count; font_index++ ) {
        
        font_t* font = collection->fonts + font_index;
        font->cp_metrics = memory_push_array( &collection->memory, codepoint_metrics_t, font->codepoint_count );
        font->kerning = memory_current_aligned( collection->memory, s32, sizeof( s32 ) );
        
        font_build_cache( font );
        
        for ( umm codepoint_index = 0; codepoint_index < font->codepoint_count; codepoint_index++ ) {
            
            u32 codepoint = font->codepoint_set[ codepoint_index ];
            
            if ( codepoint == font_replacement_codepoint ) {
                font->replacement_codepoint_index = ( u32 ) codepoint_index;
            }
            
            codepoint_metrics_t* cm = font->cp_metrics + codepoint_index;
            memory_zero_struct( cm );
            
            cm->kerning = memory_current_aligned( collection->memory, u32, sizeof( u32 ) );
            memory_get_on_stack( temp, kibioctet( 4 ) );
            
            for ( umm kerning_index = 0; kerning_index < font->codepoint_count; kerning_index++ ) {
                
                u32 kerning_codepoint = font->codepoint_set[ kerning_index ];
                s32 kerning_advance = stbtt_GetCodepointKernAdvance( &font->info, codepoint, kerning_codepoint );
                
                if ( kerning_advance ) {
                    memory_push_u32( &collection->memory, kerning_codepoint, sizeof( u32 ) );
                    memory_push_s32( &temp, kerning_advance, 1 );
                    cm->kerning_count++;
                }
            }
            
            s32* destination = memory_push_array( &collection->memory, s32, cm->kerning_count ); 
            _assert( destination );
            memory_copy_p( destination, temp.bytes, temp.used );
        }
        
        _assert( font->codepoint_set[ font->replacement_codepoint_index ] == font_replacement_codepoint );
    }
    
    profiler_event_end( font_build_collection );
    
    font_build_atlas( collection );
}

/* TODO simon: better cache by building it up with things that are realy used. */
stu u32 font_get_codepoint_metrics_index( font_t* font, u32 codepoint ) {
    
    u32 result;
    u32 cache_index = codepoint - font->cache_offset;
    
    if ( cache_index < array_count( font->cache ) ) {
        result = font->cache[ cache_index ];
    } else {
        
        result = font->codepoint_count;
        
        smm left = font->first_index_out_of_cache;
        smm right = font->codepoint_count - 1;
        
        while ( left <= right ) {
            
            smm middle = ( left + right ) / 2;
            
            if ( font->codepoint_set[ middle ] < codepoint ) {
                left = middle + 1;
            } else if ( font->codepoint_set[ middle ] > codepoint ) {
                right = middle - 1;
            } else {
                result = ( u32 ) middle;
                break;
            }
        }
        
        if ( left > right ) {
            result = font->replacement_codepoint_index;
        }
    }
    
    return result;
}

stu codepoint_metrics_t* font_get_codepoint_metrics( font_t* font, u32 codepoint ) {
    
    u32 index = font_get_codepoint_metrics_index( font, codepoint );
    codepoint_metrics_t* result = font->cp_metrics + index;
    return result;
}

stu s32 font_get_kerning( font_t* font, codepoint_metrics_t* cm, u32 codepoint ) {
    
    s32 result = 0;
    
    if ( cm->kerning_count > 0 ) {
        
        u32* kerning = cm->kerning;
        s32* values = cast( s32*, kerning + cm->kerning_count );
        
        /* NOTE simon: I tried binary search, but there are so few entry in the array that it's faster to just do a linear search. */
        for ( u32 i = 0; i < cm->kerning_count; i++ ) {
            
            if ( kerning[ i ] == codepoint ) {
                result = values[ i ];
                break;
            }
        }
    }
    
    return result;
}

stu f32 font_get_line_height( font_t* font, b32 include_line_gap ) {
    
    f32 height = ( -font->descent + ( ( include_line_gap ) ? font->line_gap : 0 ) + font->ascent ) * font->scale;
    return height;
}

stu f32 font_get_baseline( font_t* font ) {
    
    f32 result = -font->descent * font->scale;
    return result;
}

font_metrics_t font_get_metrics( font_collection_t* collection, u32 font_name ) {
    
    _assert( collection );
    font_metrics_t result = { 0 };
    
    if ( font_name < collection->font_count ) {
        result.font = collection->fonts + font_name;
        result.name = font_name;
        result.line_height = font_get_line_height( result.font, false );
        result.baseline = font_get_baseline( result.font );
        result.top_to_baseline = result.line_height - result.baseline;
    }
    
    return result;
}

#define font_first_character_offset( font, cm ) ( ( ( font )->is_monospaced ) ? 0 : ( cm )->x0 )

/* NOTE simon: You still need to build the collection after this. */
font_collection_t* font_create_default_collection( umm font_size, u32* codepoint_set, umm codepoint_count ) {
    
    if ( !font_size ) {
        font_size = 14;
    }
    
    if ( !codepoint_set ) {
        codepoint_set = font_codepoint_set;
        codepoint_count = array_count( font_codepoint_set );
    }
    
    data_t consola_file = file_read_entire( string_l( "C:\\Windows\\Fonts\\consola.ttf" ), false, &g_file_error );
    _assert( no_error( &g_file_error ) );
    data_t verdana_file = file_read_entire( string_l( "C:\\Windows\\Fonts\\Verdana.ttf" ), false, &g_file_error );
    _assert( no_error( &g_file_error ) );
    data_t memory = memory_get_virtual( mebioctet( 1 ), 0 );
    font_collection_t* collection = memory_push_struct( &memory, font_collection_t );
    collection->memory = memory;
    font_add_font( collection, consola_file, ( f32 ) font_size, codepoint_set, ( u32 ) codepoint_count, true );
    font_add_font( collection, verdana_file, ( f32 ) font_size, codepoint_set, ( u32 ) codepoint_count, false );
    
    return collection;
}

#define FONT_H

#endif
