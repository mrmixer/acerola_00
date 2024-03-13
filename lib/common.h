#if !defined( COMMON_H )

#include "../lib/profiler.h"

/* NOTE simon:
Compiler macros help: https://sourceforge.net/p/predef/wiki/Compilers/
*/
#include "../lib/defines.h"

#if defined( NO_CRT )
#include "../lib/no_crt.h"
#endif

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#if defined( WINDOWS )

#if !defined( WIN32_LEAN_AND_MEAN )
#define WIN32_LEAN_AND_MEAN 1
#endif

#include <windows.h>
#include <intrin.h> // NOTE simon: https://msdn.microsoft.com/en-us/library/26td21ds.aspx

#elif defined( LINUX )

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <x86intrin.h>
#include <X11/Xlib.h>
#include <GL/glx.h>

#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t b8;
typedef uint32_t b32;

typedef uintptr_t umm;
typedef intptr_t smm;

typedef float f32;
typedef double f64;

#define true 1
#define false 0

#define u8_alignment sizeof( u8 )
#define u16_alignment sizeof( u16 )
#define u32_alignment sizeof( u32 )
#define u64_alignment sizeof( u64 )

#define s8_alignment sizeof( s8 )
#define s16_alignment sizeof( s16 )
#define s32_alignment sizeof( s32 )
#define s64_alignment sizeof( s64 )

#define b8_alignment sizeof( b8 )
#define b32_alignment sizeof( b32 )

#define umm_alignment sizeof( umm )
#define smm_alignment sizeof( smm )

#define f32_alignment sizeof( f32 )
#define f64_alignment sizeof( f64 )

/* NOTE simon: single translation unit. */
#define stu static

// NOTE simon: 1##high so that a number starting with a 0 is not an octal number.
#define bit( high, low ) _bit( 1##high, 1##low )
#define _bit( high, low ) \
( ( ( ( high / 1000 ) & 0x01 ) << 7 ) \
| ( ( ( high / 100 ) & 0x01 ) << 6 ) \
| ( ( ( high / 10 ) & 0x01 ) << 5 ) \
| ( ( ( high / 1 ) & 0x01 ) << 4 ) \
| ( ( ( low / 1000 ) & 0x01 ) << 3 ) \
| ( ( ( low / 100 ) & 0x01 ) << 2 ) \
| ( ( ( low / 10 ) & 0x01 ) << 1 ) \
| ( ( ( low / 1 ) & 0x01 ) << 0 ) )

#define u8_max 0xff
#define u16_max 0xffff
#define u32_max 0xffffffff
#define u64_max 0xffffffffffffffff

#define s8_max 0x7f
#define s8_min ( -0x7f - 1 )
#define s16_max 0x7fff
#define s16_min ( -0x7fff - 1 )
#define s32_max 0x7fffffff
#define s32_min ( -0x7fffffff - 1 )
#define s64_max 0x7fffffffffffffff
#define s64_min ( -0x7fffffffffffffff - 1 )

#define f32_epsilon 1.192092896e-07F
#define f32_max 3.402823466e+38F
#define f32_min 1.175494351e-38F

#define f64_epsilon 2.2204460492503131e-016
#define f64_max 1.7976931348623158e+308
#define f64_min 2.2250738585072014e-308

#define umm_max ( ( ( umm ) 0 ) - 1 )
#if defined( TARGET_x64 )
#define smm_max ( ( smm ) 0x7fffffffffffffff )
#define smm_min ( ( smm ) 0x8000000000000000 )
#elif defined( TARGET_x32 )
#define smm_max ( ( smm ) 0x7fffffff )
#define smm_min ( ( smm ) 0x80000000 )
#else
#error Target architecture not defined.
#endif

#define pointer_to_integer( pointer, type ) ( type ) ( umm ) ( pointer )
#define integer_to_pointer( integer ) ( void* ) ( umm ) ( integer )
#define pointer_add_offset( type, pointer, offset ) cast( type*, cast( u8*, ( pointer ) ) + ( offset ) )
#define pointer_sub_offset( type, pointer, offset ) cast( type*, cast( u8*, ( pointer ) ) - ( offset ) )

#define array_count( array ) ( ( umm ) ( sizeof( array ) / sizeof( ( array )[ 0 ] ) ) )
#define array_count_field( type, name ) array_count( ( ( type* ) 0 )->name )
#define u32_will_wrap( value, addin ) ( ( u32_max - ( value ) ) >= ( addin ) )
#define u64_will_wrap( value, addin ) ( ( u64_max - ( value ) ) >= ( addin ) )
#define swap_type( a, b, type ) { type __backup__ = ( a ); ( a ) = ( b ); ( b ) = __backup__; }
#define sizeof_field( type, field ) sizeof( ( ( type* ) 0 )->field )
#define offset_of( type, field ) ( ( umm ) ( &( ( type* ) 0 )->field ) )
#if !defined( __cplusplus )
#define align_of( type ) offset_of( struct { u8 a; type b; }, b )
#endif
#define not_used( something ) ( void ) something

#define no_error( error_pointer ) ( !deref( error_pointer ) )
#define is_error( error_pointer ) ( deref( error_pointer ) )
#if DEBUG
#define set_error( error_pointer, error_value ) safe_macro( ( deref( error_pointer ) = ( deref( error_pointer ) ? deref( error_pointer ) : error_value ) ); __debugbreak( ); )
#else
#define set_error( error_pointer, error_value ) ( deref( error_pointer ) = ( deref( error_pointer ) ? deref( error_pointer ) : error_value ) )
#endif
#define set_user_error( error_pointer, error_value ) ( deref( error_pointer ) = ( deref( error_pointer ) ? deref( error_pointer ) : error_value ) )
// #define set_error( error_pointer, error_value ) if ( no_error( error_pointer ) ) { deref( error_pointer ) = error_value; __debugbreak( ); }
#define clear_error( error_pointer ) ( deref( error_pointer ) = 0 )

#define kibioctet( value ) ( ( value ) * 0x400 )
#define mebioctet( value ) ( ( value ) * 0x100000 )
#define gibioctet( value ) ( ( value ) * 0x40000000 )
#define tebioctet( value ) ( ( value ) * 0x10000000000 )

#define deref( ptr ) ( *( ptr ) )
#define cast( type, value ) ( ( type ) ( value ) )
#define stringify( expression ) #expression
#define concatenate( a, b ) a ## b

#define temporary

#define scope_start_end_2( start, end, count ) \
for ( u32 concatenate( scope_start_end_i_, count ) = ( start, 1 ); \
concatenate( scope_start_end_i_, count ) ; \
concatenate( scope_start_end_i_, count )--, end )
#define scope_start_end_1( start, end, count )  scope_start_end_2( start, end, count )
#define scope_start_end( start, end ) scope_start_end_1( ( start ), ( end ), __COUNTER__ )

#define breakable_2( counter ) for ( u32 concatenate( breakable_, counter ) = 0; concatenate( breakable_, counter ) == 0; concatenate( breakable_, counter )++ )
#define breakable_1( counter ) breakable_2( counter )
#define breakable breakable_1( __COUNTER__ )
#define breakable_check( error ) if ( is_error( error ) ) { break; }

#define breakable_name( name )\
b32 breakable_##name##_skip = false;\
breakable_##name##_label:\
for ( ; 0 == breakable_##name##_skip++; )

#define break_name( name ) goto breakable_##name##_label;

#define defer_scope for ( int defer_first_time = 1, defer_count = 1, defer_id = 0; defer_first_time || defer_count; defer_first_time = 0, defer_id = 0, defer_count-- )
#define defer if ( defer_first_time ) defer_count++; defer_id++; if ( !defer_first_time && ( defer_id ) == defer_count )
#define now if ( defer_first_time ) 

#if defined( COMPILER_CL )
typedef union float_special_t {
    u32 i;
    f32 f;
} float_special_t;

/* NOTE simon: in C89 (and C++ ?) union can only initialize the first member.
https://www.ibm.com/support/knowledgecenter/SSLTBW_2.1.0/com.ibm.zos.v2r1.cbclx01/strin.htm
*/
#if 0

float_special_t IND0000 = { 0xffc00000 };
float_special_t QNANP = { 0x7f800000 };
float_special_t QNANN = { 0xff800000 };
float_special_t infP = { 0x7f800000 };
float_special_t infN = { 0xff800000 };

#else

float_special_t float_msvc_positive_indeterminate_form = { 0x7fc00000 }; /* NOTE simon: This is just NaN with only bit 23 set in the mantissa ? */
float_special_t float_msvc_negative_indeterminate_form = { 0xffc00000 }; /* NOTE simon: This is just NaN with only bit 23 set in the mantissa ? */
float_special_t float_quiet_positive_nan = { 0x7f800000 }; /* NOTE simon: First 23 can't be zero, but can be anything. */
float_special_t float_quiet_negative_nan = { 0xff800000 }; /* NOTE simon: First 23 can't be zero, but can be anything. */
float_special_t float_positive_infinity = { 0x7f800000 };
float_special_t float_negative_infinity = { 0xff800000 };

#define float_sign_mask 0x80000000
#define float_exponent_mask 0x7f800000
#define float_mantissa_mask 0x007fffff

b32 float_is_positive_nan( f32 f ) {
    
    float_special_t a;
    a.f = f;
    
    u32 sign_exponent = a.i & ( float_sign_mask | float_exponent_mask );
    u32 mantissa = a.i & float_mantissa_mask;
    
    b32 result = sign_exponent == float_quiet_positive_nan.i;
    result = result && ( mantissa != 0 );
    
    return result;
}

b32 float_is_negative_nan( f32 f ) {
    
    float_special_t a;
    a.f = f;
    
    u32 sign_exponent = a.i & ( float_sign_mask | float_exponent_mask );
    u32 mantissa = a.i & float_mantissa_mask;
    
    b32 result = sign_exponent == float_quiet_negative_nan.i;
    result = result && ( mantissa != 0 );
    
    return result;
}

b32 float_is_nan( f32 f ) {
    
    float_special_t a;
    a.f = f;
    
    u32 exponent = a.i & float_exponent_mask;
    u32 mantissa = a.i & float_mantissa_mask;
    
    b32 result = exponent == 0x7f800000;
    result = result && ( mantissa != 0 );
    
    return result;
}

b32 float_is_denormalized( f32 f ) {
    
    float_special_t a;
    a.f = f;
    
    u32 exponent = a.i & float_exponent_mask;
    u32 mantissa = a.i & float_mantissa_mask;
    
    b32 result = exponent == 0;
    result = result && ( mantissa != 0 );
    
    return result;
}

b32 f32_is_integer( f32 f ) {
    s32 i = cast( s32, f );
    f32 back = cast( f32, i );
    b32 result = ( back == f );
    return result;
}

#endif

#endif

#if 0
#include <utility>
template <typename f_t>
struct defer_t {
    defer_t( f_t f ) : f_t( f ) {}
    ~defer_t( ) { f( ); }
    f_t f;
};

template <typename f_t>
defer_t<f_t> make_defer( f_t f ) {
    return defer_t<f_t>( f );
};

#define __defer( line ) defer_ ## line
#define _defer( line ) __defer( line )

struct defer_dummy_t { };
template<typename f_t>
defer_t<f_t> operator+( defer_dummy_t, f_t&& f )
{
    return make_defer<f_t>( std::forward<f_t>( f ) );
}

#define defer auto _defer( __LINE__ ) = defer_dummy_t( ) + [ & ]( )
#endif

#define safe_macro( macro ) do { macro; } while ( 0 )

#if defined( _ASSERT )

#define IF_ASSERT( expression ) expression

#if defined( WINDOWS )

#define _assert( expression, ... ) safe_macro( if ( !( expression ) ) { __debugbreak( ); } )
#define debug_break( ) __debugbreak( )
#define invalid_code_path( ) __debugbreak( )

#elif defined( LINUX )

#if defined( __clang__ )
#define _assert( expression, ... ) safe_macro( if ( !( expression ) ) { __builtin_debugtrap( ); } )
#define debug_break( ) __builtin_debugtrap( )
#define invalid_code_path( ) __builtin_debugtrap( )
#endif /* __clang__ */

#endif /* WINDOWS LINUX */

#if !defined( _assert )
#define _assert( expression, ... ) safe_macro( if ( !( expression ) ) { __asm__( "int $3" ); } )
// #define _assert( expression ) safe_macro( if ( !( expression ) ) { *( ( uintptr_t* ) 0 ) = 0x00f00f00; } )
#endif /* _assert */

#if !defined( debug_break )
#define debug_break( ) safe_macro( deref( cast( uintptr_t*, 0 ) ) = 0x00f00f00; )
#endif /* debug_break */

#if !defined( invalid_code_path )
#define invalid_code_path( ) safe_macro( deref( cast( uintptr_t*, 0 ) ) = 0x00f00f00; )
#endif /* invalid_code_path */

#define breakpoint_line( ) { int x = 0; x++; }
#define IF_DEBUG( expression ) expression

#else /* _ASSERT */

#define IF_ASSERT( expression )
#define _assert( expression )
#define debug_break( )
#define invalid_code_path( )
#define breakpoint_line( )
#define IF_DEBUG( expression )

#endif /* _ASSERT */

#define _ctassert3( expression, count ) struct Foo##count { int foo[ ( expression ) ? 1 : -1 ]; }
#define _ctassert2( expression, count ) _ctassert3( expression, count )
#define _ctassert( expression ) _ctassert2( expression, __COUNTER__ )

_ctassert( u32_max + 1 == 0 );
_ctassert( 0u - 1 == u32_max );
_ctassert( umm_max + 1 == 0 );

#if defined( TARGET_x32 )
_ctassert( 0ul - 1 == umm_max );
#else
_ctassert( 0ull - 1 == umm_max );
#endif

#define math_min( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#define math_max( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#define math_clamp( min, value, max ) ( ( ( value ) < ( min ) ) ? ( min ) : ( ( value ) > ( max ) ) ? ( max ) : ( value ) )

#define for_p( type, pointer, count, name ) for ( type* name = ( pointer ), *name##_end = ( pointer ) + ( count ); name < name##_end; name++ )
#define for_pi( type, pointer, count, name ) umm name##_index = 0; for ( type* name = ( pointer ), *name##_end = ( pointer ) + ( count ); name < name##_end; name++, name##_index++ )

#define for_a( type, array, name ) for_p( type, ( array ), array_count( array ), name )
#define for_ai( type, array, name ) for_pi( type, ( array ), array_count( array ), name )

#define equal_any( value, ... ) internal_equal_any( value, ( umm [] ) { __VA_ARGS__ }, array_count( ( ( umm [] ) { __VA_ARGS__ } ) ) )

stu b32 internal_equal_any( umm value, umm* array, umm count ) {
    
    b32 result = false;
    
    for ( umm i = 0; !result && i < count; i++ ) {
        result = ( array[ i ] == value );
    }
    
    return result;
}

/* MARK flags */

/* NOTE simon: Storing a lot of enum values in a small amount of space (from Ryan Fleury)

enum EntityPropertiy
{
  EntityProperty_RenderSprite,
  EntityProperty_ChasePlayer,
  EntityProperty_ControlledByUser,
  EntityProperty_HasPhysicsShape,
  // etc.
  EntityProperty_COUNT
};

struct Entity
{
  U64 properties[(EntityProperty_COUNT + 63) / 64];
  // other data...
};

internal b32
EntityHasProperty(Entity *entity, EntityProperty property)
{
    return !!(entity->properties[property / 64] & (1ull << (property % 64)));
}

internal Entity *
SetEntityProperty(Entity *entity, EntityProperty property)
{
    entity->properties[property / 64] |= 1ull << (property % 64);
    return entity;
}

*/

stu u8 flag_toggle_u8( u8* flags, u8 mask ) {
    u8 current = ( *flags ) & mask;
    u8 result = ( ~current ) & mask;
    ( *flags ) = ( ( *flags ) & ( ~mask ) ) | result;
    return result;
}

stu u16 flag_toggle_u16( u16* flags, u16 mask ) {
    u16 current = ( *flags ) & mask;
    u16 result = ( ~current ) & mask;
    ( *flags ) = ( ( *flags ) & ( ~mask ) ) | result;
    return result;
}

stu u32 flag_toggle_u32( u32* flags, u32 mask ) {
    u32 current = ( *flags ) & mask;
    u32 result = ( ~current ) & mask;
    ( *flags ) = ( ( *flags ) & ( ~mask ) ) | result;
    return result;
}

stu u64 flag_toggle_u64( u64* flags, u64 mask ) {
    u64 current = ( *flags ) & mask;
    u64 result = ( ~current ) & mask;
    ( *flags ) = ( ( *flags ) & ( ~mask ) ) | result;
    return result;
}

/* TODO simon: Rename to flag_are_set_xx ? */
stu u8 flag_is_set_u8( u8 flags, u8 mask ) {
    u8 result = ( ( flags & mask ) == mask ) ? 1 : 0;
    return result;
}

stu u8 flag_is_set_u16( u16 flags, u16 mask ) {
    u8 result = ( ( flags & mask ) == mask ) ? 1 : 0;
    return result;
}

stu u8 flag_is_set_u32( u32 flags, u32 mask ) {
    u8 result = ( ( flags & mask ) == mask ) ? 1 : 0;
    return result;
}

stu u8 flag_is_set_u64( u64 flags, u64 mask ) {
    u8 result = ( ( flags & mask ) == mask ) ? 1 : 0;
    return result;
}

stu u8 flag_is_any_set_u8( u8 flags, u8 mask ) {
    u8 result = ( ( flags & mask ) != 0 ) ? 1 : 0;
    return result;
}

stu u8 flag_is_any_set_u16( u16 flags, u16 mask ) {
    u8 result = ( ( flags & mask ) != 0 ) ? 1 : 0;
    return result;
}

stu u8 flag_is_any_set_u32( u32 flags, u32 mask ) {
    u8 result = ( ( flags & mask ) != 0 ) ? 1 : 0;
    return result;
}

stu u8 flag_is_any_set_u64( u64 flags, u64 mask ) {
    u8 result = ( ( flags & mask ) != 0 ) ? 1 : 0;
    return result;
}

stu void flag_set_u8( u8* flags, u8 mask ) {
    ( *flags ) |= mask;
}

stu void flag_set_u16( u16* flags, u16 mask ) {
    ( *flags ) |= mask;
}

stu void flag_set_u32( u32* flags, u32 mask ) {
    ( *flags ) |= mask;
}

stu void flag_set_u64( u64* flags, u64 mask ) {
    ( *flags ) |= mask;
}

stu void flag_clear_u8( u8* flags, u8 mask ) {
    ( *flags ) &= ( ~mask );
}

stu void flag_clear_u16( u16* flags, u16 mask ) {
    ( *flags ) &= ( ~mask );
}

stu void flag_clear_u32( u32* flags, u32 mask ) {
    ( *flags ) &= ( ~mask );
}

stu void flag_clear_u64( u64* flags, u64 mask ) {
    ( *flags ) &= ( ~mask );
}

stu void flag_set_or_clear_u8( u8* flags, b32 set, u8 mask ) {
    if ( set ) {
        flag_set_u8( flags, mask );
    } else {
        flag_clear_u8( flags, mask );
    }
}

stu void flag_set_or_clear_u16( u16* flags, b32 set, u16 mask ) {
    if ( set ) {
        flag_set_u16( flags, mask );
    } else {
        flag_clear_u16( flags, mask );
    }
}

stu void flag_set_or_clear_u32( u32* flags, b32 set, u32 mask ) {
    if ( set ) {
        flag_set_u32( flags, mask );
    } else {
        flag_clear_u32( flags, mask );
    }
}

stu void flag_set_or_clear_u64( u64* flags, b32 set, u64 mask ) {
    if ( set ) {
        flag_set_u64( flags, mask );
    } else {
        flag_clear_u64( flags, mask );
    }
}

stu u8 flag_only_allowed_u8( u8 flags, u8 allowed ) {
    
    u8 mask = ~allowed;
    u8 result = ( flags & mask ) == 0;
    return result;
}

stu u16 flag_only_allowed_u16( u16 flags, u16 allowed ) {
    
    u16 mask = ~allowed;
    u16 result = ( flags & mask ) == 0;
    return result;
}

stu u32 flag_only_allowed_u32( u32 flags, u32 allowed ) {
    
    u32 mask = ~allowed;
    u32 result = ( flags & mask ) == 0;
    return result;
}

stu u64 flag_only_allowed_u64( u64 flags, u64 allowed ) {
    
    u64 mask = ~allowed;
    u64 result = ( flags & mask ) == 0;
    return result;
}

stu u8 flag_required_allowed_u8( u8 flags, u8 required, u8 allowed ) {
    
    u8 result = 1;
    
    if ( required ) {
        result = flag_is_set_u8( flags, required );
        allowed |= required;
    }
    
    if ( allowed ) {
        result = result && flag_only_allowed_u8( flags, allowed );
    } else {
        result = ( flags == 0 );
    }
    
    return result;
}

stu u16 flag_required_allowed_u16( u16 flags, u16 required, u16 allowed ) {
    
    u16 result = 1;
    
    if ( required ) {
        result = flag_is_set_u16( flags, required );
        allowed |= required;
    }
    
    if ( allowed ) {
        result = result && flag_only_allowed_u16( flags, allowed );
    } else {
        result = ( flags == 0 );
    }
    
    return result;
}

stu u32 flag_required_allowed_u32( u32 flags, u32 required, u32 allowed ) {
    
    u32 result = 1;
    
    if ( required ) {
        result = flag_is_set_u32( flags, required );
        allowed |= required;
    }
    
    if ( allowed ) {
        result = result && flag_only_allowed_u32( flags, allowed );
    } else {
        result = ( flags == 0 );
    }
    
    return result;
}

stu u64 flag_required_allowed_u64( u64 flags, u64 required, u64 allowed ) {
    
    u64 result = 1;
    
    if ( required ) {
        result = flag_is_set_u64( flags, required );
        allowed |= required;
    }
    
    if ( allowed ) {
        result = result && flag_only_allowed_u64( flags, allowed );
    } else {
        result = ( flags == 0 );
    }
    
    return result;
}

typedef struct data_t {
    
    umm reserved;
    umm used;
    
    union {
        u8* bytes;
        char* text;
        void* raw;
        umm offset;
    };
    
} data_t;

#define data_t_alignment sizeof_field( data_t, reserved )

stu data_t memory_construct( umm reserved, umm used, void* memory ) {
    
    data_t result;
    result.reserved = reserved;
    result.used = used;
    result.raw = memory;
    
    return result;
}

// MARK memory

u32 memory_page_size = 0x1000;
u32 memory_allocation_granularity = 0x10000;
void* memory_minimum_address = cast( void*, 0x10000 ); /* NOTE simon: windows reserve the first 16K to use as handles. */
void* memory_maximum_address = cast( void*, tebioctet( 128 ) - 1 ); /* NOTE simon: windows 7 should be tebioctet( 8 ) - 1. */

#if 0
#define memory_pointer_add_offset( pointer, offset ) ( pointer ) = cast( void*, cast( u8*, pointer ) + ( offset ) )
#define memory_pointer_sub_offset( pointer, offset ) ( pointer ) = cast( void*, cast( u8*, pointer ) - ( offset ) )
#else
/* NOTE simon: Use pointer_add_offset pointer_sub_offset instead ? */
#define memory_pointer_add_offset( pointer, offset ) debug_break( )
#define memory_pointer_sub_offset( pointer, offset ) debug_break( )
#endif

/* NOTE simon: from https://en.wikipedia.org/wiki/Data_structure_alignment#Computing_padding */
stu umm memory_get_padding( void* address, umm alignment ) {
    
    umm a = cast( umm, address );
    umm padding = ( ( ~( a - 1 ) ) & ( alignment - 1 ) );
    return padding;
}

stu void* memory_get_aligned( void* address, umm alignment ) {
    
    umm a = ( umm ) address;
    void* result = cast( void*, ( a + ( alignment - 1 ) ) & ( ~( alignment - 1 ) ) );
    return result;
}

#if defined( WINDOWS )

stu void memory_get_info( u32* page_size, u32* allocation_granularity, void* minimum_address, void* maximum_address ) {
    
#if 0
    typedef struct _SYSTEM_INFO {
        union {
            DWORD  dwOemId;
            struct {
                WORD wProcessorArchitecture;
                WORD wReserved;
            };
        };
        DWORD     dwPageSize;
        LPVOID    lpMinimumApplicationAddress;
        LPVOID    lpMaximumApplicationAddress;
        DWORD_PTR dwActiveProcessorMask;
        DWORD     dwNumberOfProcessors;
        DWORD     dwProcessorType;
        DWORD     dwAllocationGranularity;
        WORD      wProcessorLevel;
        WORD      wProcessorRevision;
    } SYSTEM_INFO;
#endif
    
    SYSTEM_INFO info;
    
    GetSystemInfo( &info );
    
    if ( page_size ) {
        *page_size = info.dwPageSize;
    }
    
    if ( allocation_granularity ) {
        *allocation_granularity = info.dwAllocationGranularity;
    }
    
    if ( minimum_address ) {
        /* NOTE simon: should be 65536 */
        deref( cast( u64**, minimum_address ) ) = cast( u64*, info.lpMinimumApplicationAddress );
    }
    
    if ( maximum_address ) {
        /* NOTE simon: should be 0x7fff fffe ffff and only a single byte can be reserved. */
        deref( cast( u64**, maximum_address ) ) = cast( u64*, info.lpMaximumApplicationAddress );
    }
}

stu umm memory_get_offset( void* start, void* end ) {
    
    umm result = cast( umm, cast( u8*, end ) - cast( u8*, start ) );
    return result;
}

stu b32 memory_is_address_in_block( data_t* block, void* address ) {
    void* start = cast( void*, block->bytes );
    void* end = cast( void*, block->bytes + block->reserved );
    b32 result = ( address >= start && address < end );
    return result;
}

/* NOTE simon: Virtual alloc rounds to page boundaries. So it may return more space then requested. Also pages need to be 64 K aligned (allocation granularity) so requesting a 4k page wastes 60k of memory. */
#if defined( DEBUG )
umm debug_total_virtual_memory = 0;
#endif

stu data_t memory_get_virtual( umm size, umm address ) {
    
    data_t memory = { 0 };
    memory.reserved = size;
    memory.bytes = cast( u8*, VirtualAlloc( ( LPVOID ) address, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE ) );
#if defined( DEBUG )
    if ( memory.bytes ) {
        debug_total_virtual_memory += size;
    }
#endif
    return memory;
}

stu void memory_free_virtual( data_t* memory ) {
    
    if ( memory->bytes != 0 ) {
        VirtualFree( memory->bytes, 0, MEM_RELEASE );
        memory->bytes = 0;
        memory->reserved = memory->used = 0;
#if defined( DEBUG )
        debug_total_virtual_memory -= memory->reserved;
#endif
    }
}

/* NOTE simon: Address will be rounded down to match the allocation granularity. The size will be rounded up to allocation granularity. */
stu data_t memory_reserve( umm size, umm address ) {
    
    size += memory_get_padding( cast( void*, size ), memory_allocation_granularity );
    
    data_t memory;
    memory.raw = VirtualAlloc( ( LPVOID ) address, size, MEM_RESERVE, PAGE_READWRITE );
    memory.reserved = size;
    memory.used = 0;
    
    _assert( memory.raw );
    
    return memory;
}

/* NOTE simon: Size will be rounded up to allocation granularity. */
stu void* memory_commit( data_t* memory, umm size ) {
    
    void* result = 0;
    
    size += memory_get_padding( cast( void*, size ), memory_page_size );
    
    if ( memory->used + size <= memory->reserved ) {
        
        result = VirtualAlloc( memory->bytes + memory->used, size, MEM_COMMIT, PAGE_READWRITE );
        
        if ( result ) {
            memory->used += size;
        }
    }
    
    return result;
}

/* NOTE simon: Size will be rounded up to allocation granularity. */
stu b32 memory_decommit( data_t* memory, umm size ) {
    
    size += memory_get_padding( cast( void*, size ), memory_page_size );
    _assert( memory->used >= size );
    _assert( memory_get_padding( memory->bytes + memory->used, memory_page_size ) == 0 );
    b32 result = VirtualFree( memory->bytes + memory->used - size, size, MEM_DECOMMIT );
    memory->used -= size;
    
    _assert( result );
    return result;
}

#elif defined LINUX

stu data_t memory_get_virtual( umm size, umm address ) {
    
    data_t memory = { };
    memory.reserved = size;
    memory.bytes = cast( u8*, mmap( cast( void*, address ), size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 ) );
    return memory;
}

stu void memory_free_virtual( data_t* memory ) {
    
    if ( memory->bytes != 0 ) {
        munmap( memory->bytes, memory->reserved );
    }
}

stu data_t memory_reserve( umm size, umm address ) {
    
    size += memory_get_padding( cast( void*, size ), memory_allocation_granularity );
    
    data_t memory;
    memory.raw = mmap( cast( void*, address ), size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
    memory.reserved = size;
    memory.used = 0;
    
    _assert( memory.raw );
    
    return memory;
}

stu void* memory_commit( data_t* memory, umm size ) {
    
    size += memory_get_padding( cast( void*, size ), memory_page_size );
    _assert( memory->used + size <= memory->reserved );
    b32 failure = mprotect( memory->bytes + memory->used, size, PROT_READ | PROT_WRITE );
    void* result = memory->bytes + memory->used;
    memory->used += size;
    
    _assert( failure == 0 );
    
    return result;
}

/* TODO simon: This is not a proper implementation as it it keeps memory around. */
stu b32 memory_decommit( data_t* memory, umm size ) {
    
    size += memory_get_padding( cast( void*, size ), memory_page_size );
    _assert( memory->used >= size );
    _assert( memory_get_padding( memory->bytes + memory->used, memory_page_size ) == 0 );
    b32 result = mprotect( memory->bytes + memory->used - size, size, PROT_NONE );
    memory->used -= size;
    
    _assert( result == 0 );
    return result;
}

#else
#error Unsupported platform.
#endif

#define memory_get_on_stack( name, size ) u8 name_##name##_stack_memory[ size ]; data_t name = memory_construct( size, 0, name_##name##_stack_memory )

#define raw_string_l( string ) { sizeof( string ) /* reserved */, sizeof( string ) - 1 /* used */, cast( u8*, string ) /* text */ }
#define string_l( string ) ( ( data_t ) raw_string_l( string ) )

/* TODO simon: Test for alignment with the custom allocator because as far as I know everything could be miss-aligned. */
umm memory_default_alignment = sizeof( umm );

stu data_t memory_get_block( data_t* memory, umm size, umm alignment ) {
    
    data_t block = { 0 };
    umm use_alignment = alignment;
    
    _assert( alignment == 0 || alignment == 1 || alignment == 2 || alignment == 4 || alignment == 8 || alignment == 16 || alignment == 32 || alignment == 64 );
    
    if ( alignment == 0 ) {
        use_alignment = memory_default_alignment;
    }
    
    umm padding = memory_get_padding( memory->bytes + memory->used, use_alignment );
    
    _assert( memory->used + size + padding <= memory->reserved );
    
    if ( memory->used + size + padding <= memory->reserved ) {
        block.bytes = memory->bytes + padding + memory->used;
        memory->used += ( size + padding );
        block.reserved = size;
    }
    
    return block;
}

stu data_t memory_get_block_remaining( data_t* memory, umm alignment ) {
    
    data_t block = { 0 };
    
    umm use_alignment = alignment;
    
    _assert( alignment == 0 || alignment == 1 || alignment == 2 || alignment == 4 || alignment == 8 || alignment == 16 || alignment == 32 || alignment == 64 );
    
    if ( alignment == 0 ) {
        use_alignment = memory_default_alignment;
    }
    
    umm padding = memory_get_padding( memory->bytes + memory->used, use_alignment );
    
    if ( memory->used + padding <= memory->reserved ) {
        block.bytes = memory->bytes + memory->used + padding;
        block.reserved = memory->reserved - memory->used - padding;
        memory->used = memory->reserved;
    }
    
    return block;
}

stu data_t memory_get_temp( data_t memory ) {
    
    data_t result = { 0 };
    umm padding = memory_get_padding( memory.bytes + memory.used, 64 );
    
    if ( memory.used + padding <= memory.reserved ) {
        result.bytes = memory.bytes + memory.used + padding;
        result.reserved = memory.reserved - memory.used - padding;
    }
    
    return result;
}

stu data_t memory_get_temp_unaligned( data_t memory ) {
    
    data_t result = { 0 };
    
    if ( memory.used <= memory.reserved ) {
        result.bytes = memory.bytes + memory.used;
        result.reserved = memory.reserved - memory.used;
    }
    
    return result;
}

#define memory_from_array( array ) memory_construct( sizeof( array ), 0, ( array ) )

stu void memory_invalidate( data_t* memory ) {
    memory->used = memory->reserved = 0;
    memory->raw = 0;
}

stu void memory_free( data_t* memory ) {
    memory->used = 0;
}

stu umm memory_tight( data_t* memory ) {
    umm result = memory->reserved - memory->used;
    memory->reserved = memory->used;
    return result;
}

stu data_t memory_start_size( void* start, umm size ) {
    
    data_t result;
    result.reserved = result.used = size;
    result.raw = start;
    
    return result;
}

stu data_t memory_start_end( void* start, void* end ) {
    
    data_t result;
    result.reserved = result.used = cast( u8*, end ) - cast( u8*, start );
    result.bytes = cast( u8*, start );
    
    return result;
}

#define memory_string_array( array ) memory_start_size( array, sizeof( array ) - 1 )

stu u8* memory_end( data_t memory ) {
    u8* result = memory.bytes + memory.used;
    return result;
}

stu u8* memory_block_end( data_t memory ) {
    u8* result = memory.bytes + memory.reserved;
    return result;
}

stu void memory_set_used( data_t* memory, void* end_arg ) {
    
    u8* end = cast( u8*, end_arg );
    umm used = ( umm ) ( end - memory->bytes );
    _assert( used <= memory->reserved );
    
    memory->used = used;
}

/* NOTE simon: Alignment for a structure should be to the size of its biggest member. If it contains a "sub struct" the member of the sub struct must be considered. Source: http://www.catb.org/esr/structure-packing/. */

#define memory_push_type( memory, type ) cast( type*, memory_push_size( ( memory ), sizeof( type ), sizeof( type ) ) )
#define memory_push_struct( memory, type ) cast( type*, memory_push_size( ( memory ), sizeof( type ), type##_alignment ) )
#define memory_push_struct_aligned( memory, type, alignment ) cast( type*, memory_push_size( ( memory ), sizeof( type ), ( alignment ) ) )
#define memory_push_array( memory, type, count ) cast( type*, memory_push_size( ( memory ), ( count ) * sizeof( type ), type##_alignment ) )
#define memory_push_array_aligned( memory, type, count, alignment ) cast( type*, memory_push_size( ( memory ), ( count ) * sizeof( type ), ( alignment ) ) )

stu void* memory_push_size( data_t* memory, umm size, umm alignment ) {
    
    void* result = 0;
    
    umm use_alignment = alignment;
    
    _assert( alignment == 0 || alignment == 1 || alignment == 2 || alignment == 4 || alignment == 8 );
    
    if ( alignment == 0 ) {
        use_alignment = memory_default_alignment;
    }
    
    umm padding = memory_get_padding( memory->bytes + memory->used, use_alignment );
    
#if 0
    _assert( memory->used + size + padding <= memory->reserved );
#endif
    
    if ( memory->used + size + padding <= memory->reserved ) {
        result = memory->bytes + memory->used + padding;
        memory->used += size + padding;
    }
    
    return result;
}

#define memory_could_push_struct( memory, type ) memory_could_push_size( ( memory ), sizeof( type ), type##_alignment )
#define memory_could_push_struct_aligned( memory, type, alignment ) memory_could_push_size( ( memory ), sizeof( type ), ( alignment ) )

stu b32 memory_could_push_size( data_t* memory, umm size, umm alignment ) {
    
    b32 result = false;
    umm use_alignment = alignment;
    
    _assert( alignment == 0 || alignment == 1 || alignment == 2 || alignment == 4 || alignment == 8 );
    
    if ( alignment == 0 ) {
        use_alignment = memory_default_alignment;
    }
    
    umm padding = memory_get_padding( memory->bytes + memory->used, use_alignment );
    result = ( memory->used + size + padding <= memory->reserved );
    return result;
}

#define memory_as( memory, type ) cast( type*, ( memory ).bytes )
#define memory_as_aligned( memory, type, alignment ) cast( type*, memory_get_aligned( ( memory ).bytes, ( alignment ) ) )
#define memory_array( memory, type, index ) ( cast( type*, ( memory ).bytes )[ ( index ) ] )
#define memory_array_length( memory, type ) cast( umm, ( memory ).used / sizeof( type ) )
#define memory_current( memory, type ) cast( type*, ( memory ).bytes + ( memory ).used )
#define memory_current_aligned( memory, type, alignment ) cast( type*, memory_get_aligned( ( memory ).bytes + ( memory ).used, ( alignment ) ) )

#define memory_left( memory ) ( ( memory ).reserved - ( memory ).used )

stu u8* memory_align( data_t* memory, umm alignment ) {
    
    umm padding = memory_get_padding( memory_current( deref( memory ), u8 ), alignment );
    memory_push_size( memory, padding, 1 );
    
    return memory_current( deref( memory ), u8 );
}

stu u8* memory_push_u8( data_t* memory, u8 number, umm alignment ) {
    
    if ( alignment == 0 ) {
        alignment = sizeof( number );
    }
    
    u8* n = cast( u8*, memory_push_size( memory, sizeof( number ), alignment ) );
    
    if ( n ) {
        deref( n ) = number;
    }
    
    return n;
}

stu s8* memory_push_s8( data_t* memory, s8 number, umm alignment ) {
    
    if ( alignment == 0 ) {
        alignment = sizeof( number );
    }
    
    s8* n = cast( s8*, memory_push_size( memory, sizeof( number ), alignment ) );
    
    if ( n ) {
        *n = number;
    }
    return n;
}

stu u16* memory_push_u16( data_t* memory, u16 number, umm alignment ) {
    
    if ( alignment == 0 ) {
        alignment = sizeof( number );
    }
    
    u16* n = cast( u16*, memory_push_size( memory, sizeof( number ), alignment ) );
    
    if ( n ) {
        *n = number;
    }
    
    return n;
}

stu s16* memory_push_s16( data_t* memory, s16 number, umm alignment ) {
    
    if ( alignment == 0 ) {
        alignment = sizeof( number );
    }
    
    s16* n = cast( s16*, memory_push_size( memory, sizeof( number ), alignment ) );
    
    if ( n ) {
        *n = number;
    }
    
    return n;
}

stu u32* memory_push_u32( data_t* memory, u32 number, umm alignment ) {
    
    if ( alignment == 0 ) {
        alignment = sizeof( number );
    }
    
    u32* n = cast( u32*, memory_push_size( memory, sizeof( number ), alignment ) );
    
    if ( n ) {
        *n = number;
    }
    
    return n;
}

stu s32* memory_push_s32( data_t* memory, s32 number, umm alignment ) {
    
    if ( alignment == 0 ) {
        alignment = sizeof( number );
    }
    
    s32* n = cast( s32*, memory_push_size( memory, sizeof( number ), alignment ) );
    
    if ( n ) {
        *n = number;
    }
    
    return n;
}

stu u64* memory_push_u64( data_t* memory, u64 number, umm alignment ) {
    
    if ( alignment == 0 ) {
        alignment = sizeof( number );
    }
    
    u64* n = cast( u64*, memory_push_size( memory, sizeof( number ), alignment ) );
    
    if ( n ) {
        *n = number;
    }
    
    return n;
}

stu s64* memory_push_s64( data_t* memory, s64 number, umm alignment ) {
    
    if ( alignment == 0 ) {
        alignment = sizeof( number );
    }
    
    s64* n = cast( s64*, memory_push_size( memory, sizeof( number ), alignment ) );
    
    if ( n ) {
        *n = number;
    }
    
    return n;
}

stu umm* memory_push_umm( data_t* memory, umm number, umm alignment ) {
    
    if ( alignment == 0 ) {
        alignment = sizeof( number );
    }
    
    umm* n = cast( umm*, memory_push_size( memory, sizeof( number ), alignment ) );
    
    if ( n ) {
        *n = number;
    }
    
    return n;
}

stu smm* memory_push_smm( data_t* memory, smm number, umm alignment ) {
    
    if ( alignment == 0 ) {
        alignment = sizeof( number );
    }
    
    smm* n = cast( smm*, memory_push_size( memory, sizeof( number ), alignment ) );
    
    if ( n ) {
        *n = number;
    }
    
    return n;
}

#define memory_pop_array( memory, type, count ) memory_pop_size( ( memory ), sizeof( type ) * ( count ) )
#define memory_pop_struct( memory, type ) memory_pop_size( ( memory ), sizeof( type ) )
#define memory_pop_type( memory, type ) memory_pop_size( ( memory ), sizeof( type ) )

stu void memory_pop_size( data_t* memory, umm size ) {
    memory->used -= size;
}

#define memory_zero_block( block ) memory_zero( ( block )->bytes, ( block )->reserved )
#define memory_zero_array( memory, type, count ) memory_zero( cast( u8*, memory ), ( count ) * sizeof( type ) )
#define memory_zero_struct( pointer ) memory_zero( cast( u8*, pointer ), sizeof( *( pointer ) ) )

stu void memory_zero( void* bytes, umm size ) {
    
    u8* b = cast( u8*, bytes );
    
    while ( size > 0 ) {
        size--;
        *b++ = 0;
    }
}

stu void memory_set( void* bytes, umm size, u8 value ) {
    
    u8* b = cast( u8*, bytes );
    
    while ( size > 0 ) {
        size--;
        *b++ = value;
    }
}

stu void memory_set_d( data_t* memory, u8 value ) {
    memory_set( memory->bytes, memory->reserved, value );
}

stu void memory_advance( data_t* memory, umm advance_byte_count ) {
    
    if ( memory->reserved >= advance_byte_count ) {
        memory->bytes += advance_byte_count;
        memory->used -= math_min( advance_byte_count, memory->used );
        memory->reserved -= advance_byte_count;
    }
}

stu data_t memory_advanced( data_t memory, umm advance_byte_count ) {
    memory_advance( &memory, advance_byte_count );
    return memory;
}

#if 0
// dst and src must be 16-byte aligned
// size must be multiple of 16*2 = 32 bytes
static void CopyWithSSENoCache(uint8_t* dst, uint8_t* src, size_t size)
{
    size_t stride = 2 * sizeof(__m128);
    while (size)
    {
        __m128 a = _mm_load_ps((float*)(src + 0*sizeof(__m128)));
        __m128 b = _mm_load_ps((float*)(src + 1*sizeof(__m128)));
        _mm_stream_ps((float*)(dst + 0*sizeof(__m128)), a);
        _mm_stream_ps((float*)(dst + 1*sizeof(__m128)), b);
        
        size -= stride;
        src += stride;
        dst += stride;
    }
}

#endif

stu void memory_copy_p( void* destination, void* source, umm size ) {
    
    _assert( destination || size == 0 );
    _assert( source || size == 0 );
    
    u8* d = cast( u8*, destination );
    u8* s = cast( u8*, source );
    u8* s_end = s + size;
    
    while ( s < s_end ) {
        deref( d ) = deref( s );
        d++;
        s++;
    }
}

stu void memory_copy( data_t* destination, data_t source ) {
    _assert( destination->reserved >= source.used );
    memory_copy_p( destination->bytes, source.bytes, source.used );
    destination->used = source.used;
}

stu void memory_copy_from_end_p( void* destination, void* source, umm size ) {
    
    _assert( destination || size == 0 );
    _assert( source || size == 0 );
    
    if ( size ) {
        
        u8* d = cast( u8*, destination ); 
        u8* d_end = d + size - 1;
        u8* s_end = cast( u8*, source ) + size - 1;
        
        while ( d_end >= d ) {
            *d_end = *s_end;
            d_end--;
            s_end--;
        }
    }
}

stu void memory_copy_from_end( data_t* destination, data_t source ) {
    _assert( destination->reserved <= source.used );
    memory_copy_from_end_p( destination->bytes, source.bytes, source.used );
    destination->used = source.used;
}

stu void memory_replace_byte( data_t memory, void* search, void* replace ) {
    
    u8* s = cast( u8*, search );
    u8* r = cast( u8*, replace );
    
    umm search_count = 0;
    umm replace_count = 0;
    
    while ( s[ search_count ] ) {
        search_count++;
    }
    
    while ( r[ replace_count ] ) {
        replace_count++;
    }
    
    _assert( search_count == replace_count );
    
    for ( umm i = 0; i < memory.used; i++ ) {
        
        for ( umm j = 0; j < search_count; j++ ) {
            if ( memory.bytes[ i ] == s[ j ] ) {
                memory.bytes[ i ] = r[ j ];
            }
        }
    }
}

stu void* memory_push_copy_p( data_t* memory, void* source, umm size, umm alignment ) {
    
    void* destination = 0;
    
    if ( size ) {
        
        destination = memory_push_size( memory, size, alignment );
        
        if ( destination ) {
            memory_copy_p( destination, source, size );
        }
    }
    
    return destination;
}

stu void* memory_push_copy( data_t* memory, data_t source, umm alignment ) {
    
    void* destination = 0;
    
    if ( source.used ) {
        
        destination = memory_push_size( memory, source.used, alignment );
        
        if ( destination ) {
            memory_copy_p( destination, source.bytes, source.used );
        }
    }
    
    return destination;
}

#define memory_push_copy_l( memory, string ) memory_push_copy_p( ( memory ), ( string ), sizeof( string ) - 1, 1 )

stu umm memory_push_copy_region_d( data_t* memory, data_t* source, umm start_offset, umm size, b32 size_from_end ) {
    
    umm result = 0;
    
    if ( start_offset <= source->used ) {
        
        umm copy_size = source->used - start_offset;
        
        if ( size ) {
            
            if ( size_from_end ) {
                
                if ( size < copy_size ) {
                    size = copy_size - size;
                } else {
                    size = 0;
                }
                
            } else {
                size = ( size <= copy_size ) ? size : copy_size;
            }
            
        } else {
            size = copy_size;
        }
        
        umm left = memory_left( *memory );
        
        if ( size <= left ) {
            memory_push_copy_p( memory, source->bytes + start_offset, size, 1 );
            result = size;
        }
    }
    
    return result;
}

stu b32 memory_replace_range_will_succeed( data_t* memory, u8* start, u8* end, data_t to_insert ) {
    
    _assert( start <= end );
    _assert( start >= memory->bytes && start <= memory->bytes + memory->used );
    _assert( end >= memory->bytes && end <= memory->bytes + memory->used );
    b32 result = false;
    
    umm range_size = end - start;
    result = ( range_size >= to_insert.used );
    result = result || ( ( to_insert.used - range_size ) <= memory_left( deref( memory ) ) );
    
    return result;
}

/* NOTE simon: Replace a range of memory contained in 'memory' defined by 'start' and 'end' with the content of 'to_instert',
Conceptually, you can consider this as cutting the memory between start and end, and than inserting the text. */
stu b32 memory_replace_range( data_t* memory, u8* start, u8* end, data_t to_insert ) {
    
    _assert( start <= end );
    _assert( start >= memory->bytes && start <= memory->bytes + memory->used );
    _assert( end >= memory->bytes && end <= memory->bytes + memory->used );
    b32 result = false;
    
    breakable {
        
        u8* buffer_end = memory_end( deref( memory ) );
        u8* from = end;
        u8* to = start + to_insert.used;
        umm size = buffer_end - from;
        
        umm range_size = end - start;
        
        if ( range_size < to_insert.used ) {
            
            umm left = memory_left( deref( memory ) );
            umm needed = to_insert.used - range_size;
            
            if ( needed > left ) {
                break;
            }
            
            memory_copy_from_end_p( to, from, size );
            memory->used += needed;
            
        } else if ( range_size > to_insert.used ) {
            
            memory_copy_p( to, from, size ); 
            memory->used -= range_size - to_insert.used;
        }
        
        memory_copy_p( start, to_insert.bytes, to_insert.used );
        
        result = true;
    }
    
    return result;
}

stu b32 memory_are_equal_p( void* a, umm a_length, void* b, umm b_length ) {
    
    _assert( ( ( a_length > 0 ) && a ) || a_length == 0 );
    _assert( ( ( b_length > 0 ) && b ) || b_length == 0 );
    
    b32 result = ( a_length == b_length );
    
    umm i = 0;
    u8* a_bytes = cast( u8*, a );
    u8* b_bytes = cast( u8*, b );
    
    while ( result && i < a_length ) {
        result = a_bytes[ i ] == b_bytes[ i ];
        i++;
    }
    
    return result;
}

stu b32 memory_are_equal( data_t a, data_t b ) {
    b32 result = memory_are_equal_p( a.bytes, a.used, b.bytes, b.used );
    return result;
}

stu b32 memory_starts_with_p( void* start_arg, void* end_arg, void* what_arg, umm what_length ) {
    
    _assert( start_arg );
    _assert( what_arg );
    
    u8* current = cast( u8*, start_arg );
    u8* end = current + what_length;
    
    b32 result = end <= cast( u8*, end_arg );
    u8* what = cast( u8*, what_arg );
    
    while ( result && current < end ) {
        result = ( *current ) == ( *what );
        current++;
        what++;
    }
    
    return result;
}

stu b32 memory_starts_with( data_t in, data_t what ) {
    b32 result = memory_starts_with_p( in.bytes, memory_end( in ), what.bytes, what.used );
    return result;
}

/* TODO simon: this search algorithm https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm */
stu void* memory_seek_p( void* start_param, void* end_param, void* what_param, umm what_size ) {
    
    u8* start = cast( u8*, start_param );
    u8* end = cast( u8*, end_param );
    umm in_size = end - start;
    u8* what = cast( u8*, what_param );
    u8* result = end;
    
    if ( start && end && start < end && what_size > 0 && what_size <= in_size ) {
        
        end -= ( what_size - 1 );
        
        while ( start < end ) {
            
            u8* bytes = start;
            umm size = 0;
            
            while ( size < what_size && bytes[ size ] == what[ size ] ) {
                size++;
            }
            
            if ( size == what_size ) {
                result = start;
                break;
            } else {
                start++;
            }
        }
    }
    
    return result;
}

stu void* memory_seek_backward_p( void* start_param, void* end_param, void* what_param, umm what_size ) {
    
    u8* start = cast( u8*, start_param );
    u8* end = cast( u8*, end_param );
    umm in_size = end - start;
    u8* what = cast( u8*, what_param );
    void* result = end;
    
    if ( start && end && start < end && what_size > 0 && what_size <= in_size ) {
        
        end -= what_size;
        
        while ( end >= start ) {
            
            u8* bytes = end;
            umm size = 0;
            
            while ( size < what_size && bytes[ size ] == what[ size ] ) {
                size++;
            }
            
            if ( size == what_size ) {
                result = end;
                break;
            } else {
                end--;
            }
        }
    }
    
    return result;
}

#define memory_seek( in, what ) memory_seek_p( ( in ).bytes, ( in ).bytes + ( in ).used, ( what ).bytes, ( what ).used )
#define memory_seek_backward( in, what ) memory_seek_backward_p( ( in ).bytes, ( in ).bytes + ( in ).used, ( what ).bytes, ( what ).used )

b32 memory_contains_any( data_t memory, data_t byte_list ) {
    
    b32 result = false;
    
    for ( umm i = 0; !result && i < memory.used; i++ ) {
        
        u8 byte = memory.bytes[ i ];
        
        for ( umm j = 0; !result && j < byte_list.used; j++ ) {
            result = ( byte == byte_list.bytes[ j ] );
        }
    }
    
    return result;
}

typedef struct memory_temp_t {
    data_t* memory;
    data_t backup;
} memory_temp_t;

stu memory_temp_t memory_temp_start( data_t* memory ) {
    
    memory_temp_t result = { 0 };
    result.memory = memory;
    result.backup = deref( memory );
    
    return result;
}

stu data_t memory_temp_commit( memory_temp_t* temp ) {
    
    _assert( temp->memory );
    
    data_t result = { 0 };
    result.bytes = memory_current( temp->backup, u8 );
    result.reserved = result.used = temp->memory->used - temp->backup.used;
    deref( temp ) = ( memory_temp_t ) { 0 };
    
    return result;
}

stu void memory_temp_revert( memory_temp_t* temp ) {
    deref( temp->memory ) = temp->backup;
    deref( temp ) = ( memory_temp_t ) { 0 };
}

#define array_is_element_in_range_and_aligned( array, element, type ) array_is_element_in_range_and_aligned_size( ( array ).bytes, memory_end( array ), ( u8* ) ( element ), sizeof( type ) )
#define array_is_element_in_range_and_aligned_p( start, end, element, type ) array_is_element_in_range_and_aligned_size( ( u8* ) ( start ), ( u8* ) ( end ), ( u8* ) ( element ), sizeof( type ) )

stu b32 array_is_element_in_range_and_aligned_size( u8* start, u8* end, u8* element, umm size ) {
    
    b32 in_range = element >= start;
    in_range = in_range && ( element < end );
    umm offset = element - start;
    umm check = ( offset / size ) * size;
    b32 aligned = ( check == offset );
    b32 result = in_range && aligned;
    
    return result;
}

#define array_is_element_aligned( array, element, type ) array_is_element_aligned_size( ( array ).bytes, ( u8* ) ( element ), sizeof( type ) )
#define array_is_element_aligned_p( start, element, type ) array_is_element_aligned_size( ( u8* ) ( start ), ( u8* ) ( element ), sizeof( type ) )

stu b32 array_is_element_aligned_size( u8* start, u8* element, umm size ) {
    
    umm offset = element - start;
    umm check = ( offset / size ) * size;
    b32 result = ( check == offset );
    
    return result;
}

/* MARK reverse stack */

typedef struct reverse_stack_t {
    
    data_t* stack;
    umm used;
    
    union {
        void* data;
        u8* bytes;
        char* text;
        umm offset;
    };
    
} reverse_stack_t;

stu reverse_stack_t memory_reverse_stack_create( data_t* memory ) {
    
    reverse_stack_t reverse_stack = { 0 };
    reverse_stack.stack = memory;
    reverse_stack.bytes = memory->bytes + memory->reserved;
    reverse_stack.used = 0;
    
    return reverse_stack;
}

stu void* memory_reverse_stack_push( reverse_stack_t* reverse_stack, umm size ) {
    
    void* result = 0;
    _assert( reverse_stack->used + size < reverse_stack->stack->reserved - reverse_stack->stack->used );
    result = reverse_stack->bytes - reverse_stack->used - size;
    reverse_stack->used += size;
    
    return result;
}

// MARK array

#define array_reserved( array, type ) array_reserved_size( ( array ), sizeof( type ) )

stu umm array_reserved_size( data_t* array, umm elementSize ) {
    umm result = array->reserved / elementSize;
    return result;
}

#define array_used( array, type ) array_used_size( ( array ), sizeof( type ) )

stu umm array_used_size( data_t* array, umm elementSize ) {
    umm result = array->used / elementSize;
    return result;
}

#define array_get_pointer( array, type, index ) ( cast( ( type )*, ( array ).bytes ) + ( index ) )
#define array_get( array, type, index ) deref( array_get_pointer( ( array ), ( type ), ( index ) ) )
#define array_set( array, type, index, value ) ( deref( array_get_pointer( ( array ), ( type ), ( index ) ) ) = ( value ) )

#define array_push( array, type, value ) array_push_size( ( array ), sizeof( type ), &( value ) )

stu umm array_push_size( data_t* array, umm size, void* value ) {
    
    umm result = umm_max;
    
    if ( array->used + size <= array->reserved ) {
        result = array_used_size( array, size );
        memory_copy_p( array->bytes + array->used, value, size );
        array->used += size;
    }
    
    return result;
}

// MARK char

stu b32 char_is_upper_case( u32 c ) {
    b32 result = ( c >= 'A' && c <= 'Z' );
    return result;
}

stu b32 char_is_lower_case( u32 c ) {
    b32 result = ( c >= 'a' && c <= 'z' );
    return result;
}

stu b32 char_is_letter( u32 c ) {
    b32 result = ( char_is_upper_case( c ) || char_is_lower_case( c ) );
    return result;
}

stu u32 char_to_lower_case( u32 c ) {
    u32 result = ( c >= 'A' && c <= 'Z' ) ? c + 0x20 : c;
    return result;
}

stu u32 char_to_upper_case( u32 c ) {
    u32 result = ( c >= 'a' && c <= 'z' ) ? c - 0x20 : c;
    return result;
}

stu b32 char_is_decimal( u32 c ) {
    b32 result = ( c >= '0' && c <= '9' );
    return result;
}

stu b32 char_is_hexadecimal( u32 c ) {
    b32 result = ( c >= '0' && c <= '9' ) || ( c >= 'a' && c <= 'f' ) || ( c >= 'A' && c <= 'F' );
    return result;
}

stu b32 char_is_alpha_numeric( u32 c ) {
    b32 result = ( char_is_decimal( c ) || char_is_letter( c ) );
    return result;
}

stu b32 char_is_alpha_numeric_underscore( u32 c ) {
    b32 result = ( ( c == '_' ) || char_is_alpha_numeric( c ) );
    return result;
}

stu b32 char_is_whitespace( u32 c ) {
    b32 result = ( c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\n' || c == '\r' );
    return result;
}

stu b32 char_is_whitespace_not_new_line( u32 c ) {
    b32 result = ( c == ' ' || c == '\t' || c == '\v' || c == '\f' );
    return result;
}

stu b32 char_is_new_line( u32 c ) {
    b32 result = ( c == '\n' || c == '\r' );
    return result;
}

stu b32 char_is_control( u32 c ) {
    b32 result = ( c < 0x20 || ( c >= 0x80 && c < 0xa0 ) );
    return result;
}

stu b32 char_is_any_u8( u32 c, void* list, umm count ) {
    
    b32 result = 0;
    u8* str = cast( u8*, list );
    u8* end = str + count;
    
    while ( str < end && !result ) {
        result = ( c == *str );
        str++;
    }
    
    return result;
}

stu b32 char_is_any_u16( u32 c, u16* list, umm count ) {
    
    b32 result = 0;
    u16* end = list + count;
    
    while ( list < end && !result ) {
        result = ( c == *list );
        list++;
    }
    
    return result;
}

stu b32 char_is_any_u32( u32 c, u32* list, umm count ) {
    
    profiler_event_start( char_is_any_u32 );
    
    b32 result = 0;
    u32* end = list + count;
    
    while ( list < end && !result ) {
        result = ( c == *list );
        list++;
    }
    
    profiler_event_end( char_is_any_u32 );
    
    return result;
}

stu u8 char_decimal_to_value( u32 c ) {
    u8 result = ( u8 ) ( c - '0' );
    return result;
}

stu u8 char_hexadecimal_to_value( u32 c ) {
    u8 result = 0;
    if ( c >= '0' && c <= '9' ) {
        result = ( u8 ) ( c - '0' );
    } else if ( c >= 'a' && c <= 'f' ) {
        result = ( u8 ) ( 10 + c - 'a' );
    } else {
        result = ( u8 ) ( 10 + c - 'A' );
    }
    return result;
}

// MARK linked list

/* NOTE simon:
The idea with the linked list macros, is to define other single line macros that use the correct fields.
For example: 
#define dll_init( list ) doubly_linked_list_initialize( list->head, list->tail, &list->sentinel, previous, next )
#define dll_insert_first( list, node ) doubly_linked_list_insert_first( list->head, list->tail, node, previous, next )
*/

#define doubly_linked_list_initialize( head, tail, sentinel, previous, next ) \
( head ) = ( sentinel );\
( tail ) = ( sentinel );\
( head )->next = ( tail );\
( tail )->previous = ( head );

#define doubly_linked_list_insert_last( head, tail, node, pervious, next ) \
( node )->previous = ( tail )->previous;\
( node )->next = ( tail );\
( node )->previous->next = ( node );\
( node )->next->previous = ( node );

#define doubly_linked_list_insert_first( head, tail, node, previous, next ) \
( node )->previous = ( head );\
( node )->next = ( head )->next;\
( node )->previous->next = ( node );\
( node )->next->previous = ( node );

#define doubly_linked_list_insert_before( head, tail, before, node, previous, next ) \
( node )->previous = ( before )->previous;\
( node )->next = ( before );\
( node )->previous->next = ( node );\
( node )->next->previous = ( node );

#define doubly_linked_list_insert_after( head, tail, after, node, previous, next ) \
( node )->previous = ( after );\
( node )->next = ( after )->next;\
( node )->previous->next = ( node );\
( node )->next->previous = ( node );

#define doubly_linked_list_remove( head, tail, node, previous, next ) \
( node )->next->previous = ( node )->previous;\
( node )->previous->next = ( node )->next;

#define singly_linked_list_insert_first( head, node, next )\
( node )->next = ( head );\
( head ) = ( node );

#define singly_linked_list_remove_first( head, next )\
if ( ( head ) ) { ( head ) = ( head )->next; };

// MARK string

#define string_zero_terminate( string ) if ( ( string ).used < ( string ).reserved ) { ( string ).bytes[ ( string ).used ] = 0; }
#define string_zero_terminate_utf16( string ) if ( ( string ).used + 1 < ( string ).reserved ) { u16* u16_pointer = cast( u16*, ( string ).bytes + ( string ).used ); u16_pointer[ 0 ] = 0; }
#define string_zero_terminate_on_stack( new_variable_name, size, source, source_length ) \
memory_get_on_stack( new_variable_name, size ); \
_assert( ( source_length ) + 1 < new_variable_name.reserved ); \
memory_push_copy_p( &new_variable_name, source, source_length, 1 ); \
memory_push_u8( &new_variable_name, 0, 1 );

stu data_t string_p( void* text, umm length ) {
    
    data_t s;
    s.raw = text;
    s.used = s.reserved = length;
    
    return s;
}

stu u32 string_length( void* string ) {
    
    u32 result;
    u8* start = cast( u8*, string );
    u8* current = cast( u8*, string );
    
    if ( current ) {
        while ( *current != '\0' ) {
            current++;
        }
    }
    
    result = cast( u32, current - start );
    
    return result;
}

stu void string_remove_leading_whitespace( data_t* string ) {
    
    u8* current = string->bytes;
    u8* end = memory_end( deref( string ) );
    u8* characters = current;
    
    while ( characters < end && char_is_whitespace( deref( characters ) ) ) {
        characters++;
    }
    
    string->used = end - characters;
    
    while ( characters < end ) {
        deref( current ) = deref( characters );
        characters++;
        current++;
    }
}

stu void string_copy( void* destination, void* source, umm length ) {
    
    u8* d = cast( u8*, destination );
    u8* s = cast( u8*, source );
    
    if ( d && s ) {
        while ( length > 0 ) {
            length--;
            *d++ = *s++;
        }
    }
}

#define string_push_length( memory, source, length ) string_push_length_( memory, cast( void*, source ), length )

stu u8* string_push_length_( data_t* memory, void* source, umm length ) {
    
    void* destination = memory_push_size( memory, length, 1 );
    string_copy( destination, source, length );
    
    return cast( u8*, destination );
}

#define string_seek_ci( in, what ) string_seek_ci_p( ( in ).bytes, memory_end( in ), ( what ).bytes, ( what ).used )
#define string_seek_backward_ci( in, what ) string_seek_backward_ci_p( ( in ).bytes, memory_end( in ), ( what ).bytes, ( what ).used )

stu void* string_seek_ci_p( void* start_param, void* end_param, void* what_param, umm what_size ) {
    
    u8* start = cast( u8*, start_param );
    u8* end = cast( u8*, end_param );
    umm in_size = end - start;
    u8* what = cast( u8*, what_param );
    u8* result = end;
    
    if ( start && end && what_size > 0 && what_size <= in_size ) {
        
        end -= ( what_size - 1 );
        
        while ( start < end ) {
            
            u8* bytes = start;
            umm size = 0;
            
            while ( size < what_size ) {
                
                u8 a = ( u8 ) char_to_lower_case( bytes[ size ] );
                u8 b = ( u8 ) char_to_lower_case( what[ size ] );
                
                if ( a != b ) {
                    break;
                }
                
                size++;
            }
            
            if ( size == what_size ) {
                result = start;
                break;
            } else {
                start++;
            }
        }
    }
    
    return result;
}

stu void* string_seek_backward_ci_p( void* start_param, void* end_param, void* what_param, umm what_size ) {
    
    u8* start = cast( u8*, start_param );
    u8* end = cast( u8*, end_param );
    umm in_size = end - start;
    u8* what = cast( u8*, what_param );
    void* result = end;
    
    if ( start && end && what_size > 0 && what_size <= in_size ) {
        
        end -= what_size;
        
        while ( end >= start ) {
            
            u8* bytes = end;
            umm size = 0;
            
            while ( size < what_size ) {
                
                u8 a = ( u8 ) char_to_lower_case( bytes[ size ] );
                u8 b = ( u8 ) char_to_lower_case( what[ size ] );
                
                if ( a != b ) {
                    break;
                }
                
                size++;
            }
            
            if ( size == what_size ) {
                result = end;
                break;
            } else {
                end--;
            }
        }
    }
    
    return result;
}

stu void* string_seek_white_space_p( void* start_param, void* end_param ) {
    
    u8* start = cast( u8*, start_param );
    u8* end = cast( u8*, end_param );
    u8* result = 0;
    
    while ( start < end ) {
        
        if ( char_is_whitespace( *start ) ) {
            result = start;
            break;
        }
        
        start++;
    }
    
    return result;
}

stu void* string_seek_white_space_backward_p( void* start_param, void* end_param ) {
    
    u8* start = cast( u8*, start_param );
    u8* end = cast( u8*, end_param );
    u8* result = 0;
    
    while ( end > start ) {
        
        end--;
        
        if ( char_is_whitespace( *end ) ) {
            result = end;
            break;
        }
    }
    
    return result;
}

#define string_seek_white_space( in ) string_seek_white_space_p( ( in ).bytes, ( in ).bytes + ( in ).used )
#define string_seek_white_space_backward( in ) string_seek_white_space_backward_p( ( in ).bytes, ( in ).bytes + ( in ).used )

stu void* string_seek_new_line_p( void* start_param, void* end_param ) {
    
    u8* start = cast( u8*, start_param );
    u8* end = cast( u8*, end_param );
    u8* result = 0;
    
    while ( start < end ) {
        
        if ( deref( start ) == '\n' || deref( start ) == '\r' ) {
            result = start;
            break;
        }
        
        start++;
    }
    
    return result;
}

stu void* string_seek_new_line_backward_p( void* start_param, void* end_param ) {
    
    u8* start = cast( u8*, start_param );
    u8* end = cast( u8*, end_param );
    u8* result = 0;
    
    while ( end > start ) {
        
        end--;
        
        if ( deref( end ) == '\n' || deref( end ) == '\r' ) {
            result = end;
            break;
        }
    }
    
    return result;
}

#define string_seek_new_line( in ) string_seek_new_line_p( ( in ).bytes, ( in ).bytes + ( in ).used )
#define string_seek_new_line_backward( in ) string_seek_new_line_backward_p( ( in ).bytes, ( in ).bytes + ( in ).used )

stu void* string_seek_non_white_space_p( void* start_param, void* end_param ) {
    
    u8* start = cast( u8*, start_param );
    u8* end = cast( u8*, end_param );
    u8* result = 0;
    
    while ( start < end ) {
        
        if ( !char_is_whitespace( *start ) ) {
            result = start;
            break;
        }
        
        start++;
    }
    
    return result;
}

stu void* string_seek_non_white_space_backward_p( void* start_param, void* end_param ) {
    
    u8* start = cast( u8*, start_param );
    u8* end = cast( u8*, end_param );
    u8* result = 0;
    
    while ( end > start ) {
        
        end--;
        
        if ( !char_is_whitespace( *end ) ) {
            result = end;
            break;
        }
    }
    
    return result;
}

#define string_seek_non_white_space( in ) string_seek_white_space_p( ( in ).bytes, ( in ).bytes + ( in ).used )
#define string_seek_non_white_space_backward( in ) string_seek_white_space_backward_p( ( in ).bytes, ( in ).bytes + ( in ).used )

stu u8* string_append_p( data_t* memory, data_t* destination, void* string, umm length ) {
    
    u8* result = string_push_length( memory, string, length );
    
    if ( !destination->used ) {
        destination->bytes = result;
        destination->reserved = destination->used = 0;
    }
    
    destination->used += length;
    destination->reserved += length;
    
    return result;
}

stu u8* string_append( data_t* memory, data_t* destination, data_t string ) {
    
    void* result = memory_push_size( memory, string.used, 1 );
    string_copy( result, string.text, string.used );
    
    if ( !destination->used ) {
        destination->raw = result;
        destination->reserved = destination->used = 0;
    }
    
    destination->used += string.used;
    destination->reserved += string.used;
    
    return cast( u8*, result );
}

stu s32 string_compare( void* string_1, void* string_2, umm length ) {
    
    s32 result = 0;
    umm index = 0;
    u8* s1 = cast( u8*, string_1 );
    u8* s2 = cast( u8*, string_2 );
    
    _assert( s1 != 0 );
    _assert( s2 != 0 );
    
    while ( index < length ) {
        
        result = *s1 - *s2;
        
        if ( *s1 == '\0' || *s2 == '\0' || result != 0 ) {
            break;
        }
        
        s1++;
        s2++;
        index++;
    }
    
    return result;
}

stu b32 string_are_equal_p( void* a, umm a_length, void* b, umm b_length ) {
    b32 result = memory_are_equal_p( a, a_length, b, b_length );
    return result;
}

stu b32 string_are_equal_ci( data_t* string_a, data_t* string_b ) {
    
    b32 result = string_a->used == string_b->used;
    
    if ( result ) {
        
        u8 offset = 'a' - 'A';
        u8* ap = string_a->bytes;
        u8* bp = string_b->bytes;
        _assert( ap != 0 );
        _assert( bp != 0 );
        
        for ( umm i = 0; i < string_a->used; i++ ) {
            
            u8 a = *ap;
            u8 b = *bp;
            if ( a >= 'A' && a <= 'Z' ) {
                a = a + offset;
            }
            
            if ( b >= 'A' && b <= 'Z' ) {
                b = b + offset;
            }
            
            if ( a != b ) {
                result = false;
                break;
            }
            ap++;
            bp++;
        }
    }
    
    return result;
}

stu b32 string_start_with_ci_p( void* start_arg, void* end_arg, void* what_arg, umm what_length ) {
    
    _assert( start_arg );
    _assert( what_arg );
    
    u8* current = cast( u8*, start_arg );
    u8* end = current + what_length;
    
    b32 result = end <= cast( u8*, end_arg );
    u8* what = cast( u8*, what_arg );
    u8 offset = 'a' - 'A';
    
    while ( result && current < end ) {
        
        u8 in_c = *current;
        u8 what_c = *what;
        
        if ( in_c >= 'A' && in_c <= 'Z' ) {
            in_c += offset;
        }
        
        if ( what_c >= 'A' && what_c <= 'Z' ) {
            what_c += offset;
        }
        
        result = ( in_c == what_c );
        current++;
        what++;
    }
    
    return result;
}

stu b32 string_start_with_ci( data_t in, data_t what ) {
    b32 result = string_start_with_ci_p( in.bytes, memory_end( in ), what.bytes, what.used );
    return result;
}

stu void string_to_lower_case( data_t* string ) {
    
    for ( umm i = 0; i < string->used; i++ ) {
        if ( char_is_upper_case( string->bytes[ i ] ) ) {
            string->bytes[ i ] = ( u8 ) char_to_lower_case( string->bytes[ i ] );
        }
    }
}

stu void string_to_upper_case( data_t* string ) {
    
    for ( umm i = 0; i < string->used; i++ ) {
        if ( char_is_lower_case( string->bytes[ i ] ) ) {
            string->bytes[ i ] = ( u8 ) char_to_upper_case( string->bytes[ i ] );
        }
    }
}

#define string_to_s64( string ) string_to_s64_p( ( string ).bytes, ( string ).used )

stu s64 string_to_s64_p( u8* string, umm string_size ) {
    
    s64 result = 0;
    s32 sign = 1;
    umm at = 0;
    
    while ( at < string_size
           && ( string[ at ] == ' '
               || string[ at ] == '\t'
               || string[ at ] == '\v'
               || string[ at ] == '\f'
               || string[ at ] == '\n'
               || string[ at ] == '\r' ) ) {
        at++;
    }
    
    if ( at < string_size ) {
        
        b32 base = 10;
        
        if ( string[ at ] == '-' ) {
            sign = -1;
            at++;
        } else if ( string[ at ] == '+' ) {
            sign = 1;
            at++;
        }
        
        if ( at + 2 < string_size && string[ at ] == '0' && string[ at + 1 ] == 'x' ) {
            at += 2;
            base = 16;
        }
        
        while ( at < string_size ) {
            
            if ( string[ at ] >= '0' && string[ at ] <= '9' ) {
                result = result * base + ( string[ at ] - '0' );
            } else if ( base == 16 && string[ at ] >= 'a' && string[ at ] <= 'f' ) {
                result = result * base + 10 + ( string[ at ] - 'a' );
            } else if ( base == 16 && string[ at ] >= 'A' && string[ at ] <= 'F' ) {
                result = result * base + 10 + ( string[ at ] - 'A' );
            } else {
                break;
            }
            
            at++;
        }
    }
    
    result *= sign;
    
    return result;
}

#define string_to_u64( string ) string_to_u64_p( ( string ).bytes, ( string ).used )

stu u64 string_to_u64_p( void* number_string, umm string_size ) {
    
    u8* string = cast( u8*, number_string );
    u64 result = 0;
    umm at = 0;
    b32 complement = false;
    
    while ( at < string_size
           && ( string[ at ] == ' '
               || string[ at ] == '\t'
               || string[ at ] == '\v'
               || string[ at ] == '\f'
               || string[ at ] == '\n'
               || string[ at ] == '\r'
               || string[ at ] == '+' ) ) {
        at++;
    }
    
    if ( at < string_size ) {
        
        b32 base = 10;
        
        if ( string[ at ] == '-' ) {
            complement = true;
            at++;
        }
        
        if ( at + 2 < string_size && string[ at ] == '0' && string[ at + 1 ] == 'x' ) {
            at += 2;
            base = 16;
        }
        
        while ( at < string_size ) {
            
            if ( string[ at ] >= '0' && string[ at ] <= '9' ) {
                result = result * base + ( string[ at ] - '0' );
            } else if ( base == 16 && string[ at ] >= 'a' && string[ at ] <= 'f' ) {
                result = result * base + 10 + ( string[ at ] - 'a' );
            } else if ( base == 16 && string[ at ] >= 'A' && string[ at ] <= 'F' ) {
                result = result * base + 10 + ( string[ at ] - 'A' );
            } else {
                break;
            }
            
            at++;
        }
    }
    
    if ( complement ) {
        result = 0 - result;
    }
    
    return result;
}

typedef enum string_format_flag_t {
    
    string_format_flag_none = 0,
    string_format_flag_hexadecimal = 1 << 0,
    string_format_flag_hexadecimal_no_prefix = 1 << 1,
    string_format_flag_hexadecimal_prefix_befor_filling = 1 << 2,
    string_format_flag_hexadecimal_space_after_8_bits = 1 << 3,
    string_format_flag_hexadecimal_space_after_16_bits = 1 << 4,
    string_format_flag_hexadecimal_space_after_32_bits = 1 << 5,
    string_format_flag_hexadecimal_space_after_64_bits = 1 << 6,
    string_format_flag_hexadecimal_force_8_bits = 1 << 7,
    string_format_flag_hexadecimal_force_16_bits = 1 << 8,
    string_format_flag_hexadecimal_force_32_bits = 1 << 9,
    string_format_flag_hexadecimal_force_64_bits = 1 << 10,
    
    string_format_flag_left_align = 1 << 11,
    string_format_flag_sign_force = 1 << 12,
    string_format_flag_sign_before_filling = 1 << 13,
    string_format_flag_decimal_space_after_3_digits = 1 << 14,
    
    string_format_flag_floating_point_scientific_notation = 1 << 15,
    
} string_format_flag_t;

stu umm string_from_s64( void* output_buffer, umm buffer_size, s64 integer, umm minimum_character_count, u8 fill_with, u32 format ) {
    
    u8* buffer = cast( u8*, output_buffer );
    umm c = 0;
    b32 s64_min_fix = false;
    
    b32 hexadecimal = format & string_format_flag_hexadecimal;
    b32 hexadecimal_prefix = hexadecimal && !( format & string_format_flag_hexadecimal_no_prefix );
    b32 hexadecimal_force_8 = format & string_format_flag_hexadecimal_force_8_bits;
    b32 hexadecimal_force_16 = format & string_format_flag_hexadecimal_force_16_bits;
    b32 hexadecimal_force_32 = format & string_format_flag_hexadecimal_force_32_bits;
    b32 hexadecimal_force_64 = format & string_format_flag_hexadecimal_force_64_bits;
    b32 hexadecimal_space_8 = format & string_format_flag_hexadecimal_space_after_8_bits;
    b32 hexadecimal_space_16 = format & string_format_flag_hexadecimal_space_after_16_bits;
    b32 hexadecimal_space_32 = format & string_format_flag_hexadecimal_space_after_32_bits;
    b32 hexadecimal_space_64 = format & string_format_flag_hexadecimal_space_after_64_bits;
    b32 decimal_3 = format & string_format_flag_decimal_space_after_3_digits;
    
    u8 base = ( hexadecimal ) ? 16 : 10;
    b32 is_negative = false;
    
    if ( c < buffer_size && integer < 0 ) {
        
        is_negative = true;
        
        if ( integer == s64_min ) {
            integer = integer + 1;
            s64_min_fix = true;
        }
        
        integer = integer * -1;
    }
    
    s64 number = integer;
    u32 digits = 1;
    
    while ( number > ( u8 ) ( base - 1 ) ) {
        number /= base;
        digits++;
    }
    
    umm extra = ( is_negative || ( format & string_format_flag_sign_force ) ) ? 1 : 0;
    extra += ( hexadecimal_prefix ) ? 2 : 0;
    
    umm force = digits;
    
    if ( hexadecimal ) {
        
        if ( hexadecimal_force_8 ) {
            force = 2;
        } else if ( hexadecimal_force_16 ) {
            force = 4;
        } else if ( hexadecimal_force_32 ) {
            force = 8;
        } else if ( hexadecimal_force_64 ) {
            force = 16;
        }
        
        if ( force < digits ) {
            force = digits;
        }
        
        if ( hexadecimal_space_8 ) {
            extra += ( force - 1 ) / 2;
        } else if ( hexadecimal_space_16 ) {
            extra += ( force - 1 ) / 4;
        } else if ( hexadecimal_space_32 ) {
            extra += ( force - 1 ) / 8;
        } else if ( hexadecimal_space_64 ) {
            
        }
        
    } else if ( decimal_3 ) {
        extra += ( digits - 1 ) / 3;
    }
    
    umm filling = 0;
    
    if ( minimum_character_count > force + extra ) {
        filling = minimum_character_count - ( force + extra );
    }
    
    if ( format & string_format_flag_sign_before_filling ) {
        if ( is_negative ) {
            buffer[ c++ ] = '-';
        } else if ( format & string_format_flag_sign_force ) {
            buffer[ c++ ] = '+';
        }
    }
    
    if ( hexadecimal_prefix ) {
        
        if ( c < buffer_size ) {
            buffer[ c++ ] = '0';
        }
        
        if ( c < buffer_size ) {
            buffer[ c++ ] = 'x';
        }
    }
    
    if ( !( format & string_format_flag_left_align ) ) {
        for ( umm i = 0; i < filling && c < buffer_size; i++ ) {
            buffer[ c++ ] = fill_with;
        }
    }
    
    if ( !( format & string_format_flag_sign_before_filling ) ) {
        if ( is_negative ) {
            buffer[ c++ ] = '-';
        } else if ( format & string_format_flag_sign_force ) {
            buffer[ c++ ] = '+';
        }
    }
    
    if ( hexadecimal && ( force > digits ) ) {
        
        while ( force > digits ) {
            
            if ( c < buffer_size ) {
                buffer[ c++ ] = '0';
            }
            
            force--;
            
            if ( ( hexadecimal_space_8 && ( ( force % 2 ) == 0 ) )
                || ( hexadecimal_space_16 && ( ( force % 4 ) == 0 ) )
                || ( hexadecimal_space_32 && ( ( force % 8 ) == 0 ) )
                || ( decimal_3 && ( ( force % 3 ) == 0 ) ) ) {
                buffer[ c++ ] = ' ';
            }
        }
    }
    
    u32 digit_count = digits;
    
    while ( digits > 0 ) {
        
        if ( buffer_size - c ) {
            
            if ( digits != digit_count && ( ( hexadecimal_space_8 && ( ( digits % 2 ) == 0 ) )
                                           || ( hexadecimal_space_16 && ( ( digits % 4 ) == 0 ) )
                                           || ( hexadecimal_space_32 && ( ( digits % 8 ) == 0 ) )
                                           || ( decimal_3 && ( ( digits % 3 ) == 0 ) ) ) ) {
                buffer[ c++ ] = ' ';
            }
        }
        
        s64 divider = 1;
        
        for ( u32 i = 0; i < ( digits - 1 ); i++ ) {
            divider *= base;
        }
        
        s64 digit = integer / divider;
        
        if ( digits <= ( buffer_size - c ) ) {
            
            if ( s64_min_fix ) {
                
                if ( hexadecimal ) {
                    if ( digits == digit_count ) {
                        digit += 1;
                    } else {
                        digit = 0;
                    }
                } else if( digits == 1 ) {
                    digit += 1;
                }
            }
            
            if ( digit < 10 ) {
                buffer[ c++ ] = ( char ) ( '0' + digit );
            } else {
                buffer[ c++ ] = ( char ) ( 'a' + ( digit - 10 ) );
            }
        }
        
        integer -= divider * digit;
        digits--;
    }
    
    if ( format & string_format_flag_left_align ) {
        for ( umm i = 0; i < filling && c < buffer_size; i++ ) {
            buffer[ c++ ] = fill_with;
        }
    }
    
    return c;
}

/* NOTE simon: minimum_character_count includes sign, spaces, ...*/
stu umm string_from_u64( void* output_buffer, umm buffer_size, u64 integer, umm minimum_character_count, u8 fill_with, u32 format ) {
    
    /* TODO simon: Investigate this properly. */
    // https://github.com/jeaiii/itoa/blob/main/itoa/to_text_from_integer.h#L28
    // https://jk-jeon.github.io/posts/2022/02/jeaiii-algorithm/
    
    u8* buffer = cast( u8*, output_buffer );
    umm c = 0;
    
    b32 hexadecimal = format & string_format_flag_hexadecimal;
    b32 hexadecimal_prefix = hexadecimal && !( format & string_format_flag_hexadecimal_no_prefix );
    b32 hexadecimal_force_8 = format & string_format_flag_hexadecimal_force_8_bits;
    b32 hexadecimal_force_16 = format & string_format_flag_hexadecimal_force_16_bits;
    b32 hexadecimal_force_32 = format & string_format_flag_hexadecimal_force_32_bits;
    b32 hexadecimal_force_64 = format & string_format_flag_hexadecimal_force_64_bits;
    b32 hexadecimal_space_8 = format & string_format_flag_hexadecimal_space_after_8_bits;
    b32 hexadecimal_space_16 = format & string_format_flag_hexadecimal_space_after_16_bits;
    b32 hexadecimal_space_32 = format & string_format_flag_hexadecimal_space_after_32_bits;
    b32 hexadecimal_space_64 = format & string_format_flag_hexadecimal_space_after_64_bits;
    b32 decimal_3 = !hexadecimal && format & string_format_flag_decimal_space_after_3_digits;
    b32 force_sign = format & string_format_flag_sign_force;
    
    u8 base = ( hexadecimal ) ? 16 : 10;
    
    u64 number = integer;
    u32 digits = 1;
    
    while ( number > ( u8 ) ( base - 1 ) ) {
        number /= base;
        digits++;
    }
    
    smm extra = ( hexadecimal_prefix ) ? 2 : 0;
    extra += ( force_sign ) ? 1 : 0;
    
    umm force = digits;
    
    if ( hexadecimal ) {
        
        if ( hexadecimal_force_8 ) {
            force = 2;
        } else if ( hexadecimal_force_16 ) {
            force = 4;
        } else if ( hexadecimal_force_32 ) {
            force = 8;
        } else if ( hexadecimal_force_64 ) {
            force = 16;
        }
        
        if ( force < digits ) {
            force = digits;
        }
        
        if ( hexadecimal_space_8 ) {
            extra += ( force - 1 ) / 2;
        } else if ( hexadecimal_space_16 ) {
            extra += ( force - 1 ) / 4;
        } else if ( hexadecimal_space_32 ) {
            extra += ( force - 1 ) / 8;
        } else if ( hexadecimal_space_64 ) {
            
        }
        
    } else if ( decimal_3 && minimum_character_count ) {
        
        umm spaces = ( digits - 1 ) / 3;
        
        smm left = ( ( smm ) minimum_character_count ) - ( extra + spaces + digits );
        umm pos = 1 + digits + spaces;
        
        while ( left > 0 ) {
            
            if ( pos % 4 == 0 ) {
                spaces++;
            }
            
            pos++;
            left--;
        }
        
        extra += spaces;
    }
    
    umm filling = 0;
    
    if ( minimum_character_count > force + extra ) {
        filling = minimum_character_count - ( force + extra );
    }
    
    if ( force_sign && ( format & string_format_flag_sign_before_filling ) ) {
        buffer[ c++ ] = '+';
    }
    
    if ( hexadecimal_prefix ) {
        
        if ( c < buffer_size ) {
            buffer[ c++ ] = '0';
        }
        
        if ( c < buffer_size ) {
            buffer[ c++ ] = 'x';
        }
    }
    
    if ( !( format & string_format_flag_left_align ) ) {
        
        umm total_size = digits + filling;
        
        for ( umm i = 0; i < filling && c < buffer_size; i++ ) {
            buffer[ c++ ] = fill_with;
            
            if ( decimal_3 && c < buffer_size && ( total_size - i ) % 3 == 1 ) {
                buffer[ c++ ] = ' ';
            }
        }
    }
    
    if ( force_sign && !( format & string_format_flag_sign_before_filling ) ) {
        buffer[ c++ ] = '+';
    }
    
    if ( hexadecimal && ( force > digits ) ) {
        
        while ( force > digits ) {
            
            if ( c < buffer_size ) {
                buffer[ c++ ] = '0';
            }
            
            force--;
            
            if ( ( hexadecimal_space_8 && ( ( force % 2 ) == 0 ) )
                || ( hexadecimal_space_16 && ( ( force % 4 ) == 0 ) )
                || ( hexadecimal_space_32 && ( ( force % 8 ) == 0 ) ) ) {
                buffer[ c++ ] = ' ';
            }
        }
    }
    
    u32 digit_count = digits;
    
    while ( digits > 0 ) {
        
        if ( buffer_size - c ) {
            
            if ( digits != digit_count && ( ( hexadecimal_space_8 && ( ( digits % 2 ) == 0 ) )
                                           || ( hexadecimal_space_16 && ( ( digits % 4 ) == 0 ) )
                                           || ( hexadecimal_space_32 && ( ( digits % 8 ) == 0 ) )
                                           || ( decimal_3 && ( ( digits % 3 ) == 0 ) ) ) ) {
                buffer[ c++ ] = ' ';
            }
        }
        
        u64 divider = 1;
        
        for ( u32 i = 0; i < ( digits - 1 ); i++ ) {
            divider *= base;
        }
        
        u64 digit = integer / divider;
        
        if ( digits <= ( buffer_size - c ) ) {
            
            if ( digit < 10 ) {
                buffer[ c++ ] = ( char ) ( '0' + digit );
            } else {
                buffer[ c++ ] = ( char ) ( 'a' + ( digit - 10 ) );
            }
        }
        
        integer -= divider * digit;
        digits--;
    }
    
    if ( format & string_format_flag_left_align ) {
        
        for ( umm i = 0; i < filling && c < buffer_size; i++ ) {
            buffer[ c++ ] = fill_with;
        }
    }
    
    return c;
}

stu f64 string_to_r64( void* string_arg, umm string_size ) {
    
    u8* string = cast( u8*, string_arg );
    f64 result = 0;
    f64 sign = 1;
    u32 at = 0;
    
    while ( at < string_size
           && ( string[ at ] == ' '
               || string[ at ] == '\t'
               || string[ at ] == '\v'
               || string[ at ] == '\f'
               || string[ at ] == '\n'
               || string[ at ] == '\r' ) ) {
        at++;
    }
    
    if ( at < string_size ) {
        
        f64 decimal_divisor;
        b32 decimal;
        
        if ( string[ at ] == '-' ) {
            sign = -1;
            at++;
        } else if ( string[ at ] == '+' ) {
            sign = 1;
            at++;
        }
        
        decimal_divisor = 10;
        decimal = false;
        
        while ( at < string_size ) {
            
            if ( string[ at ] == '.' ) {
                
                decimal = true;
                
            } else if ( string[ at ] >= '0' && string[ at ] <= '9' ) {
                
                if ( !decimal ) {
                    result = result * 10 + ( string[ at ] - '0' );
                } else {
                    result = result + ( string[ at ] - '0' ) / decimal_divisor;
                    decimal_divisor *= 10;
                }
                
            } else {
                
                break;
            }
            
            at++;
        }
    }
    
    result *= sign;
    
    return result;
}

stu umm string_push_u64( data_t* memory, u64 n, umm desired_digit_count, u8 fill_with, u32 hexadecimal_format ) {
    
    umm size = string_from_u64( memory_current( *memory, u8 ), memory_left( *memory ), n, desired_digit_count, fill_with, hexadecimal_format );
    memory->used += size;
    
    return size;
}

stu umm string_push_s64( data_t* memory, s64 n, umm desired_digit_count, u8 fill_with, u32 hexadecimal_format ) {
    
    umm size = string_from_s64( memory_current( *memory, u8 ), memory_left( *memory ), n, desired_digit_count, fill_with, hexadecimal_format );
    memory->used += size;
    
    return size;
}

#if defined( TARGET_x64 )

#include "../lib/floating_point_to_decimal.h"

stu void internal_string_from_u64( data_t* output, u64 n ) {
    
    _assert( memory_left( *output ) >= 20 );
    
    u8* current = output->bytes + output->reserved;
    u8* end = current;
    current--;
    
    while ( n >= 10 ) {
        ( *current ) = '0' + ( n % 10 );
        current--;
        n /= 10;
    }
    
    ( *current ) = '0' + ( u8 ) n;
    
    output->bytes = current;
    output->used = output->reserved = end - current;
}

stu void internal_string_from_floating_point( data_t* output, u64 significand, s32 exponent, b32 is_negative, u32 format ) {
    
    b32 scientific_notation = flag_is_set_u32( format, string_format_flag_floating_point_scientific_notation );
    
    memory_get_on_stack( digits, 20 );
    internal_string_from_u64( &digits, significand );
    
    u8* current = digits.bytes;
    u8* end = memory_end( digits );
    
    if ( is_negative ) {
        memory_push_u8( output, '-', 1 );
    }
    
    if ( scientific_notation ) {
        
        memory_push_u8( output, *current, 1 );
        memory_push_u8( output, '.', 1 );
        current++;
        
        if ( current < end ) {
            
            memory_push_copy_p( output, current, end - current, 1 );
            exponent += ( s32 ) ( end - current );
            
            if ( exponent ) {
                memory_push_u8( output, 'e', 1 );
                
                if ( exponent < 0 ) {
                    memory_push_u8( output, '-', 1 );
                    exponent *= -1;
                }
                
                memory_get_on_stack( exponent_digits, 20 );
                internal_string_from_u64( &exponent_digits, exponent );
                memory_push_copy( output, exponent_digits, 1 );
            }
            
        } else {
            memory_push_u8( output, '0', 1 );
        }
        
    } else {
        
        if ( exponent > 0 ) {
            
            memory_push_copy( output, digits, 1 );
            
            for ( umm i = 0; i < exponent; i++ ) {
                memory_push_u8( output, '0', 1 );
            }
            
            memory_push_copy_l( output, ".0" );
            
        } else {
            
            if ( digits.used > -exponent ) {
                
                umm first_part = digits.used + exponent;
                umm second_part = digits.used - first_part;
                
                memory_push_copy_p( output, digits.bytes, first_part, 1 );
                memory_push_u8( output, '.', 1 );
                memory_push_copy_p( output, digits.bytes + first_part, second_part, 1 );
                
            } else {
                
                umm zero_count = ( -exponent ) - digits.used;
                memory_push_copy_l( output, "0." );
                
                for ( umm i = 0; i < zero_count; i++ ) {
                    memory_push_u8( output, '0', 1 );
                }
                
                memory_push_copy( output, digits, 1 );
            }
        }
    }
}

stu data_t internal_string_format_floating_point( data_t* output, data_t to_format, s32 before, u8 fill, s32 after, u32 format ) {
    
    _assert( to_format.used );
    u8* output_start = memory_current( *output, u8 );
    
    u8* current = to_format.bytes;
    u8* end = memory_end( to_format );
    
    u8 sign = 0;
    s32 before_length = 0;
    s32 after_length = 0;
    b32 is_special = false;
    
    if( current[ 0 ] == '-' ) {
        sign = '-';
        current++;
    } else if ( current[ 0 ] == '+' ) {
        sign = '+';
        current++;
    } else if ( flag_is_set_u32( format, string_format_flag_sign_force ) ) {
        sign = '+';
    }
    
    u8* before_start = current;
    
    while ( current < end && *current != '.' ) {
        current++;
    }
    
    before_length = ( s32 ) ( current - before_start );
    
    b32 sign_before = flag_is_set_u32( format, string_format_flag_sign_before_filling );
    
    if ( sign && sign_before ) {
        memory_push_u8( output, sign, 1 );
    }
    
    if ( sign ) {
        before--;
    }
    
    before -= before_length;
    
    while ( before > 0 ) {
        memory_push_u8( output, fill, 1 );
        before--;
    }
    
    if ( sign && !sign_before ) {
        memory_push_u8( output, sign, 1 );
    }
    
    memory_push_copy_p( output, before_start, before_length, 1 );
    
    if ( current < end ) {
        
        memory_push_u8( output, '.', 1 );
        
        current++;
        u8* after_start = current;
        after_length = ( s32 ) ( end - current );
        
        while ( current < end && *current != 'e' ) {
            current++;
        }
        
        b32 is_scientific = ( current < end );
        
        if ( is_scientific ) {
            
            memory_push_copy_p( output, after_start, after_length, 1 );
            
            while ( after > after_length ) {
                memory_push_u8( output, ' ', 1 );
                after--;
            }
            
        } else {
            
            umm to_copy = after_length;
            
            if ( after && after < after_length ) {
                to_copy = after;
            }
            
            memory_push_copy_p( output, after_start, to_copy, 1 );
            
            while ( after > after_length ) {
                memory_push_u8( output, '0', 1 );
                after--;
            }
        }
        
    } else {
        
        /* NOTE simon: NaN, Infinity */
        
        if ( after ) {
            after += 1;
        }
        
        while ( after > 0 ) {
            memory_push_u8( output, ' ', 1 );
            after--;
        }
    }
    
    u8* output_end = memory_current( *output, u8 );
    data_t result = memory_start_end( output_start, output_end );
    
    return result;
}

stu data_t string_push_f32( data_t* output, f32 real, s32 before_decimal_point, u8 fill_with, s32 after_decimal_point, u32 format ) {
    
    /* NOTE simon: If there are performance concern, we can skip the formating if it's not necessary. */
    memory_get_on_stack( temp_buffer, 256 );
    
    if ( real == 0.0f ) {
        memory_push_copy_l( &temp_buffer, "0.0" );
    } else if ( real == -0.0f ) {
        memory_push_copy_l( &temp_buffer, "-0.0" );
    } else if ( !floating_point_to_decimal_is_finite_32( real ) ) {
        
        u32 u32_f = deref( cast( u32*, &real ) );
        
        if ( u32_f == 0x7f800000 ) {
            memory_push_copy_l( &temp_buffer, "+infinity" );
        } else if ( u32_f == 0xff800000 ) {
            memory_push_copy_l( &temp_buffer, "-infinity" );
        } else if ( u32_f & 0x80000000 ) {
            memory_push_copy_l( &temp_buffer, "-NaN" );
        } else {
            memory_push_copy_l( &temp_buffer, "+NaN" );
        }
        
    } else {
        
        floating_point_decimal_32_t decimal = floating_point_to_decimal_32( real, 0 );
        internal_string_from_floating_point( &temp_buffer, decimal.significand, decimal.exponent, decimal.is_negative, format );
    }
    
    data_t result = internal_string_format_floating_point( output, temp_buffer, before_decimal_point, fill_with, after_decimal_point, format );
    
    return result;
}

stu umm string_from_f32( void* buffer, umm buffer_length, f32 real, u32 before_decimal_point, u8 fill_with, u32 after_decimal_point, u32 format ) {
    data_t output = memory_construct( buffer_length, 0, buffer );
    data_t result = string_push_f32( &output, real, before_decimal_point, fill_with, after_decimal_point, format );
    return result.used;
}

stu data_t string_push_f64( data_t* output, f64 real, u32 before_decimal_point, u8 fill_with, u32 after_decimal_point, u32 format ) {
    
    /* NOTE simon: If there are performance concern, we can skip the formating if it's not necessary. */
    memory_get_on_stack( temp_buffer, kibioctet( 1 ) );
    
    if ( real == 0.0 ) {
        memory_push_copy_l( &temp_buffer, "0.0" );
    } else if ( real == -0.0 ) {
        memory_push_copy_l( &temp_buffer, "-0.0" );
    } else if ( !floating_point_to_decimal_is_finite_64( real ) ) {
        
        u64 u64_f = deref( cast( u64*, &real ) );
        
        if ( u64_f == 0x7ff0000000000000ull ) {
            memory_push_copy_l( &temp_buffer, "+infinity" );
        } else if ( u64_f == 0xfff0000000000000ull ) {
            memory_push_copy_l( &temp_buffer, "-infinity" );
        } else if ( u64_f & 0x8000000000000000ull ) {
            memory_push_copy_l( &temp_buffer, "-NaN" );
        } else {
            memory_push_copy_l( &temp_buffer, "+NaN" );
        }
        
    } else {
        
        floating_point_decimal_64_t decimal = floating_point_to_decimal_64( real, 0 );
        internal_string_from_floating_point( &temp_buffer, decimal.significand, decimal.exponent, decimal.is_negative, format );
    }
    
    data_t result = internal_string_format_floating_point( output, temp_buffer, before_decimal_point, fill_with, after_decimal_point, format );
    
    return result;
}

stu umm string_from_f64( void* buffer, umm buffer_length, f64 real, u32 before_decimal_point, u8 fill_with, u32 after_decimal_point, u32 format ) {
    data_t output = memory_construct( buffer_length, 0, buffer );
    data_t result = string_push_f64( &output, real, before_decimal_point, fill_with, after_decimal_point, format );
    return result.used;
}

#else

stu umm string_from_f64( void* output_buffer, umm buffer_size, f64 real, s32 not_decimal_character_count, u8 fill_with, s32 decimal_count, u32 format ) {
    
    u8* buffer = cast( u8*, output_buffer );
    umm result = 0;
    
    s64 integer_part = ( s64 ) real;
    umm integer_part_count = 0;
    
    if ( format & string_format_flag_left_align ) {
        integer_part_count = string_from_s64( buffer, buffer_size, integer_part, 0, 0, format & ( ~string_format_flag_left_align ) );
    } else {
        integer_part_count = string_from_s64( buffer, buffer_size, integer_part, not_decimal_character_count, fill_with, format );
    }
    
    result += integer_part_count;
    
    if ( real < 0.0 ) {
        real *= -1.0;
    }
    
    if ( decimal_count && result < buffer_size ) {
        buffer[ result++ ] = '.';
    }
    
    f64 decimal = real - ( u64 ) real;
    
    while ( decimal_count > 0 ) {
        
        decimal *= 10;
        
        if ( decimal < 1.0 && result < buffer_size ) {
            *( buffer + result ) = '0';
            result++;
        }
        
        decimal_count--;
    }
    
    u64 decimal_part = ( u64 ) decimal;
    
    if ( decimal_part > 0 ) {
        result += string_from_u64( buffer + result, buffer_size - result, decimal_part, 0, 0, 0 );
    }
    
    if ( format & string_format_flag_left_align ) {
        
        smm left = not_decimal_character_count - integer_part_count;
        
        while ( result < buffer_size && left > 0 ) {
            buffer[ result++ ] = fill_with;
            left--;
        }
    }
    
    return result;
}

stu data_t string_push_f64( data_t* buffer, f64 real, u32 before_decimal_point, u8 fill_with, u32 after_decimal_point, u32 format ) {
    
    u8* start = buffer->bytes + buffer->used; 
    umm length = string_from_f64( start, buffer->reserved - buffer->used, real, before_decimal_point, fill_with, after_decimal_point, format );
    buffer->used += length;
    data_t result = memory_construct( length, length, start );
    
    return result;
}

stu data_t string_push_f32( data_t* buffer, f32 real, u32 before_decimal_point, u8 fill_with, u32 after_decimal_point, u32 format ) {
    data_t result = string_push_f64( buffer, ( f64 ) real, before_decimal_point, fill_with, after_decimal_point, format );
    return result;
}

#endif

umm string_printable_size_from_u64_p( umm number, u8* buffer, umm buffer_size ) {
    
    data_t output = { buffer_size, 0, buffer };
    
    u64 t = tebioctet( 1 );
    u64 g = gibioctet( 1 );
    umm m = mebioctet( 1 );
    umm k = kibioctet( 1 );
    
    data_t Tio = string_l( "Tio" );
    data_t Gio = string_l( "Gio" );
    data_t Mio = string_l( "Mio" );
    data_t Kio = string_l( "Kio" );
    data_t o = string_l( "o" );
    
    data_t* unit = &o;
    
    if ( number < k ) {
        output.used += string_from_u64( output.bytes, output.reserved, number, 0, 0, 0 );
    } else {
        
        f64 n = ( f64 ) number;
        
        if ( number >= t ) {
            n /= t;
            unit = &Tio;
        } else if ( number >= g ) {
            n /= g;
            unit = &Gio;
        } else if ( number >= m ) {
            n /= m;
            unit = &Mio;
        } else {
            n /= k;
            unit = &Kio;
        }
        
        output.used += string_from_f64( output.bytes, output.reserved, n, 0, 0, 3, 0 );
    }
    
    memory_push_copy( &output, string_l( " " ), 1 );
    memory_push_copy( &output, *unit, 1 );
    
    return output.used;
}

data_t string_printable_size_from_u64( umm number, data_t* buffer ) {
    data_t result = memory_get_block( buffer, 16, 1 );
    result.used += string_printable_size_from_u64_p( number, result.bytes, result.reserved );
    memory_pop_size( buffer, result.reserved - result.used );
    result.reserved = result.used;
    return result;
}

#if __cplusplus

#define string_format_3( memory, counter, ... ) { string_format_t internal_formats_ ## counter [ ] = { __VA_ARGS__ }; string_format_execute( ( memory ), internal_formats_ ## counter, array_count( internal_formats_ ## counter ) ); }
#define string_format_2( memory, counter, ... ) string_format_3( memory, counter, __VA_ARGS__ )
#define string_format( memory, ... ) string_format_2( memory, __COUNTER__, __VA_ARGS__ )

#else

#define string_format( memory, ... ) string_format_execute( ( memory ), ( string_format_t [ ] ) { __VA_ARGS__ }, array_count( ( ( string_format_t [ ] ) { __VA_ARGS__ } ) ) )
#endif

typedef enum string_format_type_t {
    
    string_format_type_text,
    
    string_format_type_unsigned,
    string_format_type_signed,
    
    string_format_type_floating_point_32,
    string_format_type_floating_point_64,
    
} string_format_type_t;

typedef struct string_format_t {
    
    union {
        void* t;
        
        u64 u;
        s64 s;
        f32 f_32;
        f64 f_64;
    };
    
    union {
        umm length;
        struct {
            u8 desired_digit_count;
            u8 fill_with;
            u8 hexadecimal_format;
        };
        struct {
            u32 format;
            u8 before_decimal_point;
            u8 fill_with_float;
            u8 after_decimal_point;
        };
    };
    
    u8 type;
} string_format_t;

#define tf_l( literal ) { .t = ( literal ), .length = sizeof( literal ) - 1, .type = string_format_type_text }
#define tf_p( pointer, length_arg ) { .t = ( pointer ), .length = ( length_arg ), .type = string_format_type_text }
#define tf_d( data ) { .t = ( data ).bytes, .length = ( data ).used, .type = string_format_type_text }

#define uf( u_arg ) { .u = u_arg, .type = string_format_type_unsigned }
#define sf( s_arg ) { .s = s_arg, .type = string_format_type_signed }
#define f32f( f_arg ) { .f_32 = f_arg, .after_decimal_point = 6, .type = string_format_type_floating_point_32 }
#define f64f( f_arg ) { .f_64 = f_arg, .after_decimal_point = 6, .type = string_format_type_floating_point_64 }

#define uf_e( u_arg, desired_digit_count_arg, fill_with_arg, hexadecimal_format_arg )\
{\
.u = u_arg,\
.desired_digit_count = desired_digit_count_arg,\
.fill_width = fill_with_arg,\
.hexadecimal_format = hexadecimal_format_arg,\
.type = string_format_type_unsigned\
}

#define sf_e( s_arg, desired_digit_count_arg, fill_with_arg, hexadecimal_format_arg )\
{\
.s = s_arg, .desired_digit_count = desired_digit_count_arg,\
.fill_with = fill_with_arg,\
.hexadecimal_format = hexadecimal_format_arg,\
.type = string_format_type_signed\
}

#define f32f_e( f_arg, before_decimal_point_arg, fill_with_arg, after_decimal_point_arg, format_arg )\
{\
.f_32 = f_arg,\
.before_decimal_point = before_decimal_point_arg,\
.fill_with_float = fill_with_arg,\
.after_decimal_point = after_decimal_point_arg,\
.format = format_arg,\
.type = string_format_type_floating_point_32\
}

#define f64f_e( f_arg, before_decimal_point_arg, fill_with_arg, after_decimal_point_arg, format_arg )\
{\
.f_64 = f_arg,\
.before_decimal_point = before_decimal_point_arg,\
.fill_with_float = fill_with_arg,\
.after_decimal_point = after_decimal_point_arg,\
.format = format_arg,\
.type = string_format_type_floating_point_64\
}

stu data_t string_format_execute( data_t* memory, string_format_t* formats, umm count ) {
    
    data_t result = memory_construct( 0, 0, memory_current( deref( memory ), u8 ) );
    
    for ( umm i = 0; i < count; i++ ) {
        
        string_format_t* format = formats + i;
        
        switch ( format->type ) {
            
            case string_format_type_text: {
                memory_push_copy_p( memory, format->t, format->length, 1 );
            } break;
            
            case string_format_type_unsigned: {
                string_push_u64( memory, format->u, format->desired_digit_count, format->fill_with, format->hexadecimal_format );
            } break;
            
            case string_format_type_signed: {
                string_push_s64( memory, format->s, format->desired_digit_count, format->fill_with, format->hexadecimal_format );
            } break;
            
            case string_format_type_floating_point_32: {
                string_push_f32( memory, format->f_32, format->before_decimal_point, format->fill_with_float, format->after_decimal_point, format->format );
            } break;
            
            case string_format_type_floating_point_64: {
                string_push_f64( memory, format->f_64, format->before_decimal_point, format->fill_with_float, format->after_decimal_point, format->format );
            } break;
        }
    }
    
    result.reserved = result.used = memory_current( deref( memory ), u8 ) - result.bytes;
    
    return result;
}

stu data_t string_shrink_from_start( data_t string, data_t byte_list ) {
    
    u8* end = memory_end( string );
    
    while ( string.bytes < end ) {
        
        if ( char_is_any_u8( deref( string.bytes ), byte_list.bytes, byte_list.used ) ) {
            string.bytes++;
            string.used--;
        } else {
            break;
        }
    }
    
    return string;
}

stu data_t string_shrink_from_end( data_t string, data_t byte_list ) {
    
    u8* end = memory_end( string ) - 1; /* NOTE simon: -1 because memory_end is one past the last character of the string. */
    
    while ( end >= string.bytes ) {
        
        if ( char_is_any_u8( deref( end ), byte_list.bytes, byte_list.used ) ) {
            string.used--;
            end--;
        } else {
            break;
        }
    }
    
    return string;
}

/* MARK string iterator */

typedef struct string_iterator_t {
    u8* start, *end, *current;
    data_t value;
} string_iterator_t;

string_iterator_t string_iterator_make( data_t buffer ) {
    string_iterator_t result = { 0 };
    result.start = result.current = buffer.bytes;
    result.end = memory_end( buffer );
    return result;
}

string_iterator_t string_iterator_make_start_end( u8* start, u8* end ) {
    string_iterator_t result = { 0 };
    result.start = result.current = start;
    result.end = end;
    return result;
}

#define string_iterator_has_byte_left( iterator ) ( ( iterator )->current < ( iterator )->end )

#define string_iterator_advance_byte( iterator ) ( ( iterator )->current++ )

#define string_iterator_skip_whitespace( iterator ) \
while ( string_iterator_has_byte_left( iterator ) && char_is_whitespace( ( *( ( iterator ) )->current ) ) ) { \
string_iterator_advance_byte( iterator ); \
}

data_t string_iterator_next_non_whitespace( string_iterator_t* iterator ) {
    
    data_t result = { 0 };
    
    if ( string_iterator_has_byte_left( iterator ) ) {
        
        string_iterator_skip_whitespace( iterator );
        
        u8* start = iterator->current;
        
        while ( string_iterator_has_byte_left( iterator ) && !char_is_whitespace( *( iterator->current ) ) ) {
            string_iterator_advance_byte( iterator );
        }
        
        if ( iterator->current - start ) {
            result = memory_start_end( start, iterator->current );
        }
    }
    
    iterator->value = result;
    
    return result;
}

data_t string_iterator_next_whitespaces( string_iterator_t* iterator ) {
    
    data_t result = { 0 };
    
    if ( string_iterator_has_byte_left( iterator ) ) {
        
        while ( !char_is_whitespace( *iterator->current ) ) {
            string_iterator_advance_byte( iterator );
        }
        
        u8* start = iterator->current;
        
        while ( string_iterator_has_byte_left( iterator ) && char_is_whitespace( *iterator->current ) ) {
            string_iterator_advance_byte( iterator );
        }
        
        if ( iterator->current - start ) {
            result = memory_start_end( start, iterator->current );
        }
    }
    
    iterator->value = result;
    
    return result;
}

data_t string_iterator_next_until( string_iterator_t* iterator, u8 end_byte ) {
    
    data_t result = { 0 };
    u8* start = iterator->current;
    
    while ( string_iterator_has_byte_left( iterator ) && *iterator->current != end_byte ) {
        string_iterator_advance_byte( iterator );
    }
    
    if ( iterator->current - start ) {
        result = memory_start_end( start, iterator->current );
    }
    
    iterator->value = result;
    
    return result;
}

// MARK hash

/* NOTE simon: http://www.isthe.com/chongo/tech/comp/fnv/ */

_ctassert( sizeof( 0x811c9dc5ul ) == 4 );

u32 hash_fnv_32( void* bytes_arg, umm length ) {
    
    u8* bytes = cast( u8*, bytes_arg );
    u8* end = bytes + length;
    
    u32 result = 0x811c9dc5ul;
    
    while ( bytes < end ) {
        result ^= bytes[ 0 ];
        result *= 0x1000193ul;
        bytes++;
    }
    
    return result;
}

_ctassert( sizeof( 0xcbf29ce484222325ull ) == 8 );

u64 hash_fnv_64( void* bytes_arg, umm length ) {
    
    u8* bytes = cast( u8*, bytes_arg );
    u8* end = bytes + length;
    
    u64 result = 0xcbf29ce484222325ull;
    
    while ( bytes < end ) {
        result ^= bytes[ 0 ];
        result *= 0x100000001b3ull;
        bytes++;
    }
    
    return result;
}

/* The MIT License

   Copyright (C) 2012 Zilong Tan (eric.zltan@gmail.com)

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

/* NOTE simon:
https://code.google.com/archive/p/fast-hash/
https://github.com/ztanml/fast-hash
*/

u64 internal_hash_fasthash_mix( u64 n ) {
    
    n ^= ( n >> 23 );
    n *= 0x2127599bf4325c37ull;
    n ^= ( n >> 47 );
    
    return n;
}

/* NOTE simon: The only use I found of fasthash passed 0 as the seed, so I guess it's ok to pass 0. */
u64 hash_fasthash_64( void* bytes, umm length, u64 seed ) {
    
    u64 m = 0x880355f21e6d1965ull;
    u64* current = cast( u64*, bytes );
    u64* end = current + ( length / 8 );
    
    u64 result = seed ^ ( length * m );
    
    while ( current < end ) {
        
        u64 x = *current;
        result ^= internal_hash_fasthash_mix( x );
        result *= m;
        current++;
    }
    
    u8* bytes_current = cast( u8*, current );
    u8 byte_count = length & 7;
    u64 x = 0;
    
    _assert( byte_count <= 7 );
    
    if ( byte_count ) {
        
        for ( umm i = 0; i < byte_count; i++ ) {
            u64 byte = ( u64 ) *bytes_current;
            byte <<= ( 8 * ( i - 1 ) );
            x |= byte;
            bytes_current++;
        }
        
        result ^= internal_hash_fasthash_mix( x );
        result *= m;
    }
    
    result = internal_hash_fasthash_mix( result );
    
    return result;
}

u32 hash_fasthash_32( void* bytes, umm length, u32 seed ) {
    
    u64 hash = hash_fasthash_64( bytes, length, seed );
    u32 result = ( u32 ) ( hash - ( hash >> 32 ) );
    return result;
}

// MARK math

/* TODO simon: investigate
unlerp(a, x, b) = lerp(-a/(b-a), x, (1-a)/(b-a))
linear_remap(x, a,b,c,d) = lerp(c, unlerp(a,x,b), d) // == (x-a)/(b-a)*(d-c)+c
*/

#include <math.h>

f32 math_pi = 3.14159265358979323846f;
f32 math_tau = 6.28318530717958647692f;
f32 math_degree_to_radian = 0.01745329251994329576f; /* 0.01745329251994329576923690768489f = ( math_pi / 180.0f ) */
f32 math_radian_to_degree = 57.29577951308232087679f; /* 57.295779513082320876798154814105f = ( 180.0f / math_pi ) */;

#define math_deg_to_rad( n ) ( ( n ) * math_degree_to_radian )
#define math_rad_to_deg( n ) ( ( n ) * math_radian_to_degree )
#define math_turn_to_rad( n ) ( ( n ) * math_tau )
#define math_rad_to_turn( n ) ( ( n ) / math_tau )
#define math_turn_to_deg( n ) ( ( n ) * 360.0f )
#define math_deg_to_turn( n ) ( ( n ) / 360.0f )

stu u32 math_rotate_right_u32( u32 value, u8 bit_count ) {
    
#if defined( COMPILER_CLANG ) && defined( TARGET_x64 )
    u32 result = __builtin_rotateright32( value, bit_count );
#elif defined( COMPILER_CL )
    u32 result = _rotr( value, bit_count );
#else
    u32 result = ( value >> bit_count ) | ( value << ( 32 - bit_count ) );
#endif
    
    return result;
}

stu u32 math_rotate_left_u32( u32 value, u8 bit_count ) {
    
#if defined( COMPILER_CLANG ) && defined( TARGET_x64 )
    u32 result = __builtin_rotateleft32( value, bit_count );
#elif defined( COMPILER_CL )
    u32 result = _rotl( value, bit_count );
#else
    u32 result = ( value << bit_count ) | ( value >> ( 32 - bit_count ) );
#endif
    
    return result;
}

stu u64 math_rotate_right_u64( u64 value, u8 bit_count ) {
    
#if defined( COMPILER_CLANG ) && defined( TARGET_x64 )
    u64 result = __builtin_rotateright64( value, bit_count );
#elif defined( COMPILER_CL )
    u64 result = _rotr64( value, bit_count );
#else
    u64 result = ( value >> bit_count ) | ( value << ( 64 - bit_count ) );
#endif
    return result;
}

stu u64 math_rotate_left_u64( u64 value, u8 bit_count ) {
    
#if defined( COMPILER_CLANG ) && defined( TARGET_x64 )
    u64 result = __builtin_rotateleft64( value, bit_count );
#elif defined( COMPILER_CL )
    u64 result = _rotl64( value, bit_count );
#else
    u64 result = ( value << bit_count ) | ( value >> ( 64 - bit_count ) );
#endif
    return result;
}

stu u32 math_safe_truncate_to_u32( u64 value ) {
    
    u32 result;
    _assert( value <= u32_max );
    result = ( u32 ) value;
    
    return result;
}

stu s32 math_safe_truncate_to_s32( s64 value ) {
    
    u32 result;
    _assert( value >= s32_min && value <= s32_max);
    result = ( s32 ) value;
    
    return result;
}

stu u16 math_safe_truncate_to_u16( u64 value ) {
    
    u16 result;
    _assert( value <= u16_max );
    result = ( u16 ) value;
    
    return result;
}

stu u16 math_safe_truncate_to_s16( s64 value ) {
    
    u16 result;
    _assert( value >= s16_min && value <= s16_max );
    result = ( u16 ) value;
    
    return result;
}

u64 math_add_no_wrap_u64( u64 initial_value, u64 to_add ) {
    
    u64 after = u64_max - initial_value;
    u64 result = initial_value + math_min( after, to_add );
    return result;
}

u64 math_sub_no_wrap_u64( u64 initial_value, u64 to_sub ) {
    
    u64 before = initial_value;
    u64 result = initial_value - math_min( before, to_sub );
    return result;
}

stu f32 math_abs( f32 number ) {
    
    if ( number < 0 ) {
        number *= -1;
    }
    
    return number;
}

// NOTE simon: fast inverse square root (quake 3)
// https://en.wikipedia.org/wiki/Fast_inverse_square_root
// https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Approximations_that_depend_on_the_floating_point_representation
stu f32 math_rsqrt( f32 number ) {
    
    s32 i;
    f32 x2, y;
    const f32 threehalfs = 1.5f;
    
    x2 = number * 0.5f;
    y  = number;
    i  = deref( cast( s32*, &y ) );                       // evil floating point bit level hacking
    i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
    y  = deref( cast( f32*, &i ) );
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
    //	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
    
    return y;
}

stu f32 math_sign( f32 number ) {
    
    f32 sign;
    
    if ( number < 0 ) {
        sign = -1;
    } else {
        sign = 1;
    }
    
    return sign;
}

stu f32 math_sign_or_zero( f32 number ) {
    
    f32 sign = 0;
    
    if ( number > 0.0f ) {
        sign = 1.0f;
    } else if ( number < 0.0f ) {
        sign = -1.0f;
    }
    
    return sign;
}

/* NOTE simon: This is not properly tested. */
stu s32 math_s24_to_s32( u8* s24 ) {
    
    s32 result;
    
    s32 raw_value = deref( cast( s32*, s24 ) );
    s32 sign = ( raw_value & 0x00800000 ) >> 23;
    result = raw_value & 0x007fffff;
    result = result | ( 0xff800000 * sign );
    
    return result;
}

/* NOTE simon: This is not properly tested. */
stu s32 math_s32_to_s24( s32 s ) {
    
    s32 result;
    
    s32 sign = ( s & 0x80000000 ) >> 8;
    result = s & 0x00ffffff;
    result |= sign;
    
    return result;
}

f32 math_bezier_quadratic( f32 start, f32 end, f32 control, f32 x ) {
    
    f32 y = 1 - x;
    
#if 0
    f32 p1 = start * y + control * x;
    f32 p2 = control * y + end * x;
    f32 result = p1 * y + p2 * x;
#else
    /* 
    ( ( start * y ) + ( control * x ) ) * y + ( ( control * y ) + ( end * x ) ) * x;
    ( y * ( start * y ) + y * ( control * x ) ) + ( x * ( control * y ) + x ( end * x ) )
    start*y*y + y*x*control + x*y*control + x*x*end
    */
    f32 result = start * y * y + 2 * y * x * control + x * x * end;
#endif
    
    return result;
}

f32 math_bezier_cubic( f32 start, f32 end, f32 control_1, f32 control_2, f32 x ) {
    
    f32 y = 1 - x;
    
#if 0
    f32 p1 = start * y + control_1 * x;
    f32 p2 = control_1 * y + control_2 * x;
    f32 p3 = control_2 * y + end * x;
    f32 p4 = p1 * y + p2 * x;
    f32 p5 = p2 * y + p3 * x;
    f32 result = p4 * y + p5 * x;
#else
    
    /*
    ( p1 * y + p2 * x ) * y + ( p2 * y + p3 * x ) * x

      ( ( start * y + control_1 * x ) * y + ( control_1 * y + control_2 * x ) * x ) * y
    + ( ( control_1 * y + control_2 * x ) * y + ( control_2 * y + end * x ) * x ) * x

      ( start * y * y + control_1 * x * y + control_1 * y * x + control_2 * x * x ) * y
    + ( control_1 * y * y + control_2 * x * y + control_2 * y * x + end * x * x ) * x

      start * y * y * y + control_1 * x * y * y + control_1 * y * x * y + control_2 * x * x * y
    + control_1 * y * y * x + control_2 * x * y * x + control_2 * y * x * x + end * x * x * x 

    start * yyy + control_1 * xyy + control_1 * xyy + control_2 * xxy + control_1 * yyx + control_2 * xxy + control_2 * xxy + end * xxx

    start * yyy + control_1 * xyy * 2 + control_2 * xxy * 2 + control_1 * xyy + control_2 * xxy + end * xxx
    */
    
    f32 yyy = y * y * y;
    f32 xxx = x * x * x;
    f32 xyy = x * y * y;
    f32 xxy = x * x * y;
    f32 result = start * yyy + control_1 * xyy * 3 + control_2 * xxy * 3 + end * xxx; 
#endif
    
    return result;
}

#if defined( WINDOWS )

#if defined( COMPILER_CLANG ) || defined( COMPILER_GCC )
#include <xmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#endif

#if defined( COMPILER_CL )
#define m128_f32( vector, index ) ( vector.m128_f32[ ( index ) ] )
#define m128_f64( vector, index ) ( vector.m128d_f64[ ( index ) ] )
#elif defined( COMPILER_CLANG ) || defined( COMPILER_GCC )
#define m128_f32( vector, index ) ( vector[ ( index ) ] )
#define m128_f64( vector, index ) ( vector[ ( index ) ] )
#else
# error Unsupported platform.
#endif


double math_fabs( double x ) {
    union { double f; uint64_t i; } u = { x };
    u.i &= 0x7fffffffffffffff;
    return u.f;
}

float math_fabsf( float x ) {
    union { float f; uint32_t i; } u = { x };
    u.i &= 0x7fffffff;
    return u.f;
}

double math_round( double x ) {
    
    __m128d a = _mm_set_pd1( x );
    __m128d result = _mm_round_pd( a, _MM_FROUND_CUR_DIRECTION );
    
    return m128_f64( result, 0 );
}

float math_roundf( float x ) {
    
    __m128 a = _mm_set_ps1( x );
    __m128 result = _mm_round_ps( a, _MM_FROUND_CUR_DIRECTION );
    
    return m128_f32( result, 0 );
}

stu void math_roundf_4( f32* xyzw ) {
    __m128 a = _mm_loadu_ps( xyzw );
    __m128 result = _mm_round_ps( a, _MM_FROUND_CUR_DIRECTION );
    _mm_storeu_ps( xyzw, result );
}

double math_ceil( double x ) {
    
    __m128d a = _mm_set_pd1( x );
    __m128d result = _mm_ceil_pd( a );
    
    return m128_f64( result, 0 );
}

float math_ceilf( float x ) {
    
    __m128 a = _mm_set_ps1( x );
    __m128 result = _mm_ceil_ps( a );
    
    return m128_f32( result, 0 );
}

double math_floor( double x ) {
    
    __m128d a = _mm_set_pd1( x );
    __m128d result = _mm_floor_pd( a );
    
    return m128_f64( result, 0 );
}

float math_floorf( float x ) {
    
    __m128 a = _mm_set_ps1( x );
    __m128 result = _mm_floor_ps( a );
    
    return m128_f32( result, 0 );
}

double math_sqrt( double x ) {
    
    __m128d a = _mm_set_pd1( x );
    __m128d result = _mm_sqrt_pd( a );
    
    return m128_f64( result, 0 );
}

float math_sqrtf( float x ) {
    
    __m128 a = _mm_set_ps1( x );
    __m128 result = _mm_sqrt_ps( a );
    
    return m128_f32( result, 0 );
}

f64 math_lerp( f64 start, f64 factor, f64 end ) {
    f64 result = ( start * ( 1.0 - factor ) ) + ( end * factor );
    return result;
}

f32 math_lerpf( f32 start, f32 factor, f32 end ) {
    f32 result = ( start * ( 1.0f - factor ) ) + ( end * factor );
    return result;
}

#if defined( TARGET_x64 ) && defined( WINDOWS )

/* TODO simon: It seems that it might be better to not define the 128bit type because
sometimes msvc generates very slow code (code with __addcarry_u64). */

typedef struct u128_t {
    u64 low, high;
} u128_t;

#define u128_make( low, high ) ( u128_t ) { low, high }

u128_t math_u128_mul( u128_t a, u128_t b ) {
    
    /* NOTE simon: 
    Idea is to do similiar thing as written multiplication:
    multiply each digit from a with each digit of b and add each multiplication result.
    But we use 64bit instead of digits.

    The a.low + b.low is a 64bit * 64bit multiply where the result is 128bits. We keep every 128bits
    using the `mul` instruction using rax (implied) and another register and the result is stored
    in rdx (high 64bits) and rax (low 64bits).

    We than multiply a.low with b.high which is a regular 64bit multiply because we can't store
    the bit that would overflow (bit that would be outside of the 128bits). We do a similar thing with
    a.high and b.low.

    We don't need to multiply a.high with b.high as that can only produce bits higher than the 128bits.
    e.g. If we used 2 digits numbers, the smallest we could do would be 10 * 10 which is 100 and
    requires 3 digits.

    The result is adding the results from those calculations.
    */
    
    u128_t result;
    
    result.low = _umul128( a.low, b.low, &result.high );
    u64 high_1 = a.low * b.high;
    u64 high_2 = a.high * b.low;
    result.high += high_1 + high_2;
    
    return result;
}

u128_t math_u128_add( u128_t a, u128_t b ) {
    
    u128_t result;
    
#if 1
    /* NOTE simon: This was faster than the other versions. */
    result.low = a.low + b.low;
    result.high = a.high + b.high + ( ( result.low < a.low ) ? 1 : 0 );
#else
    /* NOTE simon: clang ? __builtin_addcll */
    /* NOTE simon: GCC ? __builtin_ia32_addcarryx_u64 */
    u8 carry = _addcarry_u64( 0, a.low, b.low, &result.low );
    _addcarry_u64( carry, a.high, b.high, &result.high );
#endif
    
    return result;
}

u128_t math_u128_add_u64( u128_t a, u64 b ) {
    
    u128_t result;
    
#if 1
    result.low = a.low + b;
    result.high = a.high + ( ( result.low < a.low ) ? 1 : 0 );
#else
    u8 carry = _addcarry_u64( 0, a.low, b, &result.low );
    _addcarry_u64( carry, a.high, 0, &result.high );
#endif
    
    return result;
}

u128_t math_u128_left_shift( u128_t a, u8 bit_count ) {
    
    u128_t result;
    
#if 1
    result.high = __shiftleft128( a.low, a.high, bit_count );
    result.low = a.low << bit_count;
#else
    
    if ( bit_count <= 64 ) {
        result.low = a.low << bit_count;
        result.high = a.high << bit_count;
        result.high |= a.low >> ( 64 - bit_count );
    } else {
        result.low = 0;
        result.high = a.low << ( bit_count - 64 );
    }
#endif
    
    return result;
}

u128_t math_u128_right_shift( u128_t a, u8 bit_count ) {
    
    u128_t result;
    
#if 1
    result.low = __shiftright128( a.low, a.high, bit_count );
    result.high = a.high >> bit_count;
#else
    
    if ( bit_count <= 64 ) {
        result.high = a.high >> bit_count;
        result.low = a.low >> bit_count;
        result.low |= a.high << ( 64 - bit_count );
    } else {
        result.high = 0;
        result.low = a.high >> ( bit_count - 64 );
    }
#endif
    
    return result;
}

stu u64 math_u32_multiply( u32 a, u32 b ) {
    
#if 1
    u64 result = __emulu( a, b );
#else
    u64 result = ( u64 ) a * ( u64 ) b;
#endif
    
    return result;
}

stu u128_t math_u64_multiply( u64 a, u64 b ) {
    
    u128_t result;
    result.low = _umul128( a, b, &result.high );
    
    return result;
}

/* NOTE simon: Multiply two 64bits with 128 precision but return only the high bits of the 128bits.*/
stu u64 math_u64_multiply_high( u64 a, u64 b ) {
    u64 result = __umulh( a, b );
    return result;
}

stu u128_t math_multiply_u64_u128_high( u64 a, u128_t b ) {
    
    u128_t result = math_u64_multiply( a, b.high );
    u64 temp = math_u64_multiply_high( a, b.low );
    result = math_u128_add_u64( result, temp );
    
    return result;
}

stu u128_t math_multiply_u64_u128_low( u64 a, u128_t b ) {
    
    u128_t result = math_u64_multiply( a, b.low );
    result.high += ( a * b.high );
    
    return result;
}

stu u64 math_multiply_u32_u64_high( u32 a, u64 b ) {
    u64 result = math_u64_multiply_high( ( ( u64 ) a ) << 32, b );
    return result;
}

stu u64 math_multiply_u32_u64_low( u32 a, u64 b ) {
    u64 result = a * b;
    return result;
}

#endif

#endif

stu f32 interpolation_linear( f32 start, f32 end, f32 t ) {
    _assert( t >= 0 && t <= 1 );
    f32 result = start * ( 1 - t ) + end * t;
    return result;
}

stu f32 interpolation_quadratic( f32 start, f32 cp, f32 end, f32 t ) {
    _assert( t >= 0 && t <= 1 );
    f32 u = 1 - t;
    f32 result = ( start * u * u ) + ( cp * 2 * u * t ) + ( end * t * t );
    return result;
}

stu f32 interpolation_cubic( f32 start, f32 cp_1, f32 cp_2, f32 end, f32 t ) {
    _assert( t >= 0 && t <= 1 );
    f32 u = 1 - t;
    f32 result =  ( u * u * u * start ) + ( 3 * u * u * t * cp_1 ) + ( 3 * u * t * t * cp_2 ) + ( t * t * t * end );
    return result;
}

// MARK sort

void internal_quick_sort_u8_ascending( u8* array, umm start, umm end ) {
    
    if ( array && start < end ) {
        
        umm pivot_index = ( start + end ) / 2;
        u8 pivot_value = array[ pivot_index ];
        umm left = start;
        umm right = end;
        
        while ( 1 ) {
            
            while ( array[ left ] < pivot_value ) {
                left++;
            }
            
            while ( array[ right ] > pivot_value ) {
                right--;
            }
            
            if ( left < right ) {
                swap_type( array[ left ], array[ right ], u8 );
                left++;
                right--;
            } else {
                break;
            }
        }
        
        internal_quick_sort_u8_ascending( array, start, right );
        internal_quick_sort_u8_ascending( array, right + 1, end );
    }
}

void internal_quick_sort_u8_descending( u8* array, umm start, umm end ) {
    
    if ( array && start < end ) {
        
        umm pivot_index = ( start + end ) / 2;
        u8 pivot_value = array[ pivot_index ];
        umm left = start;
        umm right = end;
        
        while ( 1 ) {
            
            while ( array[ left ] > pivot_value ) {
                left++;
            }
            
            while ( array[ right ] < pivot_value ) {
                right--;
            }
            
            if ( left < right ) {
                swap_type( array[ left ], array[ right ], u8 );
                left++;
                right--;
            } else {
                break;
            }
        }
        
        internal_quick_sort_u8_descending( array, start, right );
        internal_quick_sort_u8_descending( array, right + 1, end );
    }
}

void quick_sort_u8_ascending( u8* array, umm count ) {
    if ( count ) {
        internal_quick_sort_u8_ascending( array, 0, count - 1 );
    }
}

void quick_sort_u8_descending( u8* array, umm count ) {
    if ( count ) {
        internal_quick_sort_u8_descending( array, 0, count - 1 );
    }
}

void internal_quick_sort_u32_ascending( u32* array, umm start, umm end ) {
    
    if ( array && start < end ) {
        
        umm pivot_index = ( start + end ) / 2;
        u32 pivot_value = array[ pivot_index ];
        umm left = start;
        umm right = end;
        
        while ( 1 ) {
            
            while ( array[ left ] < pivot_value ) {
                left++;
            }
            
            while ( array[ right ] > pivot_value ) {
                right--;
            }
            
            if ( left < right ) {
                swap_type( array[ left ], array[ right ], u32 );
                left++;
                right--;
            } else {
                break;
            }
        }
        
        internal_quick_sort_u32_ascending( array, start, right );
        internal_quick_sort_u32_ascending( array, right + 1, end );
    }
}

void internal_quick_sort_u32_descending( u32* array, umm start, umm end ) {
    
    if ( array && start < end ) {
        
        umm pivot_index = ( start + end ) / 2;
        u32 pivot_value = array[ pivot_index ];
        umm left = start;
        umm right = end;
        
        while ( 1 ) {
            
            while ( array[ left ] > pivot_value ) {
                left++;
            }
            
            while ( array[ right ] < pivot_value ) {
                right--;
            }
            
            if ( left < right ) {
                swap_type( array[ left ], array[ right ], u32 );
                left++;
                right--;
            } else {
                break;
            }
        }
        
        internal_quick_sort_u32_descending( array, start, right );
        internal_quick_sort_u32_descending( array, right + 1, end );
    }
}

void quick_sort_u32_ascending( u32* array, umm count ) {
    if ( count ) {
        internal_quick_sort_u32_ascending( array, 0, count - 1 );
    }
}

void quick_sort_u32_descending( u32* array, umm count ) {
    if ( count ) {
        internal_quick_sort_u32_descending( array, 0, count - 1 );
    }
}

void internal_quick_sort_u64_ascending( u64* array, umm start, umm end ) {
    
    if ( array && start < end ) {
        
        umm pivot_index = ( start + end ) / 2;
        u64 pivot_value = array[ pivot_index ];
        umm left = start;
        umm right = end;
        
        while ( 1 ) {
            
            while ( array[ left ] < pivot_value ) {
                left++;
            }
            
            while ( array[ right ] > pivot_value ) {
                right--;
            }
            
            if ( left < right ) {
                swap_type( array[ left ], array[ right ], u64 );
                left++;
                right--;
            } else {
                break;
            }
        }
        
        internal_quick_sort_u64_ascending( array, start, right );
        internal_quick_sort_u64_ascending( array, right + 1, end );
    }
}

void internal_quick_sort_u64_descending( u64* array, umm start, umm end ) {
    
    if ( array && start < end ) {
        
        umm pivot_index = ( start + end ) / 2;
        u64 pivot_value = array[ pivot_index ];
        umm left = start;
        umm right = end;
        
        while ( 1 ) {
            
            while ( array[ left ] > pivot_value ) {
                left++;
            }
            
            while ( array[ right ] < pivot_value ) {
                right--;
            }
            
            if ( left < right ) {
                swap_type( array[ left ], array[ right ], u64 );
                left++;
                right--;
            } else {
                break;
            }
        }
        
        internal_quick_sort_u64_descending( array, start, right );
        internal_quick_sort_u64_descending( array, right + 1, end );
    }
}

void quick_sort_u64_ascending( u64* array, umm count ) {
    if ( count ) {
        internal_quick_sort_u64_ascending( array, 0, count - 1 );
    }
}

void quick_sort_u64_descending( u64* array, umm count ) {
    if ( count ) {
        internal_quick_sort_u64_descending( array, 0, count - 1 );
    }
}

void internal_quick_sort_umm_ascending( umm* array, umm start, umm end ) {
    
    if ( array && start < end ) {
        
        umm pivot_index = ( start + end ) / 2;
        umm pivot_value = array[ pivot_index ];
        umm left = start;
        umm right = end;
        
        while ( 1 ) {
            
            while ( array[ left ] < pivot_value ) {
                left++;
            }
            
            while ( array[ right ] > pivot_value ) {
                right--;
            }
            
            if ( left < right ) {
                swap_type( array[ left ], array[ right ], umm );
                left++;
                right--;
            } else {
                break;
            }
        }
        
        internal_quick_sort_umm_ascending( array, start, right );
        internal_quick_sort_umm_ascending( array, right + 1, end );
    }
}

void internal_quick_sort_umm_descending( umm* array, umm start, umm end ) {
    
    if ( array && start < end ) {
        
        umm pivot_index = ( start + end ) / 2;
        umm pivot_value = array[ pivot_index ];
        umm left = start;
        umm right = end;
        
        while ( 1 ) {
            
            while ( array[ left ] > pivot_value ) {
                left++;
            }
            
            while ( array[ right ] < pivot_value ) {
                right--;
            }
            
            if ( left < right ) {
                swap_type( array[ left ], array[ right ], umm );
                left++;
                right--;
            } else {
                break;
            }
        }
        
        internal_quick_sort_umm_descending( array, start, right );
        internal_quick_sort_umm_descending( array, right + 1, end );
    }
}

void quick_sort_umm_ascending( umm* array, umm count ) {
    if ( count ) {
        internal_quick_sort_umm_ascending( array, 0, count - 1 );
    }
}

void quick_sort_umm_descending( umm* array, umm count ) {
    if ( count ) {
        internal_quick_sort_umm_descending( array, 0, count - 1 );
    }
}

/* MARK text */

u32 g_ucs_error = 0;

/* NOTE simon:
utf-8 spec:
http://www.unicode.org/versions/Unicode11.0.0/ch02.pdf#G13708 chapter 2.5
http://www.unicode.org/versions/Unicode11.0.0/ch03.pdf#G7404 chapter 3.9
https://tools.ietf.org/html/rfc3629
*/

/*
https://www.unicode.org/faq/utf_bom.html
https://www.unicode.org/faq/private_use.html
https://unicodebook.readthedocs.io/guess_encoding.html#is-utf8
*/

/* NOTE simon: utf8 tests
https://www.w3.org/2001/06/utf-8-test/UTF-8-demo.html
*/

/* NOTE:
codepoint: number that represent a character.
codeunit: group of bit that represent one part of the encoding of a codepoint. 8 bits in utf-8, 16 bits in utf-16.
== From the Unicode spec V11 ==
http://www.unicode.org/versions/Unicode11.0.0/ch03.pdf#G7404
codeunit: The minimal bit combination that can represent a unit of encoded text for processing or interchange.*/

u32 ucs_invalid_codepoint = 0xffffffff;
u32 ucs_last_codepoint = 0x10ffff;
u32 ucs_replacement_codepoint = 0xfffd;

u16 utf16_hi_surrogate_start = 0xD800;
u16 utf16_lo_surrogate_start = 0xDC00;
u16 utf16_surrogate_end = 0xDFFF;

typedef enum utf_error {
    
    utf_error_none,
    
    utf_error_end_of_bytes,
    utf_error_not_enough_bytes_to_decode,
    utf_error_not_enough_bytes_to_encode,
    utf_error_invalid_codepoint_to_encode,
    utf_error_end_is_before_start,
    
    utf_error_utf8_invalid_lead_byte,
    utf_error_utf8_invalid_continuation_byte,
    utf_error_utf8_invalid_encoding,
    
    utf_error_utf16_invalid_surrogate_pair,
    
    utf_error_count
} utf_error;

stu uint32_t ucs_is_codepoint_valid( uint32_t codepoint ) {
    uint32_t result = codepoint < utf16_hi_surrogate_start;
    result = result || ( codepoint > utf16_surrogate_end && codepoint <= ucs_last_codepoint );
    return result;
}

/* MARK utf-8 */

#define utf8_is_lead_byte( byte ) ( \
( ( ( byte ) & b1000_0000 ) == 0 ) \
|| ( ( ( byte ) & b1110_0000 ) == b1100_0000 ) \
|| ( ( ( byte ) & b1111_0000 ) == b1110_0000 ) \
|| ( ( ( byte ) & b1111_1000 ) == b1111_0000 ) \
)

#define utf8_is_continuation_byte( byte ) ( ( ( byte ) & b1100_0000 ) == b1000_0000 )

/* When error is set, you can't use codeunit_count to advance the bytes pointer, you should advance only 1 byte. */
stu void utf8_decode( void* bytes, void* bytes_end, uint32_t* codepoint, uint8_t* codeunit_count, uint32_t* error ) {
    
    if ( no_error( error ) ) {
        
        deref( codepoint ) = ucs_invalid_codepoint;
        deref( codeunit_count ) = 0;
        
        if ( bytes < bytes_end ) {
            
            uint8_t bytes_seen = 0;
            uint8_t bytes_needed = 0;
            uint32_t lower_boundary = 0x80;
            uint32_t upper_boundary = 0xbf;
            
            uint8_t* current = cast( uint8_t*, bytes );
            uint8_t* end = cast( uint8_t*, bytes_end );
            uint8_t byte = deref( current );
            
            if ( byte <= 0x7f ) {
                
                deref( codeunit_count ) = 1;
                deref( codepoint ) = byte;
                
            } else if ( byte >= 0xc2 && byte <= 0xdf ) {
                
                bytes_needed = 1;
                deref( codeunit_count ) = 2;
                deref( codepoint ) = byte & 0x1f;
                
            } else if ( byte >= 0xe0 && byte <= 0xef ) {
                
                if ( byte == 0xe0 ) {
                    lower_boundary = 0xa0;
                } else if ( byte == 0xed ) {
                    upper_boundary = 0x9f;
                }
                
                bytes_needed = 2;
                deref( codeunit_count ) = 3;
                deref( codepoint ) = byte & 0xf;
                
            } else if ( byte >= 0xf0 && byte <= 0xf4 ) {
                
                if ( byte == 0xf0 ) {
                    lower_boundary = 0x90;
                } else if ( byte == 0xf4 ) {
                    upper_boundary = 0x8f;
                }
                
                bytes_needed = 3;
                deref( codeunit_count ) = 4;
                deref( codepoint ) = byte & 0x7;
                
            } else {
                set_error( error, utf_error_utf8_invalid_lead_byte );
            }
            
            if ( no_error( error ) ) {
                
                if ( current + bytes_needed < end ) {
                    
                    while ( no_error( error ) && bytes_seen < bytes_needed ) {
                        
                        current++;
                        byte = deref( current );
                        
                        if ( byte < lower_boundary || byte > upper_boundary ) {
                            
                            set_error( error, utf_error_utf8_invalid_continuation_byte );
                            
                        } else {
                            
                            deref( codepoint ) = ( deref( codepoint ) << 6 ) | ( byte & 0x3f );
                            bytes_seen++;
                            lower_boundary = 0x80;
                            upper_boundary = 0xbf;
                        }
                    }
                    
                } else {
                    set_error( error, utf_error_not_enough_bytes_to_decode );
                }
            }
            
            if ( is_error( error ) ) {
                deref( codepoint ) = ucs_invalid_codepoint;
            }
        } else {
            set_error( error, utf_error_end_of_bytes );
        }
    }
}

stu void utf8_encode( void* bytes, void* bytes_end, uint32_t codepoint, uint8_t* codeunit_count, uint32_t* error ) {
    
    if ( no_error( error ) ) {
        
        deref( codeunit_count ) = 0;
        
        if ( ucs_is_codepoint_valid( codepoint ) ) {
            
            uint8_t temp[ 4 ] = { 0 };
            
            if ( codepoint <= 0x7f ) {
                deref( codeunit_count ) = 1;
                temp[ 0 ] = ( uint8_t ) codepoint;
            } else if ( codepoint < 0x7ff ) {
                deref( codeunit_count ) = 2;
                temp[ 0 ] = ( uint8_t ) ( ( ( codepoint >> 6 ) & 0x1f ) | 0xc0 );
                temp[ 1 ] = ( uint8_t ) ( ( ( codepoint >> 0 ) & 0x3f ) | 0x80 );
            } else if ( codepoint < 0xffff ) {
                deref( codeunit_count ) = 3;
                temp[ 0 ] = ( uint8_t ) ( ( ( codepoint >> 12 ) & 0x0f ) | 0xe0 );
                temp[ 1 ] = ( uint8_t ) ( ( ( codepoint >> 6 ) & 0x3f ) | 0x80 );
                temp[ 2 ] = ( uint8_t ) ( ( ( codepoint >> 0 ) & 0x3f ) | 0x80 );
            } else if ( codepoint < 0x10ffff ) {
                deref( codeunit_count ) = 4;
                temp[ 0 ] = ( uint8_t ) ( ( ( codepoint >> 18 ) & 0x07 ) | 0xf0 );
                temp[ 1 ] = ( uint8_t ) ( ( ( codepoint >> 12 ) & 0x3f ) | 0x80 );
                temp[ 2 ] = ( uint8_t ) ( ( ( codepoint >> 6 ) & 0x3f ) | 0x80 );
                temp[ 3 ] = ( uint8_t ) ( ( ( codepoint >> 0 ) & 0x3f ) | 0x80 );
            }
            
            if ( bytes ) {
                
                uint8_t* current = cast( uint8_t*, bytes );
                uint8_t* end = cast( uint8_t*, bytes_end );
                uintptr_t length = end - current;
                
                if ( end > current && length >= *codeunit_count ) {
                    
                    for ( uintptr_t i = 0; i < *codeunit_count; i++ ) {
                        current[ i ] = temp[ i ];
                    }
                } else {
                    set_error( error, utf_error_not_enough_bytes_to_encode );
                }
            }
        } else {
            set_error( error, utf_error_invalid_codepoint_to_encode );
        }
    }
}

stu umm utf8_get_codepoint_count( void* codeunits, void* codeunits_end ) {
    
    umm codepoint_count = 0;
    u8* current = cast( u8*, codeunits );
    u8* end = cast( u8*, codeunits_end );
    
    while ( current < end ) {
        
        u32 error = 0;
        u32 codepoint = ucs_invalid_codepoint;
        u8 codeunit_count = 0;
        utf8_decode( current, end, &codepoint, &codeunit_count, &error );
        codepoint_count++;
        
        if ( no_error( &error ) ) {
            current += codeunit_count;
        } else {
            current += 1;
        }
    }
    
    return codepoint_count;
}

/* MARK utf-16 */

stu uint16_t utf16_get_codeunit( uint8_t* bytes, uint32_t big_endian ) {
    
    uint16_t byte_1 = bytes[ 0 ];
    uint16_t byte_2 = bytes[ 1 ];
    uint16_t codeunit;
    
    if ( big_endian ) {
        codeunit = ( byte_1 << 8 ) + byte_2;
    } else {
        codeunit = ( byte_2 << 8 ) + byte_1;
    }
    
    return codeunit;
}

/* NOTE simon: on error don't use codeunit_count to advance the bytes pointer, advance 1 codeunit (2 bytes). */
stu void utf16_decode( void* bytes, void* bytes_end, uint32_t* codepoint, uint8_t* codeunit_count, uint32_t big_endian, uint32_t* error ) {
    
    if ( no_error( error ) ) {
        
        uint8_t* current = cast( uint8_t*, bytes );
        uint8_t* end = cast( uint8_t*, bytes_end );
        deref( codepoint ) = ucs_invalid_codepoint;
        deref( codeunit_count ) = 0;
        
        if ( current + 1 < end ) {
            
            uint16_t codeunit = utf16_get_codeunit( current, big_endian );
            current += 2;
            
            if ( codeunit < utf16_hi_surrogate_start || codeunit > utf16_surrogate_end ) {
                
                deref( codepoint ) = codeunit;
                deref( codeunit_count ) = 1;
                
            } else if ( current + 1 < end ) {
                
                uint16_t hi_surrogate = codeunit;
                uint16_t lo_surrogate = utf16_get_codeunit( current, big_endian );
                deref( codeunit_count ) = 2;
                
                uint32_t hi_is_valid = hi_surrogate < utf16_lo_surrogate_start;
                uint32_t lo_is_valid = lo_surrogate >= utf16_lo_surrogate_start && lo_surrogate <= utf16_surrogate_end;
                
                if ( hi_is_valid && lo_is_valid ) {
                    deref( codepoint ) = ( hi_surrogate - utf16_hi_surrogate_start ) << 10;
                    deref( codepoint ) |= ( lo_surrogate - utf16_lo_surrogate_start );
                    deref( codepoint ) += 0x10000;
                } else {
                    set_error( error, utf_error_utf16_invalid_surrogate_pair );
                }
                
            } else {
                deref( codeunit_count ) = 2;
                set_error( error, utf_error_not_enough_bytes_to_decode );
            }
            
        } else {
            deref( codeunit_count ) = 1;
            set_error( error, utf_error_end_of_bytes );
        }
    }
}

stu void utf16_encode( void* bytes, void* bytes_end, uint32_t codepoint, uint8_t* codeunit_count, uint32_t big_endian, uint32_t* error ) {
    
    if ( no_error( error ) ) {
        
        deref( codeunit_count ) = 0;
        
        if ( ucs_is_codepoint_valid( codepoint ) ) {
            
            uint16_t temp[ 2 ] = { 0 };
            
            if ( codepoint <= 0xffff ) {
                deref( codeunit_count ) = 1;
                temp[ 0 ] = ( uint16_t ) codepoint;
            } else {
                deref( codeunit_count ) = 2;
                codepoint -= 0x10000;
                temp[ 0 ] = ( uint16_t ) ( ( codepoint >> 10 ) + utf16_hi_surrogate_start );
                temp[ 1 ] = ( uint16_t ) ( ( codepoint & 0x3ff ) + utf16_lo_surrogate_start );
            }
            
            if ( bytes ) {
                
                uint8_t* current = cast( uint8_t*, bytes );
                uint8_t* end = cast( uint8_t*, bytes_end );
                uintptr_t length = end - current;
                
                if ( length >= deref( codeunit_count ) * 2 ) {
                    
                    for ( uintptr_t i = 0; i < deref( codeunit_count ); i++ ) {
                        
                        if ( big_endian ) {
                            deref( current ) = ( uint8_t ) ( temp[ i ] >> 8 );
                            current++;
                            deref( current ) = ( uint8_t ) ( temp[ i ] & 0xff );
                        } else {
                            deref( current ) = ( uint8_t ) ( temp[ i ] & 0xff );
                            current++;
                            deref( current ) = ( uint8_t ) ( temp[ i ] >> 8 );
                        }
                        
                        current++;
                    }
                    
                } else {
                    set_error( error, utf_error_not_enough_bytes_to_encode );
                }
            }
        } else {
            set_error( error, utf_error_invalid_codepoint_to_encode );
        }
    }
}

stu umm utf16_get_codepoint_count( void* codeunits, void* codeunits_end ) {
    
    umm codepoint_count = 0;
    u16* current = cast( u16*, codeunits );
    u16* end = cast( u16*, codeunits_end );
    
    while ( current < end ) {
        
        u32 error = 0;
        u32 codepoint = ucs_invalid_codepoint;
        u8 codeunit_count = 0;
        utf16_decode( current, end, &codepoint, &codeunit_count, 0, &error );
        codepoint_count++;
        
        if ( no_error( &error ) ) {
            current += codeunit_count;
        } else {
            current += 1;
        }
    }
    
    return codepoint_count;
}

stu umm utf16_get_codeunit_count( void* source ) {
    
    u16* start = cast( u16*, source );
    u16* end = start;
    
    while ( deref( end ) != 0 ) {
        end++;
    }
    
    umm result = end - start;
    return result;
}

stu umm utf16_to_utf8_p( void* source_codeunits, umm source_codeunit_count, void* destination_codeunits, umm output_buffer_byte_count, u32* error ) {
    
    umm result = 0;
    
    u16* u16_current = cast( u16*, source_codeunits );
    u16* u16_end = u16_current + source_codeunit_count;
    u8* u8_current = cast( u8*, destination_codeunits );
    u8* u8_end = u8_current + output_buffer_byte_count;
    
    while ( no_error( error ) && u16_current < u16_end ) {
        
        u32 codepoint = 0;
        u8 u8_codeunit_count = 0;
        u8 u16_codeunit_count = 0;
        
        utf16_decode( u16_current, u16_end, &codepoint, &u16_codeunit_count, 0, error );
        
        if ( is_error( error ) ) {
            codepoint = ucs_replacement_codepoint;
            u16_codeunit_count = 1;
            clear_error( error );
        }
        
        utf8_encode( u8_current, u8_end, codepoint, &u8_codeunit_count, error );
        
        if ( no_error( error ) ) {
            u8_current += u8_codeunit_count;
            u16_current += u16_codeunit_count;
            result += u8_codeunit_count;
        }
    }
    
    return result;
}

/* NOTE simon: This hasn't been tested at all. */
stu umm utf16_to_utf8( data_t source, data_t* destination, u32* error ) {
    
    u8* current = memory_current( *destination, u8 );
    umm left = memory_left( *destination );
    umm utf16_codeunit_count = source.used / 2;
    _assert( ( utf16_codeunit_count * 2 ) == source.used );
    umm result = utf16_to_utf8_p( source.bytes, utf16_codeunit_count, current, left, error );
    destination->used += result;
    
    return result;
}

/* NOTE simon: The return value is the number of codeunit count, which is the number of u16. It might need to be multiplied by 2 if counting bytes. */
stu umm utf8_to_utf16_p( void* source_codeunits, umm source_codeunit_count, void* destination_codeunits, umm output_buffer_byte_count, u32* error ) {
    
    umm result = 0;
    
    u8* u8_current = cast( u8*, source_codeunits );
    u8* u8_end = u8_current + source_codeunit_count;
    u16* u16_current = cast( u16*, destination_codeunits );
    u16* u16_end = cast( u16*, cast( u8*, u16_current ) + output_buffer_byte_count );
    
    while ( no_error( error ) && u8_current < u8_end ) {
        
        u32 codepoint = 0;
        u8 u8_codeunit_count = 0;
        u8 u16_codeunit_count = 0;
        
        utf8_decode( u8_current, u8_end, &codepoint, &u8_codeunit_count, error );
        
        if ( is_error( error ) ) {
            codepoint = ucs_replacement_codepoint;
            u8_codeunit_count= 1;
            clear_error( error );
        }
        
        utf16_encode( u16_current, u16_end, codepoint, &u16_codeunit_count, 0, error );
        
        if ( no_error( error ) ) {
            u8_current += u8_codeunit_count;
            u16_current += u16_codeunit_count;
            result += u16_codeunit_count;
        }
    }
    
    return result;
}

stu umm utf8_to_utf16( data_t source, data_t* destination, u32* error ) {
    
    u8* current = memory_current( *destination, u8 );
    umm left = memory_left( *destination );
    umm result = utf8_to_utf16_p( source.bytes, source.used, current, left, error );
    destination->used += ( result * 2 );
    
    return result;
}

/* NOTE simon: codepoint and codeunit_count can be null pointers. */
/* TODO simon: Should this always return a valid pointer if there is no error ? If there are no characters (start == end) return start ? */
stu u8* utf8_back_one_codepoint( void* bytes, void* bytes_end, u32* codepoint, u8* codeunit_count, u32* error ) {
    
    u8* result = 0;
    
    breakable {
        
        if ( is_error( error ) ) {
            break;
        }
        
        u8* start = cast( u8*, bytes );
        u8* end = cast( u8*, bytes_end );
        
        if ( end < start ) {
            set_error( error, utf_error_end_is_before_start );
            break;
        }
        
        u8* current = end;
        
        while ( current > start ) {
            
            current--;
            
            if ( end - current > 4 ) {
                set_error( error, utf_error_utf8_invalid_encoding );
                break;
            }
            
            if ( utf8_is_lead_byte( deref( current ) ) ) {
                
                u32 local_codepoint = ucs_invalid_codepoint;
                u8 local_codeunit_count = 0;
                utf8_decode( current, end, &local_codepoint, &local_codeunit_count, error );
                
                if ( is_error( error ) ) {
                    break;
                }
                
                if ( local_codeunit_count != end - current ) {
                    set_error( error, utf_error_utf8_invalid_encoding );
                    break;
                }
                
                if ( codepoint ) {
                    deref( codepoint ) = local_codepoint;
                }
                
                if ( codeunit_count ) {
                    deref( codeunit_count ) = local_codeunit_count;
                }
                
                result = current;
                break;
            }
        }
    }
    
    return result;
}

stu u8* utf8_seek_whitespace_p( void* start_param, void* end_param, u32* error ) {
    
    u8* result = 0;
    
    breakable {
        
        if ( is_error( error ) ) {
            break;
        }
        
        u8* start = cast( u8*, start_param );
        u8* end = cast( u8*, end_param );
        
        if ( end < start ) {
            set_error( error, utf_error_end_is_before_start );
            break;
        }
        
        while ( start < end ) {
            
            if ( !utf8_is_lead_byte( deref( start ) ) ) {
                set_error( error, utf_error_utf8_invalid_encoding );
                break;
            }
            
            u32 codepoint = ucs_invalid_codepoint;
            u8 codeunit_count = 0;
            
            utf8_decode( start, end, &codepoint, &codeunit_count, error );
            
            if ( is_error( error ) ) {
                break;
            }
            
            if ( char_is_whitespace( codepoint ) ) {
                result = start;
                break;
            }
            
            start += codeunit_count;
        }
    }
    
    return result;
}

stu u8* utf8_seek_whitespace_backward_p( void* start_param, void* end_param, u32* error ) {
    
    u8* result = 0;
    
    breakable {
        
        if ( is_error( error ) ) {
            break;
        }
        
        u8* start = cast( u8*, start_param );
        u8* end = cast( u8*, end_param );
        
        if ( end < start ) {
            set_error( error, utf_error_end_is_before_start );
            break;
        }
        
        u8* codepoint_end = end;
        
        while ( end > start ) {
            
            end--;
            
            if ( codepoint_end - end > 4 ) {
                set_error( error, utf_error_utf8_invalid_encoding );
                break;
            }
            
            if ( utf8_is_lead_byte( deref( end ) ) ) {
                
                u32 codepoint = ucs_invalid_codepoint;
                u8 codeunit_count = 0;
                utf8_decode( end, codepoint_end, &codepoint, &codeunit_count, error );
                
                if ( is_error( error ) ) {
                    break;
                }
                
                if ( codepoint_end - end != codeunit_count ) {
                    set_error( error, utf_error_utf8_invalid_encoding );
                    break;
                }
                
                if ( char_is_whitespace( codepoint ) ) {
                    result = end;
                    break;
                }
                
                codepoint_end = end;
            }
        }
    }
    
    return result;
}

stu u8* utf8_seek_non_whitespace_p( void* start_param, void* end_param, u32* error ) {
    
    u8* result = 0;
    
    breakable {
        
        if ( is_error( error ) ) {
            break;
        }
        
        u8* start = cast( u8*, start_param );
        u8* end = cast( u8*, end_param );
        
        if ( end < start ) {
            set_error( error, utf_error_end_is_before_start );
            break;
        }
        
        while ( start < end ) {
            
            if ( !utf8_is_lead_byte( deref( start ) ) ) {
                set_error( error, utf_error_utf8_invalid_encoding );
                break;
            }
            
            u32 codepoint = ucs_invalid_codepoint;
            u8 codeunit_count = 0;
            
            utf8_decode( start, end, &codepoint, &codeunit_count, error );
            
            if ( is_error( error ) ) {
                break;
            }
            
            if ( !char_is_whitespace( codepoint ) ) {
                result = start;
                break;
            }
            
            start += codeunit_count;
        }
    }
    
    return result;
}

stu u8* utf8_seek_non_whitespace_backward_p( void* start_param, void* end_param, u32* error ) {
    
    u8* result = 0;
    
    breakable {
        
        if ( is_error( error ) ) {
            break;
        }
        
        u8* start = cast( u8*, start_param );
        u8* end = cast( u8*, end_param );
        
        if ( end < start ) {
            set_error( error, utf_error_end_is_before_start );
            break;
        }
        
        u8* current = end;
        u8* codepoint_end = end;
        
        while ( current > start ) {
            
            current--;
            
            if ( codepoint_end - current > 4 ) {
                set_error( error, utf_error_utf8_invalid_encoding );
                break;
            }
            
            if ( utf8_is_lead_byte( deref( end ) ) ) {
                
                u32 codepoint = ucs_invalid_codepoint;
                u8 codeunit_count = 0;
                utf8_decode( current, codepoint_end, &codepoint, &codeunit_count, error );
                
                if ( is_error( error ) ) {
                    break;
                }
                
                if ( codepoint_end - current != codeunit_count ) {
                    set_error( error, utf_error_utf8_invalid_encoding );
                    break;
                }
                
                if ( !char_is_whitespace( codepoint ) ) {
                    result = current;
                    break;
                }
                
                codepoint_end = current;
            }
        }
    }
    
    return result;
}

/* MARK date */

typedef struct date_time_t {
    
    u16 year; /* NOTE simon: full year xxxx */
    u8 month; /* NOTE simon: 1 -> 12 */
    u8 day; /* NOTE simon: 1 -> 31 */
    
    u8 hour; /* NOTE simon: 0 -> 23 */
    u8 minute; /* NOTE simon: 0 -> 59 */
    u8 second; /* NOTE simon: 0 -> 59 */
    
} date_time_t;

#define date_time_t_alignment sizeof_field( date_time_t, year )

stu b32 date_is_leap_year( umm year ) {
    b32 result = ( ( year % 4 ) == 0 && ( ( year % 100 ) != 0 || ( year % 400 ) == 0 ) );
    return result;
}

stu u64 date_to_u64( date_time_t d ) {
    
    u64 offset = 1;
    u64 result = d.second * offset;
    offset *= 100;
    result += d.minute * offset;
    offset *= 100;
    result += d.hour * offset;
    offset *= 100;
    result += d.day * offset;
    offset *= 100;
    result += d.month * offset;
    offset *= 100;
    result += d.year * offset;
    
    return result;
}

stu b32 date_is_left_after_right( date_time_t left, date_time_t right ) {
    
    u64 left_u = date_to_u64( left );
    u64 right_u = date_to_u64( right );
    b32 result = left_u > right_u;
    
    return result;
}

stu b32 date_is_left_before_right( date_time_t left, date_time_t right ) {
    
    u64 left_u = date_to_u64( left );
    u64 right_u = date_to_u64( right );
    b32 result = left_u < right_u;
    
    return result;
}

stu b32 date_is_zero( date_time_t* a ) {
    
    b32 result = !a->year && !a->month && !a->day && !a->hour && !a->minute && !a->second;
    return result;
}

stu b32 date_is_same_day( date_time_t* a, date_time_t* b ) {
    
    b32 result = a->year == b->year && a->month == b->month && a->day == b->day;
    return result;
}

stu date_time_t date_add_offset( date_time_t date, smm years, smm months, smm days, smm hours, smm minutes, smm seconds ) {
    
    date_time_t result;
    
    smm s = date.second + seconds;
    
    if ( s < 0 ) {
        
        while ( s < 0 ) {
            minutes -= 1;
            s += 60;
        }
    } else {
        
        while ( s >= 60 ) {
            minutes += 1;
            s -= 60;
        }
    }
    
    smm m = date.minute + minutes;
    
    if ( m < 0 ) {
        
        while ( m < 0 ) {
            hours -= 1;
            m += 60;
        }
    } else {
        
        while ( m >= 60 ) {
            hours += 1;
            m -= 60;
        }
    }
    
    smm h = date.hour + hours;
    
    if ( h < 0 ) {
        
        while ( h < 0 ) {
            days -= 1;
            h += 24;
        }
    } else {
        
        while ( h >= 24 ) {
            days += 1;
            h -= 24;
        }
    }
    
    smm d = date.day;
    
    if ( days < 0 ) {
        
        days *= -1;
        
        while ( days ) {
            
            if ( days < d ) {
                d -= days;
                days = 0;
            } else {
                
                days -= d;
                
                if ( date.month == 1 ) {
                    date.month = 12;
                    date.year -= 1;
                } else {
                    date.month -= 1;
                }
                
                u8 days_in_month[ 12 ] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
                b32 leap_year = date_is_leap_year( date.year );
                days_in_month[ 1 ] += ( u8 ) leap_year;
                d = days_in_month[ date.month - 1 ];
            }
        }
    } else {
        
        while ( days ) {
            
            u8 days_in_month[ 12 ] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
            b32 leap_year = date_is_leap_year( date.year );
            days_in_month[ 1 ] += ( u8 ) leap_year;
            
            if ( d + days < days_in_month[ date.month - 1 ] ) {
                d += days;
                days = 0;
            } else {
                
                days -= days_in_month[ date.month - 1 ] - d;
                d = 1;
                
                if ( date.month == 12 ) {
                    date.month = 1;
                    date.year += 1;
                } else {
                    date.month += 1;
                }
            }
        }
    }
    
    smm mo = date.month;
    
    if ( months < 0 ) {
        
        months *= -1;
        
        while ( months ) {
            
            if ( months < mo ) {
                mo -= months;
                months = 0;
                
                u8 days_in_month[ 12 ] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
                b32 leap_year = date_is_leap_year( date.year );
                days_in_month[ 1 ] += ( u8 ) leap_year;
                
                if ( days_in_month[ mo - 1 ] < d ) {
                    d = days_in_month[ mo - 1 ];
                }
            } else {
                
                months -= mo;
                mo = 12;
                date.year -= 1;
            }
        }
    } else {
        
        while ( months ) {
            
            if ( mo + months <= 12 ) {
                
                mo += months;
                months = 0;
                
                u8 days_in_month[ 12 ] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
                b32 leap_year = date_is_leap_year( date.year );
                days_in_month[ 1 ] += ( u8 ) leap_year;
                
                if ( days_in_month[ mo - 1 ] < d ) {
                    d = days_in_month[ mo - 1 ];
                }
            } else {
                
                months -= 13 - mo;
                mo = 1;
                date.year += 1;
            }
        }
    }
    
    smm y = date.year + years;
    
    if ( y < 0 ) {
        y = 0;
    } else if ( y > u16_max ) {
        y = u16_max;
    }
    
    b32 leap_year = date_is_leap_year( y );
    
    if ( mo == 2 && d == 29 && !leap_year ) {
        d = 28;
    }
    
    result.year = ( u16 ) y;
    result.month = ( u8 ) mo;
    result.day = ( u8 ) d;
    result.hour = ( u8 ) h;
    result.minute = ( u8 ) m;
    result.second = ( u8 ) s;
    
    return result;
}

stu data_t date_to_string_timestamp( data_t* memory, date_time_t date ) {
    
    data_t result = memory_construct( memory_left( *memory ), 0, memory_current( *memory, u8 ) );
    
    string_push_u64( memory, date.year, 4, '0', 0 );
    string_push_u64( memory, date.month, 2, '0', 0 );
    string_push_u64( memory, date.day, 2, '0', 0 );
    memory_push_u8( memory, '_', 1 );
    string_push_u64( memory, date.hour, 2, '0', 0 );
    string_push_u64( memory, date.minute, 2, '0', 0 );
    string_push_u64( memory, date.second, 2, '0', 0 );
    
    result.reserved = result.used;
    
    return result;
}

stu date_time_t date_from_string_timestamp( data_t string ) {
    
    date_time_t result = { 0 };
    data_t format = string_l( "yyyymmdd_hhmmss" );
    b32 ok = string.used >= format.used;
    u8* current = string.bytes;
    u8* end = string.bytes + 8;
    
    while ( ok && current < end ) {
        ok = char_is_decimal( *current );
        current++;
    }
    
    ok = ok && ( *current == '_' );
    current++;
    
    end = string.bytes + format.used;
    
    while ( ok && current < end ) {
        ok = char_is_decimal( *current );
        current++;
    }
    
    if ( ok ) {
        
        u8* start = string.bytes;
        
        date_time_t date = { 0 };
        date.year = ( u16 ) string_to_u64_p( start, 4 );
        date.month = ( u8 ) string_to_u64_p( start + 4, 2 );
        date.day = ( u8 ) string_to_u64_p( start + 6, 2 );
        date.hour = ( u8 ) string_to_u64_p( start + 9, 2 );
        date.minute = ( u8 ) string_to_u64_p( start + 11, 2 );
        date.second = ( u8 ) string_to_u64_p( start + 13, 2 );
        
        u8 days_in_month[ 12 ] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        b32 leap_year = date_is_leap_year( date.year );
        days_in_month[ 1 ] += ( u8 ) leap_year;
        
        ok = ok && date.month >= 1 && date.month <= 12;
        ok = ok && date.day >= 1 && date.day <= days_in_month[ date.month ];
        ok = ok && date.hour >= 0 && date.hour <= 23;
        ok = ok && date.minute >= 0 && date.minute <= 59;
        ok = ok && date.second >= 0 && date.second <= 59;
        
        result = date;
    }
    
    return result;
}

#if defined( WINDOWS )

typedef enum date_to_string_flag_t {
    
    date_to_string_full = 0,
    date_to_string_no_date = 1 << 0,
    date_to_string_no_time = 1 << 1,
    
} date_to_string_flag_t;

/* NOTE simon: GetLocaleInfoEx with LOCALE_SLONGDATE to get the date format. https://msdn.microsoft.com/en-us/library/dd318103(v=vs.85).aspx */
stu data_t date_to_string( date_time_t date, data_t* memory, u32 flags, data_t separator ) {
    profiler_event_start( date_to_string );
    
    /* NOTE simon: daylight time is from spring to winter, the clock is advanced by 1 hour. */
    data_t result = { 0 };
    
    SYSTEMTIME utc_date = { 0 };
    utc_date.wYear = date.year;
    utc_date.wMonth = date.month;
    utc_date.wDay = date.day;
    utc_date.wHour = date.hour;
    utc_date.wMinute = date.minute;
    utc_date.wSecond = date.second;
    
    SYSTEMTIME local_time = { 0 };
    
    DYNAMIC_TIME_ZONE_INFORMATION time_zone_info;
    u32 zone_id = GetDynamicTimeZoneInformation( &time_zone_info );
    _assert( zone_id == TIME_ZONE_ID_STANDARD || zone_id == TIME_ZONE_ID_DAYLIGHT );
    b32 success = SystemTimeToTzSpecificLocalTimeEx( &time_zone_info, &utc_date, &local_time );
    
    u16* buffer[ 1024 ] = { 0 };
    _ctassert( sizeof( u16 ) == sizeof( wchar_t ) );
    wchar_t* utf16 = ( LPWSTR ) buffer;
    
    u32 error = 0;
    umm codeunit_count = 0;
    
    if ( !flag_is_set_u32( flags, date_to_string_no_date ) ) {
        codeunit_count = GetDateFormatEx( LOCALE_NAME_USER_DEFAULT, DATE_LONGDATE, &local_time, 0, utf16, array_count( buffer ), 0 );
    }
    
    if ( separator.used && flag_is_set_u32( flags, date_to_string_full ) ) {
        codeunit_count--;
        umm buffer_bytes_left = sizeof( buffer ) - codeunit_count * sizeof( u16 );
        codeunit_count += utf8_to_utf16_p( separator.bytes, separator.used, utf16 + codeunit_count, buffer_bytes_left, &error );
        utf16 += codeunit_count;
    }
    
    if ( !flag_is_set_u32( flags, date_to_string_no_time ) ) {
        codeunit_count += GetTimeFormatEx( LOCALE_NAME_USER_DEFAULT, 0, &local_time, 0, utf16, array_count( buffer ) - ( s32 ) codeunit_count );
    }
    
    if ( codeunit_count ) {
        codeunit_count--;
    }
    
    result.bytes = memory_current( *memory, u8 );
    result.used = utf16_to_utf8_p( buffer, codeunit_count, result.bytes, memory_left( *memory ), &error );
    result.reserved = result.used;
    memory->used += result.used;
    
    profiler_event_end( date_to_string );
    
    return result;
}

stu date_time_t date_get_current( void ) {
    
    date_time_t result = { 0 };
    
    SYSTEMTIME utc_time;
    GetSystemTime( &utc_time );
    result.year = utc_time.wYear;
    result.month = ( u8 ) utc_time.wMonth;
    result.day = ( u8 ) utc_time.wDay;
    result.hour = ( u8 ) utc_time.wHour;
    result.minute = ( u8 ) utc_time.wMinute;
    result.second = ( u8 ) utc_time.wSecond;
    
    return result;
}

#elif defined( LINUX )

stu data_t date_to_string( date_time_t date, data_t* memory, b32 use_date, b32 use_time, data_t separator ) {
    
    /* NOTE simon: daylight time is from spring to winter, the clock is advanced by 1 hour. */
    data_t result = { 0 };
    
    time_t timestamp = time( 0 );
    struct tm a = *localtime( &timestamp );
    struct tm b = *gmtime( &timestamp );
    time_t at = mktime( &a );
    time_t bt = mktime( &b );
    
    struct tm utc_date = { 0 };
    utc_date.tm_year = date.year - 1900;
    utc_date.tm_mon = date.month - 1;
    utc_date.tm_mday = date.day;
    utc_date.tm_hour = date.hour;
    utc_date.tm_min = date.minute;
    utc_date.tm_sec = date.second + ( s32 ) difftime( at, bt );
    
    /* NOTE simon: instead of adding a difftime we could maybe use timegm on linux, but it's not a standard function. */
    // timegm( &utc_date );
    
    /* NOTE simon: mktime updates the given struct tm members.
    e.g. a value of 65 seconds will add a minute and set seconds to 5. tm_yday and tm_wday will be set to the correct values... */
    mktime( &utc_date );
    
    memory_get_on_stack( date_format, 1024 );
    
    if ( use_date ) {
#define DATE_FORMAT_TEMP "%A %B %e %Y"
        memory_push_copy_p( &date_format, DATE_FORMAT_TEMP, sizeof( DATE_FORMAT_TEMP ) - 1, 1 );
#undef DATE_FORMAT_TEMP
    }
    
    if ( separator.used ) {
        memory_push_copy( &date_format, separator, 1 );
    }
    
    if ( use_time ) {
#define TIME_FORMAT_TEMP "%T"
        memory_push_copy_p( &date_format, TIME_FORMAT_TEMP, sizeof( TIME_FORMAT_TEMP ) - 1, 1 );
#undef TIME_FORMAT_TEMP
    }
    
    string_zero_terminate( date_format );
    
    result.bytes = memory_current( *memory, u8 );
    umm length = strftime( result.text, memory_left( *memory ), date_format.text, &utc_date );
    result.used = length;
    result.reserved = length;
    
    memory->used += length;
    
    return result;
}

stu date_time_t date_get_current( void ) {
    
    date_time_t result = { 0 };
    
    time_t utc_timestamp = time( 0 );
    struct tm* utc_time = gmtime( &utc_timestamp );
    result.year = ( u16 ) ( 1900 + utc_time->tm_year );
    result.month = 1 + ( u8 ) utc_time->tm_mon;
    result.day = ( u8 ) utc_time->tm_mday;
    result.hour = ( u8 ) utc_time->tm_hour;
    result.minute = ( u8 ) utc_time->tm_min;
    result.second = ( u8 ) utc_time->tm_sec;
    
    return result;
}

#else
# error Unsupported platform.
#endif

/* MARK file */

u32 g_file_error = 0;

stu data_t log_windows_error_message( u32 error );

/* TODO simon: On Windows I should update those function to use the wide string version of the functions. */

typedef enum file_mode_t {
    file_mode_read = 0,
    file_mode_write = 1
} file_mode_t;

#if defined( WINDOWS )

typedef HANDLE file_handle_t;
#define file_invalid_handle INVALID_HANDLE_VALUE

#elif defined( LINUX )

typedef s32 file_handle_t;
#define file_invalid_handle -1

#endif

typedef enum file_error_t {
    
    file_error_none,
    file_error_open,
    file_error_close,
    file_error_invalid_handle,
    file_error_set_pointer,
    file_error_read_size, /* NOTE simon: The amount of data read is not equal to the amount requested. */
    file_error_write_size, /* NOTE simon: The amount of data written is not equal to the amount requested. */
    file_error_read,
    file_error_write,
    file_error_not_implemented,
    file_error_move,
    file_error_copy,
    file_error_delete,
    file_error_end_of_file,
    file_error_get_file_size,
    file_error_read_allocation_failed,
    file_error_create_directory,
    
    file_error_name_convertion_utf8_to_utf16,
    
} file_error_t;

typedef struct file_list_t {
    
    data_t* memory; /* TODO simon: Why is that a pointer ? Look at use cases. */
    u64 memory_reset;
    u8** names; /* NOTE simon: names are zero terminated. */
    u64* sizes;
    u16* names_codeunit_count; /* NOTE simon: names_codeunit_count doesn't include the null terminator. This is a number of codeunits, not codepoints. */
    u8* directories;
    u64* last_write_times;
    u32 count;
    u32 max_count;
    
} file_list_t;

typedef enum file_list_flag_t {
    file_list_recursive = 1 << 0,
    file_list_files = 1 << 1,
    file_list_directories = 1 << 2,
    file_list_add_self_and_parent = 1 << 3,
    file_list_only_leaf_directories = 1 << 4,
} file_list_flag_t;

stu umm file_list_memory_requirement( umm max_file_count, umm max_expected_name_codeunit ) {
    
    umm result = 0;
    result += max_expected_name_codeunit; 
    result += sizeof( u64 ); /* size */
    result += sizeof( u16 ); /* codeunit_count */
    result += sizeof( u8 ); /* directory */
    result += sizeof( u64 ); /* last_write_times */
    result *= max_file_count;
    
    return result;
}

stu file_list_t file_list_make( data_t* memory, u32 max_file_count ) {
    
    data_t backup = *memory;
    
    u8** names = memory_push_array_aligned( memory, u8*, max_file_count, 8 );
    u64* sizes = memory_push_array( memory, u64, max_file_count );
    u16* names_codeunit_count = memory_push_array( memory, u16, max_file_count );
    u8* directories = memory_push_array( memory, u8, max_file_count );
    u64* last_write_times = memory_push_array( memory, u64, max_file_count );
    
    file_list_t result = { 0 };
    
    if ( names && sizes && names_codeunit_count && directories && last_write_times ) {
        result.memory = memory;
        result.memory_reset = memory->used;
        result.names = names;
        result.sizes = sizes;
        result.names_codeunit_count = names_codeunit_count;
        result.directories = directories;
        result.last_write_times = last_write_times;
        result.count = 0;
        result.max_count = max_file_count;
    } else {
        *memory = backup;
    }
    
    return result;
}

stu void file_list_reset( file_list_t* list ) {
    list->memory->used = list->memory_reset;
    list->count = 0;
}

stu void file_free_memory( data_t* file ) {
    memory_free_virtual( file );
}

stu data_t file_get_extension_from_name_p( void* file_name_arg, umm length ) {
    
    data_t result = { 0 };
    u8* start = cast( u8*, file_name_arg );
    u8* end = start + length;
    u8* dot = cast( u8*, memory_seek_backward_p( start, end, ".", 1 ) );
#if defined( WINDOWS )
    u8* slash = cast( u8*, memory_seek_backward_p( start, end, "\\", 1 ) );
#elif defined( LINUX )
    u8* slash = cast( u8*, memory_seek_backward_p( start, end, "/", 1 ) );
#else
#error Unsupported platform.
#endif
    
    if ( dot < end && ( slash >= end || slash < dot ) ) {
        result.bytes = dot + 1;
        result.reserved = result.used = end - result.bytes;
    }
    
    return result;
}

stu data_t file_get_extension_from_name( data_t file_name ) {
    data_t result = file_get_extension_from_name_p( file_name.bytes, file_name.used );
    return result;
}

stu data_t file_name_remove_extension_p( void* file_name_arg, umm length ) {
    data_t result = memory_construct( length, length, file_name_arg );
    u8* dot = cast( u8*, memory_seek_backward_p( result.bytes, memory_end( result ), ".", 1 ) );
    
    if ( dot < memory_end( result ) ) {
        result.reserved = result.used = dot - result.bytes;
    }
    
    return result;
}

stu data_t file_name_remove_extension( data_t file_name ) {
    data_t result = file_name_remove_extension_p( file_name.bytes, file_name.used );
    return result;
}

stu data_t file_name_remove_path_p( void* file_name_arg, umm length ) {
    
    data_t result = memory_construct( length, length, file_name_arg );
    u8* separator = cast( u8*, memory_seek_backward_p( result.bytes, memory_end( result ), "\\", 1 ) );
    
    if ( separator >= memory_end( result ) ) {
        separator = cast( u8*, memory_seek_backward_p( result.bytes, memory_end( result ), "/", 1 ) );
    }
    
    if ( separator < memory_end( result ) ) {
        separator += 1;
        result.reserved = result.used = memory_end( result ) - separator;
        result.bytes = separator;
    }
    
    return result;
}

stu data_t file_name_remove_path( data_t file_name ) {
    data_t result = file_name_remove_path_p( file_name.bytes, file_name.used );
    return result;
}

stu data_t file_name_get_path( data_t file_name ) {
    
    data_t result = { 0 };
    u8* separator = cast( u8*, memory_seek_backward( file_name, string_l( "\\" ) ) );
    
    if ( separator >= memory_end( file_name ) ) {
        separator = cast( u8*, memory_seek_backward( file_name, string_l( "/" ) ) );
    }
    
    if ( separator < memory_end( file_name ) ) {
        separator += 1;
        result = memory_start_end( file_name.bytes, separator );
    }
    
    return result;
}

stu data_t file_name_previous_folder( data_t path ) {
    
    data_t result = path;
    
    while ( result.used ) {
        
        u8 c = result.bytes[ result.used - 1 ];
        
        if ( c == '\\' || c == '/' ) {
            result.used--; 
        } else {
            break;
        }
    }
    
    while ( result.used ) {
        
        u8 c = result.bytes[ result.used - 1 ];
        
        if ( c != '\\' && c != '/' ) {
            result.used--;
        } else {
            break;
        }
    }
    
    if ( result.used == 0 ) {
        result = path;
    }
    
    return result;
}

stu data_t file_name_clean_separators( data_t* memory, data_t source, u8 separator ) {
    
    data_t result = memory_construct( memory_left( *memory ), 0, memory->bytes );
    b32 last_is_separator = false;
    
    for ( umm i = 0; i < source.used; i++ ) {
        
        if ( result.used < result.reserved ) {
            
            u8 c = source.bytes[ i ];
            
            if ( c == '/' || c == '\\' ) {
                
                if ( !last_is_separator ) {
                    result.bytes[ result.used ] = separator;
                    result.used++;
                    last_is_separator = true;
                }
                
            } else {
                
                last_is_separator = false;
                result.bytes[ result.used ] = c;
                result.used++;
            }
            
        } else {
            break;
        }
    }
    
    memory->used += result.used;
    
    return result;
}

stu u64 file_name_get_timestamp( data_t filename ) {
    
    u8* start = cast( u8*, memory_seek_backward( filename, string_l( "\\" ) ) );
    u8* end = memory_end( filename );
    
    if ( start != end ) {
        start++;
    } else {
        start = filename.bytes;
    }
    
    u64 timestamp = 0;
    
    data_t working_string = memory_start_end( start, end );
    working_string = file_name_remove_extension( working_string );
    
    data_t format = string_l( "yyyymmdd_hhmmss" );
    
    while ( timestamp == 0 && working_string.used >= format.used ) {
        
        while ( !char_is_decimal( working_string.bytes[ 0 ] ) ) {
            memory_advance( &working_string, 1 );
        }
        
        if ( working_string.used >= format.used ) {
            date_time_t date = date_from_string_timestamp( working_string );
            timestamp = date_to_u64( date );
        }
    }
    
    return timestamp;
}

stu data_t file_list_get_name( file_list_t* list, umm index ) {
    data_t result = memory_construct( list->names_codeunit_count[ index ], list->names_codeunit_count[ index ], list->names[ index ] );
    return result;
}

stu u32 internal_split_comma_separated_into_list( data_t extensions, data_t* extension_list, u32 max_extension_count ) {
    
    u32 extension_count = 0;
    
    u8* current = extensions.bytes;
    u8* end = memory_end( extensions );
    
    while ( current < end && extension_count < max_extension_count ) {
        
        while ( current < end && char_is_whitespace( *current ) ) {
            current++;
        }
        
        u8* extension_start = current;
        
        while ( current < end && ( *current != ',' ) && !char_is_whitespace( *current ) ) {
            current++;
        }
        
        if ( extension_start < current ) {
            extension_list[ extension_count ] = memory_start_end( extension_start, current );
            extension_count++;
        }
        
        while ( current < end && char_is_whitespace( *current ) ) {
            current++;
        }
        
        if ( *current == ',' ) {
            current++;
        }
    }
    
    return extension_count;
}

#if defined( WINDOWS )

#if 0
/* NOTE simon: This is working but require an include and a lib for the linker (shell32.lib). */
#include <Shellapi.h>

stu b32 file_delete_directory_s( void* folder_name_arg ) {
    
    u8* folder_name = cast( u8*, folder_name_arg );
    
    b32 result = false;
    umm length = string_length( folder_name );
    char folder[ 2048 ];
    
    if ( length < array_count( folder ) - 2 ) {
        
        string_copy( folder, folder_name, length );
        folder[ length ] = 0;
        folder[ length + 1 ] = 0;
        
        SHFILEOPSTRUCT fileop = {
            
            0,
            FO_DELETE,
            folder,
            0,
            FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
            0, 0, 0
        };
        
        result = !SHFileOperation( &fileop );
        result = result && fileop.fAnyOperationsAborted;
    }
    
    return result;
}
#endif

stu b32 file_is_handle_valid( file_handle_t file ) {
    b32 result = ( file != INVALID_HANDLE_VALUE );
    return result;
}

stu u64 file_get_size_handle( file_handle_t handle, u32* error ) {
    
    u64 result = 0;
    
    if ( no_error( error ) ) {
        
        if ( file_is_handle_valid( handle ) ) {
            
            LARGE_INTEGER file_size;
            
            if ( GetFileSizeEx( handle, &file_size ) ) {
                result = file_size.QuadPart;
            } else {
                set_error( error, file_error_get_file_size );
            }
        } else {
            set_error( error, file_error_invalid_handle );
        }
    }
    
    return result;
}

stu file_handle_t file_open_p( void* file_name_arg, umm file_name_length, u8 mode, b32 replace, u32* error ) {
    
    file_handle_t result = INVALID_HANDLE_VALUE;
    
    if ( no_error( error ) ) {
        
        memory_get_on_stack( file_utf16, 2048 );
        
        g_ucs_error = 0;
        utf8_to_utf16( memory_construct( file_name_length, file_name_length, file_name_arg ), &file_utf16, &g_ucs_error );
        memory_push_u16( &file_utf16, 0, 0 );
        
        if ( no_error( &g_ucs_error ) ) {
            result = CreateFileW( cast( u16*, file_utf16.bytes ), ( mode == file_mode_read ) ? GENERIC_READ : GENERIC_WRITE, ( mode == file_mode_read ) ? FILE_SHARE_READ : 0, 0, ( mode == file_mode_read ) ? OPEN_EXISTING : ( replace ) ? CREATE_ALWAYS : CREATE_NEW, 0, 0 );
            
            if ( !file_is_handle_valid( result ) ) {
                set_error( error, file_error_open );
            }
            
        } else {
            set_error( error, file_error_name_convertion_utf8_to_utf16 );
        }
    }
    
    return result;
}

stu void file_close( file_handle_t file, u32* error ) {
    
    if ( file_is_handle_valid( file ) ) {
        CloseHandle( file );
    } else {
        if ( no_error( error ) ) {
            set_error( error, file_error_close );
        }
    }
}

stu u64 file_read_handle( file_handle_t file, s64 position, u64 count, void* destination, u32* error ) {
    
    u64 total_read = 0;
    
    if ( no_error( error ) ) {
        
        if ( !file_is_handle_valid( file ) ) {
            set_error( error, file_error_invalid_handle );
        } else {
            
            LARGE_INTEGER offset;
            offset.QuadPart = position;
            b32 success = SetFilePointerEx( file, offset, 0, FILE_BEGIN );
            
            if ( !success ) {
                set_error( error, file_error_set_pointer );
            } else {
                
                DWORD bytes_read = 0;
                u64 size = count;
                u32 to_read = ( u32 ) ( ( size > u32_max ) ? u32_max : size );
                
                while ( no_error( error ) && total_read < count ) {
                    
                    success = ReadFile( file, cast( u8*, destination ) + total_read, to_read, &bytes_read, 0 );
                    
                    if ( !success ) {
                        set_error( error, file_error_read );
                    } else if ( bytes_read == 0 ) {
                        /* NOTE simon: if you request bytes but the file pointer is at the end of the file, ReadFile doesn't fail and bytes_read is set to 0. */
                        set_error( error, file_error_end_of_file );
                    } else {
                        total_read += bytes_read;
                        size -= bytes_read;
                        to_read = ( u32 ) ( ( size > u32_max ) ? u32_max : size );
                    }
                }
                
                if ( no_error( error ) && total_read != count ) {
                    set_error( error, file_error_read_size );
                }
            }
        }
    }
    
    return total_read;
}

stu u64 file_write_handle( file_handle_t file, s64 position, u64 count, void* source, u32* error ) {
    
    u64 total_written = 0;
    
    if ( no_error( error ) ) {
        
        if ( !file_is_handle_valid( file ) ) {
            set_error( error, file_error_invalid_handle );
        } else {
            
            LARGE_INTEGER offset;
            offset.QuadPart = position;
            b32 success = SetFilePointerEx( file, offset, 0, FILE_BEGIN );
            
            if ( !success ) {
                set_error( error, file_error_set_pointer );
            } else {
                
                DWORD bytes_written = 0;
                u64 size = count;
                u32 to_write = ( u32 ) ( ( size > u32_max ) ? u32_max : size );
                
                while ( no_error( error ) && total_written < count ) {
                    
                    success = WriteFile( file, cast( u8*, source ) + total_written, to_write, &bytes_written, 0 );
                    
                    if ( !success ) {
                        set_error( error, file_error_write );
                    } else {
                        total_written += bytes_written;
                        size -= bytes_written;
                        to_write = ( u32 ) ( ( size > u32_max ) ? u32_max : size );
                    }
                }
                
                if ( no_error( error ) && count != total_written ) {
                    set_error( error, file_error_write_size );
                }
            }
        }
    }
    
    return total_written;
}

stu void file_delete_p( void* file_name_arg, umm file_name_length, u32* error ) {
    
    if ( no_error( error ) ) {
        
        memory_get_on_stack( file_utf16, 2048 );
        g_ucs_error = 0;
        utf8_to_utf16( memory_construct( file_name_length, file_name_length, file_name_arg ), &file_utf16, &g_ucs_error );
        memory_push_u16( &file_utf16, 0, 0 );
        
        if ( no_error( &g_ucs_error ) ) {
            
            b32 result = DeleteFileW( cast( u16*, file_utf16.bytes ) );
            
            if ( !result ) {
                set_error( error, file_error_delete );
            }
            
        } else {
            set_error( error, file_error_name_convertion_utf8_to_utf16 );
        }
    }
}

/* TODO simon: Look at this: https://docs.microsoft.com/en-us/windows/win32/fileio/master-file-table */

stu void file_list_get_internal( file_list_t* list, data_t directory, data_t* extensions, umm extension_count, data_t* exceptions, umm exception_count, u32 flags, data_t working_memory ) {
    
    b32 first_entry_in_directory = true;
    
    _assert( ( directory.used / 2 ) * 2 == directory.used );
    /* NOTE simon: directory is in utf16. */
    /* NOTE simon: working memory can be modified by recursive call. Don't expect it to be the same after a recursive call. */
    
    data_t wildcard = string_l( "\\*\0" );
    data_t directory_wildcard = directory;
    g_ucs_error = 0; /* TODO simon: Handle ucs error ? */
    utf8_to_utf16( wildcard, &directory_wildcard, &g_ucs_error );
    
    WIN32_FIND_DATAW find_data;
    
#if 1
    /* NOTE simon: When searching only for directories, this is supposed to be faster.
In a test I got ~0.115s instead of ~0.125 when listing 19247 files in 1431 folders (listing only folders).
But the speed up seems to come from the FindExInfoBasic more than from the directory_only thing.
If fact the 'op' doesn't seems to work on Windows 10.
*/
    s32 op = 0;
    
    if ( flag_is_set_u32( flags, file_list_directories ) && !flag_is_set_u32( flags, file_list_files ) ) {
        op = FindExSearchLimitToDirectories;
    }
    
    HANDLE search_handle = FindFirstFileExW( ( LPCWSTR ) directory_wildcard.raw, FindExInfoBasic, &find_data, op, 0, FIND_FIRST_EX_CASE_SENSITIVE );
#else
    HANDLE search_handle = FindFirstFileW( ( LPCWSTR ) directory_wildcard.raw, &find_data );
#endif
    
    if ( search_handle != INVALID_HANDLE_VALUE ) {
        
        b32 has_file = true;
        
        while ( has_file && list->count < list->max_count ) {
            
            data_t name = memory_get_temp( working_memory );
            
            u8 is_directory = ( find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY;
            
            umm name_codeunit_count = utf16_get_codeunit_count( find_data.cFileName );
            name.used = utf16_to_utf8_p( find_data.cFileName, name_codeunit_count, name.bytes, name.reserved, &g_ucs_error );
            _assert( no_error( &g_ucs_error ) );
            
            b32 is_exception = false;
            
            for ( umm i = 0; !is_exception && i < exception_count; i++ ) {
                is_exception = memory_are_equal( name, exceptions[ i ] );
            }
            
            if ( !is_exception ) {
                
                if ( is_directory && flag_is_set_u32( flags, file_list_directories ) ) {
                    
                    if ( flag_is_set_u32( flags, file_list_add_self_and_parent ) || ( !memory_are_equal( name, string_l( "." ) ) && !memory_are_equal( name, string_l( ".." ) ) ) ) {
                        
                        if ( first_entry_in_directory && list->count && flag_is_set_u32( flags, file_list_only_leaf_directories ) ) {
                            list->count--;
                            list->memory->used -= list->names_codeunit_count[ list->count ];
                            list->names[ list->count ] = 0;
                            list->names_codeunit_count[ list->count ] = 0;
                            list->directories[ list->count ] = 0;
                            list->sizes[ list->count ] = 0;
                            list->last_write_times[ list->count ] = 0;
                        }
                        
                        list->directories[ list->count ] = true;
                        
                        u8* current = memory_current( *list->memory, u8 );
                        umm left = memory_left( *list->memory );
                        umm codeunit_count = utf16_to_utf8_p( directory.bytes, directory.used / 2, current, left, &g_ucs_error );
                        _assert( no_error( &g_ucs_error ) );
                        list->memory->used += codeunit_count;
                        list->names[ list->count ] = current;
                        memory_push_u8( list->memory, '\\', 1 );
                        memory_push_copy( list->memory, name, 1 );
                        list->names_codeunit_count[ list->count ] = math_safe_truncate_to_u16( codeunit_count + 1 + name.used );
                        
                        list->sizes[ list->count ] = ( ( u64 ) find_data.nFileSizeHigh << 32 ) | ( u64 ) find_data.nFileSizeLow;
                        list->last_write_times[ list->count ] = ( ( u64 ) find_data.ftLastWriteTime.dwLowDateTime ) | ( ( ( u64 ) find_data.ftLastWriteTime.dwHighDateTime ) << 32 );
                        list->count++;
                        
                        first_entry_in_directory = false;
                    }
                    
                }  else if ( !is_directory && flag_is_set_u32( flags, file_list_files ) ) {
                    
                    b32 valid = false;
                    
                    if ( extension_count ) {
                        
                        data_t extension = file_get_extension_from_name( name );
                        
                        for ( umm index = 0; index < extension_count; index++ ) {
                            
                            if ( memory_are_equal( extensions[ index ], extension ) ) {
                                valid = true;
                                break;
                            }
                        }
                    } else {
                        valid = true;
                    }
                    
                    if ( valid ) {
                        list->directories[ list->count ] = false;
                        
                        u8* current = memory_current( *list->memory, u8 );
                        umm left = memory_left( *list->memory );
                        umm codeunit_count = utf16_to_utf8_p( directory.bytes, directory.used / 2, current, left, &g_ucs_error );
                        _assert( no_error( &g_ucs_error ) );
                        list->memory->used += codeunit_count;
                        list->names[ list->count ] = current;
                        memory_push_u8( list->memory, '\\', 1 );
                        memory_push_copy( list->memory, name, 1 );
                        list->names_codeunit_count[ list->count ] = math_safe_truncate_to_u16( codeunit_count + 1 + name.used );
                        
                        list->sizes[ list->count ] = ( ( u64 ) find_data.nFileSizeHigh << 32 ) | ( u64 ) find_data.nFileSizeLow;
                        list->last_write_times[ list->count ] = ( ( u64 ) find_data.ftLastWriteTime.dwLowDateTime ) | ( ( ( u64 ) find_data.ftLastWriteTime.dwHighDateTime ) << 32 );
                        list->count++;
                    }
                }
                
                if ( is_directory && flag_is_set_u32( flags, file_list_recursive ) && list->count < list->max_count ) {
                    
                    if ( !memory_are_equal( name, string_l( "." ) ) && !memory_are_equal( name, string_l( ".." ) ) ) {
                        
                        data_t sub_dir = directory;
                        memory_push_u16( &sub_dir, '\\', 0 );
                        memory_push_copy_p( &sub_dir, find_data.cFileName, name_codeunit_count * 2, 1 );
                        // memory_push_u16( list->memory, 0 ); /* NOTE simon: adding a zero seems wrong here. */
                        
                        file_list_get_internal( list, sub_dir, extensions, extension_count, exceptions, exception_count, flags, working_memory );
                    }
                }
            }
            
            has_file = FindNextFileW( search_handle, &find_data );
        }
        
        FindClose( search_handle );
    }
}

stu void file_list_get( file_list_t* list, data_t directory, data_t comma_separated_extensions, data_t comma_separated_exceptions, u32 flags ) {
    
#if defined( TARGET_x32 )
    void* old_value = 0;
    Wow64DisableWow64FsRedirection( &old_value );
#endif
    
    data_t extensions[ 64 ] = { 0 };
    umm extension_count = internal_split_comma_separated_into_list( comma_separated_extensions, extensions, array_count( extensions ) );
    
    data_t exceptions[ 64 ] = { 0 };
    umm exception_count = internal_split_comma_separated_into_list( comma_separated_exceptions, exceptions, array_count( exceptions ) );
    
    memory_get_on_stack( working_memory, 1024 );
    
    memory_get_on_stack( directory_wildcard, 2048 );
    _assert( directory.used <= directory_wildcard.reserved );
    
    /* NOTE simon:
\\?\ need to be used if the path is more than MAX_PATH (260 characters) and only works with the W functions.
But if you use it, you can't specify relative paths so I let it up to the user to add it in the path.
 */
    g_ucs_error = 0;
    utf8_to_utf16( directory, &directory_wildcard, &g_ucs_error );
    _assert( no_error( &g_ucs_error ) );
    
    if ( !flag_is_set_u32( flags, file_list_files ) && !flag_is_set_u32( flags, file_list_directories ) ) {
        flags |= ( file_list_files | file_list_directories );
    }
    
    if ( flag_is_set_u32( flags, file_list_only_leaf_directories ) ) {
        flags &= ( ~file_list_files );
        flags |= ( file_list_directories );
    }
    
    file_list_get_internal( list, directory_wildcard, extensions, extension_count, exceptions, exception_count, flags, working_memory );
    
#if defined( TARGET_x32 )
    Wow64RevertWow64FsRedirection( old_value );
#endif
}

stu data_t file_read_entire_p( void* file_name_arg, umm file_name_length, umm reserve_at_end, u32* error ) {
    
    data_t result = { 0 };
    file_handle_t file = file_open_p( file_name_arg, file_name_length, file_mode_read, false, error );
    
    if ( no_error( error ) ) {
        
        LARGE_INTEGER file_size;
        
        if ( GetFileSizeEx( file, &file_size ) ) {
            
            u64 addin = reserve_at_end;
            u64 size = file_size.QuadPart;
            result = memory_get_virtual( size + addin, 0 );
            
            if ( result.bytes ) {
                
                result.used = file_read_handle( file, 0, size, result.bytes, error );
                
                if ( result.used != size ) {
                    _assert( *error == file_error_read_size );
                    memory_free_virtual( &result );
                    memory_invalidate( &result );
                }
                
            } else if ( ( size + addin ) != 0 ) {
                
                set_error( error, file_error_read_allocation_failed );
            }
            
        } else {
            
            set_error( error, file_error_get_file_size );
        }
        
        file_close( file, error );
    }
    
    return result;
}

/* TODO simon: Do I want to handle creating all directories in the path if they don't exist ? */
stu b32 file_create_directory_p( void* directory_name_arg, umm directory_name_length, u32* error ) {
    
    b32 result = false;
    
    if ( no_error( error ) ) {
        
        memory_get_on_stack( directory_utf16, 2048 );
        
        g_ucs_error = 0;
        utf8_to_utf16( memory_construct( directory_name_length, directory_name_length, directory_name_arg ), &directory_utf16, &g_ucs_error );
        memory_push_u16( &directory_utf16, 0, 0 );
        
        if ( no_error( &g_ucs_error ) ) {
            
            result = CreateDirectoryW( cast( u16*, directory_utf16.bytes ), 0 );
            
            if ( !result ) {
                u32 last_error = GetLastError( );
                result = ( last_error == ERROR_ALREADY_EXISTS );
            }
            
            if ( !result ) {
                set_error( error, file_error_create_directory );
            }
        } else {
            set_error( error, file_error_name_convertion_utf8_to_utf16 );
        }
    }
    
    return result;
}

stu b32 file_move_p( void* source_arg, umm source_length, void* destination_arg, umm destination_length, b32 replace, u32* error ) {
    
    b32 result = false;
    
    if ( no_error( error ) ) {
        
        // NOTE simon: Can't move a directory to a different drive/filesystem.
        // Need to call copy when it's a different drive/filesystem.
        
        memory_get_on_stack( source_utf16, 2048 );
        memory_get_on_stack( destination_utf16, 2048 );
        
        g_ucs_error = 0;
        utf8_to_utf16( memory_construct( source_length, source_length, source_arg ), &source_utf16, &g_ucs_error );
        utf8_to_utf16( memory_construct( destination_length, destination_length, destination_arg ), &destination_utf16, &g_ucs_error );
        memory_push_u16( &source_utf16, 0, 0 );
        memory_push_u16( &destination_utf16, 0, 0 );
        
        if ( no_error( &g_ucs_error ) ) {
            
            DWORD flags = MOVEFILE_COPY_ALLOWED;
            flags |= ( replace ) ? MOVEFILE_REPLACE_EXISTING : 0;
            result = MoveFileExW( cast( u16*, source_utf16.bytes ), cast( u16*, destination_utf16.bytes ), flags );
            
            if ( !result ) {
                set_error( error, file_error_move );
            }
            
        } else {
            set_error( error, file_error_name_convertion_utf8_to_utf16 );
        }
    }
    
    return result;
}

stu b32 file_copy_p( void* source_arg, umm source_length, void* destination_arg, umm destination_length, b32 replace, u32* error ) {
    
    b32 result = false;
    
    if ( no_error( error ) ) {
        
        memory_get_on_stack( source_utf16, 2048 );
        memory_get_on_stack( destination_utf16, 2048 );
        
        g_ucs_error = 0;
        utf8_to_utf16( memory_construct( source_length, source_length, source_arg ), &source_utf16, &g_ucs_error );
        utf8_to_utf16( memory_construct( destination_length, destination_length, destination_arg ), &destination_utf16, &g_ucs_error );
        memory_push_u16( &source_utf16, 0, 0 );
        memory_push_u16( &destination_utf16, 0, 0 );
        
        if ( no_error( &g_ucs_error ) ) {
            
            result = CopyFileExW( cast( u16*, source_utf16.bytes ), cast( u16*, destination_utf16.bytes ), 0, 0, 0, replace ? 0 : COPY_FILE_FAIL_IF_EXISTS );
            
            if ( !result ) {
                set_error( error, file_error_copy );
            }
            
        } else {
            set_error( error, file_error_name_convertion_utf8_to_utf16 );
        }
    }
    
    return result;
}

stu b32 file_exists_p( void* file_name_arg, umm file_name_length, u32* error ) {
    
    b32 result = false;
    
    if ( no_error( error ) ) {
        
        memory_get_on_stack( file_utf16, 2048 );
        
        g_ucs_error = 0;
        utf8_to_utf16( memory_construct( file_name_length, file_name_length, file_name_arg ), &file_utf16, &g_ucs_error );
        memory_push_u16( &file_utf16, 0, 0 );
        
        if ( no_error( &g_ucs_error ) ) {
            u32 attributes = GetFileAttributesW( cast( u16*, file_utf16.bytes ) );
            result = ( attributes != INVALID_FILE_ATTRIBUTES ) && !( attributes & FILE_ATTRIBUTE_DIRECTORY );
        } else {
            set_error( error, file_error_name_convertion_utf8_to_utf16 );
        }
    }
    
    return result;
}

stu b32 file_directory_exists_p( void* directory_name_arg, umm directory_name_length, u32* error ) {
    
    b32 result = false;
    
    if ( no_error( error ) ) {
        
        memory_get_on_stack( directory_utf16, 2048 );
        
        g_ucs_error = 0;
        utf8_to_utf16( memory_construct( directory_name_length, directory_name_length, directory_name_arg ), &directory_utf16, &g_ucs_error );
        memory_push_u16( &directory_utf16, 0, 0 );
        
        if ( no_error( &g_ucs_error ) ) {
            u32 attributes = GetFileAttributesW( cast( u16*, directory_utf16.bytes ) );
            result = attributes != INVALID_FILE_ATTRIBUTES && ( attributes & FILE_ATTRIBUTE_DIRECTORY );
        } else {
            set_error( error, file_error_name_convertion_utf8_to_utf16 );
        }
    }
    
    return result;
}

#elif defined( LINUX )

#include <stdio.h>

stu void file_delete_p( void* filename_arg, umm length, u32* error ) {
    
    memory_get_on_stack( filename, kibioctet( 2 ) );
    memory_push_copy_p( &filename, filename_arg, length, 1 );
    string_zero_terminate( filename );
    
    if ( *error == file_error_none ) {
        
        if ( remove( cast( const char*, filename.bytes ) ) != 0 ) {
            *error = file_error_delete;
        }
    }
}

stu b32 file_is_handle_valid( file_handle_t handle ) {
    b32 result = ( handle >= 0 );
    return result;
}

stu u64 file_get_size_handle( file_handle_t handle, u32* error ) {
    
    u64 result = 0;
    
    if ( no_error( error ) ) {
        
        if ( file_is_handle_valid( handle ) ){
            
            struct stat stats = {};
            
            if ( fstat( handle, &stats ) >= 0 ) {
                result = stats.st_size; 
            } else {
                set_error( error, file_error_get_file_size );
            }
        } else {
            set_error( error, file_error_invalid_handle );
        }
    }
    
    return result;
}

stu u64 file_get_size( data_t file_name, u32* error );

stu file_handle_t file_open_p( void* file_name, umm file_name_length, u8 mode, b32 replace, u32* error ) {
    
    file_handle_t handle = 0;
    
    if ( no_error( error ) ) {
        
        s32 flags;
        
        if ( mode == file_mode_read ) {
            flags = O_RDONLY;
        } else {
            flags = O_WRONLY | O_CREAT;
        }
        
        if ( mode == file_mode_write && !replace ) {
            flags |= O_EXCL;
        }
        
        mode_t linux_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
        
        memory_get_on_stack( zero_terminated, kibioctet( 2 ) ); 
        memory_push_copy_p( &zero_terminated, file_name, file_name_length, 1 );
        string_zero_terminate( zero_terminated );
        handle = open( cast( const char*, zero_terminated.bytes ), flags, linux_mode );
        
        if ( !file_is_handle_valid( handle ) ) {
            set_error( error, file_error_open );
        }
    }
    
    return handle;
}

stu void file_close( file_handle_t handle, u32* error ) {
    
    if ( file_is_handle_valid( handle ) ) {
        close( handle );
    } else if ( no_error( error ) ) {
        set_error( error, file_error_close );
    }
}

stu u64 file_write_handle( file_handle_t file, s64 position, u64 count, void* source, u32* error ) {
    
    u64 total_written = 0;
    
    if ( no_error( error ) ) {
        
        if ( !file_is_handle_valid( file ) ) {
            set_error( error, file_error_invalid_handle );
        } else {
            
            s64 result = lseek( file, position, SEEK_SET );
            
            if ( result == -1 ) {
                set_error( error, file_error_set_pointer );
            } else {
                
                size_t to_write = ( count > SSIZE_MAX ) ? SSIZE_MAX : count;
                
                while ( no_error( error ) && total_written < count ) {
                    
                    ssize_t bytes_written = write( file, cast( u8*, source ) + total_written, to_write );
                    
                    if ( bytes_written == -1 ) {
                        set_error( error, file_error_write );
                    } else {
                        total_written += bytes_written;
                        to_write = ( size_t ) ( count - total_written );
                        to_write = ( to_write > SSIZE_MAX ) ? SSIZE_MAX : to_write;
                    }
                }
                
                if ( no_error( error ) && count != total_written ) {
                    set_error( error, file_error_write_size );
                }
            }
        }
    }
    
    return total_written;
}

stu u64 file_read_handle( file_handle_t file, s64 position, u64 count, void* destination, u32* error ) {
    
    /* TODO simon: use pread / pwrite to avoid lseek ? */
    u64 total_read = 0;
    
    if ( no_error( error ) ) {
        
        if ( !file_is_handle_valid( file ) ) {
            set_error( error, file_error_invalid_handle );
        } else {
            
            s64 result = lseek( file, position, SEEK_SET );
            
            if ( result < 0 ) {
                set_error( error, file_error_set_pointer );
            } else {
                
                size_t to_read = ( count > SSIZE_MAX ) ? SSIZE_MAX : count;
                
                while ( no_error( error ) && total_read < count ) {
                    
                    ssize_t bytes_read = read( file, cast( u8*, destination ) + total_read, to_read );
                    
                    if ( bytes_read == -1 ) {
                        set_error( error, file_error_read );
                    } else if ( bytes_read == 0 ) {
                        /* NOTE simon: if you request bytes but the file pointer is at the end of the file, read returns 0. */
                        set_error( error, file_error_end_of_file );
                    } else {
                        total_read += bytes_read;
                        to_read = ( size_t ) ( count - total_read );
                        to_read = ( to_read > SSIZE_MAX ) ? SSIZE_MAX : to_read;
                    }
                }
                
                if ( no_error( error ) && total_read != count ) {
                    set_error( error, file_error_read_size );
                }
            }
        }
    }
    
    return total_read;
}

#if 0

stu data_t file_read_entire_s( void* file_name, b32 zero_terminated ) {
    
    data_t file = { };
    
    file_handle_t handle = open( cast( const char*, file_name ), O_RDONLY );
    
    if ( handle >= 0 ) {
        
        struct stat stats = {};
        
        if ( fstat( handle, &stats ) >= 0 ) {
            
            u32 addin =( zero_terminated ? 1 : 0 );
            u64 size = stats.st_size + addin;
            file = memory_get_virtual( size, 0 );
            file.used = file.reserved;
            
            if ( file.bytes != 0 ) {
                
                u64 total_read = 0;
                size_t to_read = ( file.reserved > SSIZE_MAX ) ? SSIZE_MAX : file.reserved;
                
                while ( total_read < ( file.reserved - addin ) ) {
                    
                    ssize_t bytes_read = read( handle, file.bytes + total_read, to_read );
                    
                    if ( bytes_read == -1 ) {
                        break;
                    } else {
                        
                        total_read += bytes_read;
                        to_read = ( size_t ) ( file.reserved - addin - total_read );
                        to_read = ( to_read > SSIZE_MAX ) ? SSIZE_MAX : to_read;
                        
                        if ( bytes_read == 0 ) {
                            break;
                        }
                    }
                }
                
                if ( total_read != ( file.reserved - addin ) ) {
                    
                    file_free_memory( &file );
                    file.bytes = 0;
                    file.reserved = file.used = 0;
                    
                } else if ( zero_terminated ) {
                    *( file.bytes + file.reserved - 1 ) = '\0';
                }
                
            } else {
                
                file.reserved = file.used = 0;
            }
        }
        
        close( handle );
    }
    
    return file;
}

#endif

stu u64 file_write_entire_s( void* file_name, void* content, umm size, b32 replace ) {
    invalid_code_path( ); /* TODO simon: Only allow the _p version. */
    u64 total_written = 0;
    s32 flags = O_WRONLY | O_CREAT;
    
    if ( !replace ) {
        flags |= O_EXCL;
    }
    
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
    file_handle_t handle = open( cast( const char*, file_name ), flags, mode );
    
    if ( handle >= 0 ) {
        
        size_t to_write = ( size > SSIZE_MAX ) ? SSIZE_MAX : size;
        
        while ( total_written < size ) {
            
            ssize_t bytes_written = write( handle, cast( u8*, content ) + total_written, to_write );
            
            if ( bytes_written == -1 ) {
                break;
            }
            
            total_written += bytes_written;
            to_write = ( size_t ) ( size - total_written );
            to_write = ( to_write > SSIZE_MAX ) ? SSIZE_MAX : to_write;
        }
        
        close( handle );
    }
    
    return total_written;
}

stu void file_list_get_internal( file_list_t* list, data_t directory, data_t* extensions, umm extension_count, data_t* exceptions, umm exception_count, u32 flags ) {
    
    data_t directory_wildcard = directory;
    string_zero_terminate( directory_wildcard );
    
    DIR* dir = opendir( directory_wildcard.text );
    
    if ( dir != 0 ) {
        
        struct dirent* data_read = readdir( dir );
        
        while ( data_read && list->count < list->max_count ) {
            
            u8 is_directory = ( data_read->d_type == DT_DIR );
            umm length = string_length( data_read->d_name );
            data_t name = memory_construct( length, length, data_read->d_name );
            
            b32 is_exception = false;
            
            for ( umm i = 0; !is_exception && i < exception_count; i++ ) {
                is_exception = memory_are_equal( name, exceptions[ i ] );
            }
            
            if ( !is_exception ) {
                
                if ( is_directory && flag_is_set_u32( flags, file_list_directories ) ) {
                    
                    if ( flag_is_set_u32( flags, file_list_add_self_and_parent ) || ( !memory_are_equal( name, string_l( "." ) ) && !memory_are_equal( name, string_l( ".." ) ) ) ) {
                        
                        list->directories[ list->count ] = true;
                        list->names[ list->count ] = cast( u8*, memory_push_copy( list->memory, directory, 1 ) );
                        memory_push_u8( list->memory, '/', 1 );
                        memory_push_copy( list->memory, name, 1 );
                        memory_push_u8( list->memory, 0, 1 );
                        list->names_codeunit_count[ list->count ] = math_safe_truncate_to_u16( directory.used + 1 + length );
                        
                        struct stat stats = { 0 };
                        int stat_result = stat( cast( char*, list->names[ list->count ] ), &stats );
                        /* TODO simon: A symbolic link to a file or directory that doesn't exists would trigger the assert. */
                        _assert( stat_result >= 0 );
                        
                        list->sizes[ list->count ] = stats.st_size;
                        list->last_write_times[ list->count ] = stats.st_mtime;
                        list->count++;
                    }
                    
                }  else if ( !is_directory && flag_is_set_u32( flags, file_list_files ) ) {
                    
                    b32 valid = false;
                    
                    if ( extension_count ) {
                        
                        data_t extension = file_get_extension_from_name_p( data_read->d_name, length );
                        
                        for ( umm index = 0; index < extension_count; index++ ) {
                            
                            if ( memory_are_equal( extensions[ index ], extension ) ) {
                                valid = true;
                                break;
                            }
                        }
                    } else {
                        valid = true;
                    }
                    
                    if ( valid ) {
                        list->directories[ list->count ] = false;
                        list->names[ list->count ] = cast( u8*, memory_push_copy( list->memory, directory, 1 ) );
                        memory_push_u8( list->memory, '/', 1 );
                        memory_push_copy( list->memory, name, 1 );
                        memory_push_u8( list->memory, 0, 1 );
                        list->names_codeunit_count[ list->count ] = math_safe_truncate_to_u16( directory.used + 1 + name.used );
                        
                        struct stat stats = { 0 };
                        int stat_result = stat( cast( char*, list->names[ list->count ] ), &stats );
                        _assert( stat_result >= 0 );
                        
                        list->sizes[ list->count ] = stats.st_size;
                        list->last_write_times[ list->count ] = stats.st_mtime;
                        list->count++;
                    }
                }
                
                if ( is_directory && flag_is_set_u32( flags, file_list_recursive ) && list->count < list->max_count ) {
                    
                    if ( !memory_are_equal( name, string_l( "." ) ) && !memory_are_equal( name, string_l( ".." ) ) ) {
                        
                        data_t sub_dir = directory;
                        memory_push_u8( &sub_dir, '/', 1 );
                        memory_push_copy( &sub_dir, name, 1 );
                        memory_push_u8( list->memory, 0, 1 );
                        
                        file_list_get_internal( list, sub_dir, extensions, extension_count, exceptions, exception_count, flags );
                    }
                }
            }
            
            data_read = readdir( dir );
        }
        
        closedir( dir );
    }
}

stu void file_list_get( file_list_t* list, data_t directory, data_t comma_separated_extensions, data_t comma_separated_exceptions, u32 flags ) {
    
    data_t extensions[ 64 ] = { 0 };
    umm extension_count = internal_split_comma_separated_into_list( comma_separated_extensions, extensions, array_count( extensions ) );
    
    data_t exceptions[ 64 ] = { 0 };
    umm exception_count = internal_split_comma_separated_into_list( comma_separated_exceptions, exceptions, array_count( exceptions ) );
    
    memory_get_on_stack( directory_wildcard, 1024 );
    _assert( directory.used <= directory_wildcard.reserved );
    memory_push_copy( &directory_wildcard, directory, 1 );
    
    if ( !flag_is_set_u32( flags, file_list_files ) && !flag_is_set_u32( flags, file_list_directories ) ) {
        flags |= ( file_list_files | file_list_directories );
    }
    
    file_list_get_internal( list, directory_wildcard, extensions, extension_count, exceptions, exception_count, flags );
}

stu void file_list_get_old( file_list_t* list, data_t directory, data_t comma_separated_extensions, b32 recursive ) {
    
    memory_get_on_stack( directory_wildcard, 1024 );
    _assert( directory.used <= directory_wildcard.reserved );
    memory_push_copy( &directory_wildcard, directory, 1 );
    memory_push_u8( &directory_wildcard, 0, 1 );
    
    DIR* dir = opendir( directory_wildcard.text );
    
    if ( dir != 0 ) {
        
        struct dirent* data_read = 0;
        
        data_t extensions[ 64 ] = { 0 };
        umm extension_count = internal_split_comma_separated_into_list( comma_separated_extensions, extensions, array_count( extensions ) );
        
        while ( ( data_read = readdir( dir ) ) != 0 ) {
            
            u32 length = string_length( data_read->d_name );
            b32 valid = false;
            
            if ( extension_count ) {
                
                data_t extension = file_get_extension_from_name_p( data_read->d_name, length );
                
                for ( umm index = 0; index < extension_count; index++ ) {
                    
                    if ( memory_are_equal( extensions[ index ], extension ) ) {
                        valid = true;
                        break;
                    }
                }
                
            } else {
                valid = true;
            }
            
            u8 is_directory = ( data_read->d_type == DT_DIR );
            
            if ( valid && list->count < list->max_count ) {
                
                list->directories[ list->count ] = is_directory;
                list->names[ list->count ] = cast( u8*, memory_push_copy( list->memory, directory, 1 ) );
                memory_push_u8( list->memory, '/', 1 );
                memory_push_copy_p( list->memory, data_read->d_name, length, 1 );
                memory_push_u8( list->memory, 0, 1 );
                list->names_codeunit_count[ list->count ] = math_safe_truncate_to_u16( directory.used + 1 + length );
                
                struct stat stats = { 0 };
                int stat_result = stat( cast( char*, list->names[ list->count ] ), &stats );
                _assert( stat_result >= 0 );
                
                list->sizes[ list->count ] = stats.st_size;
                list->last_write_times[ list->count ] = stats.st_mtime;
                list->count++;
            }
            
            if ( recursive && is_directory && list->count < list->max_count ) {
                
                if ( !memory_are_equal_p( ".", 1, data_read->d_name, length ) && !memory_are_equal_p( "..", 2, data_read->d_name, length ) ) {
                    
                    directory_wildcard.used = 0;
                    memory_push_copy( &directory_wildcard, directory, 1 );
                    memory_push_u8( &directory_wildcard, '/', 1 );
                    memory_push_copy_p( &directory_wildcard, data_read->d_name, length, 1 );
                    memory_push_u8( list->memory, 0, 1 );
                    file_list_get_old( list, directory_wildcard, comma_separated_extensions, recursive );
                }
            }
        }
        
        closedir( dir );
    }
}

stu data_t file_read_entire_p( void* file_name_arg, umm file_name_length, umm reserve_at_end, u32* error ) {
    
    data_t result = { };
    file_handle_t file = file_open_p( file_name_arg, file_name_length, file_mode_read, false, error ); 
    
    if ( no_error( error ) ){
        
        struct stat stats = {};
        
        if ( fstat( file, &stats ) >= 0 ) {
            
            u64 addin = reserve_at_end;
            u64 size = stats.st_size;
            result = memory_get_virtual( size + addin, 0 ); 
            
            if ( result.bytes ) {
                
                result.used = file_read_handle( file, 0, size, result.bytes, error );
                
                if ( result.used != size ) {
                    _assert( *error == file_error_read_size );
                    memory_free_virtual( &result );
                    memory_invalidate( &result );
                }
                
            } else if ( ( size + addin ) != 0 ) {
                
                set_error( error, file_error_read_allocation_failed );
            }
            
        } else {
            
            set_error( error, file_error_get_file_size );
        }
        
        file_close( file, error );    
    } 
    
    return result;
}

stu b32 file_create_directory_p( void* directory_name_arg, umm directory_name_length, u32* error ) {
    
    memory_get_on_stack( directory_name, kibioctet( 2 ) );
    memory_push_copy_p( &directory_name, directory_name_arg, directory_name_length, 1 );
    string_zero_terminate( directory_name );
    
    b32 result = false;
    
    mode_t mode = S_IRWXU | S_IRWXG | S_IRGRP | S_IXOTH;
    
    if ( mkdir( cast( const char*, directory_name.bytes ), mode ) == 0 ) {
        result = true;
    }
    
    return result;
}

stu b32 file_move_p( void* source_arg, umm source_length, void* destination_arg, umm destination_length, b32 replace, u32* error ) {
    
    b32 result = false; 
    
    if ( no_error( error ) ) {
        
        memory_get_on_stack( source, kibioctet( 2 ) );
        memory_push_copy_p( &source, source_arg, source_length, 1 );
        string_zero_terminate( source );
        
        memory_get_on_stack( destination, kibioctet( 2 ) );
        memory_push_copy_p( &destination, destination_arg, destination_length, 1 );
        string_zero_terminate( destination );
        
        s32 failure = false;
        
        if ( replace ) {
            failure = rename( cast( const char*, source.bytes ), cast( const char*, destination.bytes ) );
        } else {
            int local_RENAME_NOREPLACE = 1; /* NOTE simon: This is defined in <linux/fs.h> */
            failure = syscall( SYS_renameat2, AT_FDCWD, source.bytes, AT_FDCWD, destination.bytes, local_RENAME_NOREPLACE );
        }
        
        if ( !failure ) {
            result = true;
        } else {
            set_error( error, file_error_move );
        }
    }
    
    return result;
}

/* TODO simon: there is no check to see if the whole content of the file is copied. sendfile return
* the number of bytes copied which could be less then the input file. */
stu b32 file_copy_p( void* source_arg, umm source_length, void* destination_arg, umm destination_length, b32 replace, u32* error ){
    
    b32 result = false;
    
    memory_get_on_stack( source, kibioctet( 2 ) ); 
    memory_push_copy_p( &source, source_arg, source_length, 1 );
    string_zero_terminate( source );
    
    s32 in_file = open( cast( const char*, source.bytes ), O_RDONLY );
    
    if ( in_file >= 0 ) {
        
        s32 flags = O_WRONLY | O_CREAT;
        
        if ( !replace ) {
            flags |= O_EXCL;
        }
        
        struct stat stats = {};
        
        if ( fstat( in_file, &stats ) >= 0 ) {
            
            memory_get_on_stack( destination, kibioctet( 2 ) );
            memory_push_copy_p( &destination, destination_arg, destination_length, 1 );
            string_zero_terminate( destination );
            
            s32 out_file = open( cast( const char*, destination.bytes ), flags, stats.st_mode );
            
            if ( out_file >= 0 ) {
                
                _assert( stats.st_size <= 0x7ffff000 );
                s32 bytes_written = sendfile( out_file, in_file, 0, stats.st_size );
                
                if ( bytes_written != -1 ) {
                    result = true;
                }
            }
            
            close( out_file );
        }
        
        close( in_file );
    }
    
    return result;
}

stu b32 file_exists_p( void* file_name_arg, umm file_name_length, u32* error ) {
    
    memory_get_on_stack( file_name, kibioctet( 2 ) );
    memory_push_copy_p( &file_name, file_name_arg, file_name_length, 1 ); 
    string_zero_terminate( file_name );
    
    b32 result = false;
    struct stat buffer;
    s32 status = stat( cast( const char*, file_name.bytes ), &buffer );
    
    if ( status == 0 && ( buffer.st_mode & S_IFDIR ) == 0 ) {
        result = true;
    }
    
    return result;
}

stu b32 file_directory_exists_p( void* directory_name_arg, umm directory_name_length, u32* error ) {
    
    memory_get_on_stack( directory_name, kibioctet( 2 ) );
    memory_push_copy_p( &directory_name, directory_name_arg, directory_name_length, 1 );
    string_zero_terminate( directory_name );
    
    b32 result = false;
    struct stat buffer;
    s32 status = stat( cast( const char*, directory_name.bytes ), &buffer );
    
    if ( status == 0 && ( buffer.st_mode & S_IFDIR ) != 0 ) {
        result = true;
    }
    
    return result;
}

#else
# error Unsupported platform.
#endif

stu void internal_file_list_sort_date_ascending( file_list_t* list, umm start, umm end ) {
    
    if ( list && start < end ) {
        
        umm pivot_index = ( start + end ) / 2;
        u64 pivot = list->last_write_times[ pivot_index ];
        
        umm i = start, j = end;
        
        while ( 1 ) {
            
            while ( list->last_write_times[ i ] < pivot ) {
                i++;
            }
            
            while ( list->last_write_times[ j ] > pivot ) {
                j--;
            }
            
            if ( i < j ) {
                
                u8* name_temp = list->names[ i ];
                u64 size_temp = list->sizes[ i ];
                u16 name_codeunit_count_temp = list->names_codeunit_count[ i ];
                u8 directory_temp = list->directories[ i ];
                u64 last_write_time_temp = list->last_write_times[ i ];
                
                list->names[ i ] = list->names[ j ];
                list->sizes[ i ] = list->sizes[ j ];
                list->names_codeunit_count[ i ] = list->names_codeunit_count[ j ];
                list->directories[ i ] = list->directories[ j ];
                list->last_write_times[ i ] = list->last_write_times[ j ];
                
                list->names[ j ] = name_temp;
                list->sizes[ j ] = size_temp;
                list->names_codeunit_count[ j ] = name_codeunit_count_temp;
                list->directories[ j ] = directory_temp;
                list->last_write_times[ j ] = last_write_time_temp;
                
                i++;
                j--;
                
            } else {
                break;
            }
        }
        
        internal_file_list_sort_date_ascending( list, start, j );
        internal_file_list_sort_date_ascending( list, j + 1, end );
    }
}

stu void file_list_sort_date_ascending( file_list_t* list ) {
    if ( list->count ) {
        internal_file_list_sort_date_ascending( list, 0, list->count - 1 );
    }
}

stu void internal_file_list_sort_name_ascending( file_list_t* list, umm start, umm end ) {
    
    if ( list && start < end ) {
        
        umm pivot_index = ( start + end ) / 2;
        u8* pivot = list->names[ pivot_index ];
        u16 pivot_length = list->names_codeunit_count[ pivot_index ];
        
        umm i = start, j = end;
        
        while ( 1 ) {
            
            while ( string_compare( list->names[ i ], pivot, math_min( list->names_codeunit_count[ i ], pivot_length ) ) < 0 ) {
                i++;
            }
            
            while ( string_compare( list->names[ j ], pivot, math_min( list->names_codeunit_count[ j ], pivot_length ) ) > 0 ) {
                j--;
            }
            
            if ( i < j ) {
                
                u8* name_temp = list->names[ i ];
                u64 size_temp = list->sizes[ i ];
                u16 name_codeunit_count_temp = list->names_codeunit_count[ i ];
                u8 directory_temp = list->directories[ i ];
                u64 last_write_time_temp = list->last_write_times[ i ];
                
                list->names[ i ] = list->names[ j ];
                list->sizes[ i ] = list->sizes[ j ];
                list->names_codeunit_count[ i ] = list->names_codeunit_count[ j ];
                list->directories[ i ] = list->directories[ j ];
                list->last_write_times[ i ] = list->last_write_times[ j ];
                
                list->names[ j ] = name_temp;
                list->sizes[ j ] = size_temp;
                list->names_codeunit_count[ j ] = name_codeunit_count_temp;
                list->directories[ j ] = directory_temp;
                list->last_write_times[ j ] = last_write_time_temp;
                
                i++;
                j--;
                
            } else {
                break;
            }
        }
        
        internal_file_list_sort_name_ascending( list, start, j );
        internal_file_list_sort_name_ascending( list, j + 1, end );
    }
}

stu void file_list_sort_name_ascending( file_list_t* list ) {
    if ( list->count ) {
        internal_file_list_sort_name_ascending( list, 0, list->count - 1 );
    }
}

stu void internal_file_list_sort_user_ascending( file_list_t* list, umm start, umm end, u64* user_data ) {
    
    if ( list && start < end ) {
        
        umm pivot_index = ( start + end ) / 2;
        u64 pivot = user_data[ pivot_index ];
        
        umm i = start, j = end;
        
        while ( 1 ) {
            
            while ( user_data[ i ] < pivot ) {
                i++;
            }
            
            while ( user_data[ j ] > pivot ) {
                j--;
            }
            
            if ( i < j ) {
                
                u8* name_temp = list->names[ i ];
                u64 size_temp = list->sizes[ i ];
                u16 name_codeunit_count_temp = list->names_codeunit_count[ i ];
                u8 directory_temp = list->directories[ i ];
                u64 last_write_time_temp = list->last_write_times[ i ];
                u64 user_data_temp = user_data[ i ];
                
                list->names[ i ] = list->names[ j ];
                list->sizes[ i ] = list->sizes[ j ];
                list->names_codeunit_count[ i ] = list->names_codeunit_count[ j ];
                list->directories[ i ] = list->directories[ j ];
                list->last_write_times[ i ] = list->last_write_times[ j ];
                user_data[ i ] = user_data[ j ];
                
                list->names[ j ] = name_temp;
                list->sizes[ j ] = size_temp;
                list->names_codeunit_count[ j ] = name_codeunit_count_temp;
                list->directories[ j ] = directory_temp;
                list->last_write_times[ j ] = last_write_time_temp;
                user_data[ j ] = user_data_temp;
                
                i++;
                j--;
                
            } else {
                break;
            }
        }
        
        internal_file_list_sort_user_ascending( list, start, j, user_data );
        internal_file_list_sort_user_ascending( list, j + 1, end, user_data );
    }
}

stu void file_list_sort_user_ascending( file_list_t* list, u64* user_data ) {
    if ( list->count ) {
        internal_file_list_sort_user_ascending( list, 0, list->count - 1, user_data );
    }
}

stu void file_delete( data_t file_name, u32* error ) {
    file_delete_p( file_name.bytes, file_name.used, error );
}

#if 1

stu file_handle_t file_open( data_t file_name, u8 mode, b32 replace, u32* error ) {
    file_handle_t result = file_open_p( file_name.bytes, file_name.used, mode, replace, error );
    return result;
}

stu u64 file_get_size_p( void* file_name_arg, umm file_name_length, u32* error ) {
    
    file_handle_t file = file_open_p( file_name_arg, file_name_length, file_mode_read, 0, error );
    u64 result = file_get_size_handle( file, error );
    file_close( file, error );
    return result;
}

stu u64 file_get_size( data_t file_name, u32* error ) {
    u64 result = file_get_size_p( file_name.bytes, file_name.used, error );
    return result;
}

stu data_t file_read_entire( data_t file_name, umm reserve_at_end, u32* error ) {
    data_t result = file_read_entire_p( file_name.bytes, file_name.used, reserve_at_end, error );
    return result;
}

stu void file_read_p( void* file_name_arg, umm file_name_length, s64 position, u64 count, void* destination, u32* error ) {
    file_handle_t file = file_open_p( file_name_arg, file_name_length, file_mode_read, false, error );
    file_read_handle( file, position, count, destination, error );
    file_close( file, error );
}

stu void file_read( data_t* file_name, s64 position, u64 count, void* destination, u32* error ) {
    file_read_p( file_name->bytes, file_name->used, position, count, destination, error );
}

stu u64 file_write_entire_p( void* file_name_arg, umm file_name_length, void* bytes, umm size, b32 replace, u32* error ) {
    file_handle_t file = file_open_p( file_name_arg, file_name_length, file_mode_write, replace, error );
    u64 result = file_write_handle( file, 0, size, bytes, error );
    file_close( file, error );
    return result;
}

stu u64 file_write_entire( data_t file_name, void* bytes, umm size, b32 replace, u32* error ) {
    u64 result = file_write_entire_p( file_name.bytes, file_name.used, bytes, size, replace, error );
    return result;
}

stu void file_write_p( void* file_name_arg, umm file_name_length, u8* source, s64 position, u64 count, b32 replace, u32* error ) {
    file_handle_t file = file_open_p( file_name_arg, file_name_length, file_mode_write, replace, error );
    file_write_handle( file, position, count, source, error );
    file_close( file, error );
}

stu void file_write( data_t file_name, u8* source, s64 position, u64 count, b32 replace, u32* error ) {
    file_write_p( file_name.bytes, file_name.used, source, position, count, replace, error );
}

stu b32 file_create_directory( data_t directory_name, u32* error ) {
    b32 result = file_create_directory_p( directory_name.bytes, directory_name.used, error );
    return result;
}

#if 0
stu b32 file_delete_directory_p( void* directory_name_arg, umm directory_name_length ) {
    string_zero_terminate_on_stack( directory_name, 512, directory_name_arg, directory_name_length );
    b32 result = file_delete_directory_s( directory_name.bytes );
    return result;
}

stu b32 file_delete_directory( data_t* directory_name ) {
    b32 result = file_delete_directory_p( directory_name->bytes, directory_name->used );
    return result;
}
#endif

stu void file_move( data_t source, data_t destination, b32 replace, u32* error ) {
    file_move_p( source.bytes, source.used, destination.bytes, destination.used, replace, error );
}

stu b32 file_copy( data_t source, data_t destination, b32 replace, u32* error ) {
    b32 result = file_copy_p( source.bytes, source.used, destination.bytes, destination.used, replace, error );
    return result;
}

stu void file_list_get_p( file_list_t* list, void* directory_name_arg, umm directory_name_length, data_t comma_separeted_extensions, data_t comma_separated_exceptions, u32 flags ) {
    
    data_t directory = memory_construct( directory_name_length, directory_name_length, directory_name_arg );
    file_list_get( list, directory, comma_separeted_extensions, comma_separated_exceptions, flags );
}

#define file_list_get_l( list, directory_name, comma_separeted_extensions, comma_separated_exceptions, flags ) \
file_list_get_p( ( list ), ( directory_name ), sizeof( directory_name ) - 1, ( comma_separeted_extensions ), ( comma_separated_exceptions ), ( flags ) )

stu b32 file_exists( data_t file_name, u32* error ) {
    b32 result = file_exists_p( file_name.bytes, file_name.used, error );
    return result;
}

stu b32 file_directory_exists( data_t directory_name, u32* error ) {
    b32 result = file_directory_exists_p( directory_name.bytes, directory_name.used, error );
    return result;
}

stu b32 file_create_directory_numbered( data_t pattern, umm number_size, data_t* directory_name, u32* error ) {
    
    u64 directory_number = 0;
    
    _assert( pattern.used + number_size <= directory_name->reserved );
    memory_push_copy( directory_name, pattern, 1 );
    u8* number_start = memory_current( *directory_name, u8 );
    
    u64 max_number = 1;
    
    for ( umm i = 0; i < number_size; i++ ) {
        max_number *= 10;
        memory_push_u8( directory_name, '0', 1 );
    }
    
    while ( file_directory_exists( *directory_name, error ) && directory_number < max_number ) {
        
        directory_number++;
        umm count = string_from_u64( number_start, number_size, directory_number, number_size, '0', 0 );
    }
    
    b32 directory_created = file_create_directory( *directory_name, error );
    
    return directory_created;
}

/* NOTE simon (03/10/23 17:33:43): Extension without the '.'. This is to mirror what the read function requires. */
stu u64 file_write_timestamped( data_t prefix, data_t extension, void* bytes, umm size, b32 replace, u32* error ) {
    
    date_time_t date = date_get_current( );
    
    memory_get_on_stack( filename, 1024 );
    memory_push_copy( &filename, prefix, 1 );
    memory_push_u8( &filename, '_', 1 );
    date_to_string_timestamp( &filename, date );
    memory_push_u8( &filename, '.', 1 );
    memory_push_copy( &filename, extension, 1 );
    
    u64 result = file_write_entire( filename, bytes, size, replace, error );
    
    return result;
}

stu data_t file_read_timestamped( data_t prefix, data_t extension, umm reserve_at_end, u32* error ) {
    
    data_t result = { 0 };
    
    data_t current_dir = string_l( ".\\" );
    data_t directory = file_name_get_path( prefix );
    data_t filename = file_name_remove_path( prefix );
    
    if ( !directory.used ) {
        directory = current_dir;
    }
    
    memory_get_on_stack( list_memory, kibioctet( 64 ) );
    
    file_list_t list = file_list_make( &list_memory, 100 );
    file_list_get( &list, directory, extension, string_l( "" ), file_list_files );
    data_t file_to_load = { 0 };
    umm most_recent_timestamp = 0;
    
    for ( umm file_index = 0; file_index < list.count; file_index++ ) {
        
        data_t name = memory_construct( list.names_codeunit_count[ file_index ], list.names_codeunit_count[ file_index ], list.names[ file_index ] );
        data_t short_name = file_name_remove_path( name );
        
        if ( memory_starts_with( short_name, filename ) ) {
            
            u64 timestamp = file_name_get_timestamp( name );
            
            if ( timestamp > most_recent_timestamp ) {
                file_to_load = name;
                most_recent_timestamp = timestamp;
            }
        }
    }
    
    if ( file_exists( file_to_load, error ) ) {
        result = file_read_entire( file_to_load, reserve_at_end, error );
    }
    
    return result;
}

#endif

// MARK performance

#if defined( WINDOWS )

#define perf_get_frequency( u64_ptr ) QueryPerformanceFrequency( cast( LARGE_INTEGER*, ( u64_ptr ) ) )
#define perf_get_time( u64_ptr ) QueryPerformanceCounter( cast( LARGE_INTEGER*, ( u64_ptr ) ) )
#define perf_get_cycle_count( ) __rdtsc( )

#elif defined( LINUX )

#define perf_get_frequency( u64_ptr ) ( *( u64_ptr ) ) = 1000000000
#define perf_get_time( u64_ptr ) { struct timespec spec; clock_gettime( CLOCK_MONOTONIC, &spec ); ( *( u64_ptr ) ) = ( spec.tv_sec * ( u64 ) 1000000000 + spec.tv_nsec ); }
#define perf_get_cycle_count( ) __rdtsc( )

#else
#error Unsupported platform.
#endif

stu f64 perf_get_elapsed_time( u64 start, u64 end, u64 frequency ) {
    f64 result = ( ( f64 ) ( end - start ) ) / ( f64 ) frequency;
    return result;
}

// MARK random

/* NOTE simon: https://www.pcg-random.org/posts/developing-a-seed_seq-alternative.html */
/* NOTE simon (07/10/23 13:08:23): https://dotat.at/@/2023-06-21-pcg64-dxsm.html use pcg dxsm for 64bit versions ? */

typedef struct random_entropy_mixer_t {
    u32 source[ 8 ];
    umm count;
} random_entropy_mixer_t;

stu void random_entropy_mixer_add_source( random_entropy_mixer_t* mixer, u32 source ) {
    
    if ( mixer->count < array_count( mixer->source ) ) {
        mixer->source[ mixer->count ] = source;
        mixer->count++;
    }
}

stu u32 internal_random_entropy_mixer_hash( u32 value ) {
    
    static u32 hash_multiplier = 0x43b07e5;
    
    value ^= hash_multiplier;
    hash_multiplier *= 0x931e8875;
    value *= hash_multiplier;
    value ^= value >> 16;
    
    return value;
}

#if 0
stu u32 internal_random_entropy_mixer_mix( u32 x, u32 y ) {
    
    u32 result = 0xca01f9dd * x - 0x4973f715 * y;
    result ^= result >> 16;
    
    return result;
}
#endif

stu void random_entropy_mixer_mix( random_entropy_mixer_t* mixer ) {
    
    for ( umm i = 0; i < mixer->count; i++ ) {
        mixer->source[ i ] = internal_random_entropy_mixer_hash( mixer->source[ i ] );
    }
    
    for ( umm i = 0; i < mixer->count; i++ ) {
        
        for ( umm j = 0; j < mixer->count; j++ ) {
            
            if ( i != j ) {
                
                u32 h = internal_random_entropy_mixer_hash( mixer->source[ i ] );
                u32 mix = 0xca01f9dd * h - 0x4973f715 * mixer->source[ j ];
                mixer->source[ j ] = mix ^ ( mix >> 16 );
            }
        }
    }
}

stu void random_entropy_mixer_generate( random_entropy_mixer_t* mixer, u32* destination, umm destination_count ) {
    
    static u32 hash_multiplier = 0x8b51f9dd;
    static u32 source_index = 0;
    
    for ( umm i = 0; i < destination_count; i++ ) {
        
        u32 value = mixer->source[ source_index ];
        
        value ^= hash_multiplier;
        hash_multiplier *= 0x58f38ded;
        value *= hash_multiplier;
        value ^= value >> 16;
        
        destination[ i ] = value;
        
        source_index++;
        
        if ( source_index >= mixer->count ) {
            source_index = 0;
        }
    }
}

stu void random_seed_get( void* dest, umm byte_count ) {
    
    static random_entropy_mixer_t mixer = { 0 };
    
    if ( mixer.count == 0 ) {
        
        u64 cycles = perf_get_cycle_count( );
        u64 function_address = ( u64 ) &random_seed_get;
        u64 variable_address = ( u64 ) &mixer;
        u64 external_address = ( u64 ) dest;
        
        random_entropy_mixer_add_source( &mixer, ( u32 ) cycles );
        random_entropy_mixer_add_source( &mixer, ( u32 ) function_address );
        random_entropy_mixer_add_source( &mixer, ( u32 ) variable_address );
        random_entropy_mixer_add_source( &mixer, ( u32 ) external_address );
        
        u32 cycles_fnv = ( u32 ) ( cycles >> 32 );
        u32 function_fnv = ( u32 ) ( function_address >> 32 );
        u32 variable_fnv = ( u32 ) ( variable_address >> 32 );
        u32 external_fnv = ( u32 ) ( external_address >> 32 );
        
        random_entropy_mixer_add_source( &mixer, hash_fnv_32( &cycles_fnv, 4 ) );
        random_entropy_mixer_add_source( &mixer, hash_fnv_32( &function_fnv, 4 ) );
        random_entropy_mixer_add_source( &mixer, hash_fnv_32( &variable_fnv, 4 ) );
        random_entropy_mixer_add_source( &mixer, hash_fnv_32( &external_fnv, 4 ) );
        
        random_entropy_mixer_mix( &mixer );
    }
    
    umm count = byte_count / 4;
    _assert( count * 4 == byte_count );
    
    random_entropy_mixer_generate( &mixer, dest, count );
}

stu u64 random_seed_get_u64( void ) {
    u64 result;
    random_seed_get( &result, sizeof( result ) );
    return result;
}

#if TARGET_x64
stu u128_t random_seed_get_u128( void ) {
    u128_t result;
    random_seed_get( &result, sizeof( result ) );
    return result;
}
#endif

/* NOTE simon: random_32 and random_64 are modifications of PCG to fit my style. They should produce the same result as the official PCG. */

/*
 * PCG Random Number Generation for C.
 *
 * Copyright 2014-2019 Melissa O'Neill <oneill@pcg-random.org>,
 *                     and the PCG Project contributors.
 *
 * SPDX-License-Identifier: (Apache-2.0 OR MIT)
 *
 * Licensed under the Apache License, Version 2.0 (provided in
 * LICENSE-APACHE.txt and at http://www.apache.org/licenses/LICENSE-2.0)
 * or under the MIT license (provided in LICENSE-MIT.txt and at
 * http://opensource.org/licenses/MIT), at your option. This file may not
 * be copied, modified, or distributed except according to those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 * For additional information about the PCG random number generation scheme,
 * visit http://www.pcg-random.org/.
 */

/* NOTE simon: On linux to get a good seed, we can read the file /dev/urandom. */
/* TODO simon: Handle generating floating point values. */

#define random_32_multiplier  6364136223846793005ull

typedef struct random_32_t {
    u64 state;
    u64 increment;
} random_32_t;

stu random_32_t random_32_make( u64 seed, u64 sequence ) {
    
    random_32_t result = { 0 };
    result.increment = ( sequence << 1 ) | 1;
    result.state = result.increment + seed;
    result.state = ( result.state * random_32_multiplier ) + result.increment;
    
    return result;
}

stu u32 random_32_get( random_32_t* random_32 ) {
    
    /* NOTE simon: 64 xsh rr 32 */
    u64 temp = random_32->state;
    
    random_32->state = ( random_32->state * random_32_multiplier ) + random_32->increment;
    
    /* NOTE simon: 2^5 => 32 bit rotation. */
    u8 rotation = ( u8 ) ( temp >> ( 64 - 5 ) );
    temp = ( ( temp >> 18 ) ^ temp ) >> 27;
    u32 result = ( u32 ) temp;
    result = ( result >> rotation ) | ( result << ( 32 - rotation ) );
    
    return result;
}

stu u32 random_32_get_range( random_32_t* random_32, u32 min, u32 max ) {
    
    u32 result = 0;
    u32 range = max - min;
    u32 threshold = ( u32 ) ( ( 0x100000000ull - range ) % range );
    
    while ( true ) {
        
        u32 number = random_32_get( random_32 );
        
        if ( number >= threshold ) {
            result = min + ( number % range );
            break;
        }
    }
    
    return result;
}

stu void random_32_advance( random_32_t* random_32, u64 delta ) {
    
    u64 multiplier = random_32_multiplier;
    u64 increment = random_32->increment;
    
    u64 multiply_accumulator = 1;
    u64 increment_accumulator = 0;
    
    while ( delta > 0 ) {
        
        if ( delta & 1 ) {
            multiply_accumulator *= multiplier;
            increment_accumulator = ( increment_accumulator * multiplier ) + increment;
        }
        
        increment = ( multiplier + 1 ) * increment;
        multiplier *= multiplier;
        delta /= 2;
    }
    
    random_32->state = multiply_accumulator * random_32->state + increment_accumulator;
}

#if defined( TARGET_x32 )

typedef struct random_64_t {
    random_32_t rng_high;
    random_32_t rng_low;
} random_64_t;

stu random_64_t random_64_make( u64 seed_1, u64 sequence_1, u64 seed_2, u64 sequence_2 ) {
    
    u64 mask = ~0ull >> 1;
    
    if ( ( sequence_1 & mask ) == ( sequence_2 & mask ) ) {
        sequence_2 = ~sequence_2;
    }
    
    random_64_t result = { 0 };
    result.rng_high = random_32_make( seed_1, sequence_1 );
    result.rng_low = random_32_make( seed_2, sequence_2 );
    
    return result;
}

stu u64 random_64_get( random_64_t* random_64 ) {
    
    u64 result = ( ( u64 ) random_32_get( &random_64->rng_high ) ) << 32;
    result = result | random_32_get( &random_64->rng_low );
    
    return result;
}

#else

#define random_64_multiplier ( u128_t ) { /* low */ 4865540595714422341ull, /* high */ 2549297995355413924ull }

typedef struct random_64_t {
    u128_t state;
    u128_t increment;
} random_64_t;

stu random_64_t random_64_make( u128_t seed, u128_t sequence ) {
    
    random_64_t result = { 0 };
    
    result.increment = math_u128_left_shift( sequence, 1 );
    result.increment.low |= 1;
    
    result.state = math_u128_add( result.increment, seed );
    result.state = math_u128_mul( result.state, random_64_multiplier );
    result.state = math_u128_add( result.state, result.increment );
    
    return result;
}

stu u64 random_64_get( random_64_t* random_64 ) {
    
    /* NOTE simon: 128 xsl rr 64 */
    
    u64 result = 0;
    random_64->state = math_u128_mul( random_64->state, random_64_multiplier );
    random_64->state = math_u128_add( random_64->state, random_64->increment );
    
#if 0
    u8 rotation = ( u8 ) math_u128_right_shift( random_64->state, 122 ).low;
    u64 temp = math_u128_right_shift( random_64->state, 64 ).low;
    temp ^= random_64->state.low;
#else
    /* NOTE simon: 2^6 => 64 bit rotation. */
    u8 rotation = ( u8 ) ( random_64->state.high >> ( 64 - 6 ) );
    u64 temp = random_64->state.high ^ random_64->state.low;
#endif
    
    result = ( temp >> rotation ) | ( temp << ( 64 - rotation ) );
    
    return result;
}

stu void random_64_advance( random_64_t* random_64, u128_t delta ) {
    
    u128_t multiplier = random_64_multiplier;
    u128_t increment = random_64->increment;
    
    u128_t multiply_accumulator = u128_make( 1, 0 );
    u128_t increment_accumulator = u128_make( 0, 0 );
    
    while ( delta.low > 0 || delta.high > 0 ) {
        
        if ( delta.low & 1 ) {
            multiply_accumulator = math_u128_mul( multiply_accumulator, multiplier );
            increment_accumulator = math_u128_mul( increment_accumulator, multiplier );
            increment_accumulator = math_u128_add( increment_accumulator, increment );
        }
        
        u128_t temp = math_u128_add( multiplier, u128_make( 1, 0 ) );
        increment = math_u128_mul( temp, increment );
        multiplier = math_u128_mul( multiplier, multiplier );
        delta = math_u128_right_shift( delta, 1 ); /* NOTE simon: delta /= 2. */
    }
    
    random_64->state = math_u128_mul( multiply_accumulator, random_64->state );
    random_64->state = math_u128_add( random_64->state, increment_accumulator );
}

#endif

/* NOTE simon: Max not included. */
stu u64 random_64_get_range( random_64_t* random_64, u64 min, u64 max ) {
    
    /* NOTE simon: There might be a faster solution: 
    https://gist.github.com/mmozeiko/1561361cd4105749f80bb0b9223e9db8#file-pcg64-h
    https://github.com/apple/swift/pull/39143
    */
    
    u64 result = 0;
    u64 range = max - min;
    u64 threshold = ( ( u64 ) ( -( s64 ) range ) ) % range;
    
    while ( true ) {
        
        u64 number = random_64_get( random_64 );
        
        if ( number >= threshold ) {
            result = min + ( number % range );
            break;
        }
    }
    
    return result;
}

stu f32 random_32_get_f32( random_32_t* random_32 ) {
    
    u32 n = random_32_get( random_32 );
    f32 f = cast( f32, n >> 8 ) * 0x1.0p-24f;
    
    return f;
}

/* NOTE simon: This generates number between 0.0f and 1.0f included. */
stu f32 random_32_get_f32_one_included( random_32_t* random_32 ) {
    
    u32 n = random_32_get_range( random_32, 0x3f800000, 0x40000000 + 1 );
    f32 result;
    deref( cast( u32*, &result ) ) = n;
    result -= 1.0f;
    
    return result;
}

stu f32 random_64_get_f32( random_64_t* random_64 ) {
    u64 n = random_64_get( random_64 );
    f32 f = cast( f32, n >> 40 ) * 0x1.0p-24f;
    return f;
}

stu f64 random_64_get_f64( random_64_t* random_64 ) {
    u64 n = random_64_get( random_64 );
    f64 f = cast( f64, n >> 11 ) * 0x1.0p-53;
    return f;
}

/* NOTE simon: This generates number between 0.0 and 1.0 included. */
stu f64 random_64_get_f64_one_included( random_64_t* random_64 ) {
    /* NOTE simon: I'm assuming this works. */
    u64 n = random_64_get_range( random_64, 0x3ff0000000000000, 0x4000000000000000 + 1 );
    f64 result;
    deref( cast( u64*, &result ) ) = n;
    result -= 1.0;
    
    return result;
}

#if 1
/* TODO simon: Use PCG (above) instead of those. */
stu u64 random_u64( u64* series ) {
    
    // NOTE simon: list of RNG http://www.pcg-random.org/other-rngs.html
    // NOTE simon: xorshift64* https://en.wikipedia.org/wiki/Xorshift
    u64 x = *series; // The state must be seeded with a nonzero value.
    x ^= x >> 12; // a
    x ^= x << 25; // b
    x ^= x >> 27; // c
    x *= 2685821657736338717ull;
    *series = x;
    /* NOTE simon: I think this should only return the top 32bits for "good" rng. */
    
    return x;
}

/* NOTE simon: returns a number between min included and max excluded. */
stu u64 random_range_u64( u64* series, u64 min, u64 max ) {
    
    u64 n = random_u64( series );
    n %= ( max - min );
    n += min;
    
    return n;
}

stu f64 random_f64( u64* series ) {
    
    f64 n = random_u64( series ) / ( f64 ) u64_max;
    
    return n;
}

stu f64 random_range_f64( u64* series, f64 min, f64 max ) {
    
    f64 n = random_f64( series );
    n *= max - min;
    n += min;
    
    return n;
}

stu f32 random_f32( u64* series ) {
    
    f32 n = ( f32 ) random_f64( series );
    return n;
}

stu f32 random_range_f32( u64* series, f32 min, f32 max ) {
    
    f32 n = random_f32( series );
    n *= max - min;
    n += min;
    
    return n;
}

#endif

/* MARK linear algebra */

/* NOTE simon (11/02/24 13:28:07): I put the sRGB things here. I might want to move them in the gl_ parts ? */

stu f32 srgb_to_linear_precise( f32 srgb ) {
    
    f32 linear = 0;
    
    if ( srgb <= 0.04045f ) {
        linear = srgb / 12.92f;
    } else {
        srgb += 0.055f;
        srgb /= 1.055f;
        linear = powf( srgb, 2.4f );
    }
    
    return linear;
}

stu f32 linear_to_srgb_precise( f32 linear ) {
    
    f32 srgb = 0;
    
    if ( linear <= 0.0031308f ) {
        srgb = 12.92f * linear;
    } else {
        linear = powf( linear, 1/2.4f );
        linear *= 1.055f;
        srgb = linear - 0.055f;
    }
    
    return srgb;
}

stu f32 srgb_to_linear( f32 srgb ) {
    f32 linear = powf( srgb, 2.2f );
    return linear;
}

stu f32 linear_to_srgb( f32 linear ) {
    f32 srgb = powf( linear, 1/2.2f );
    return srgb;
}

stu f32 srgb_to_linear_fast( f32 srgb ) {
    f32 linear = srgb * srgb;
    return linear;
}

stu f32 linear_to_srgb_fast( f32 linear ) {
    f32 srgb = math_sqrtf( linear );
    return srgb;
}

typedef union vec2 {
    
    struct {
        f32 x, y;
    };
    
    struct {
        f32 left, right;
    };
    
    struct {
        f32 u, v;
    };
    
    f32 e[ 2 ];
} vec2;

#define vec2_alignment sizeof( f32 )

typedef union vec2i {
    
    struct {
        s32 x, y;
    };
    
    s32 e[ 2 ];
} vec2i;

#define vec2i_alignment sizeof( s32 )

typedef union vec3 {
    
    struct {
        f32 x, y, z;
    };
    
    struct {
        f32 r, g, b;
    };
    
    struct {
        vec2 xy;
    };
    
    struct {
        f32 u, v, w;
    };
    
    struct {
        vec2 uv;
    };
    
    f32 e[ 3 ];
} vec3;

#define vec3_alignment sizeof( f32 )

typedef union vec3i {
    
    struct {
        s32 x, y, z;
    };
    
    struct {
        s32 r, g, b;
    };
    
    struct {
        vec2i xy;
    };
    
    struct {
        s32 u, v, w;
    };
    
    struct {
        vec2i uv;
    };
    
    s32 e[ 3 ];
} vec3i;

#define vec3i_alignment sizeof( f32 )

typedef union vec4 {
    
    struct {
        f32 x, y, z, w;
    };
    
    struct {
        f32 top, bottom, left, right;
    };
    
    struct {
        f32 r, g, b, a;
    };
    
    struct {
        vec2 xy;
    };
    
    struct {
        vec3 xyz;
    };
    
    struct {
        vec3 rgb;
    };
    
    f32 e[ 4 ];
} vec4;

#define vec4_alignment sizeof( f32 )

typedef union vec4i {
    
    struct {
        s32 x, y, z, w;
    };
    
    struct {
        s32 top, bottom, left, right;
    };
    
    struct {
        s32 r, g, b, a;
    };
    
    struct {
        vec2i xy;
    };
    
    struct {
        vec3i xyz;
    };
    
    struct {
        vec3i rgb;
    };
    
    s32 e[ 4 ];
} vec4i;

#define vec4i_alignment sizeof( s32 )

stu vec2 v2( f32 x, f32 y ) {
    vec2 result;
    result.x = x;
    result.y = y;
    
    return result;
}

stu vec2 v2_s32( s32 x, s32 y ) {
    vec2 result;
    result.x = ( f32 ) x;
    result.y = ( f32 ) y;
    
    return result;
}

stu vec2 v2_u32( u32 x, u32 y ) {
    vec2 result;
    result.x = ( f32 ) x;
    result.y = ( f32 ) y;
    
    return result;
}

stu vec2i v2i( s32 x, s32 y ) {
    vec2i result;
    result.x = x;
    result.y = y;
    
    return result;
}

stu vec2i v2i_from_r32( f32 x, f32 y ) {
    vec2i result;
    result.x = ( s32 ) x;
    result.y = ( s32 ) y;
    
    return result;
}

stu vec2 v2_from_v2i( vec2i v ) {
    vec2 result;
    result.x = ( f32 ) v.x;
    result.y = ( f32 ) v.y;
    return result;
}

stu vec3 v3( f32 x, f32 y, f32 z ) {
    vec3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    
    return result;
}

stu vec3i v3i( s32 x, s32 y, s32 z ) {
    vec3i result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

stu vec3 v3_from_v2( vec2 v )  {
    vec3 result;
    result.xy = v;
    result.z = 0;
    
    return result;
}

stu vec4 v4( f32 x, f32 y, f32 z, f32 w ) {
    vec4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    
    return result;
}

stu vec4i v4i( s32 x, s32 y, s32 z, s32 w ) {
    vec4i result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    
    return result;
}

stu vec4 v4_tblr( f32 t, f32 b, f32 l, f32 r ) {
    vec4 result;
    result.top = t;
    result.bottom = b;
    result.left = l;
    result.right = r;
    
    return result;
}

stu vec4 v4_add( vec4 a, vec4 b ) {
    
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    a.w += b.w;
    
    return a;
}

stu vec4i v4i_tblr( s32 t, s32 b, s32 l, s32 r ) {
    vec4i result;
    result.top = t;
    result.bottom = b;
    result.left = l;
    result.right = r;
    
    return result;
}

#define c_white { 1.0f, 1.0f, 1.0f, 1.0f }
#define c_black { 0.0f, 0.0f, 0.0f, 1.0f }
#define c_red { 1.0f, 0.0f, 0.0f, 1.0f }
#define c_green { 0.0f, 1.0f, 0.0f, 1.0f }
#define c_blue { 0.0f, 0.0f, 1.0f, 1.0f }
#define c_yellow { 1.0f, 1.0f, 0.0f, 1.0f }
#define c_magenta { 1.0f, 0.0f, 1.0f, 1.0f }
#define c_cyan { 0.0f, 1.0f, 1.0f, 1.0f }
#define c_gray_lightest { 0.875f, 0.875f, 0.875f, 1.0f }
#define c_gray_lighter { 0.75f, 0.75f, 0.75f, 1.0f }
#define c_gray_light { 0.625f, 0.625f, 0.625f, 1.0f }
#define c_gray { 0.5f, 0.5f, 0.5f, 1.0f }
#define c_gray_dark { 0.375f, 0.375f, 0.375f, 1.0f }
#define c_gray_darker { 0.25f, 0.25f, 0.25f, 1.0f }
#define c_gray_darkest { 0.125f, 0.125f, 0.125f, 1.0f }
#define c_orange { 1.0f, 0.5f, 0.0f, 1.0f }
#define c_pink { 1.0f, 0.0f, 1.0f, 1.0f }

vec4 white = c_white;
vec4 black = c_black;
vec4 red = c_red;
vec4 green = c_green;
vec4 blue = c_blue;
vec4 yellow = c_yellow;
vec4 magenta = c_magenta;
vec4 cyan = c_cyan;
vec4 gray_lightest = c_gray_lightest;
vec4 gray_lighter = c_gray_lighter;
vec4 gray_light = c_gray_light;
vec4 gray = c_gray;
vec4 gray_dark = c_gray_dark;
vec4 gray_darker = c_gray_darker;
vec4 gray_darkest = c_gray_darkest;
vec4 orange = c_orange;
vec4 pink = c_pink;

#define v2_zero( ) v2( 0, 0 )
#define v2i_zero( ) v2i( 0, 0 )
#define v3_zero( ) v3( 0, 0, 0 )
#define v3i_zero( ) v3i( 0, 0, 0 )
#define v4_zero( ) v4( 0, 0, 0, 0 )
#define v4i_zero( ) v4i( 0, 0, 0, 0 )

stu u32 v4_to_u32_color_slow( vec4 v ) {
    
    u32 result = cast( u8, math_roundf( v.r * 255.0f ) ) << 0;
    result |= cast( u8, math_roundf( v.g * 255.0f ) ) << 8;
    result |= cast( u8, math_roundf( v.b * 255.0f ) ) << 16;
    result |= cast( u8, math_roundf( v.a * 255.0f ) ) << 24;
    return result;
}

stu u32 v4_to_u32_color( vec4 v ) {
    
    __m128 a = _mm_loadu_ps( v.e );
    __m128 b = _mm_set_ps1( 255.0f );
    __m128 c = _mm_mul_ps( a, b );
    /* NOTE simon (13/02/24 18:33:00): cvtps_epi32 will do the rounding. */
    // __m128 d = _mm_round_ps( c, _MM_FROUND_CUR_DIRECTION );
    __m128i e = _mm_cvtps_epi32( c ); 
    __m128i f = _mm_packus_epi32( e, e );
    __m128i g = _mm_packus_epi16( f, f );
    u32 result = _mm_cvtsi128_si32( g );
    
    return result;
}

stu f32 v4_to_f32_color( vec4 v ) {
    u32 c = v4_to_u32_color( v );
    f32 result = deref( cast( f32*, &c ) );
    return result;
}

stu vec4 v4_from_u32_color_slow( u32 c ) {
    vec4 result;
    f32 normalize = 1.0f / 255.0f;
    result.r = ( ( c & 0x000000ff ) >> 0 ) * normalize;
    result.g = ( ( c & 0x0000ff00 ) >> 8 ) * normalize;
    result.b = ( ( c & 0x00ff0000 ) >> 16 ) * normalize;
    result.a = ( ( c & 0xff000000 ) >> 24 ) * normalize;
    return result;
}

stu vec4 v4_from_u32_color( u32 color ) {
    
    vec4 result = { 0 };
    __m128i a = _mm_loadu_si32( &color );
    __m128i mask = _mm_setr_epi8( 0, 0x80, 0x80, 0x80, 0x1, 0x80, 0x80, 0x80, 0x2, 0x80, 0x80, 0x80, 0x3, 0x80, 0x80, 0x80 );
    __m128i b = _mm_shuffle_epi8( a, mask );
    __m128 d = _mm_cvtepi32_ps( b );
    __m128 e = _mm_set_ps1( 1/255.0f );
    __m128 f = _mm_mul_ps( d, e );
    _mm_storeu_ps( result.e, f );
    return result;
}

stu vec4 v4_from_u8_color( u8 r, u8 g, u8 b, u8 a ) {
    vec4 result;
    f32 normalize = 1.0f / 255.0f;
    result.r = r * normalize;
    result.g = g * normalize;
    result.b = b * normalize;
    result.a = a * normalize;
    return result;
}

stu vec4 v4_srgb_to_linear_precise( vec4 srgb ) {
    vec4 linear = v4( srgb_to_linear_precise( srgb.x ), srgb_to_linear_precise( srgb.y ), srgb_to_linear_precise( srgb.z ), srgb.a );
    return linear;
}

stu vec4 v4_srgb_to_linear( vec4 srgb ) {
    vec4 linear = v4( srgb_to_linear( srgb.x ), srgb_to_linear( srgb.y ), srgb_to_linear( srgb.z ), srgb.a );
    return linear;
}

stu vec4 v4_srgb_to_linear_fast( vec4 srgb ) {
    vec4 linear = v4( srgb_to_linear_fast( srgb.x ), srgb_to_linear_fast( srgb.y ), srgb_to_linear_fast( srgb.z ), srgb.a );
    return linear;
}

stu vec4 v4_linear_to_srgb_precise( vec4 linear ) {
    vec4 srgb = v4( linear_to_srgb_precise( linear.x ), linear_to_srgb_precise( linear.y ), linear_to_srgb_precise( linear.z ), linear.a );
    return srgb;
}

stu vec4 v4_linear_to_srgb( vec4 linear ) {
    vec4 srgb = v4( linear_to_srgb( linear.x ), linear_to_srgb( linear.y ), linear_to_srgb( linear.z ), srgb.a );
    return srgb;
}

stu vec4 v4_linear_to_srgb_fast( vec4 linear ) {
    vec4 srgb = v4( linear_to_srgb_fast( linear.x ), linear_to_srgb_fast( linear.y ), linear_to_srgb_fast( linear.z ), srgb.a );
    return srgb;
}

stu b32 v4_are_equal( vec4 a, vec4 b ) {
    b32 result = ( a.x == b.x ) && ( a.y == b.y ) && ( a.z == b.z ) && ( a.w == b.w );
    return result;
}

stu vec2 v2_negate( vec2 v ) {
    
    v.x = -v.x;
    v.y = -v.y;
    
    return v;
}

stu vec2 v2_add( vec2 a, vec2 b ) {
    
    a.x += b.x;
    a.y += b.y;
    
    return a;
}

stu vec2i v2i_add( vec2i a, vec2i b ) {
    
    a.x += b.x;
    a.y += b.y;
    
    return a;
}

stu vec2 v2_sub( vec2 a, vec2 b ) {
    
    a.x -= b.x;
    a.y -= b.y;
    
    return a;
}

stu vec2i v2i_sub( vec2i a, vec2i b ) {
    
    a.x -= b.x;
    a.y -= b.y;
    
    return a;
}

stu vec2 v2_mul( vec2 v, f32 s ) {
    
    v.x *= s;
    v.y *= s;
    
    return v;
}

stu vec2i v2i_mul( vec2i v, s32 s ) {
    
    v.x *= s;
    v.y *= s;
    
    return v;
}

stu vec2 v2_div( vec2 v, f32 s ) {
    
    v.x /= s;
    v.y /= s;
    
    return v;
}

stu vec2i v2i_div( vec2i v, s32 s ) {
    
    v.x /= s;
    v.y /= s;
    
    return v;
}

stu vec2 v2_hadamard( vec2 a, vec2 b ) {
    
    a.x *= b.x;
    a.y *= b.y;
    
    return a;
}

stu vec2i v2i_hadamard( vec2i a, vec2i b ) {
    
    a.x *= b.x;
    a.y *= b.y;
    
    return a;
}

stu void v2_normalize( vec2* v ) {
    f32 length = sqrtf( v->x * v->x + v->y * v->y );
    v->x = v->x / length;
    v->y = v->y / length;
}

stu vec2 v2_normalized( vec2 v ) {
    vec2 result = v;
    v2_normalize( &result );
    return result;
}

stu void v2_normalize_fast( vec2* v ) {
    f32 rsqrt = math_rsqrt( v->x * v->x + v->y * v->y );
    v->x = v->x * rsqrt;
    v->y = v->y * rsqrt;
}

stu vec2 v2_normalized_fast( vec2 v ) {
    vec2 result = v;
    v2_normalize_fast( &result );
    return result;
}

stu f32 v2_length( vec2 v ) {
    
    f32 result = sqrtf( v.x * v.x + v.y * v.y );
    
    return result;
}

stu f32 v2_length_squared( vec2 v ) {
    
    f32 result = v.x * v.x + v.y * v.y;
    
    return result;
}

stu f32 v2_dot( vec2 a, vec2 b ) {
    
    f32 result = a.x * b.x + a.y * b.y;
    
    return result;
}

stu vec2 v2_perp( vec2 v ) {
    
    vec2 result;
    result.x = -v.y;
    result.y = v.x;
    
    return result;
}

stu vec2 v2_perp2( vec2 v, vec2 direction ) {
    
    vec2 result;
    result.x = -v.y;
    result.y = v.x;
    
    if ( v2_dot( result, direction ) < 0 ) {
        result.x = -result.x;
        result.y = -result.y;
    }
    
    return result;
}

stu b32 v2_equals( vec2 a, vec2 b, f32 epsilon ) {
    
    f32 x = ( a.x - b.x );
    f32 y = ( a.y - b.y );
    f32 e = epsilon * epsilon;
    b32 result = ( x * x  <= e ) && ( y * y <= e );
    
    return result;
}

/* NOTE simon: It seems (need to verify) that the 2d crossproduct is |a|*|b|*sin( angle ) with is similar to dot product, but with sin instead of cos. */
/* NOTE simon: 2d cross product is also the determinant of a matrix:
| a.x, b.x |
| b.y, b.y |
*/
/* NOTE simon: The 2d cross product between a and b is equal to the dot product of the perpendicular of a with b.
The result indicate if b ( 0 -> b ) is on the left (positive) or on the right (negative) of a ( 0 -> a ). */
stu f32 v2_cross( vec2 a, vec2 b ) {
    f32 result = -a.y * b.x + a.x * b.y;
    return result;
}

/* NOTE simon: The range test for the right part was changed from >= to >. I should test that is
all code... r2_contains does a similar thing so it make more sens the way it is now. */
stu b32 v2_contains( vec2 range, f32 p ) {
    b32 result = ( range.left <= p && range.right > p );
    return result;
}

stu b32 v2_contains_v2( vec2 contains, vec2 is_contained ) {
    b32 result = ( contains.left <= is_contained.left && contains.right > is_contained.right );
    return result;
}

stu b32 v2_overlap( vec2 a, f32 left, f32 right ) {
    b32 result = ( a.left < right && a.right > left );
    return result;
}

stu b32 v2_overlap_v2( vec2 a, vec2 b ) {
    b32 result = ( a.left < b.right && a.right > b.left );
    return result;
}

stu vec2 v2_lerp( vec2 a, vec2 b, f32 c ) {
    
    vec2 result = v2_add( v2_mul( a, 1.0f - c ), v2_mul( b, c ) );
    
    return result;
}

stu vec2 v2_min( vec2 a, vec2 b ) {
    
    vec2 result;
    result.x = ( ( a.x < b.x ) ? a.x : b.x );
    result.y = ( ( a.y < b.y ) ? a.y : b.y );
    
    return result;
}

stu vec2 v2_max( vec2 a, vec2 b ) {
    
    vec2 result;
    result.x = ( ( a.x > b.x ) ? a.x : b.x );
    result.y = ( ( a.y > b.y ) ? a.y : b.y );
    
    return result;
}

stu vec3 v3_sub( vec3 a, vec3 b ) {
    
    vec3 result;
    result.x = a.x - b.x;
    result.y = a.x - b.y;
    result.z = a.z - b.z;
    
    return result;
}

stu vec3 v3_mul( vec3 a, f32 s ) {
    
    a.x *= s;
    a.y *= s;
    a.z *= s;
    
    return a;
}

stu vec3 v3_normalized( vec3 v ) {
    
    f32 length = sqrtf( v.x * v.x + v.y * v.y + v.z * v.z );
    v.x = v.x / length;
    v.y = v.y / length;
    v.z = v.z / length;
    
    return v;
}

stu vec3 v3_normalized_fast( vec3 v ) {
    
    f32 rsqrt = math_rsqrt( v.x * v.x + v.y * v.y + v.z * v.z );
    v.x = v.x * rsqrt;
    v.y = v.y * rsqrt;
    v.z = v.z * rsqrt;
    
    return v;
}

stu vec3 v3_cross( vec3 a, vec3 b )  {
    
    vec3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    
    return result;
}

stu vec4 v4_hadamard( vec4 a, vec4 b ) {
    
    a.x *= b.x;
    a.y *= b.y;
    a.z *= b.z;
    a.w *= b.w;
    
    return a;
}

stu vec4 v4_mul( vec4 a, f32 s ) {
    
    a.x *= s;
    a.y *= s;
    a.z *= s;
    a.w *= s;
    
    return a;
}

stu vec4 v4_lerp( vec4 a, vec4 b, f32 c ) {
    
    vec4 result = v4_add( v4_mul( a, 1.0f - c ), v4_mul( b, c ) );
    
    return result;
}

typedef union rect2_t {
    
    struct {
        f32 left;
        f32 bottom;
        f32 right;
        f32 top;
    };
    
    struct {
        vec2 min;
        vec2 max;
    };
    
    f32 e[ 4 ];
} rect2_t;

stu void r2_zero( rect2_t* rect ) {
    
    for ( umm i = 0; i < array_count( rect->e ); i++ ) {
        rect->e[ i ] = 0.0f;
    }
}

stu rect2_t r2_min_size( f32 x, f32 y, f32 w, f32 h ) {
    
    rect2_t result;
    result.min.x = x;
    result.min.y = y;
    result.max.x = x + w;
    result.max.y = y + h;
    
    return result;
}

stu rect2_t r2_min_size_s32( s32 x, s32 y, s32 w, s32 h ) {
    
    rect2_t result;
    result.min.x = ( f32 ) x;
    result.min.y = ( f32 ) y;
    result.max.x = ( f32 ) ( x + w );
    result.max.y = ( f32 ) ( y + h );
    
    return result;
}

stu rect2_t r2_min_size_v( vec2 min, vec2 size ) {
    
    rect2_t result;
    result.min = min;
    result.max.x = min.x + size.x;
    result.max.y = min.y + size.y;
    
    return result;
}

stu rect2_t r2_min_max( f32 minX, f32 minY, f32 maxX, f32 maxY ) {
    
    rect2_t result;
    result.min.x = minX;
    result.min.y = minY;
    result.max.x = maxX;
    result.max.y = maxY;
    
    return result;
}

stu rect2_t r2_min_max_v( vec2 min, vec2 max ) {
    
    rect2_t result;
    result.min = min;
    result.max = max;
    
    return result;
}

stu rect2_t r2_top_left_size( f32 left, f32 top, f32 width, f32 height ) {
    
    rect2_t result;
    result.left = left;
    result.top = top;
    result.right = left + width;
    result.bottom = top - height;
    
    return result;
}

stu rect2_t r2_center_size( f32 cx, f32 cy, f32 w, f32 h ) {
    
    rect2_t result;
    result.min.x = cx - w * 0.5f;
    result.min.y = cy - h * 0.5f;
    result.max.x = cx + w * 0.5f;
    result.max.y = cy + h * 0.5f;
    
    return result;
}

stu rect2_t r2_center_size_v( vec2 center, vec2 size ) {
    rect2_t result = r2_center_size( center.x, center.y, size.x, size.y );
    return result;
}

stu rect2_t r2_center_radius( f32 cx, f32 cy, f32 rw, f32 rh ) {
    
    rect2_t result;
    result.min.x = cx - rw;
    result.min.y = cy - rh;
    result.max.x = cx + rw;
    result.max.y = cy + rh;
    
    return result;
}

stu rect2_t r2_center_radius_v( vec2 center, vec2 radius ) {
    rect2_t result = r2_center_radius( center.x, center.y, radius.x, radius.y );
    return result;
}

stu vec2 r2_get_size( rect2_t rect ) {
    
    vec2 result = v2_sub( rect.max, rect.min );
    
    return result;
}

stu f32 r2_get_width( rect2_t rect ) {
    
    f32 result = rect.max.x - rect.min.x;
    return result;
}

stu void r2_set_width( rect2_t* rect, f32 width ) {
    rect->max.x = rect->min.x + width;
}

stu void r2_add_width( rect2_t* rect, f32 width ) {
    rect->max.x += width;
}

stu f32 r2_get_height( rect2_t rect ) {
    
    f32 result = rect.max.y - rect.min.y;
    return result;
}

stu void r2_set_height( rect2_t* rect, f32 height ) {
    rect->max.y = rect->min.y + height;
}

stu void r2_add_height( rect2_t* rect, f32 height ) {
    rect->max.y += height;
}

stu void r2_sub_radius( rect2_t* rect, f32 width, f32 height ) {
    rect->min.x += width;
    rect->min.y += height;
    rect->max.x -= width;
    rect->max.y -= height;
}

stu rect2_t r2_copy_sub_radius( rect2_t rect, f32 width, f32 height ) {
    rect2_t result = rect;
    r2_sub_radius( &result, width, height );
    return result;
}

stu void r2_add_radius( rect2_t* rect, f32 width, f32 height ) {
    rect->min.x -= width;
    rect->min.y -= height;
    rect->max.x += width;
    rect->max.y += height;
}

stu rect2_t r2_copy_add_radius( rect2_t rect, f32 width, f32 height ) {
    rect2_t result = rect;
    r2_add_radius( &result, width, height );
    return result;
}

stu void r2_add_border( rect2_t* rect, vec4 border ) {
    rect->min.x -= border.left;
    rect->min.y -= border.bottom;
    rect->max.x += border.right;
    rect->max.y += border.top;
}

stu rect2_t r2_copy_add_border( rect2_t rect, vec4 border ) {
    rect2_t result = rect;
    r2_add_border( &result, border );
    return result;
}

stu void r2_sub_border( rect2_t* rect, vec4 border ) {
    rect->min.x += border.left;
    rect->min.y += border.bottom;
    rect->max.x -= border.right;
    rect->max.y -= border.top;
}

stu rect2_t r2_copy_sub_border( rect2_t rect, vec4 border ) {
    rect2_t result = rect;
    r2_sub_border( &result, border );
    return result;
}

stu void r2_translate( rect2_t* rect, f32 x_offset, f32 y_offset ) {
    rect->min.x += x_offset;
    rect->max.x += x_offset;
    rect->min.y += y_offset;
    rect->max.y += y_offset;
}

stu rect2_t r2_translated( rect2_t rect, f32 x_offset, f32 y_offset ) {
    rect2_t result = rect;
    r2_translate( &result, x_offset, y_offset );
    return result;
}

stu void r2_translate_x( rect2_t* rect, f32 x_offset ) {
    rect->min.x += x_offset;
    rect->max.x += x_offset;
}

stu void r2_translate_y( rect2_t* rect, f32 y_offset ) {
    rect->min.y += y_offset;
    rect->max.y += y_offset;
}

stu b32 r2_contains_v2( rect2_t rect, vec2 point ) {
    
    b32 result = point.x >= rect.left && point.x < rect.right && point.y >= rect.bottom && point.y < rect.top;
    return result;
}

stu b32 r2_are_equal( rect2_t a, rect2_t b ) {
    b32 result = ( a.left == b.left ) && ( a.right == b.right ) && ( a.top == b.top ) && ( a.bottom == b.bottom );
    return result;
}

stu b32 r2_contains_r2( rect2_t container, rect2_t contained ) {
    
    b32 result = ( container.min.x <= contained.min.x
                  && container.max.x >= contained.max.x
                  && container.min.y <= contained.min.y
                  && container.max.y >= contained.max.y );
    return result;
}

stu b32 r2_overlap( rect2_t a, rect2_t b ) {
    
    b32 vertical = ( b.min.x < a.max.x && b.max.x > a.min.x );
    b32 horizontal = ( b.min.y < a.max.y && b.max.y > a.min.y );
    b32 result = vertical && horizontal;
    
    return result;
}

stu rect2_t r2_intersection( rect2_t a, rect2_t b ) {
    
    rect2_t result = { 0 };
    
    if ( r2_overlap( a, b ) ) {
        result.left = math_max( a.left, b.left );
        result.right = math_min( a.right, b.right );
        result.bottom = math_max( a.bottom, b.bottom );
        result.top = math_min( a.top, b.top );
    }
    
    return result;
}

stu b32 r2_is_zero( rect2_t rect ) {
    
    b32 result = rect.left == 0;
    result = result && rect.right == 0;
    result = result && rect.top == 0;
    result = result && rect.bottom == 0;
    
    return result;
}

stu rect2_t r2_centered_size( rect2_t container, vec2 size ) {
    f32 x_offset = ( ( container.right - container.left ) - size.x ) * 0.5f;
    f32 y_offset = ( ( container.top - container.bottom ) - size.y ) * 0.5f;
    rect2_t result = r2_min_size( container.left + x_offset, container.bottom + y_offset, size.x, size.y );
    return result;
}

stu vec2 r2_get_top_left( rect2_t rect ) {
    vec2 result = { rect.left, rect.top };
    return result;
}

typedef union rect2i_t {
    
    struct {
        s32 left;
        s32 bottom;
        s32 right;
        s32 top;
    };
    
    struct {
        vec2i min;
        vec2i max;
    };
    
    s32 e[ 4 ];
    
} rect2i_t;

stu void debug_r2i( data_t* out, rect2i_t rect ) {
    memory_push_copy_l( out, "left: " );
    string_push_s64( out, rect.left, 4, ' ', 0 );
    memory_push_copy_l( out, " bott: " );
    string_push_s64( out, rect.bottom, 4, ' ', 0 );
    memory_push_copy_l( out, " righ: " );
    string_push_s64( out, rect.right, 4, ' ', 0 );
    memory_push_copy_l( out, " top : " );
    string_push_s64( out, rect.top, 4, ' ', 0 );
}

stu rect2i_t r2i_min_size( s32 x, s32 y, s32 width, s32 height ) {
    rect2i_t rect = { x, y, x + width, y + height };
    return rect;
}

stu rect2i_t r2i_top_left_size( s32 left, s32 top, s32 width, s32 height ) {
    
    rect2i_t result;
    result.left = left;
    result.top = top;
    result.right = left + width;
    result.bottom = top - height;
    
    return result;
}

stu b32 r2i_are_equal( rect2i_t a, rect2i_t b ) {
    b32 result = ( a.left == b.left ) && ( a.right == b.right ) && ( a.top == b.top ) && ( a.bottom == b.bottom );
    return result;
}

stu b32 r2i_contains_v2i( rect2i_t rect, vec2i point ) {
    
    b32 result = point.x >= rect.left && point.x < rect.right && point.y >= rect.bottom && point.y < rect.top;
    return result;
}

stu b32 r2i_overlap( rect2i_t a, rect2i_t b ) {
    
    b32 horizontal = a.right > b.left && a.left < b.right;
    b32 vertical = a.top > b.bottom && a.bottom < b.top;
    b32 result = horizontal && vertical;
    
    return result;
}

stu rect2i_t r2i_intersection( rect2i_t a, rect2i_t b ) {
    
    rect2i_t result = { 0 };
    
    if ( r2i_overlap( a, b ) ) {
        result.left = math_max( a.left, b.left );
        result.right = math_min( a.right, b.right );
        result.bottom = math_max( a.bottom, b.bottom );
        result.top = math_min( a.top, b.top );
    }
    
    return result;
}

stu void r2i_sub_radius( rect2i_t* rect, s32 width, s32 height ) {
    rect->left += width;
    rect->right -= width;
    rect->top -= height;
    rect->bottom += height;
}

stu rect2i_t r2i_copy_sub_radius( rect2i_t rect, s32 width, s32 height ) {
    rect2i_t result = rect;
    r2i_sub_radius( &result, width, height );
    return result;
}

stu void r2i_sub_border( rect2i_t* rect, vec4i border ) {
    rect->left += border.left;
    rect->right -= border.right;
    rect->top -= border.top;
    rect->bottom += border.bottom;
}

stu rect2i_t r2i_copy_sub_border( rect2i_t rect, vec4i border ) {
    rect2i_t result = rect;
    r2i_sub_border( &result, border );
    return result;
}

stu s32 r2i_get_width( rect2i_t rect ) {
    
    s32 result = rect.right - rect.left;
    return result;
}

stu s32 r2i_get_height( rect2i_t rect ) {
    
    s32 result = rect.top - rect.bottom;
    return result;
}

stu vec2i r2i_get_top_left( rect2i_t rect ) {
    vec2i result = { rect.left, rect.top };
    return result;
}

stu rect2_t r2_from_r2i( rect2i_t a ) {
    
    rect2_t result = { 0 };
    result.left = ( f32 ) a.left;
    result.right = ( f32 ) a.right;
    result.top = ( f32 ) a.top;
    result.bottom = ( f32 ) a.bottom;
    
    return result;
}

stu b32 r2i_is_zero( rect2i_t rect ) {
    
    b32 result = rect.left == 0;
    result = result && rect.right == 0;
    result = result && rect.top == 0;
    result = result && rect.bottom == 0;
    
    return result;
}

f32 screen_get_width( f32 height, f32 window_width, f32 window_height ) {
    f32 ratio = window_width / window_height;
    f32 width = ratio * height;
    return width;
}

f32 screen_get_height( f32 width, f32 window_width, f32 window_height ) {
    f32 ratio = window_height / window_width;
    f32 height = ratio * width;
    return height;
}

// MARK matrix

/* NOTE simon: OpenGL matrix should be column first in memory (column major).
OpenGL expect things to be transformed into Normalized Device Coordinates (NDC).
NDC a -1 to 1 left handed cube (z -1 is toward the viewer, z +1 is toward the screen).
But it can be configured to use 0 to 1 on z using ARB_clip_control. */
/* TODO simon: look at https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_clip_control.txt */

/* NOTE simon: Matrix order is important. When calling multiply, you need to put the new
transformation in the lhs and the result of previous operation in the rhs.

For example to have a camera looking at X, from a point P and transforming into clip space:
- ( C ) transforming in clip space is the last transformation that needs to happen;
- ( T ) moving the camera at P is the first things that needs to happen;
- ( R ) rotating the camera to look at the X needs to happen after T;

so the desired order is TRC it should be executed in the oposite order CRT * P:
- mul( R, T, RT )
- mul( C, RT, CRT )
- mul( CRT, P )

Matrix multiplication is associative, (AB)C = A(BC). In our case it allows to combine
transformations together and multiply them with clip space matrix at the end.

Matrix A(m,n) : m rows, n colmn.
To be able to multiply two matrices, the number of column from the lhs matrix needs to be equal to
the number of row from the rhs matrix.

== Properties of matrix multiplication ==
In this table, A, B, and C are n*n matrices, I is the n*n identity matrix, and O is the n*n zero matrix

The commutative property of multiplication does not hold!
--- AB!=BA

Associative property of multiplication
--- (AB)C=A(BC)

Distributive properties
--- A(B+C)=AB+AC
--- (B+C)A=BA+CA

Multiplicative identity property
--- IA=A
--- AI=A

Multiplicative property of zero
--- OA=O
--- AO=O

Dimension property: the product of an m*n matrix and an n*k matrix is an m*k matrix.
*/

stu void matrix_create_identity3( f32* mat ) {
    mat[ 0 ] = 1.0f; mat[ 3 ] = 0.0f; mat[ 6 ] = 0.0f;
    mat[ 1 ] = 0.0f; mat[ 4 ] = 1.0f; mat[ 7 ] = 0.0f;
    mat[ 2 ] = 0.0f; mat[ 5 ] = 0.0f; mat[ 8 ] = 1.0f;
}

stu void matrix_create_identity( f32* mat ) {
    
    mat[ 0 ] = 1.0f; mat[ 4 ] = 0.0f; mat[ 8 ] = 0.0f; mat[ 12 ] = 0.0f;
    mat[ 1 ] = 0.0f; mat[ 5 ] = 1.0f; mat[ 9 ] = 0.0f; mat[ 13 ] = 0.0f;
    mat[ 2 ] = 0.0f; mat[ 6 ] = 0.0f; mat[ 10 ] = 1.0f; mat[ 14 ] = 0.0f;
    mat[ 3 ] = 0.0f; mat[ 7 ] = 0.0f; mat[ 11 ] = 0.0f; mat[ 15 ] = 1.0f;
}

stu void matrix_create_screen_space3( f32* mat, s32 screenWidth, s32 screenHeight ) {
    
    mat[ 0 ] = 2.0f / screenWidth;
    mat[ 1 ] = 0.0f;
    mat[ 2 ] = -1.0f;
    
    mat[ 3 ] = 0.0f;
    mat[ 4 ] = 2.0f / screenHeight;
    mat[ 5 ] = -1.0f;
    
    mat[ 6 ] = 0.0f;
    mat[ 7 ] = 0.0f;
    mat[ 8 ] = 1.0f;
}

stu void matrix_create_screen_space( f32* mat, s32 screen_width, s32 screen_height ) {
    
    mat[ 0 ] = 2.0f / screen_width;
    mat[ 1 ] = 0.0f;
    mat[ 2 ] = 0.0f;
    mat[ 3 ] = 0.0f;
    
    mat[ 4 ] = 0.0f;
    mat[ 5 ] = 2.0f / screen_height;
    mat[ 6 ] = 0.0f;
    mat[ 7 ] = 0.0f;
    
    mat[ 8 ] = 0.0f;
    mat[ 9 ] = 0.0f;
    mat[ 10 ] = 1.0f;
    mat[ 11 ] = 0.0f;
    
    mat[ 12 ] = -1.0f;
    mat[ 13 ] = -1.0f;
    mat[ 14 ] = 0.0f;
    mat[ 15 ] = 1.0f;
}

stu void matrix_create_screen_space_top_down3( f32* mat, s32 screenWidth, s32 screenHeight ) {
    
    mat[ 0 ] = 2.0f / screenWidth;
    mat[ 1 ] = 0.0f;
    mat[ 2 ] = -1.0f;
    
    mat[ 3 ] = 0.0f;
    mat[ 4 ] = -2.0f / screenHeight;
    mat[ 5 ] = 1.0f;
    
    mat[ 6 ] = 0.0f;
    mat[ 7 ] = 0.0f;
    mat[ 8 ] = 1.0f;
}

stu void matrix_create_screen_space_top_down( f32* mat, s32 screen_width, s32 screen_height ) {
    
    mat[ 0 ] = 2.0f / screen_width;
    mat[ 1 ] = 0.0f;
    mat[ 2 ] = 0.0f;
    mat[ 3 ] = -1.0f;
    
    mat[ 4 ] = 0.0f;
    mat[ 5 ] = -2.0f / screen_height;
    mat[ 6 ] = 0.0f;
    mat[ 7 ] = 1.0f;
    
    mat[ 8 ] = 0.0f;
    mat[ 9 ] = 0.0f;
    mat[ 10 ] = 1.0f;
    mat[ 11 ] = 0.0f;
    
    mat[ 12 ] = 0.0f;
    mat[ 13 ] = 0.0f;
    mat[ 14 ] = 0.0f;
    mat[ 15 ] = 1.0f;
}

/* TODO simon: Why is matrix a void* instead of f32* ? */
stu void matrix_projection_orthographic( void* matrix, f32 width, f32 height, f32 near_plane, f32 far_plane ) {
    
    f32* mat = cast( f32*, matrix );
    
    mat[ 0 ] = 2.0f / width;
    mat[ 1 ] = 0.0f;
    mat[ 2 ] = 0.0f;
    mat[ 3 ] = 0.0f;
    
    mat[ 4 ] = 0.0f;
    mat[ 5 ] = 2.0f / height;
    mat[ 6 ] = 0.0f;
    mat[ 7 ] = 0.0f;
    
    mat[ 8 ] = 0.0f;
    mat[ 9 ] = 0.0f;
    mat[ 10 ] = -2.0f / ( far_plane - near_plane );
    mat[ 11 ] = 0.0f;
    
    mat[ 12 ] = 0.0f;
    mat[ 13 ] = 0.0f;
    mat[ 14 ] = -( ( far_plane + near_plane ) / ( far_plane - near_plane ) );
    mat[ 15 ] = 1.0f;
}

/* NOTE simon: look at https://developer.nvidia.com/content/depth-precision-visualized */
/* NOTE simon: http://www.humus.name/index.php?ID=255 */
/* TODO simon: use ARB_clip_control, floating point depth buffer and reverse Z order ( 1 is near and 0 is far ). */
stu void matrix_projection_perspective( void* matrix, f32 horizontal_fov, f32 aspect_ratio, f32 near_plane, f32 far_plane ) {
    
    f32* mat = cast( f32*, matrix );
    f32 width = tanf( horizontal_fov * 0.5f );
    
    mat[ 0 ] = 1 / width;
    mat[ 1 ] = 0.0f;
    mat[ 2 ] = 0.0f;
    mat[ 3 ] = 0.0f;
    
    mat[ 4 ] = 0.0f;
    mat[ 5 ] = aspect_ratio / width;
    mat[ 6 ] = 0.0f;
    mat[ 7 ] = 0.0f;
    
    mat[ 8 ] = 0.0f;
    mat[ 9 ] = 0.0f;
    mat[ 10 ] = - ( far_plane + near_plane ) / ( far_plane - near_plane );
    mat[ 11 ] = -1.0f;
    
    mat[ 12 ] = 0.0f;
    mat[ 13 ] = 0.0f;
    mat[ 14 ] = -( 2 * far_plane * near_plane ) / ( far_plane - near_plane );
    mat[ 15 ] = 0.0f;
}

stu void matrix_create_translation3( f32* mat, f32 x, f32 y ) {
    
    mat[ 0 ] = 1.0f;
    mat[ 1 ] = 0.0f;
    mat[ 2 ] = x;
    
    mat[ 3 ] = 0.0f;
    mat[ 4 ] = 1.0f;
    mat[ 5 ] = y;
    
    mat[ 6 ] = 0.0f;
    mat[ 7 ] = 0.0f;
    mat[ 8 ] = 1.0f;
}

stu void matrix_create_translation3_vec2( f32* mat, f32* v ) {
    matrix_create_translation3( mat, v[ 0 ], v[ 1 ] );
}

stu void matrix_create_translation( void* matrix, f32 x, f32 y, f32 z ) {
    
    f32* mat = cast( f32*, matrix );
    
    mat[ 0 ] = 1.0f;
    mat[ 1 ] = 0.0f;
    mat[ 2 ] = 0.0f;
    mat[ 3 ] = 0.0f;
    
    mat[ 4 ] = 0.0f;
    mat[ 5 ] = 1.0f;
    mat[ 6 ] = 0.0f;
    mat[ 7 ] = 0.0f;
    
    mat[ 8 ] = 0.0f;
    mat[ 9 ] = 0.0f;
    mat[ 10 ] = 1.0f;
    mat[ 11 ] = 0.0f;
    
    mat[ 12 ] = x;
    mat[ 13 ] = y;
    mat[ 14 ] = z;
    mat[ 15 ] = 1.0f;
}

stu void matrix_create_translation_vec3( f32* mat, f32* v ) {
    matrix_create_translation( mat, v[ 0 ], v[ 1 ], v[ 2 ] );
}

stu void matrix_create_scale3( f32* mat, f32 x, f32 y ) {
    
    mat[ 0 ] = x;
    mat[ 1 ] = 0.0f;
    mat[ 2 ] = 0.0f;
    
    mat[ 3 ] = 0.0f;
    mat[ 4 ] = y;
    mat[ 5 ] = 0.0f;
    
    mat[ 6 ] = 0.0f;
    mat[ 7 ] = 0.0f;
    mat[ 8 ] = 1.0f;
}

stu void matrix_create_scale3_vec2( f32* mat, f32* v ) {
    matrix_create_scale3( mat, v[ 0 ], v[ 1 ] );
}

stu void matrix_create_scale( f32* mat, f32 x, f32 y, f32 z ) {
    
    mat[ 0 ] = x;
    mat[ 1 ] = 0.0f;
    mat[ 2 ] = 0.0f;
    mat[ 3 ] = 0.0f;
    
    mat[ 4 ] = 0.0f;
    mat[ 5 ] = y;
    mat[ 6 ] = 0.0f;
    mat[ 7 ] = 0.0f;
    
    mat[ 8 ] = 0.0f;
    mat[ 9 ] = 0.0f;
    mat[ 10 ] = z;
    mat[ 11 ] = 0.0f;
    
    mat[ 12 ] = 0.0f;
    mat[ 13 ] = 0.0f;
    mat[ 14 ] = 0.0f;
    mat[ 15 ] = 1.0f;
}

stu void matrix_create_scale_vec3( f32* mat, f32* v ) {
    matrix_create_scale( mat, v[ 0 ], v[ 1 ], v[ 2 ] );
}

stu void matrix_create( f32* mat, vec3 x, vec3 y, vec3 z, vec3 t ) {
    
    mat[ 0 ] = x.x;
    mat[ 1 ] = y.x;
    mat[ 2 ] = z.x;
    mat[ 3 ] = t.x;
    
    mat[ 4 ] = x.y;
    mat[ 5 ] = y.y;
    mat[ 6 ] = z.y;
    mat[ 7 ] = t.y;
    
    mat[ 8 ] = x.z;
    mat[ 9 ] = x.z;
    mat[ 10 ] = x.z;
    mat[ 11 ] = t.z;
    
    mat[ 12 ] = 0.0f;
    mat[ 13 ] = 0.0f;
    mat[ 14 ] = 0.0f;
    mat[ 15 ] = 1.0f;
}

stu void matrix_look_at( f32* mat, vec3 position, vec3 target, vec3 up_vector ) {
    
    vec3 z = v3_normalized( v3_sub( target, position ) );
    vec3 x = v3_normalized( v3_cross( up_vector, z ) );
    vec3 y = v3_cross( z, x );
    
    matrix_create( mat, x, y, z, v3( 0, 0, 0 ) );
}

stu void matrix_rotation_x( f32* mat, f32 degree ) {
    
    f32 radian = degree * math_degree_to_radian;
    f32 c = cosf( radian );
    f32 s = sinf( radian );
    
    mat[ 0 ] = 1; mat[ 4 ] = 0; mat[ 8 ] = 0; mat[ 12 ] = 0;
    mat[ 1 ] = 0; mat[ 5 ] = c; mat[ 9 ] = -s; mat[ 13 ] = 0;
    mat[ 2 ] = 0; mat[ 6 ] = s; mat[ 10 ] = c; mat[ 14 ] = 0;
    mat[ 3 ] = 0; mat[ 7 ] = 0; mat[ 11 ] = 0; mat[ 15 ] = 1;
}

stu void matrix_rotation_y( f32* mat, f32 degree ) {
    
    f32 radian = degree * math_degree_to_radian;
    f32 c = cosf( radian );
    f32 s = sinf( radian );
    
    mat[ 0 ] = c; mat[ 4 ] = 0; mat[ 8 ] = s; mat[ 12 ] = 0;
    mat[ 1 ] = 0; mat[ 5 ] = 1; mat[ 9 ] = 0; mat[ 13 ] = 0;
    mat[ 2 ] = -s; mat[ 6 ] = 0; mat[ 10 ] = c; mat[ 14 ] = 0;
    mat[ 3 ] = 0; mat[ 7 ] = 0; mat[ 11 ] = 0; mat[ 15 ] = 1;
}

stu void matrix_rotation_z( f32* mat, f32 degree ) {
    
    f32 radian = degree * math_degree_to_radian;
    f32 c = cosf( radian );
    f32 s = sinf( radian );
    
    mat[ 0 ] = c; mat[ 4 ] = -s; mat[ 8 ] = 0; mat[ 12 ] = 0;
    mat[ 1 ] = s; mat[ 5 ] = c; mat[ 9 ] = 0; mat[ 13 ] = 0;
    mat[ 2 ] = 0; mat[ 6 ] = 0; mat[ 10 ] = 1; mat[ 14 ] = 0;
    mat[ 3 ] = 0; mat[ 7 ] = 0; mat[ 11 ] = 0; mat[ 15 ] = 1;
}

stu void matrix_multiply_square( umm dimention, f32* lhs, f32* rhs, f32* result ) {
    
#define lhs( x, y ) ( lhs[ x * dimention + y ] )
#define rhs( x, y ) ( rhs[ x * dimention + y ] )
#define result_access( x, y ) ( result[ x * dimention + y ] )
    
    
    for ( umm x = 0; x < dimention; x++ ) {
        for ( umm y = 0; y < dimention; y++ ) {
            
            result_access( x, y ) = 0;
            
            for ( umm i = 0; i < dimention; i++ ) {
                result_access( x, y ) += lhs( i, y ) * rhs( x, i );
            }
        }
    }
    
#undef lhs
#undef rhs
#undef result_access
}

stu void matrix_multiply3( f32* lhs, f32* rhs, f32* result ) {
    matrix_multiply_square( 3, lhs, rhs, result );
}

stu void matrix_multiply( f32* lhs, f32* rhs, f32* result ) {
    matrix_multiply_square( 4, lhs, rhs, result );
}

stu void matrix_multiply4x4( f32* lhs, f32* rhs, f32* result ) {
    
#define lhs( y, x ) ( lhs[ y * 4 + x ] )
#define rhs( y, x ) ( rhs[ y * 4 + x ] )
    result[ 0 ] = lhs( 0, 0 ) * rhs( 0, 0 ) + lhs( 1, 0 ) * rhs( 0, 1 ) + lhs( 2, 0 ) * rhs( 0, 2 ) + lhs( 3, 0 ) * rhs( 0, 3 );
    result[ 1 ] = lhs( 0, 1 ) * rhs( 0, 0 ) + lhs( 1, 1 ) * rhs( 0, 1 ) + lhs( 2, 1 ) * rhs( 0, 2 ) + lhs( 3, 1 ) * rhs( 0, 3 );
    result[ 2 ] = lhs( 0, 2 ) * rhs( 0, 0 ) + lhs( 1, 2 ) * rhs( 0, 1 ) + lhs( 2, 2 ) * rhs( 0, 2 ) + lhs( 3, 2 ) * rhs( 0, 3 );
    result[ 3 ] = lhs( 0, 3 ) * rhs( 0, 0 ) + lhs( 1, 3 ) * rhs( 0, 1 ) + lhs( 2, 3 ) * rhs( 0, 2 ) + lhs( 3, 3 ) * rhs( 0, 3 );
    
    result[ 4 ] = lhs( 0, 0 ) * rhs( 1, 0 ) + lhs( 1, 0 ) * rhs( 1, 1 ) + lhs( 2, 0 ) * rhs( 1, 2 ) + lhs( 3, 0 ) * rhs( 1, 3 );
    result[ 5 ] = lhs( 0, 1 ) * rhs( 1, 0 ) + lhs( 1, 1 ) * rhs( 1, 1 ) + lhs( 2, 1 ) * rhs( 1, 2 ) + lhs( 3, 1 ) * rhs( 1, 3 );
    result[ 6 ] = lhs( 0, 2 ) * rhs( 1, 0 ) + lhs( 1, 2 ) * rhs( 1, 1 ) + lhs( 2, 2 ) * rhs( 1, 2 ) + lhs( 3, 2 ) * rhs( 1, 3 );
    result[ 7 ] = lhs( 0, 3 ) * rhs( 1, 0 ) + lhs( 1, 3 ) * rhs( 1, 1 ) + lhs( 2, 3 ) * rhs( 1, 2 ) + lhs( 3, 3 ) * rhs( 1, 3 );
    
    result[ 8 ] = lhs( 0, 0 ) * rhs( 2, 0 ) + lhs( 1, 0 ) * rhs( 2, 1 ) + lhs( 2, 0 ) * rhs( 2, 2 ) + lhs( 3, 0 ) * rhs( 2, 3 );
    result[ 9 ] = lhs( 0, 1 ) * rhs( 2, 0 ) + lhs( 1, 1 ) * rhs( 2, 1 ) + lhs( 2, 1 ) * rhs( 2, 2 ) + lhs( 3, 1 ) * rhs( 2, 3 );
    result[ 10 ] = lhs( 0, 2 ) * rhs( 2, 0 ) + lhs( 1, 2 ) * rhs( 2, 1 ) + lhs( 2, 2 ) * rhs( 2, 2 ) + lhs( 3, 2 ) * rhs( 2, 3 );
    result[ 11 ] = lhs( 0, 3 ) * rhs( 2, 0 ) + lhs( 1, 3 ) * rhs( 2, 1 ) + lhs( 2, 3 ) * rhs( 2, 2 ) + lhs( 3, 3 ) * rhs( 2, 3 );
    
    result[ 12 ] = lhs( 0, 0 ) * rhs( 3, 0 ) + lhs( 1, 0 ) * rhs( 3, 1 ) + lhs( 2, 0 ) * rhs( 3, 2 ) + lhs( 3, 0 ) * rhs( 3, 3 );
    result[ 13 ] = lhs( 0, 1 ) * rhs( 3, 0 ) + lhs( 1, 1 ) * rhs( 3, 1 ) + lhs( 2, 1 ) * rhs( 3, 2 ) + lhs( 3, 1 ) * rhs( 3, 3 );
    result[ 14 ] = lhs( 0, 2 ) * rhs( 3, 0 ) + lhs( 1, 2 ) * rhs( 3, 1 ) + lhs( 2, 2 ) * rhs( 3, 2 ) + lhs( 3, 2 ) * rhs( 3, 3 );
    result[ 15 ] = lhs( 0, 3 ) * rhs( 3, 0 ) + lhs( 1, 3 ) * rhs( 3, 1 ) + lhs( 2, 3 ) * rhs( 3, 2 ) + lhs( 3, 3 ) * rhs( 3, 3 );
#undef lhs
#undef rhs
}

stu void matrix_transpose3( f32* a ) {
#define matrix_a( x, y ) ( a[ x * 3 + y ] )
    swap_type( matrix_a( 1, 0 ), matrix_a( 0, 1 ), f32 );
    swap_type( matrix_a( 2, 0 ), matrix_a( 0, 2 ), f32 );
    swap_type( matrix_a( 2, 1 ), matrix_a( 1, 2 ), f32 );
#undef matrix_a
}

stu void matrix_transpose( f32* a ) {
#define matrix_a( x, y ) ( a[ x * 4 + y ] )
    swap_type( matrix_a( 1, 0 ), matrix_a( 0, 1 ), f32 );
    swap_type( matrix_a( 2, 0 ), matrix_a( 0, 2 ), f32 );
    swap_type( matrix_a( 3, 0 ), matrix_a( 0, 3 ), f32 );
    swap_type( matrix_a( 2, 1 ), matrix_a( 1, 2 ), f32 );
    swap_type( matrix_a( 3, 1 ), matrix_a( 1, 3 ), f32 );
    swap_type( matrix_a( 3, 2 ), matrix_a( 2, 3 ), f32 );
#undef matrix_a
}

stu vec2 m4_mul_v2( f32* lhs, f32* rhs ) {
    vec2 result;
    result.x = lhs[ 0 ] * rhs[ 0 ] + lhs[ 4 ] * rhs[ 1 ] + lhs[ 12 ];
    result.y = lhs[ 1 ] * rhs[ 0 ] + lhs[ 5 ] * rhs[ 1 ] + lhs[ 13 ];
    return result;
}

stu vec3 m4_mul_v3( f32* lhs, f32* rhs ) {
    vec3 result;
    result.x = lhs[ 0 ] * rhs[ 0 ] + lhs[ 4 ] * rhs[ 1 ] + lhs[ 8 ] * rhs[ 2 ] + lhs[ 12 ];
    result.y = lhs[ 1 ] * rhs[ 0 ] + lhs[ 5 ] * rhs[ 1 ] + lhs[ 9 ] * rhs[ 2 ] + lhs[ 13 ];
    result.z = lhs[ 2 ] * rhs[ 0 ] + lhs[ 6 ] * rhs[ 1 ] + lhs[ 10 ] * rhs[ 2 ] + lhs[ 14 ];
    return result;
}

stu vec4 m4_mul_v4( f32* lhs, f32* rhs ) {
    vec4 result;
    result.x = lhs[ 0 ] * rhs[ 0 ] + lhs[ 4 ] * rhs[ 1 ] + lhs[ 8 ] * rhs[ 2 ] + lhs[ 12 ] * rhs[ 3 ];
    result.y = lhs[ 1 ] * rhs[ 0 ] + lhs[ 5 ] * rhs[ 1 ] + lhs[ 9 ] * rhs[ 2 ] + lhs[ 13 ] * rhs[ 3 ];
    result.z = lhs[ 2 ] * rhs[ 0 ] + lhs[ 6 ] * rhs[ 1 ] + lhs[ 10 ] * rhs[ 2 ] + lhs[ 14 ] * rhs[ 3 ];
    result.w = lhs[ 3 ] * rhs[ 0 ] + lhs[ 7 ] * rhs[ 1 ] + lhs[ 11 ] * rhs[ 2 ] + lhs[ 15 ] * rhs[ 3 ];
    return result;
}

stu void matrix_transform_xy_mat3( f32* x, f32* y, f32* m ) {
    f32 xv = *x, yv = *y;
    ( *x ) = xv * m[ 0 ] + yv * m[ 1 ] + m[ 2 ];
    ( *y ) = xv * m[ 3 ] + yv * m[ 4 ] + m[ 5 ];
}

stu void matrix_transform_vec2_mat3( f32* v, f32* m ) {
    matrix_transform_xy_mat3( v, v + 1, m );
}

stu void matrix_multiply_vec2_mat3( f32* v, f32* m, f32* result ) {
    result[ 0 ] = v[ 0 ] * m[ 0 ] + v[ 1 ] * m[ 1 ] + m[ 2 ];
    result[ 1 ] = v[ 0 ] * m[ 3 ] + v[ 1 ] * m[ 4 ] + m[ 5 ];
}

stu void matrix_transform_xyz_mat3( f32* x, f32* y, f32* z, f32* m ) {
    f32 xv = *x, yv = *y, zv = *z;
    ( *x ) = xv * m[ 0 ] + yv * m[ 1 ] + zv * m[ 2 ];
    ( *y ) = xv * m[ 3 ] + yv * m[ 4 ] + zv * m[ 5 ];
    ( *z ) = xv * m[ 6 ] + yv * m[ 7 ] + zv * m[ 8 ];
}

stu void matrix_transform_vec3_mat3( f32* v, f32* m ) {
    matrix_transform_xyz_mat3( v, v + 1, v + 2, m );
}

stu void matrix_multiply_vec3_mat3( f32* v, f32* m, f32* result ) {
    result[ 0 ] = v[ 0 ] * m[ 0 ] + v[ 1 ] * m[ 1 ] + v[ 2 ] * m[ 2 ];
    result[ 1 ] = v[ 0 ] * m[ 3 ] + v[ 1 ] * m[ 4 ] + v[ 2 ] * m[ 5 ];
    result[ 2 ] = v[ 0 ] * m[ 6 ] + v[ 1 ] * m[ 7 ] + v[ 2 ] * m[ 8 ];
}

stu void matrix_transform_xyz( f32* x, f32* y, f32* z, f32* m ) {
    f32 xv = *x, yv = *y, zv = *z;
    ( *x ) = xv * m[ 0 ] + yv * m[ 1 ] + zv * m[ 2 ] + m[ 3 ];
    ( *y ) = xv * m[ 4 ] + yv * m[ 5 ] + zv * m[ 6 ] + m[ 7 ];
    ( *z ) = xv * m[ 8 ] + yv * m[ 9 ] + zv * m[ 10 ] + m[ 11 ];
}

stu void matrix_transform_vec3( f32* v, f32* m ) {
    matrix_transform_xyz( v, v + 1, v + 2, m );
}

stu void matrix_multiply_vec3( f32* v, f32* m, f32* result ) {
    result[ 0 ] = v[ 0 ] * m[ 0 ] + v[ 1 ] * m[ 1 ] + v[ 2 ] * m[ 2 ] + m[ 3 ];
    result[ 1 ] = v[ 0 ] * m[ 4 ] + v[ 1 ] * m[ 5 ] + v[ 2 ] * m[ 6 ] + m[ 7 ];
    result[ 2 ] = v[ 0 ] * m[ 8 ] + v[ 1 ] * m[ 9 ] + v[ 2 ] * m[ 10 ] + m[ 11 ];
}

stu void matrix_transform_xyzw( f32* x, f32* y, f32* z, f32* w, f32* m ) {
    f32 xv = *x, yv = *y, zv = *z, wv = *w;
    ( *x ) = xv * m[ 0 ] + yv * m[ 1 ] + zv * m[ 2 ] + wv * m[ 3 ];
    ( *y ) = xv * m[ 4 ] + yv * m[ 5 ] + zv * m[ 6 ] + wv * m[ 7 ];
    ( *z ) = xv * m[ 8 ] + yv * m[ 9 ] + zv * m[ 10 ] + wv * m[ 11 ];
    ( *w ) = xv * m[ 12 ] + yv * m[ 13 ] + zv * m[ 14 ] + wv * m[ 15 ];
}

stu void matrix_transform_vec4( f32* v, f32* m ) {
    matrix_transform_xyzw( v, v + 1, v + 2, v + 3, m );
}

stu void matrix_multiply_vec4( f32* matrix_lhs, f32* vector_rhs, f32* result ) {
    
    f32* m = matrix_lhs;
    f32* v = vector_rhs;
    
    result[ 0 ] = m[ 0 ] * v[ 0 ] + m[ 4 ] * v[ 1 ] + m[ 8 ] * v[ 2 ] + m[ 12 ] * v[ 3 ];
    result[ 1 ] = m[ 1 ] * v[ 0 ] + m[ 5 ] * v[ 1 ] + m[ 9 ] * v[ 2 ] + m[ 13 ] * v[ 3 ];
    result[ 2 ] = m[ 2 ] * v[ 0 ] + m[ 6 ] * v[ 1 ] + m[ 10 ] * v[ 2 ] + m[ 14 ] * v[ 3 ];
    result[ 3 ] = m[ 3 ] * v[ 0 ] + m[ 7 ] * v[ 1 ] + m[ 11 ] * v[ 2 ] + m[ 15 ] * v[ 3 ];
}

stu b32 matrix_are_equal( f32* a, f32* b, umm elementCount ) {
    
    b32 result = true;
    for ( umm i = 0; result && i < elementCount; i++ ) {
        result = a[ i ] == b[ i ];
    }
    
    return result;
}

stu b32 matrix_are_equal_square( f32* a, f32* b, umm dimention ) {
    
    b32 result = matrix_are_equal( a, b, dimention * dimention );
    return result;
}

stu void matrix_copy( f32* dest, f32* source, umm elementCount ) {
    for ( umm i = 0; i < elementCount; i++ ) {
        dest[ i ] = source[ i ];
    }
}

stu void matrix_copy_square( f32* dest, f32* source, umm dimention ) {
    matrix_copy( dest, source, dimention * dimention );
}

// MARK geometry

/* NOTE simon: Area is negative if the polygon is counter clock wise. */
stu f32 geometry_polygon_area( vec2* points, umm point_count ) {
    
    f32 area = 0.0f;
    
    for ( umm point_index = 0; point_index < point_count; point_index++ ) {
        
        vec2 a, b;
        umm second_index = point_index + 1;
        
        if ( second_index == point_count ) {
            second_index = 0;
        }
        
        a = points[ point_index ];
        b = points[ second_index ];
        
        area += ( b.x - a.x ) * ( b.y + a.y );
        /* NOTE: alternative:
        area += ( a.x * b.y ) - ( b.x * a.y );
        but this one would be negative for clockwise polygons.
        */
    }
    
    return area / 2.0f;
}

stu vec2 geometry_find_centroid( vec2* points, umm point_count ) {
    
    vec2 result = { 0 };
    f32 area = 0.0f;
    f32 _6A;
    
    for ( umm index = 0; index < point_count; index++ ) {
        
        vec2 a, b;
        f32 a_part;
        umm second_index = index + 1;
        
        if ( second_index == point_count ) {
            second_index = 0;
        }
        
        a = points[ index ];
        b = points[ second_index ];
        
        a_part = ( a.x * b.y ) - ( a.y * b.x );
        area += a_part;
        result.x += ( a.x + b.x ) * a_part;
        result.y += ( a.y + b.y ) * a_part;
    }
    
    _6A = 3 * area;
    result.x /= _6A;
    result.y /= _6A;
    
    return result;
}

stu vec2 geometry_find_triangle_centroid( vec2* points )  {
    
    vec2 result = { 0 };
    
    for ( umm i = 0; i < 3; i++ ) {
        result = v2_add( result, points[ i ] );
    }
    
    result = v2_div( result, 3 );
    
    return result;
}

/* NOTE simon:
- 0 colinear;
- < 0 if angle is counter clockwise;
- > 0 if angle is clockwise;
*/
stu f32 geometry_angle_orientation( vec2 a, vec2 shared, vec2 b ) {
    
    /* NOTE simon:
slope_a = ( shared.y - a.y ) / ( shared.x - a.x );
slope_b = ( b.y - shared.y ) / ( b.x - shared.x );

if ( slope_a < slope_b ) => ccw
if ( slope_a > slope_b ) => cw
if ( slope_a == slope_b ) => colinear

We want to remove the divide. If we multiply the two slopes by the same amount we don't change the
result. 

We multiply both slope by both denominator.
new_a = slope_a * ( shared.x - ax ) * ( b.x - shared.x );
new_b = slope_b * ( shared.x - ax ) * ( b.x - shared.x );

new_a = ( ( shared.y - a.y ) / ( shared.x - a.x ) ) * ( shared.x - ax ) * ( b.x - shared.x );
new_b = ( ( b.y - shared.y ) / ( b.x - shared.x ) ) * ( shared.x - ax ) * ( b.x - shared.x );

new_a = ( ( shared.y - a.y ) * ( shared.x - ax ) * ( b.x - shared.x ) ) / ( shared.x - a.x );
new_b = ( ( b.y - shared.y ) * ( shared.x - ax ) * ( b.x - shared.x ) ) / ( b.x - shared.x );

new_a = ( shared.y - a.y ) * ( b.x - shared.x );
new_b = ( b.y - shared.y ) * ( shared.x - ax );

If we take the difference of the two value we get:
- 0 if collinear;
- < 0 if ccw;
- > 0 if cw;
    */
    
    f32 result = ( shared.y - a.y ) * ( b.x - shared.x ) - ( b.y - shared.y ) * ( shared.x - a.x );
    return result;
}

stu b32 geometry_is_colinear_point_on_segment( vec2 a, vec2 b, vec2 p ) {
    
    vec2 b_rel = v2_sub( b, a );
    vec2 p_rel = v2_sub( p, a );
    f32 one = b_rel.x * b_rel.x + b_rel.y * b_rel.y;
    f32 two = p_rel.x * p_rel.x + p_rel.y * p_rel.y;
    
    b32 result = two >= 0.0f && two < one;
    return result;
}

/* NOTE simon: Othre way https://www.petercollingridge.co.uk/tutorials/computational-geometry/line-line-intersections/ */
stu b32 geometry_are_segment_interesecting( vec2 a_start, vec2 a_end, vec2 b_start, vec2 b_end ) {
    
    f32 A1 = math_sign_or_zero( geometry_angle_orientation( a_start, a_end, b_start ) );
    f32 A2 = math_sign_or_zero( geometry_angle_orientation( a_start, a_end, b_end ) );
    f32 B1 = math_sign_or_zero( geometry_angle_orientation( b_start, b_end, a_start ) );
    f32 B2 = math_sign_or_zero( geometry_angle_orientation( b_start, b_end, a_end ) );
    
    b32 result =
    ( A1 != A2 && B1 != B2 )
        || ( A1 == 0.0f && geometry_is_colinear_point_on_segment( a_start, a_end, b_start ) )
        || ( A2 == 0.0f && geometry_is_colinear_point_on_segment( a_start, a_end, b_end ) )
        || ( B1 == 0.0f && geometry_is_colinear_point_on_segment( b_start, b_end, a_start ) )
        || ( B2 == 0.0f && geometry_is_colinear_point_on_segment( b_start, b_end, a_end ) );
    
    return result;
}

/* NOTE simon: the return value is the interpolation factor between b_start (f = 0) and b_end (f=1). */
stu f32 geometry_intersection_point( vec2 a_start, vec2 a_end, vec2 b_start, vec2 b_end ) {
    
    vec2 a = v2_sub( a_end, a_start );
    
    vec2 up = v2_sub( b_start, a_start );
    vec2 down = v2_sub( b_end, a_start );
    
    f32 f_up = fabsf( v2_cross( a, up ) );
    f32 f_down = fabsf( v2_cross( a, down ) );
    f32 f = f_up / ( f_up + f_down );
    
    return f;
}

stu b32 geometry_is_polygon_valid( vec2* points, u32 point_count ) {
    
    b32 result = false;
    umm checking_index = 0;
    
    while ( !result && checking_index < point_count - 1 ) {
        
        umm current_index = checking_index + 1;
        vec2 a_start = points[ checking_index ];
        vec2 a_end = points[ checking_index + 1 ];
        
        while ( !result && current_index < point_count ) {
            
            vec2 b_start, b_end;
            f32 A1, A2, B1, B2;
            umm second_index = current_index + 1;
            
            if ( second_index == point_count ) {
                second_index = 0;
            }
            
            b_start = points[ current_index ];
            b_end = points[ second_index ];
            
            A1 = math_sign_or_zero( geometry_angle_orientation( a_start, a_end, b_start ) );
            A2 = math_sign_or_zero( geometry_angle_orientation( a_start, a_end, b_end ) );
            B1 = math_sign_or_zero( geometry_angle_orientation( b_start, b_end, a_start ) );
            B2 = math_sign_or_zero( geometry_angle_orientation( b_start, b_end, a_end ) );
            
            if ( current_index == checking_index + 1 ) { /* NOTE simon: a_end == b_start */
                result = ( A2 == 0.0f && geometry_is_colinear_point_on_segment( a_start, a_end, b_end ) );
            } else if ( checking_index == 0 && second_index == 0 ) { /* NOTE simon: a_start == b_end */
                result = ( A1 == 0.0f && geometry_is_colinear_point_on_segment( a_start, a_end, b_start ) );
            } else {
                result = ( A1 != A2 && B1 != B2 );
                result = result || ( A1 == 0.0f && geometry_is_colinear_point_on_segment( a_start, a_end, b_start ) );
                result = result || ( B1 == 0.0f && geometry_is_colinear_point_on_segment( b_start, b_end, a_start ) );
                result = result || ( A2 == 0.0f && geometry_is_colinear_point_on_segment( a_start, a_end, b_end ) );
                result = result || ( B2 == 0.0f && geometry_is_colinear_point_on_segment( b_start, b_end, a_end ) );
            }
            
            current_index++;
        }
        
        checking_index++;
    }
    
    return result == false;
}

stu b32 geometry_is_polygon_ccw( vec2* points, umm point_count ) {
    
    f32 sum = 0;
    
    for ( umm point_index = 0; point_index < point_count; point_index++ ) {
        
        f32 x0, x1, y0, y1;
        umm second_index = point_index + 1;
        
        if ( point_index == point_count - 1 ) {
            second_index = 0;
        }
        
        x0 = points[ point_index ].x;
        x1 = points[ second_index ].x;
        y0 = points[ point_index ].y;
        y1 = points[ second_index ].y;
        
        sum += ( x1 - x0 ) * ( y1 + y0 );
    }
    
    return sum < 0;
}

stu void geometry_inverse_polygon_winding( vec2* points, u32 point_count ) {
    
    for ( umm point_index = 0; point_index < point_count / 2; point_index++ ) {
        
        umm second_index = point_count - point_index - 1;
        vec2 temp = points[ point_index ];
        points[ point_index ] = points[ second_index ];
        points[ second_index ] = temp;
    }
}

/* NOTE simon: http://lab.polygonal.de/2006/08/17/calculating-the-moment-of-inertia-of-a-convex-polygon/ */
/* NOTE simon: I have no idea if those function are correct. */
stu f32 geometry_find_triangle_moment_of_inertia( vec2* points )  {
    
    b32 is_ccw = geometry_is_polygon_ccw( points, 3 );
    vec2 A = points[ 0 ];
    vec2 B = points[ 1 ];
    vec2 C = points[ 2 ];
    
    if ( !is_ccw )  {
        vec2 temp = A;
        A = B;
        B = temp;
    }
    
    vec2 AB = v2_sub( B, A );
    vec2 AC = v2_sub( C, A );
    
    vec2 ABN = v2_normalized( AB );
    
    f32 b = v2_length( AB );
    f32 h = v2_dot( v2_perp( ABN ), AC );
    f32 a = v2_dot( ABN, AC );
    
    f32 moment_of_inertia = ( ( b * b * b * h ) - ( b * b * h * a ) + ( b * h * a * a ) + ( b * h * h * h ) ) / 36;
    
    return moment_of_inertia;
}

stu f32 geometry_find_convex_moment_of_inertia( vec2* points, umm point_count, vec2 centroid ) {
    
    f32 moi = 0.0f;
    b32 is_ccw = geometry_is_polygon_ccw( points, point_count );
    
    vec2 triangle[ 3 ];
    triangle[ 0 ] = centroid;
    
    for ( umm point_index = 0; point_index < point_count; point_index++ ) {
        
        umm first_index = point_index;
        umm second_index = point_index + 1;
        
        if ( !is_ccw ) {
            first_index = point_count - point_index;
            second_index = point_count - ( point_index + 1 );
        }
        
        if ( first_index >= point_count )  {
            first_index = 0;
        }
        
        if ( second_index >= point_count ) {
            second_index = 0;
        }
        
        triangle[ 1 ] = points[ first_index ];
        triangle[ 2 ] = points[ second_index ];
        
        vec2 triangle_centroid = geometry_find_triangle_centroid( triangle );
        
#if 1
        vec2 AB = v2_sub( triangle[ 1 ], triangle[ 0 ] );
        vec2 AC = v2_sub( triangle[ 2 ], triangle[ 0 ] );
        
        vec2 ABN = v2_normalized( AB );
        
        f32 b = v2_length( AB );
        f32 h = v2_dot( v2_perp( ABN ), AC );
        f32 a = v2_dot( ABN, AC );
        
        f32 triangle_moi = ( ( b * b * b * h ) - ( b * b * h * a ) + ( b * h * a * a ) + ( b * h * h * h ) ) / 36;
        f32 triangle_mass = ( h * b ) * 0.5f;
#else
        f32 triangle_moi = geometry_find_triangle_moment_of_inertia( triangle );
        f32 triangle_mass = abs( geometry_polygon_area( triangle, 3 ) ); /* NOTE simon: This way of calculating mass is little more precise but slower. */
#endif
        
        moi += ( triangle_moi + triangle_mass * v2_length( v2_sub( centroid, triangle_centroid ) ) );
    }
    
    return moi;
}

/* NOTE simon:
Expected:
- Line don't cross;
- The last point != the first point;
- Points are counter clockwise.

Output in memory:
{
---- u32 convexCount;
---- { Convex 1
-------- u32 indexCount;
-------- u32* indexes;
---- }
---- { Convex 2 }
---- { ... }
}
*/
/* TODO simon: verify memory_push* alignments. */
// #define DEBUG_PARTITION_POLYGON 1

/* NOTE simon: This could help find a better solution: https://top.handmade.network/blogs/p/2802-writing_a_custom_vector_graphics_renderer_-_part_2#13812 */

stu void geometry_partition_polygon( data_t* memory, vec2* points, u32 point_count ) {
    
#if defined( DEBUG_PARTITION_POLYGON )
    vec2* point_list = memory_pushArray( &debug_gl.memory, vec2, point_count + 1 );
    
    for ( umm i = 0; i < point_count; i++ ) {
        point_list[ i ] = v2_add(  points[ i ], v2( 300.0f, 300.0f ) );
    }
    
    point_list[ point_count ] = point_list[ 0 ];
    
    debug_frame_init( 0 );
    debug_clear( 0 );
    debug_draw_lines( 0, point_list, point_count + 1, white );
    display_debug_render = true;
    
    memory_pop_array( &debug_gl.memory, vec2, point_count + 1 );
#endif
    
    _assert( geometry_is_polygon_ccw( points, point_count ) );
    _assert( geometry_is_polygon_valid( points, point_count ) );
    
    u32* result_count = memory_push_type( memory, u32 );
    *result_count = 0;
    
    reverse_stack_t reverse_stack = memory_reverse_stack_create( memory );
    u32* polygon_count = cast( u32*, memory_reverse_stack_push( &reverse_stack, sizeof( u32 ) ) );
    *polygon_count = 1;
    u32* working_point_count = cast( u32*, memory_reverse_stack_push( &reverse_stack, sizeof( u32 ) ) );
    *working_point_count = point_count;
    u32* working_points = cast( u32*, memory_reverse_stack_push( &reverse_stack, point_count * sizeof( u32 ) ) );
    u32 current_polygon = 0;
    
    for ( u32 i = 0; i < *working_point_count; i++ ) {
        working_points[ i ] = i;
    }
    
    while ( current_polygon < *polygon_count ) {
        
        /* NOTE simon: working_points is a polygon part (non convex) that as been pushed on the reverse stack. */
        working_points = working_point_count - *working_point_count;
        
#if defined( DEBUG_PARTITION_POLYGON )
        vec2* point_list = memory_push_array( &debug_gl.memory, vec2, *working_point_count );
        
        for ( umm i = 0; i < *working_point_count; i++ ) {
            point_list[ i ] = v2_add(  points[ *( working_points + i ) ], v2( 300.0f, 300.0f ) );
        }
        
        debug_frame_init( 1 );
        debug_draw_lines( 1, point_list, *working_point_count, yellow );
        
        memory_pop_array( &debug_gl.memory, vec2, *working_point_count );
#endif
        
        ( *result_count )++;
        u32* polygon_point_count = cast( u32*, memory_push_size( memory, sizeof( u32 ) * 3, 4 ) );
        /* NOTE simon: polygon contains the new convex that will be pushed in the output memory.*/
        u32* polygon = polygon_point_count + 1;
        
        polygon[ 0 ] = working_points[ 0 ];
        polygon[ 1 ] = working_points[ 1 ];
        *polygon_point_count = 2;
        
        u32 working_point_index = *polygon_point_count;
        s32 start_point = -1;
        
        while ( working_point_index < *working_point_count ) {
            
            vec2 a = points[ polygon[ *polygon_point_count - 2 ] ];
            vec2 b = points[ polygon[ *polygon_point_count - 1 ] ];
            vec2 c = points[ working_points[ working_point_index ] ];
            vec2 d = points[ working_points[ 0 ] ];
            vec2 e = points[ working_points[ 1 ] ];
            
#if defined( DEBUG_PARTITION_POLYGON )
            debug_frame_init( 3 );
            debug_draw_point( 3, v2_add( c, v2( 300, 300 ) ), 3, blue );
            debug_render( 0, false );
            debug_render( 1, false );
            debug_render( 3, true );
#endif
            
            memory_push_type( memory, u32 );
            polygon[ *polygon_point_count ] = working_points[ working_point_index ];
            
            b32 valid = geometry_angle_orientation( a, b, c ) <= 0.0f;
            valid = valid && geometry_angle_orientation( b, c, d ) <= 0.0f;
            valid = valid && geometry_angle_orientation( c, d, e ) <= 0.0f;
            
            if ( valid ) {
                
                /* NOTE simon: Should I test the convex agains every shapes that have been generated ? */
                /* NOTE simon: Validate that segments of convex don't intersect with segments of shape */
                /* NOTE simon: this if was !is_convex_intersecting_shape; */
                
                u32* convex_indexes = polygon;
                u32 convex_point_count = ( *polygon_point_count ) + 1;
                u32* shape_indexes = working_points;
                u32 shape_point_count = *working_point_count;
                
                b32 intersects = false;
                umm convex_index = 0;
                
                while ( !intersects && convex_index < convex_point_count ) {
                    
                    umm second_index = convex_index + 1;
                    
                    if ( second_index == convex_point_count ) {
                        second_index = 0;
                    }
                    
                    umm a_start_index = convex_indexes[ convex_index ];
                    umm a_end_index = convex_indexes[ second_index ];
                    vec2 a_start = points[ a_start_index ];
                    vec2 a_end = points[ a_end_index ];
                    
                    umm shape_index = 0;
                    
                    while ( !intersects && shape_index < shape_point_count ) {
                        
                        umm shape_second_index = shape_index + 1;
                        
                        if ( shape_second_index == shape_point_count ) {
                            shape_second_index = 0;
                        }
                        
                        umm b_start_index = shape_indexes[ shape_index ];
                        umm b_end_index = shape_indexes[ shape_second_index ];
                        vec2 b_start = points[ b_start_index ];
                        vec2 b_end = points[ b_end_index ];
                        
                        if ( a_start_index != b_start_index && a_end_index != b_end_index ) {
                            
                            f32 A1 = math_sign_or_zero( geometry_angle_orientation( a_start, a_end, b_start ) );
                            f32 A2 = math_sign_or_zero( geometry_angle_orientation( a_start, a_end, b_end ) );
                            f32 B1 = math_sign_or_zero( geometry_angle_orientation( b_start, b_end, a_start ) );
                            f32 B2 = math_sign_or_zero( geometry_angle_orientation( b_start, b_end, a_end ) );
                            
                            if ( a_end_index == b_start_index ) {
                                intersects = ( A2 == 0.0f && geometry_is_colinear_point_on_segment( a_start, a_end, b_end ) );
                            } else if ( a_start_index == b_end_index ) {
                                intersects = ( A1 == 0.0f && geometry_is_colinear_point_on_segment( a_start, a_end, b_start ) );
                            } else {
                                intersects = ( A1 != A2 && B1 != B2 );
                                intersects = intersects || ( A1 == 0.0f && geometry_is_colinear_point_on_segment( a_start, a_end, b_start ) );
                                intersects = intersects || ( B1 == 0.0f && geometry_is_colinear_point_on_segment( b_start, b_end, a_start ) );
                                intersects = intersects || ( A2 == 0.0f && geometry_is_colinear_point_on_segment( a_start, a_end, b_end ) );
                                intersects = intersects || ( B2 == 0.0f && geometry_is_colinear_point_on_segment( b_start, b_end, a_end ) );
                            }
                        }
                        
                        shape_index++;
                    }
                    
                    convex_index++;
                }
                
                valid = !intersects;
            }
            
            if ( valid ) {
                
                /* NOTE simon: this if was isAnyPointInsideConvex. */
                u32* point_indexes = working_points + working_point_index + 1;
                u32 index_count = ( *working_point_count ) - working_point_index - 1;
                u32* convex_indexes = polygon;
                u32 convex_point_count = *polygon_point_count + 1;
                
                f32 dot_product = 1.0f;
                
                if ( index_count == 0 ) {
                    dot_product = -1.0f;
                }
                
                for ( umm i = 0; i < convex_point_count && dot_product >= 0.0f; i++ ) {
                    
                    vec2 a_point = points[ convex_indexes[ i ] ];
                    vec2 b_point = ( i == convex_point_count - 1 ) ? points[ convex_indexes[ 0 ] ] : points[ convex_indexes[ i + 1 ] ];
                    vec2 side = v2_sub( b_point, a_point );
                    vec2 normal = v2_perp( side );
                    
                    for ( umm j = 0; j < index_count && dot_product >= 0.0f; j++ ) {
                        vec2 test = v2_sub( points[ point_indexes[ j ] ], a_point );
                        dot_product = v2_dot( normal, test );
                    }
                }
                
                valid = dot_product < 0.0f;
            }
            
            if ( valid ) {
                
                if ( start_point != -1 ) {
                    
                    u32 count = working_point_index - start_point + 1;
                    u32* new_polygon = cast( u32*, memory_reverse_stack_push( &reverse_stack, sizeof( u32 ) * ( count + 1 ) ) );
                    u32* new_polygon_point_count = new_polygon + count;
                    *new_polygon_point_count = count;
                    
                    for ( umm i = 0; i < *new_polygon_point_count; i++ ) {
                        new_polygon[ i ] = working_points[ start_point + i ];
                    }
                    
                    ( *polygon_count )++;
                    start_point = -1;
                }
                
                ( *polygon_point_count )++;
                
            } else {
                
                if ( start_point == -1 ) {
                    start_point = working_point_index - 1;
                }
                
                memory_pop_size( memory, sizeof( u32 ) );
            }
            
            working_point_index++;
        }
        
        if ( start_point != -1 ) {
            
            u32 count = ( *working_point_count ) - start_point + 1;
            u32* new_polygon = cast( u32*, memory_reverse_stack_push( &reverse_stack, sizeof( u32 ) * ( count + 1 ) ) );
            u32* new_polygon_point_count = new_polygon + count;
            *new_polygon_point_count = count;
            
            for ( umm i = 0; i < ( *new_polygon_point_count ) - 1; i++ ) {
                new_polygon[ i ] = working_points[ start_point + i ];
            }
            
            new_polygon[ ( *new_polygon_point_count ) - 1 ] = working_points[ 0 ];
            
            ( *polygon_count )++;
            /* start_point = -1; // NOTE simon: clang static code analyser suggest to remove this as the result is never used. */
        }
        
        working_point_count = working_points - 1;
        current_polygon++;
        
#if defined( DEBUG_PARTITION_POLYGON )
        vec2* point_list_added = memory_push_array( &debug_gl.memory, vec2, *polygon_point_count );
        
        for ( umm i = 0; i < *polygon_point_count; i++ ) {
            point_list_added[ i ] = v2_add(  points[ *( polygon + i ) ], v2( 300.0f, 300.0f ) );
        }
        
        debug_frame_init( 2 );
        debug_draw_lines( 2, point_list, *polygon_point_count, red );
        
        memory_pop_array( &debug_gl.memory, vec2, *polygon_point_count );
        
        debug_render( 0, false );
        debug_render( 1, false );
        debug_render( 2, false );
        debug_render( 3, true );
        debug_break( );
#endif
    }
}

/* MARK arguments */

/* NOTE simon: win32 writing console attributes https://msdn.microsoft.com/en-us/library/windows/desktop/ms685032(v=vs.85).aspx */

#if !defined ARGUMENTS_MAX_COUNT
#define ARGUMENTS_MAX_COUNT 128
#endif

u32 arguments_count = 0;
data_t arguments_d[ ARGUMENTS_MAX_COUNT ] = { 0 };
u8 executable_path_buffer[ 2048 ] = { 0 };
data_t executable_path;
data_t executable_name;
u8 executable_working_directory_buffer[ 4096 ] = { 0 };
data_t executable_working_directory = { 0 };

#if defined( WINDOWS )

/* NOTE simon: contains the argv strings (separated by \0) AND the argv array itself, aligned on the pointer size. */
u8 arguments_utf8_command_line[ 2048 ] = { 0 };

/* NOTE simon: https://docs.microsoft.com/en-us/previous-versions/17w5ykft(v=vs.85)
Tests:
"abc" d e => abc | d | e
a\\\b d"e f"g h => a\\\b | de fg | h
a\\\"b c d => a\"b | c | d
a\\\\"b c" d e => a\\b c | d | e
*/
stu void arguments_parse_command_line( int* argc, char*** argvp ) {
    
    u16* current = cast( u16*, GetCommandLineW( ) );
    u16* end = current;
    
    while ( *end ) {
        end++;
    }
    
    u8* utf8_start = arguments_utf8_command_line;
    u8* utf8_current = utf8_start;
    u8* utf8_end = utf8_start + array_count( arguments_utf8_command_line );
    
    b32 in_quotes = false;
    b32 previous_was_space = false;
    u32 backslash_count = 0;
    
    while ( current < end && utf8_current < utf8_end ) {
        
        u32 codepoint = ucs_invalid_codepoint, error = 0;
        u8 codeunit_count = 0;
        utf16_decode( current, end, &codepoint, &codeunit_count, 0, &error );
        current += codeunit_count;
        _assert( no_error( &error ) );
        
        if ( codepoint == '\\' ) {
            
            backslash_count++;
            previous_was_space = false;
            
        } else {
            
            if ( codepoint == '"' ) {
                
                for ( umm i = 0; i < backslash_count / 2; i++ ) {
                    if ( utf8_current < utf8_end ) {
                        *utf8_current = '\\';
                        utf8_current++;
                    }
                }
                
                if ( ( backslash_count % 2 ) == 0 ) {
                    in_quotes = !in_quotes;
                } else {
                    if ( utf8_current < utf8_end ) {
                        *utf8_current = '"';
                        utf8_current++;
                    }
                }
                
                backslash_count = 0;
                previous_was_space = false;
                
            } else {
                
                for ( umm i = 0; i < backslash_count; i++ ) {
                    if ( utf8_current < utf8_end ) {
                        *utf8_current = '\\';
                        utf8_current++;
                    }
                }
                
                backslash_count = 0;
                
                if ( codepoint == ' ' || codepoint == '\t' ) {
                    
                    if ( in_quotes ) {
                        if ( utf8_current < utf8_end ) {
                            *utf8_current = ( u8 ) codepoint;
                            utf8_current++;
                        }
                    } else {
                        
                        if ( !previous_was_space ) {
                            if ( utf8_current < utf8_end ) {
                                *utf8_current = 0;
                                utf8_current++;
                            }
                        }
                        
                        previous_was_space = true;
                    }
                    
                } else {
                    
                    previous_was_space = false;
                    u8 utf8_codeunit_count = 0;
                    utf8_encode( utf8_current, utf8_end, codepoint, &utf8_codeunit_count, &error );
                    if ( no_error( &error ) ) {
                        utf8_current += utf8_codeunit_count;
                    }
                }
            }
        }
    }
    
    if ( backslash_count ) {
        for ( umm i = 0; i < backslash_count; i++ ) {
            if ( utf8_current < utf8_end ) {
                *utf8_current = '\\';
                utf8_current++;
            }
        }
    }
    
    /* NOTE simon: If the command line ended with a space or tab there is already a 0 at the end */
    if ( utf8_current > utf8_start && utf8_current < utf8_end ) {
        
        if ( utf8_current[ -1 ] ) {
            *utf8_current = 0;
            utf8_current++;
        }
        
    } else {
        _assert( !"Empty command line or tool long command line." );
    }
    
    umm command_line_length = utf8_current - utf8_start;
    
    u8** argv = cast( u8**, utf8_current + memory_get_padding( utf8_current, sizeof( u8** ) ) );
    *argc = 0;
    b32 add = true;
    
    for ( umm i = 0; i < command_line_length && ( argv + i + 1 ) <= cast( u8**, utf8_end ); i++ ) {
        
        if ( add && utf8_start[ i ] != 0 ) {
            argv[ *argc ] = utf8_start + i;
            ( *argc )++;
            add = false;
        } else if ( utf8_start[ i ] == 0 ) {
            add = true;
        }
    }
    
    deref( argvp ) = cast( char**, argv );
}

#elif defined( LINUX )

stu void arguments_parse_command_line( int* argc, char*** argvp ) {
    /* NOTE simon: On linux this call doesn't need to do anything. */
}

#else

#error Unsupported platform.

#endif

stu void arguments_initialize( s32 argc, char** argv ) {
    
    executable_path = memory_from_array( executable_path_buffer );
    executable_working_directory = memory_from_array( executable_working_directory_buffer );
    
    _assert( argc <= array_count( arguments_d ) );
    arguments_count = argc;
    
    for ( umm i = 0; i < argc && i < array_count( arguments_d ); i++ ) {
        arguments_d[ i ].used = string_length( argv[ i ] );
        arguments_d[ i ].reserved = arguments_d[ i ].used + 1;
        arguments_d[ i ].bytes = cast( u8*, argv[ i ] );
    }
    
#if defined( WINDOWS )
    
    memory_get_on_stack( path_utf16, kibioctet( 2 ) );
    
    u32 length = GetModuleFileNameW( 0, cast( LPWSTR, path_utf16.bytes ), cast( DWORD, path_utf16.reserved / sizeof( wchar_t ) ) );
    _assert( length && length < cast( u32, path_utf16.reserved / sizeof( wchar_t ) ) );
    path_utf16.used = length * sizeof( wchar_t );
    utf16_to_utf8( path_utf16, &executable_path, &g_ucs_error );
    _assert( no_error( &g_ucs_error ) );
    
    memory_get_on_stack( working_directory_utf16, kibioctet( 2 ) );
    DWORD wd_length = GetCurrentDirectoryW( cast( DWORD, working_directory_utf16.reserved / sizeof( wchar_t ) ), cast( LPWSTR, working_directory_utf16.bytes ) );
    _assert( wd_length && wd_length < working_directory_utf16.reserved / sizeof( wchar_t ) );
    
    working_directory_utf16.used = wd_length * sizeof( wchar_t );
    
    utf16_to_utf8( working_directory_utf16, &executable_working_directory, &g_ucs_error );
    _assert( no_error( &g_ucs_error ) );
    
#elif defined( LINUX )
    
    s32 length = readlink( "/proc/self/exe", cast( char*, executable_path_buffer ), array_count( executable_path_buffer ) - 1 );
    _assert( length != -1 );
    executable_path.used = length;
    string_zero_terminate( executable_path );
    
    u8* start = cast( u8*, getcwd( cast( char*, executable_working_directory.bytes ), executable_working_directory.reserved ) );
    _assert( start );
    
    umm wd_length = 0;
    u8* end = start;
    u8* buffer_end = start + executable_working_directory.reserved;
    
    while ( end < buffer_end && deref( end ) ) {
        end++;
    }
    
    _assert( end < buffer_end && deref( end ) == 0 );
    executable_working_directory.used = end - start;
    
#else
# error Unsupported platform.
#endif
    
    u8* path_start = executable_path.bytes;
    u8* name_end = memory_end( executable_path );
    u8* name_start = name_end;
    
    while ( name_start > path_start ) {
        
        if ( deref( name_start ) == '/' || deref( name_start ) == '\\' ) {
            name_start++;
            break;
        }
        
        name_start--;
    }
    
    executable_path = memory_start_end( path_start, name_start );
    executable_name = memory_start_end( name_start, name_end );
}

#define arguments_find_l( literal, start_index ) arguments_find_p( literal, sizeof( literal ) - 1, start_index )

stu u32 arguments_find_p( void* name_arg, umm length, u32 start_index ) {
    
    u8* name = cast( u8*, name_arg );
    u32 result = 0;
    data_t search = memory_construct( length, length, name );
    
    for ( u32 arg_index = start_index; arg_index < arguments_count; arg_index++ ) {
        
        data_t* arg = arguments_d + arg_index;
        
        if ( memory_are_equal( search, *arg ) ) {
            result = arg_index;
            break;
        }
    }
    
    return result;
}

stu u32 arguments_find( data_t name, u32 start_index ) {
    u32 result = arguments_find_p( name.text, name.used, start_index );
    return result;
}

#define arguments_get_value_l( literal, start_index ) arguments_get_value_p( literal, sizeof( literal ) - 1, start_index )

stu data_t arguments_get_value_p( void* name, umm length, u32* start_index ) {
    
    data_t result = { 0 };
    u32 index = 0;
    
    if ( start_index ) {
        index = *start_index;
    }
    
    u32 argument_index = arguments_find_p( name, length, index );
    
    if ( argument_index > 0 && argument_index + 1 < arguments_count ) {
        
        if ( start_index ) {
            *start_index = ( argument_index + 1 );
        }
        
        result = arguments_d[ ( argument_index + 1 ) ];
    }
    
    return result;
}

stu data_t arguments_get_value( data_t name, u32* start_index ) {
    data_t result = arguments_get_value_p( name.text, name.used, start_index );
    return result;
}

/* MARK ascii */

typedef struct ascii_info_t {
    const char* escape_sequence;
    const char* short_name;
    const char* name;
    u8 escape_sequence_length; 
    u8 short_name_length;
    u8 name_length;
} ascii_info_t;

ascii_info_t ascii_table[ ] = { 
    "\\0", "NUL", "Null", 2, 3, 4,
    0, "SOH", "Start of heading", 0, 3, 16,
    0, "STX", "Start of text", 0, 3, 13,
    0, "ETX", "End of text", 0, 3, 11,
    0, "EOT", "End of transmission", 0, 3, 19,
    0, "ENQ", "Enquiry", 0, 3, 7,
    0, "ACK", "Acknoledgement", 0, 3, 14,
    "\\a", "BEL", "Bell", 2, 3, 4,
    "\\b", "BS", "Backspace", 2, 2, 9,
    "\\t", "HT", "Horizontal tab", 2, 2, 14,
    "\\n", "LF", "Line feed", 2, 2, 9, 
    "\\v", "VT", "Vertical tab", 2, 2, 12,
    "\\f", "FF", "Form feed", 2, 2, 9,
    "\\r", "CR", "Carriage return", 2, 2, 15,
    0, "SO", "Shift out", 0, 2, 9,
    0, "SI", "Shift in", 0, 2, 8,
    0, "DLE", "Data link escape", 0, 3, 16, 
    0, "DC1", "Device control 1", 0, 3, 16,
    0, "DC2", "Device control 2", 0, 3, 16,
    0, "DC3", "Device control 3", 0, 3, 16,
    0, "DC4", "Device control 4", 0, 3, 16,
    0, "NAK", "Negative acknoledgement", 0, 3, 23,
    0, "SYN", "Synchronous idle", 0, 3, 16,
    0, "ETB", "End of transmission block", 0, 3, 25,
    0, "CAN", "Cancel", 0, 3, 6,
    0, "EM", "End of medium", 0, 2, 13,
    0, "SUB", "Substitute", 0, 3, 10,
    "\\e", "ESC", "Escape", 2, 3, 6,
    0, "FS", "File separator", 0, 2, 14,
    0, "GS", "Group separator", 0, 2, 15,
    0, "RS", "Record separator", 0, 2, 16,
    0, "US", "Unit separator", 0, 2, 14,
    0, "DEL", "Delete", 0, 3, 6
};

stu data_t ascii_get_escape_sequence( u8 ascii ) {
    
    data_t result = { 0 };
    u8 index = ascii;
    
    if ( index == 0x7f ){
        index = 0x20;
    }
    
    if ( index < array_count( ascii_table ) &&  ascii_table[ index ].escape_sequence_length ){
        result.bytes = cast( u8*, ascii_table[ index ].escape_sequence );
        result.reserved = result.used = ascii_table[ index ].escape_sequence_length;
    }
    
    return result;
}

stu data_t ascii_get_short_name( u8 ascii ) {
    
    data_t result = { 0 };
    u8 index = ascii;
    
    if ( index == 0x7f ){
        index = 0x20;
    }
    
    if ( index < array_count( ascii_table ) ){
        result.bytes = cast( u8*, ascii_table[ index ].short_name );
        result.reserved = result.used = ascii_table[ index ].short_name_length;
    }
    
    return result;
}

stu data_t ascii_get_name( u8 ascii ) {
    
    data_t result = { 0 };
    u8 index = ascii;
    
    if ( index == 0x7f ){
        index = 0x20;
    }
    
    if ( index < array_count( ascii_table ) ){
        result.bytes = cast( u8*, ascii_table[ index ].name );
        result.reserved = result.used = ascii_table[ index ].name_length;
    }
    
    return result;
}


/* MARK terminal */

#if 0
#include <sys/ioctl.h>
#include <stdio.h>

int main (void)
{
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    
    printf ("lines %d\n", w.ws_row);
    printf ("columns %d\n", w.ws_col);
    return 0;
}
#endif

u32 g_terminal_error = 0;

typedef enum terminal_error_t {
    terminal_error_none,
    terminal_error_read,
    terminal_error_count
} terminal_error_t;

#if defined( WINDOWS )
HANDLE terminal_stdin = 0;
HANDLE terminal_stdout = 0;
HANDLE terminal_stderror = 0;
#elif defined( LINUX )
u32 terminal_stdin = STDIN_FILENO;
u32 terminal_stdout = STDOUT_FILENO;
u32 terminal_stderror = STDERR_FILENO;
#else
#error Unsupported platform.
#endif

u8 terminal_buffer[ 1024 ];

#if defined( WINDOWS )

stu void terminal_initialize( void ) {
    /* NOTE simon: Set the window console to use utf-8. Only work for IO after this point, argc and argv aren't utf8. */
    SetConsoleCP( 65001 );
    SetConsoleOutputCP( 65001 );
}

stu void terminal_write_p( void* string, umm string_length ) {
    
    _assert( string_length <= u32_max );
    
    if ( !terminal_stdout ) {
        terminal_stdout = GetStdHandle( STD_OUTPUT_HANDLE );
    }
    
    DWORD written;
    
    if ( terminal_stdout && terminal_stdout != INVALID_HANDLE_VALUE ) {
        WriteFile( terminal_stdout, string, ( u32 ) string_length, &written, 0 );
    }
}

#define terminal_write_l( string ) terminal_write_p( ( string ), sizeof( string ) - 1 )

stu void terminal_write( data_t buffer ) {
    terminal_write_p( buffer.bytes, buffer.used );
}

stu void terminal_error_p( void* string, umm string_length ) {
    
    if ( !terminal_stderror ) {
        terminal_stderror = GetStdHandle( STD_ERROR_HANDLE );
    }
    
    DWORD written;
    
    if ( terminal_stderror && terminal_stderror != INVALID_HANDLE_VALUE ) {
        WriteFile( terminal_stderror, string, ( u32 ) string_length, &written, 0 );
    }
}

#define terminal_error_l( string ) terminal_error_p( ( string ), sizeof( string ) - 1 )

stu void terminal_error( data_t buffer ) {
    terminal_error_p( buffer.bytes, buffer.used );
}

stu data_t terminal_read( u32* error ) {
    
    data_t result = memory_from_array( terminal_buffer );
    
    if ( !terminal_stdin ) {
        terminal_stdin = GetStdHandle( STD_INPUT_HANDLE );
    }
    
    if ( no_error( error ) ) { 
        
        DWORD bytes_read = 0;
        b32 success = ReadFile( terminal_stdin, result.bytes, cast( DWORD, result.reserved ), &bytes_read, 0 );
        
        if ( !success ) {
            set_error( error, terminal_error_read );
        } else {
            result.used = bytes_read;
        }
    }
    
    return result;
}

stu void terminal_clear( void ) {
    
    /* NOTE simon: From https://docs.microsoft.com/en-us/windows/console/clearing-the-screen */
    
    if ( !terminal_stdout ) {
        terminal_stdout = GetStdHandle( STD_OUTPUT_HANDLE );
    }
    
    DWORD mode = 0;
    GetConsoleMode( terminal_stdout, &mode );
    DWORD original_mode = mode;
    
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode( terminal_stdout, mode );
    
    DWORD written = 0;
    data_t sequence = string_l( "\x1b[2J" ); /* NOTE simon: "\x1b[3J" to also clear the scroll back. */
    WriteFile( terminal_stdout, sequence.bytes, cast( DWORD, sequence.used ), &written, 0 );
    
    SetConsoleMode( terminal_stdout, original_mode );
}

#elif defined( LINUX )

stu void terminal_initialize( void ) {
    
}

stu void terminal_write_p( void* string, umm string_length ) {
    write( terminal_stdout, string, string_length );
}

#define terminal_write_l( string ) terminal_write_p( ( string ), sizeof( string ) - 1 )

stu void terminal_write( data_t buffer ) {
    write( terminal_stdout, buffer.bytes, buffer.used );
}

stu void terminal_error_p( void* string, umm string_length ) {
    write( terminal_stderror, string, string_length );
}

#define terminal_error_l( string ) terminal_error_p( ( string ), sizeof( string ) - 1 )

stu void terminal_error( data_t buffer ) {
    write( terminal_stderror, buffer.bytes, buffer.used );
}

stu data_t terminal_read( u32* error ) {
    
    data_t result = memory_from_array( terminal_buffer ); 
    
    if ( no_error( error ) ) { 
        
        ssize_t size_read = read( terminal_stdin, result.bytes, result.reserved );
        
        if ( size_read < 0 ) {
            set_error( error, terminal_error_read );
        } else {
            result.used = cast( u32, size_read );
        }
    }
    
    return result;
}

stu void terminal_clear( void ) {
    terminal_write_l( "\E[1;1H\E[2J" );
}

#else
# error Unsupported platform.
#endif

/* MARK process */

#if defined( WINDOWS )

/* TODO simon: better return code to know what is happening without debugging ? Done on linux ? */
/* NOTE simon: to execute dir, rmdir or other command, you need to use "cmd /c dir" as a command. */
stu b32 process_execute( data_t command, data_t working_directory, b32 should_wait, u32* return_value ) {
    
    memory_get_on_stack( memory, kibioctet( 16 ) );
    
    utf8_to_utf16( command, &memory, &g_ucs_error );
    _assert( no_error( &g_ucs_error ) );
    memory_push_u16( &memory, 0, 0 );
    data_t command_utf16 = memory;
    
    data_t working_directory_utf16 = { 0 };
    
    if ( working_directory.bytes && working_directory.used ) {
        working_directory_utf16 = memory_get_block_remaining( &memory, sizeof( u16 ) );
        utf8_to_utf16( working_directory, &working_directory_utf16, &g_ucs_error );
        _assert( no_error( &g_ucs_error ) );
        memory_push_u16( &working_directory_utf16, 0, 0 );
    }
    
    b32 result = 0;
    
    PROCESS_INFORMATION process_information = { 0 };
    STARTUPINFOW startup_information = { 0 };
    startup_information.cb = sizeof( startup_information );
    
    if ( CreateProcessW( 0, ( LPWSTR ) command_utf16.bytes, 0, 0, 0, 0, 0, ( LPCWSTR ) working_directory_utf16.bytes, &startup_information, &process_information ) ) {
        
        result = 1;
        
        if ( should_wait && process_information.hProcess != 0 ) {
            
            DWORD wait_status = WaitForSingleObject( process_information.hProcess, INFINITE );
            
            if ( wait_status != WAIT_OBJECT_0 ) {
                result = 0;
            }
        }
        
        if ( process_information.hProcess ) {
            
            if ( return_value ) {
                
                b32 success = GetExitCodeProcess( process_information.hProcess, ( LPDWORD ) return_value );
                
                if ( !success || *return_value == STILL_ACTIVE ) {
                    result = 0;
                }
            }
            
            CloseHandle( process_information.hProcess );
        }
        
        if ( process_information.hThread ) {
            CloseHandle( process_information.hThread );
        }
    } else {
        log_windows_error_message( GetLastError( ) );
    }
    
    return result;
}

#elif defined( LINUX )

stu b32 process_execute_p( void* command_arg, umm command_length, void* working_directory_arg, umm working_directory_length, b32 should_wait, u32* return_value ) {
    
#define TERMINAL_EXECUTE_ERROR_FINALCOMMAND_LENGTH -1
#define TERMINAL_EXECUTE_ERROR_GETCWD -2
#define TERMINAL_EXECUTE_ERROR_CHDIR -3
#define TERMINAL_EXECUTE_ERROR_ARGUMENT_COUNT -4
#define TERMINAL_EXECUTE_ERROR_EXEC -5
    
    memory_get_on_stack( command, kibioctet( 2 ) );
    memory_push_copy_p( &command, command_arg, command_length, 1 );
    string_zero_terminate( command );
    
    memory_get_on_stack( working_directory, kibioctet( 2 ) );
    memory_push_copy_p( &working_directory, working_directory_arg, working_directory_length, 1 );
    string_zero_terminate( working_directory );
    
    b32 result = 0;
    s32 pid = fork( );
    
    if ( pid == 0 ) {
        
        char final_command[ 2048 ] = { 0 };
        char* cwd_error = getcwd( final_command, array_count( final_command ) );
        
        if ( cwd_error ) {
            
            umm final_command_length;
            final_command_length = string_length( final_command );
            final_command[ final_command_length++ ] = '/';
            
            char* command_end = command.text;
            
            for ( umm i = 0; i < command.used; i++ ) {
                
                if ( *command_end == 0 || *command_end == ' ' ) {
                    break;
                }
                
                if ( final_command_length < array_count( final_command ) ) {
                    
                    final_command[ final_command_length++ ] = *command_end;
                    command_end++;
                    
                } else {
                    
                    data_t error = string_l( "Final command buffer doesn't contain enough space.\n" );
                    terminal_error_p( error.text, error.used );
                    _exit( TERMINAL_EXECUTE_ERROR_FINALCOMMAND_LENGTH );
                }
            }
            
            umm path_command_length = cast( u8*, command_end ) - cast( u8*, command.bytes );
            u32 file_error = 0;
            
            if ( !file_exists_p( final_command, final_command_length, &file_error ) && path_command_length < array_count( final_command ) ) {
                string_copy( final_command, command.bytes, path_command_length );
                final_command[ path_command_length ] = 0;
            }
            
            if ( ( !working_directory.used || chdir( cast( const char*, working_directory.bytes ) ) == 0 ) ) {
                
                char* c = command_end;
                char* end = cast( char*, command.bytes ) + command_length;
                
                char* arguments[ 128 ] = { 0 };
                umm argument_count = 0;
                b32 in_quote = false;
                arguments[ argument_count++ ] = cast( char*, command.bytes );
                
                while ( c < end && *c != 0 && argument_count < array_count( arguments ) - 1 ) {
                    
                    if ( *c == '\"' ) {
                        
                        in_quote = !in_quote;
                        
                    } else {
                        
                        if ( !in_quote && *c == ' ' ) {
                            
                            *c = 0;
                            char* n = c + 1;
                            
                            if ( n < end && *n != 0 ) {
                                arguments[ argument_count++ ] = n;
                            }
                        }
                    }
                    
                    c++;
                }
                
                if ( argument_count < array_count( arguments ) ) {
                    
                    arguments[ argument_count++ ] = 0;
                    
                    if ( execvp( final_command, arguments ) == -1 ) {
                        data_t error = string_l( "exec returned -1. Which probably means that the command doesn't exists. Check errno.\n" );
                        terminal_error_p( error.text, error.used );
                        _exit( TERMINAL_EXECUTE_ERROR_EXEC );
                    }
                    
                } else {
                    
                    data_t error = string_l( "Not enough space for arguments.\n" );
                    terminal_error_p( error.text, error.used );
                    _exit( TERMINAL_EXECUTE_ERROR_ARGUMENT_COUNT );
                }
                
            } else {
                
                data_t error = string_l( "chdir returned an error.\n" );
                terminal_error_p( error.text, error.used );
                _exit( TERMINAL_EXECUTE_ERROR_CHDIR );
            }
            
        } else {
            
            data_t error = string_l( "getcwd returned NULL. Does the buffer contains enough space ?\n" );
            terminal_error_p( error.text, error.used );
            _exit( TERMINAL_EXECUTE_ERROR_GETCWD );
        }
        
    } else if ( pid > 0 ) {
        
        if ( should_wait ) {
            
            s32 status;
            waitpid( pid, &status, 0 );
            
            if ( WIFEXITED( status ) ) {
                
                result = 1;
                
                if ( return_value ) {
                    *return_value = WEXITSTATUS( status );
                }
            }
            
        } else {
            
            result = 1;
        }
    }
    
    return result;
    
#undef TERMINAL_EXECUTE_ERROR_FINALCOMMAND_LENGTH
#undef TERMINAL_EXECUTE_ERROR_GETCWD
#undef TERMINAL_EXECUTE_ERROR_CHDIR
#undef TERMINAL_EXECUTE_ERROR_ARGUMENT_COUNT
#undef TERMINAL_EXECUTE_ERROR_EXEC
}

stu b32 process_execute( data_t command, data_t working_directory, b32 should_wait, u32* return_value ) {
    
    b32 result = process_execute_p( command.bytes, command.used, working_directory.bytes, working_directory.used, should_wait, return_value );
    
    return result;
}

#else
# error Unsupported platfrom.
#endif

// MARK encoding

/* NOTE simon:
http://www.unicode.org/reports/tr44/tr44-22.html#General_Category_Values
ftp://ftp.unicode.org/Public/UNIDATA/
*/

/* NOTE simon: from UnicodeData.txt with Cc as the general type. */

#define raw_unicode_control_characters \
0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f, \
0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f, \
0x007f, \
0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087, 0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f, \
0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097, 0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f

u32 unicode_control_characters[ ] = { raw_unicode_control_characters };
_ctassert( array_count( unicode_control_characters ) == 65 );

/* NOTE simon: from PropList.txt with Cn && Noncharacter_Code_Point */

#define raw_unicode_non_characters \
/* NOTE simon: non characters in the BMP. */ \
0xfdd0, 0xfdd1, 0xfdd2, 0xfdd3,0xfdd4, 0xfdd5, 0xfdd6, 0xfdd7, 0xfdd8, 0xfdd9, 0xfdda, 0xfddb, 0xfddc, 0xfddd, 0xfdde, 0xfddf, \
0xfde0, 0xfde1, 0xfde2, 0xfde3,0xfde4, 0xfde5, 0xfde6, 0xfde7, 0xfde8, 0xfde9, 0xfdea, 0xfdeb, 0xfdec, 0xfded, 0xfdee, 0xfdef, \
/* NOTE simon: last two codepoints of each plane. */ \
0xfffe, 0xffff, \
0x1fffe, 0x1ffff, \
0x2fffe, 0x2ffff, \
0x3fffe, 0x3ffff, \
0x4fffe, 0x4ffff, \
0x5fffe, 0x5ffff, \
0x6fffe, 0x6ffff, \
0x7fffe, 0x7ffff, \
0x8fffe, 0x8ffff, \
0x9fffe, 0x9ffff, \
0xafffe, 0xaffff, \
0xbfffe, 0xbffff, \
0xcfffe, 0xcffff, \
0xdfffe, 0xdffff, \
0xefffe, 0xeffff, \
0xffffe, 0xfffff, \
0x10fffe, 0x10ffff

u32 unicode_non_characters[ ] = { raw_unicode_non_characters };
_ctassert( array_count( unicode_non_characters ) == 66 );
#if 0
u32 encoding_windows_1252_to_unicode[ ] = {
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x20ac, 0x0, 0x201a, 0x192, 0x201e, 0x2026, 0x2020, 0x2021, 0x2c6, 0x2030, 0x160, 0x2039, 0x152, 0x0, 0x17d, 0x0,
    0x0, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, 0x02dc, 0x2122, 0x161, 0x203a, 0x153, 0x0, 0x17e, 0x178,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

u32 encoding_iso_8859_15_to_unicode[ ] = {
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0xa0, 0xa1, 0xa2, 0xa3, 0x20ac, 0xa5, 0x160, 0xa7, 0x161, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
    0xb0, 0xb1, 0xb2, 0xb3, 0x17d, 0xb5, 0xb6, 0xb7, 0x17e, 0xb9, 0xba, 0xbb, 0x152, 0x153, 0x178, 0xbf,
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

_ctassert( array_count( encoding_windows_1252_to_unicode ) == 256 );
_ctassert( array_count( encoding_iso_8859_15_to_unicode ) == 256 );
#endif

// MARK log

#if defined( LOG )

typedef HANDLE thread_mutex_t;
void thread_mutex_create( thread_mutex_t* mutex );
void thread_mutex_release( thread_mutex_t* mutex );
void thread_mutex_wait( thread_mutex_t* mutex );

typedef struct log_memory_t {
    data_t memory;
    struct log_memory_t* previous;
    struct log_memory_t* next;
} log_memory_t;

typedef struct log_t {
    data_t storage;
    thread_mutex_t mutex;
    log_memory_t sentinel;
    log_memory_t* head;
    log_memory_t* tail;
} log_t;

log_t g_log = { 0 };

stu b32 log_grow( umm size ) {
    
    b32 success = false;
    log_memory_t* log_memory = memory_push_struct_aligned( &g_log.storage, log_memory_t, sizeof( umm ) );
    
    if ( log_memory ) {
        
        if ( size <= mebioctet( 1 ) ) {
            size = mebioctet( 1 );
        } else {
            umm mega = size / mebioctet( 1 );
            mega++;
            size = mega * mebioctet( 1 );
        }
        
        data_t memory = memory_get_virtual( size, 0 );
        
        if ( memory.bytes ) {
            log_memory->memory = memory;
            doubly_linked_list_insert_last( g_log.head, g_log.tail, log_memory, previous, next ); 
            success = true;
        } else {
            memory_pop_struct( &g_log.storage, log_memory_t );
        }
    }
    
    return success;
}

stu void log_initialize( void ) {
    
    if ( !g_log.head ) {
        thread_mutex_create( &g_log.mutex );
        g_log.storage = memory_get_virtual( mebioctet( 1 ), 0 );
        doubly_linked_list_initialize( g_log.head, g_log.tail, &g_log.sentinel, previous, next );
        log_grow( mebioctet( 1 ) );
    }
}

stu data_t log_get_buffer( umm length ) {
    
    data_t result = { 0 };
    
    if ( g_log.mutex ) {
        
        thread_mutex_wait( &g_log.mutex );
        
        data_t* memory = &g_log.tail->previous->memory;
        
        if ( memory_left( deref( memory ) ) < length ) {
            log_grow( length );
        }
        
        memory = &g_log.tail->previous->memory;
        result.bytes = memory_push_size( memory, length, 1 );
        
        if ( result.bytes ) {
            result.reserved = length;
        }
        
        thread_mutex_release( &g_log.mutex );
    }
    
    return result;
}

stu data_t log_p( u8* bytes, umm length ) {
    
    data_t result = log_get_buffer( length );
    
    if ( result.reserved ) {
        memory_push_copy_p( &result, bytes, length, 1 ); 
    }
    
    return result;
}

stu data_t log_indent( umm level ) {
    
    data_t indent = string_l( "    " );
    data_t result = log_get_buffer( level * indent.used );
    
    if ( result.reserved ) {
        for ( umm i = 0; i < level; i++ ) {
            memory_push_copy( &result, indent, 1 );
        }
    }
    
    return result;
}

#define log_l( literal ) log_p( ( u8* ) ( literal ), sizeof( literal ) - 1 )
#define log_d( data ) log_p( ( data ).bytes, ( data ).used )

stu data_t log_u64_format( u64 n, umm desired_digit_count, u8 fill_with, u32 hexadecimal_format ) {
    
    memory_get_on_stack( temp, 32 );
    string_push_u64( &temp, n, desired_digit_count, fill_with, hexadecimal_format );
    data_t result = log_d( temp );
    
    return result;
}

#define log_u64( n ) log_u64_format( ( n ), 0, 0, 0 )

stu data_t log_s64_format( s64 n, umm desired_digit_count, u8 fill_with, u32 hexadecimal_format ) {
    
    memory_get_on_stack( temp, 32 );
    string_push_s64( &temp, n, desired_digit_count, fill_with, hexadecimal_format );
    data_t result = log_d( temp );
    
    return result;
}

#define log_s64( n ) log_s64_format( ( n ), 0, 0, 0 )

stu data_t log_f32_format( f32 n, s32 before_decimal_point, u8 fill_with, s32 after_decimal_point, u32 format ) {
    
    memory_get_on_stack( temp, kibioctet( 1 ) );
    string_push_f32( &temp, n, before_decimal_point, fill_with, after_decimal_point, format );
    data_t result = log_d( temp );
    
    return result;
}

#define log_f32( n ) log_f32_format( ( n ), 0, 0, 0, 0 )

stu data_t log_f64_format( f64 n, s32 before_decimal_point, u8 fill_with, s32 after_decimal_point, u32 format ) {
    
    memory_get_on_stack( temp, kibioctet( 1 ) );
    string_push_f64( &temp, n, 0, 0, 0, 0 );
    data_t result = log_d( temp );
    
    return result;
}

#define log_f64( n ) log_f64_format( ( n ), 0, 0, 0, 0 )

stu data_t log_date( date_time_t date ) {
    
    data_t result = { 0 };
    
    memory_get_on_stack( temp, 4 + 1 + 2 + 1 + 2 + 3 + 2 + 1 + 2 + 1 + 2 );
    
    string_push_u64( &temp, date.year, 4, ' ', 0 );
    memory_push_u8( &temp, '/', 1 );
    string_push_u64( &temp, date.month, 2, ' ', 0 );
    memory_push_u8( &temp, '/', 1 );
    string_push_u64( &temp, date.day, 2, ' ', 0 );
    memory_push_copy_l( &temp, " - " );
    string_push_u64( &temp, date.hour, 2, '0', 0 );
    memory_push_u8( &temp, ':', 1 );
    string_push_u64( &temp, date.minute, 2, '0', 0 );
    memory_push_u8( &temp, ':', 1 );
    string_push_u64( &temp, date.second, 2, '0', 0 );
    
    log_d( temp );
    
    return result;
}

#define log_date_current( ) log_date( date_get_current( ) )

stu b32 log_is_empty( void ) {
    b32 result = ( !g_log.head || g_log.tail->previous->memory.used == 0 );
    return result;
}

stu void log_to_file( data_t filename ) {
    
    if ( !log_is_empty( ) ) {
        
        file_handle_t* file = file_open( filename, file_mode_write, true, &g_file_error );
        
        log_memory_t* log_memory = g_log.head->next;
        umm position = 0;
        
        while ( log_memory != &g_log.sentinel ) {
            
            position += file_write_handle( file, position, log_memory->memory.used, log_memory->memory.bytes, &g_file_error );
            log_memory = log_memory->next;
        }
        
        file_close( file, &g_file_error );
    }
}

#if defined( WINDOWS )

stu data_t log_windows_error_message( u32 error ) {
    
    data_t result = { 0 };
    
    u16 utf16[ 1024 ];
    u32 flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    u32 utf16_codeunit_count = FormatMessageW( flags, 0, error, 0, utf16, array_count( utf16 ), 0 );
    
    u8 utf8[ array_count( utf16 ) * 4 ];
    u32 convertion_error = 0;
    umm utf8_codeunit_count = utf16_to_utf8_p( utf16, utf16_codeunit_count, utf8, array_count( utf8 ), &convertion_error );
    result = log_p( utf8, utf8_codeunit_count );
    
    return result;
}

#else /* defined( WINDOWS ) */
# define log_windows_error_message( ... )
#endif /* defined( WINDOWS ) */

u8 debug_buffer_memory[ 2048 ] = { 0 };
data_t debug_buffer = { array_count( debug_buffer_memory ), 0, debug_buffer_memory };

stu void debug_d( data_t string ) {
    
    string_zero_terminate_on_stack( out, 4096, string.bytes, string.used );
    OutputDebugStringA( ( LPCSTR ) out.bytes );
}

#define debug_l( literal ) OutputDebugStringA( ( LPCSTR ) ( literal ) )

stu void debug_timestamp_d( data_t string ) {
    
    static u64 previous_timestamp = 0;
    static u64 frequency = 0;
    u64 timestamp;
    perf_get_time( &timestamp );
    
    if ( previous_timestamp == 0 ) {
        previous_timestamp = timestamp;
        perf_get_frequency( &frequency );
    }
    
    f64 time = perf_get_elapsed_time( previous_timestamp, timestamp, frequency );
    // previous_timestamp = timestamp;
    
    u32 hours = cast( u32, time / ( 60 * 60 ) );
    u32 minutes = cast( u32, ( time - ( hours * 60 * 60 ) ) / 60 );
    u32 seconds = cast( u32, time - ( hours * 60 * 60 + minutes * 60 ) );
    u32 micro_seconds = cast( u32, ( time - ( hours * 60 * 60 + minutes * 60 + seconds ) ) * 1000000 );
    
    memory_get_on_stack( temp, kibioctet( 1 ) );
    memory_push_u8( &temp, '[', 1 );
    string_push_u64( &temp, hours, 2, '0', 0 ); 
    memory_push_u8( &temp, ':', 1 );
    string_push_u64( &temp, minutes, 2, '0', 0 ); 
    memory_push_u8( &temp, ':', 1 );
    string_push_u64( &temp, seconds, 2, '0', 0 );
    memory_push_u8( &temp, ':', 1 );
    string_push_u64( &temp, micro_seconds, 7, '0', string_format_flag_decimal_space_after_3_digits ); 
    memory_push_copy_l( &temp, "] " );
    memory_push_copy( &temp, string, 1 );
    string_zero_terminate( temp );
    OutputDebugStringA( ( LPCSTR ) temp.bytes );
}

#define debug_timestamp_l( string ) debug_timestamp_d( string_l( string ) )

stu void debug_u64( u64 n, b32 new_line ) {
    
    memory_get_on_stack( out, 32 );
    string_push_u64( &out, n, 0, 0, 0 );
    
    if ( new_line ) {
        memory_push_u8( &out, '\n', 1 );
    }
    
    memory_push_u8( &out, 0, 1 );
    OutputDebugStringA( ( LPCSTR ) out.bytes );
}

stu void debug_s64( s64 n, b32 new_line ) {
    
    memory_get_on_stack( out, 32 );
    string_push_s64( &out, n, 0, 0, 0 );
    
    if ( new_line ) {
        memory_push_u8( &out, '\n', 1 );
    }
    
    memory_push_u8( &out, 0, 1 );
    OutputDebugStringA( ( LPCSTR ) out.bytes );
}

stu void debug_f32( f32 n, b32 new_line ) {
    
    memory_get_on_stack( out, kibioctet( 1 ) );
    string_push_f32( &out, n, 0, 0, 6, 0 );
    
    if ( new_line ) {
        memory_push_u8( &out, '\n', 1 );
    }
    
    memory_push_u8( &out, 0, 1 );
    OutputDebugStringA( ( LPCSTR ) out.bytes );
}

stu void debug_f64( f64 n, b32 new_line ) {
    
    memory_get_on_stack( out, kibioctet( 1 ) );
    string_push_f64( &out, n, 0, 0, 10, 0 );
    
    if ( new_line ) {
        memory_push_u8( &out, '\n', 1 );
    }
    
    memory_push_u8( &out, 0, 1 );
    OutputDebugStringA( ( LPCSTR ) out.bytes );
}

#else /* defined( LOG ) */

#define log_grow( ... )
#define log_initialize( ... )
#define log_get_buffer( ... ) { 0 }
#define log_p( ... )
#define log_l( ... )
#define log_d( ... )
#define log_u64_params( ... )
#define log_u64( ... )
#define log_s64_params( ... )
#define log_s64( ... )
#define log_f32_params( ... )
#define log_f32( ... )
#define log_f64_params( ... )
#define log_f64( ... )
#define log_date( ... )
#define log_date_current( ... )
#define log_to_file( ... )
#define log_windows_error_message( ... )

#endif

#define COMMON_H
#endif
