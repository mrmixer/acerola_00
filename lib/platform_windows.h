#if !defined( PLATFORM_WINDOWS_H )

#if !defined( WINDOWS )
# error This header is for windows only.
#endif

#include "../lib/custom_system_header.h"

typedef enum platform_folder_name_t {
    
    platform_folder_name_none,
    
    platform_folder_name_user,
    platform_folder_name_desktop,
    
    platform_folder_name_count
        
} platform_folder_name_t;

/* NOTE simon: From Knownfolders.h */
csh_define_guid_2( guid_roaming_app_data, 0x3eb685db, 0x65f9, 0x4cf6, 0xa03a, 0xe3ef65729f3d );
csh_define_guid_2( guid_desktop, 0xb4bfcc3a, 0xdb2c, 0x424c, 0xb029, 0x7fe99a87c641 );

data_t platform_get_path( data_t* buffer, platform_folder_name_t folder ) {
    
    data_t backup = deref( buffer );
    data_t result = { 0 };
    csh_guid_t const* guid_ref = 0;
    
    switch ( folder ) {
        
        case platform_folder_name_user: {
            guid_ref = &csh_guid_roaming_app_data;
        } break;
        
        case platform_folder_name_desktop: {
            guid_ref = &csh_guid_desktop;
        } break;
        
        default: {
        } break;
    }
    
    u16* path_utf16 = 0;
    
    breakable {
        
        if ( !guid_ref ) {
            log_l( "platform_get_path: unknow folder requested.\n" );
            break;
        }
        
        HRESULT status = SHGetKnownFolderPath( ( REFKNOWNFOLDERID ) guid_ref, 0, 0, &path_utf16 );
        
        if ( status != S_OK ) {
            log_l( "platform_get_path: couldn't retreive folder path.\n" );
            break;
        }
        
        umm codeunit_count = utf16_get_codeunit_count( path_utf16 );
        u16* path_end = path_utf16 + codeunit_count;
        umm codepoint_count = utf16_get_codepoint_count( path_utf16, path_end );
        data_t path_utf8 = memory_get_block( buffer, ( codepoint_count * 4 ), 1 );
        
        if ( path_utf8.bytes == 0 ) {
            log_l( "platform_get_path: not enough free memory in buffer.\n" );
            break;
        }
        
        g_ucs_error = 0;
        path_utf8.used = utf16_to_utf8_p( path_utf16, codeunit_count, path_utf8.bytes, path_utf8.reserved, &g_ucs_error );
        
        if ( is_error( &g_ucs_error ) ) {
            log_l( "platform_get_path: error while converting utf16 string to utf8.\n" );
            deref( buffer ) = backup;
            break;
        }
        
        umm to_remove = memory_left( path_utf8 );
        path_utf8.reserved = path_utf8.used;
        buffer->used -= to_remove;
        
        result = path_utf8;
    }
    
    if ( path_utf16 ) {
        CoTaskMemFree( path_utf16 );
    }
    
    return result;
}

#define PLATFORM_WINDOWS_H
#endif
