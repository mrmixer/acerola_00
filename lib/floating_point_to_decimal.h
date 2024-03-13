
#if !defined( FLOATING_POINT_TO_DECIMAL_H )

/* NOTE simon:
 Define FLOATING_POINT_TO_DECIMAL_USE_COMPRESSED_CACHE to save space in the binary file at the expense of some performance.
*/

#include <stdint.h>

#if defined( _MSC_VER )
#include <intrin.h>
#endif

/* NOTE simon: single translation unit*/
#define fptd_stu static

typedef struct fptd_u128_t {
    uint64_t low, high;
} fptd_u128_t;

fptd_stu fptd_u128_t fptd_math_u64_multiply( uint64_t a, uint64_t b ) {
    
    fptd_u128_t result;
#if defined( __SIZEOF_INT128__ )
    unsigned __int128 r = ( unsigned __int128 ) a * ( unsigned __int128 ) b;
    result.low = ( uint64_t ) r;
    result.high = ( uint64_t ) ( r >> 64 );
#else
    result.low = _umul128( a, b, &result.high );
#endif
    
    return result;
}

/* NOTE simon: Multiply two 64bits with 128 precision but return only the high bits of the 128bits.*/
fptd_stu uint64_t fptd_math_u64_multiply_high( uint64_t a, uint64_t b ) {
    
#if defined( __SIZEOF_INT128__ )
    unsigned __int128 r = ( unsigned __int128 ) a * ( unsigned __int128 ) b;
    
    uint64_t result = ( uint64_t ) ( r >> 64 );
#else
    uint64_t result = __umulh( a, b );
#endif
    return result;
}

fptd_stu fptd_u128_t fptd_math_u128_add_u64( fptd_u128_t a, uint64_t b ) {
    
    fptd_u128_t result;
    
#if 1
    result.low = a.low + b;
    result.high = a.high + ( ( result.low < a.low ) ? 1 : 0 );
#else
    u8 carry = _addcarry_u64( 0, a.low, b, &result.low );
    _addcarry_u64( carry, a.high, 0, &result.high );
#endif
    
    return result;
}

fptd_stu fptd_u128_t fptd_math_multiply_u64_u128_high( uint64_t a, fptd_u128_t b ) {
    
    fptd_u128_t result = fptd_math_u64_multiply( a, b.high );
    uint64_t temp = fptd_math_u64_multiply_high( a, b.low );
    result = fptd_math_u128_add_u64( result, temp );
    
    return result;
}

fptd_stu fptd_u128_t fptd_math_multiply_u64_u128_low( uint64_t a, fptd_u128_t b ) {
    
    fptd_u128_t result = fptd_math_u64_multiply( a, b.low );
    result.high += ( a * b.high );
    
    return result;
}

fptd_stu uint64_t fptd_math_u32_multiply( uint32_t a, uint32_t b ) {
    
#if 1
    u64 result = __emulu( a, b );
#else
    u64 result = ( u64 ) a * ( u64 ) b;
#endif
    
    return result;
}

fptd_stu uint32_t fptd_math_rotate_right_u32( uint32_t value, uint8_t bit_count ) {
    uint32_t result = ( value >> bit_count ) | ( value << ( 32 - bit_count ) );
    return result;
}

fptd_stu uint64_t fptd_math_rotate_right_u64( uint64_t value, uint8_t bit_count ) {
    uint64_t result = ( value >> bit_count ) | ( value << ( 64 - bit_count ) );
    return result;
}

fptd_stu uint64_t fptd_math_multiply_u32_u64_high( uint32_t a, uint64_t b ) {
    uint64_t result = fptd_math_u64_multiply_high( ( ( uint64_t ) a ) << 32, b );
    return result;
}

#define s32_floor_log10_pow2_31 9
#define s32_case_shorter_interval_left_endpoint_lower_threshold 2
#define s32_case_shorter_interval_left_endpoint_upper_threshold 3
#define s32_case_shorter_interval_right_endpoint_lower_threshold 0
#define s32_case_shorter_interval_right_endpoint_upper_threshold 3

fptd_stu int32_t fptd_log_compute( int32_t exponent, int32_t multiply, int32_t subtract, int32_t shift, int32_t min_exponent, int32_t max_exponent ) {
    _assert( min_exponent <= exponent && max_exponent >= exponent );
    int32_t result = ( ( exponent * multiply ) - subtract ) >> shift;
    return result;
}

fptd_stu int32_t fptd_floor_log10_pow2( int32_t exponent ) {
    int32_t result = fptd_log_compute( exponent, 315653, 0, 20, -2620, 2620 );
    return result;
}

fptd_stu int32_t fptd_floor_log2_pow10( int32_t exponent ) {
    int32_t result = fptd_log_compute( exponent, 1741647, 0, 19, -1233, 1233 );
    return result;
}

fptd_stu int32_t fptd_floor_log10_pow2_minus_log10_4_over_3( int32_t exponent ) {
    int32_t result = fptd_log_compute( exponent, 631305, 261663, 21, -2985, 2936 );
    return result;
}

fptd_stu uint8_t fptd_is_right_endpoint_integer_shorter_interval( int32_t exponent ) {
    uint8_t result = ( exponent >= s32_case_shorter_interval_right_endpoint_lower_threshold ) && ( exponent <= s32_case_shorter_interval_right_endpoint_upper_threshold );
    return result;
}

fptd_stu uint8_t fptd_is_left_endpoint_integer_shorter_interval( int32_t exponent ) {
    uint8_t result = ( exponent >= s32_case_shorter_interval_left_endpoint_lower_threshold ) && ( exponent <= s32_case_shorter_interval_left_endpoint_upper_threshold );
    return result;
}

/* NOTE simon: Can only use 1 flag from each category at a time. */
typedef enum floating_point_to_decimal_flags_t {
    
    floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_to_even = 0 << 0,
    floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_to_odd = 1 << 0,
    floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_toward_plus_infinity = 1 << 1,
    floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_toward_minus_infinity = 1 << 2,
    floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_toward_zero = 1 << 3,
    floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_away_from_zero = 1 << 4,
    
    floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_to_even_static_boundary = 1 << 5,
    floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_to_odd_static_boundary = 1 << 6,
    floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_toward_plus_infinity_static_boundary = 1 << 7,
    floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_toward_minus_infinity_static_boundary = 1 << 8,
    
    floating_point_to_decimal_flags_decimal_to_binary_rounding_toward_plus_infinity = 1 << 9,
    floating_point_to_decimal_flags_decimal_to_binary_rounding_toward_minus_infinity = 1 << 10,
    floating_point_to_decimal_flags_decimal_to_binary_rounding_toward_zero = 1 << 11,
    floating_point_to_decimal_flags_decimal_to_binary_rounding_away_from_zero = 1 << 12,
    
    floating_point_to_decimal_flags_decimal_to_binary_rounding_mask = 0x1fff,
    
    floating_point_to_decimal_flags_binary_to_decimal_rounding_to_even = 0 << 13,
    floating_point_to_decimal_flags_binary_to_decimal_rounding_do_not_care = 1 << 13,
    floating_point_to_decimal_flags_binary_to_decimal_rounding_to_odd = 1 << 14,
    floating_point_to_decimal_flags_binary_to_decimal_rounding_away_from_zero = 1 << 15,
    floating_point_to_decimal_flags_binary_to_decimal_rounding_toward_zero = 1 << 16,
    
    floating_point_to_decimal_flags_binary_to_decimal_rounding_mask = 0x1e000,
    
    floating_point_to_decimal_flags_trailing_zero_remove = 0 << 17,
    floating_point_to_decimal_flags_trailing_zero_ignore = 1 << 17,
    floating_point_to_decimal_flags_trailing_zero_report = 1 << 18,
    
    floating_point_to_decimal_flags_trailing_zero_mask = 0x60000,
    
} floating_point_to_decimal_flags_t;

typedef enum fptd_interval_tag_t {
    fptd_interval_tag_to_nearest,
    fptd_interval_tag_left_closed_directed,
    fptd_interval_tag_right_closed_directed,
} fptd_interval_tag_t;

typedef struct fptd_interval_t {
    uint8_t include_left_endpoint;
    uint8_t include_right_endpoint;
} fptd_interval_t;

typedef struct fptd_interval_provider_t {
    uint32_t tag;
    fptd_interval_t normal;
    fptd_interval_t shorter;
} fptd_interval_provider_t;

fptd_stu fptd_interval_provider_t fptd_get_interval_provider( uint8_t has_even_significand_bits, uint8_t is_negative, uint32_t flags ) {
    
#define interval_symmetric_boundary( is_closed ) ( fptd_interval_t ) { ( is_closed ), ( is_closed ) }
#define interval_asymmetric_boundary( is_left_closed ) ( fptd_interval_t ) { ( is_left_closed ), !( is_left_closed ) }
#define interval_closed ( fptd_interval_t ) { 1, 1 }
#define interval_open ( fptd_interval_t ) { 0, 0 }
#define interval_left_closed_right_open ( fptd_interval_t ) { 1, 0 }
#define interval_right_closed_left_open ( fptd_interval_t ) { 0, 1 }
    
    fptd_interval_provider_t result = { 0 };
    uint32_t rounding = flags & floating_point_to_decimal_flags_decimal_to_binary_rounding_mask;
    
    switch ( rounding ) {
        
        case floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_to_even: {
            result.tag = fptd_interval_tag_to_nearest;
            uint8_t is_closed = has_even_significand_bits;
            result.normal = interval_symmetric_boundary( is_closed );
            result.shorter = interval_closed;
        } break;
        
        case floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_to_odd: {
            result.tag = fptd_interval_tag_to_nearest;
            uint8_t is_left_closed = !has_even_significand_bits;
            result.normal = interval_symmetric_boundary( is_left_closed );
            result.shorter = interval_open;
        } break;
        
        case floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_toward_plus_infinity: {
            result.tag = fptd_interval_tag_to_nearest;
            uint8_t is_left_closed = !is_negative;
            result.normal = interval_asymmetric_boundary( is_left_closed );
            result.shorter = interval_asymmetric_boundary( is_left_closed );
        } break;
        
        case floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_toward_minus_infinity: {
            result.tag = fptd_interval_tag_to_nearest;
            uint8_t is_left_closed = is_negative;
            result.normal = interval_asymmetric_boundary( is_left_closed );
            result.shorter = interval_asymmetric_boundary( is_left_closed );
        } break;
        
        case floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_toward_zero: {
            result.tag = fptd_interval_tag_to_nearest;
            result.normal = interval_right_closed_left_open;
            result.shorter = interval_right_closed_left_open;
        } break;
        
        case floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_away_from_zero: {
            result.tag = fptd_interval_tag_to_nearest;
            result.normal = interval_left_closed_right_open;
            result.shorter = interval_left_closed_right_open;
        } break;
        
        case floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_to_even_static_boundary: {
            
            result.tag = fptd_interval_tag_to_nearest;
            
            if ( has_even_significand_bits ) {
                result.normal = interval_closed;
                result.shorter = interval_closed;
            } else {
                result.normal = interval_open;
                result.shorter = interval_open;
            }
            
        } break;
        
        case floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_to_odd_static_boundary: {
            
            result.tag = fptd_interval_tag_to_nearest;
            
            if ( has_even_significand_bits ) {
                result.normal = interval_open;
                result.shorter = interval_open;
            } else {
                result.normal = interval_closed;
                result.shorter = interval_closed;
            }
            
        } break;
        
        case floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_toward_plus_infinity_static_boundary: {
            
            result.tag = fptd_interval_tag_to_nearest;
            
            if ( is_negative ) {
                result.normal = interval_right_closed_left_open;
                result.shorter = interval_right_closed_left_open;
            } else {
                result.normal = interval_left_closed_right_open;
                result.shorter = interval_left_closed_right_open;
            }
            
        } break;
        
        case floating_point_to_decimal_flags_decimal_to_binary_rounding_nearest_toward_minus_infinity_static_boundary: {
            
            result.tag = fptd_interval_tag_to_nearest;
            
            if ( is_negative ) {
                result.normal = interval_left_closed_right_open;
                result.shorter = interval_left_closed_right_open;
            } else {
                result.normal = interval_right_closed_left_open;
                result.shorter = interval_right_closed_left_open;
            }
            
        } break;
        
        case floating_point_to_decimal_flags_decimal_to_binary_rounding_toward_plus_infinity: {
            
            if ( is_negative ) {
                result.tag = fptd_interval_tag_left_closed_directed;
            } else {
                result.tag = fptd_interval_tag_right_closed_directed;
            }
            
        } break;
        
        case floating_point_to_decimal_flags_decimal_to_binary_rounding_toward_minus_infinity: {
            
            if ( is_negative ) {
                result.tag = fptd_interval_tag_right_closed_directed;
            } else {
                result.tag = fptd_interval_tag_left_closed_directed;
            }
            
        } break;
        
        case floating_point_to_decimal_flags_decimal_to_binary_rounding_toward_zero: {
            result.tag = fptd_interval_tag_left_closed_directed;
        } break;
        
        case floating_point_to_decimal_flags_decimal_to_binary_rounding_away_from_zero: {
            result.tag = fptd_interval_tag_right_closed_directed;
        } break;
        
        default: {
            invalid_code_path( );
        } break;
    }
    
    return result;
    
#undef interval_symmetric_boundary
#undef interval_asymmetric_boundary
#undef interval_closed
#undef interval_open
#undef interval_left_closed_right_open
#undef interval_right_closed_left_open
}

fptd_stu uint8_t fptd_binary_to_decimal_prefer_round_down( uint8_t has_even_significand_bits, uint32_t flags ) {
    
    uint8_t result = 0;
    
    uint32_t rounding = flags & floating_point_to_decimal_flags_binary_to_decimal_rounding_mask;
    
    switch ( rounding ) {
        
        case floating_point_to_decimal_flags_binary_to_decimal_rounding_do_not_care: {
            result = 0;
        } break;
        
        case floating_point_to_decimal_flags_binary_to_decimal_rounding_to_even: {
            result = !has_even_significand_bits;
        } break;
        
        case floating_point_to_decimal_flags_binary_to_decimal_rounding_to_odd: {
            result = has_even_significand_bits;
        } break;
        
        case floating_point_to_decimal_flags_binary_to_decimal_rounding_away_from_zero: {
            result = 0;
        } break;
        
        case floating_point_to_decimal_flags_binary_to_decimal_rounding_toward_zero: {
            result = 1;
        } break;
        
        default: {
            invalid_code_path( );
        } break;
    }
    
    return result;
}

#if !defined( FLOATING_POINT_TO_DECIMAL_NO_32 )

#define kappa_32 1

#define fptd_u32_max 0xffffffff

#define significand_bit_count_32 23
// #define exponent_bit_count_32 8
#define min_exponent_32 -126
// #define max_exponent_32 127
#define exponent_bias_32 -127
// #define decimal_digits_32 9
#define sign_mask_32 0x80000000u
#define exponent_mask_32 0x7f800000u
#define significand_mask_32 0x7fffffu

// #define s32_floor_log2_pow10_kappa_plus_1_32 6
#define u32_power_10_kappa_32 10
#define u32_power_10_kappa_plus_1_32 100

#define s32_divisibility_check_by_5_threshold_32 39
#define s32_case_fc_pm_half_lower_threshold_32 -1
#define s32_shorter_interval_tie_lower_threshold_32 -35
#define s32_shorter_interval_tie_upper_threshold_32 -35

#define carrier_bit_count_32 32
// _ctassert( carrier_bit_count_32 >= significand_bit_count_32 + 2 + s32_floor_log2_pow10_kappa_plus_1_32 );

uint8_t floating_point_to_decimal_is_nonzero_32( uint32_t f ) {
    uint8_t result = ( f & ( ~sign_mask_32 ) ) != 0;
    return result;
}

uint8_t floating_point_to_decimal_is_positive_32( uint32_t f ) {
    uint8_t result = ( f < sign_mask_32 );
    return result;
}

uint8_t floating_point_to_decimal_is_negative_32( uint32_t f ) {
    uint8_t result = ( f >= sign_mask_32 );
    return result;
}

uint8_t floating_point_to_decimal_is_finite_32( float f ) {
    
    uint32_t u32_f = *( uint32_t* ) &f;
    int32_t exponent = ( int32_t ) ( ( u32_f & exponent_mask_32 ) >> significand_bit_count_32 );
    uint8_t result = ( exponent != 0xff );
    
    return result;
}

fptd_stu uint8_t fptd_has_even_significand_bits_32( uint32_t signed_significand ) {
    uint8_t result = ( ( signed_significand % 2 ) == 0 );
    return result;
}

typedef struct floating_point_decimal_32_t {
    
    int32_t exponent;
    uint32_t significand;
    uint8_t is_negative;
    uint8_t may_have_trailing_zeroes;
    
} floating_point_decimal_32_t;

fptd_stu uint8_t fptd_check_divisibility_and_divide_by_pow10_32( uint32_t* n ) {
    
    uint8_t result = 0;
    
    _ctassert( kappa_32 + 1 <= s32_floor_log10_pow2_31 );
    _assert( ( *n ) <= u32_power_10_kappa_plus_1_32 );
    
    ( *n ) *= 6554;
    uint32_t mask = ( 1u << 16 ) - 1;
    result = ( ( ( *n ) & mask ) < 6554 );
    ( *n ) = ( *n ) >> 16;
    
    return result;
}

fptd_stu uint32_t fptd_small_division_by_pow10_32( uint32_t n ) {
    
    _ctassert( kappa_32 + 1 <= s32_floor_log10_pow2_31 );
    _assert( n <= u32_power_10_kappa_plus_1_32 );
    
    uint32_t result = ( n * 6554 ) >> 16;
    
    return result;
}

fptd_stu uint32_t fptd_divide_by_pow10_32( uint32_t n, int32_t kappa, uint32_t n_max ) {
    
    _assert( kappa == 2 );
    uint32_t result = ( uint32_t ) ( fptd_math_u32_multiply( n, 1374389535u ) >> 37 );
#if 0
    /* NOTE simon: If this function gets called with kappa != 2 we should do what's below. */
    else {
        uint32_t divisor = fptd_compute_power_32( 10, kappa );
        result /= divisor;
    }
#endif
    
    return result;
}

#define cache_min_k_32 -31
#define cache_max_k_32 46
#define cache_bit_count_32 64

fptd_stu uint64_t fptd_cache_32[ ] = {
    0x81ceb32c4b43fcf5, 0xa2425ff75e14fc32, 0xcad2f7f5359a3b3f, 0xfd87b5f28300ca0e,
    0x9e74d1b791e07e49, 0xc612062576589ddb, 0xf79687aed3eec552, 0x9abe14cd44753b53,
    0xc16d9a0095928a28, 0xf1c90080baf72cb2, 0x971da05074da7bef, 0xbce5086492111aeb,
    0xec1e4a7db69561a6, 0x9392ee8e921d5d08, 0xb877aa3236a4b44a, 0xe69594bec44de15c,
    0x901d7cf73ab0acda, 0xb424dc35095cd810, 0xe12e13424bb40e14, 0x8cbccc096f5088cc,
    0xafebff0bcb24aaff, 0xdbe6fecebdedd5bf, 0x89705f4136b4a598, 0xabcc77118461cefd,
    0xd6bf94d5e57a42bd, 0x8637bd05af6c69b6, 0xa7c5ac471b478424, 0xd1b71758e219652c,
    0x83126e978d4fdf3c, 0xa3d70a3d70a3d70b, 0xcccccccccccccccd, 0x8000000000000000,
    0xa000000000000000, 0xc800000000000000, 0xfa00000000000000, 0x9c40000000000000,
    0xc350000000000000, 0xf424000000000000, 0x9896800000000000, 0xbebc200000000000,
    0xee6b280000000000, 0x9502f90000000000, 0xba43b74000000000, 0xe8d4a51000000000,
    0x9184e72a00000000, 0xb5e620f480000000, 0xe35fa931a0000000, 0x8e1bc9bf04000000,
    0xb1a2bc2ec5000000, 0xde0b6b3a76400000, 0x8ac7230489e80000, 0xad78ebc5ac620000,
    0xd8d726b7177a8000, 0x878678326eac9000, 0xa968163f0a57b400, 0xd3c21bcecceda100,
    0x84595161401484a0, 0xa56fa5b99019a5c8, 0xcecb8f27f4200f3a, 0x813f3978f8940985,
    0xa18f07d736b90be6, 0xc9f2c9cd04674edf, 0xfc6f7c4045812297, 0x9dc5ada82b70b59e,
    0xc5371912364ce306, 0xf684df56c3e01bc7, 0x9a130b963a6c115d, 0xc097ce7bc90715b4,
    0xf0bdc21abb48db21, 0x96769950b50d88f5, 0xbc143fa4e250eb32, 0xeb194f8e1ae525fe,
    0x92efd1b8d0cf37bf, 0xb7abc627050305ae, 0xe596b7b0c643c71a, 0x8f7e32ce7bea5c70,
    0xb35dbf821ae4f38c, 0xe0352f62a19e306f
};

fptd_stu uint64_t fptd_get_cache_32( int32_t k ) {
    _assert( k >= cache_min_k_32 && k <= cache_max_k_32 );
    uint64_t result = fptd_cache_32[ k - cache_min_k_32 ];
    return result;
}

fptd_stu int32_t fptd_remove_trailing_zeroes_32( uint32_t* n ) {
    
    _assert( ( *n ) != 0 );
    
#define mod_inv_5 0xcccccccdu
#define mod_inv_25 mod_inv_5 * mod_inv_5
    
    int32_t s = 0;
    
    while ( 1 ) {
        
        uint32_t q = fptd_math_rotate_right_u32( ( *n ) * mod_inv_25, 2 );
        
        if ( q <= fptd_u32_max / 100 ) {
            ( *n ) = q;
            s += 2;
        } else {
            break;
        }
    }
    
    uint32_t q = fptd_math_rotate_right_u32( ( *n ) * mod_inv_5, 1 );
    
    if ( q <= fptd_u32_max / 10 ) {
        ( *n ) = q;
        s |= 1;
    }
    
#undef mod_inv_5
#undef mod_inv_25
    
    return s;
}

fptd_stu uint32_t fptd_compute_mul_32( uint32_t u, uint64_t cache, uint8_t* is_integer  ) {
    
    uint64_t r = fptd_math_multiply_u32_u64_high( u, cache );
    uint32_t result = ( uint32_t ) ( r >> 32 );
    ( *is_integer ) = ( ( ( uint32_t ) r ) == 0 );
    
    return result;
}

fptd_stu uint32_t fptd_compute_delta_32( uint64_t cache, int32_t beta ) {
    uint32_t result = ( uint32_t ) ( cache >> ( cache_bit_count_32 - 1 - beta ) );
    return result;
}

fptd_stu uint64_t fptd_math_multiply_u32_u64_low( uint32_t a, uint64_t b ) {
    u64 result = a * b;
    return result;
}

fptd_stu uint8_t fptd_compute_mul_parity_32( uint32_t two_f, uint64_t cache, int32_t beta, uint8_t* is_integer ) {
    
    _assert( beta >= 1 );
    _assert( beta < 64 );
    
    uint64_t r = fptd_math_multiply_u32_u64_low( two_f, cache );
    uint8_t parity = ( ( r >> ( 64 - beta ) ) & 1 ) != 0;
    ( *is_integer ) = ( ( uint32_t ) ( r >> ( 32 - beta ) ) ) == 0;
    
    return parity;
}

fptd_stu uint32_t fptd_compute_left_endpoint_for_shorter_interval_case_32( uint64_t cache, int32_t beta ) {
    
    uint32_t result = ( uint32_t ) ( ( cache - ( cache >> ( significand_bit_count_32 + 2 ) ) ) >> ( cache_bit_count_32 - significand_bit_count_32 - 1 - beta ) );
    return result;
}

fptd_stu uint32_t fptd_compute_right_endpoint_for_shorter_interval_case_32( uint64_t cache, int32_t beta ) {
    
    uint32_t result = ( uint32_t ) ( ( cache + ( cache >> ( significand_bit_count_32 + 1 ) ) ) >> ( cache_bit_count_32 - significand_bit_count_32 - 1 - beta ) );
    return result;
}

fptd_stu uint32_t fptd_compute_round_up_for_shorter_interval_case_32( uint64_t cache, int32_t beta ) {
    
    uint32_t result = ( ( uint32_t ) ( cache >> ( cache_bit_count_32 - significand_bit_count_32 - 2 - beta ) ) + 1 ) / 2;
    return result;
}

#undef cache_min_k_32
#undef cache_max_k_32
#undef cache_bit_count_32

fptd_stu void fptd_on_trailing_zeroes_32( floating_point_decimal_32_t* result, uint32_t flags ) {
    
    uint32_t trailing_zero = flags & floating_point_to_decimal_flags_trailing_zero_mask;
    
    if ( trailing_zero == floating_point_to_decimal_flags_trailing_zero_remove ) {
        result->exponent += fptd_remove_trailing_zeroes_32( &result->significand );
    } else if ( trailing_zero == floating_point_to_decimal_flags_trailing_zero_report ) {
        result->may_have_trailing_zeroes = 1;
    }
}

#if 0
fptd_stu void fptd_no_trailing_zeroes_32( floating_point_decimal_64_t* result, uint32_t flags ) {
    
    if ( flags & floating_point_to_decimal_flags_trailing_zero_report ) {
        result->may_have_trailing_zeroes = 0;
    }
}
#endif

fptd_stu floating_point_decimal_32_t fptd_compute_nearest_shorter_32( int32_t exponent, fptd_interval_t interval, uint32_t flags ) {
    
    floating_point_decimal_32_t result = { 0 };
    
    // Compute k and beta.
    int32_t minus_k = fptd_floor_log10_pow2_minus_log10_4_over_3( exponent );
    int32_t beta = exponent + fptd_floor_log2_pow10( -minus_k );
    
    // Compute xi and zi.
    uint64_t cache = fptd_get_cache_32( -minus_k );
    
    uint32_t xi = fptd_compute_left_endpoint_for_shorter_interval_case_32( cache, beta );
    uint32_t zi = fptd_compute_right_endpoint_for_shorter_interval_case_32( cache, beta );
    
    // If we don't accept the right endpoint and
    // if the right endpoint is an integer, decrease it.
    if ( !interval.include_right_endpoint && fptd_is_right_endpoint_integer_shorter_interval( exponent ) ) {
        zi--;
    }
    
    // If we don't accept the left endpoint or
    // if the left endpoint is not an integer, increase it.
    if ( !interval.include_left_endpoint || !fptd_is_left_endpoint_integer_shorter_interval( exponent ) ) {
        xi++;
    }
    
    // Try bigger divisor.
    result.significand = zi / 10;
    
    // If succeed, remove trailing zeros if necessary and return.
    if ( result.significand * 10 >= xi ) {
        
        result.exponent = minus_k + 1;
        fptd_on_trailing_zeroes_32( &result, flags );
        
    } else {
        
        // Otherwise, compute the round-up of y.
        
        // no_trailing_zeroes( &result, flags );
        
        result.significand = fptd_compute_round_up_for_shorter_interval_case_32( cache, beta );
        result.exponent = minus_k;
        
        // When tie occurs, choose one of them according to the rule.
        uint8_t even_significand_bits = fptd_has_even_significand_bits_32( result.significand );
        uint8_t prefer_round_down = fptd_binary_to_decimal_prefer_round_down( even_significand_bits, flags );
        
        if ( prefer_round_down && exponent >= s32_shorter_interval_tie_lower_threshold_32 && exponent <= s32_shorter_interval_tie_upper_threshold_32 ) {
            result.significand--;
        } else if ( result.significand < xi ) {
            result.significand++;
        }
    }
    
    return result;
}

fptd_stu floating_point_decimal_32_t fptd_compute_nearest_normal_32( uint32_t two_fc, int32_t exponent, fptd_interval_t interval, uint32_t flags ) {
    
    //////////////////////////////////////////////////////////////////////
    // Step 1: Schubfach multiplier calculation
    //////////////////////////////////////////////////////////////////////
    
    floating_point_decimal_32_t result = { 0 };
    
    // Compute k and beta.
    int32_t minus_k = fptd_floor_log10_pow2( exponent ) - kappa_32;
    uint64_t cache = fptd_get_cache_32( -minus_k );
    int32_t beta = exponent + fptd_floor_log2_pow10( -minus_k );
    
    // Compute zi and deltai.
    // 10^kappa <= deltai < 10^(kappa + 1)
    uint32_t deltai = fptd_compute_delta_32( cache, beta );
    // For the case of binary32, the result of integer check is not correct for
    // 29711844 * 2^-82
    // = 6.1442653300000000008655037797566933477355632930994033813476... * 10^-18
    // and 29711844 * 2^-81
    // = 1.2288530660000000001731007559513386695471126586198806762695... * 10^-17,
    // and they are the unique counterexamples. However, since 29711844 is even,
    // this does not cause any problem for the endpoints calculations; it can only
    // cause a problem when we need to perform integer check for the center.
    // Fortunately, with these inputs, that branch is never executed, so we are fine.
    uint8_t is_z_integer;
    uint32_t zi = fptd_compute_mul_32( ( two_fc | 1 ) << beta, cache, &is_z_integer );
    
    //////////////////////////////////////////////////////////////////////
    // Step 2: Try larger divisor; remove trailing zeros if necessary
    //////////////////////////////////////////////////////////////////////
    
    uint32_t big_divisor = u32_power_10_kappa_plus_1_32;
    uint32_t small_divisor = u32_power_10_kappa_32;
    
    // Using an upper bound on zi, we might be able to optimize the division
    // better than the compiler; we are computing zi / big_divisor here.
    result.significand = fptd_divide_by_pow10_32( zi, kappa_32 + 1, ( 1u << ( significand_bit_count_32 + 1 ) ) * big_divisor - 1 );
    uint32_t r = ( uint32_t ) ( zi - big_divisor * result.significand );
    
    uint8_t finished = 0;
    uint8_t do_small_divisor_case = 0;
    
    if ( r < deltai ) {
        
        // Exclude the right endpoint if necessary.
        if ( r == 0 && is_z_integer && !interval.include_right_endpoint ) {
            
            if ( flags & floating_point_to_decimal_flags_binary_to_decimal_rounding_do_not_care ) {
                result.significand *= 10;
                result.exponent = minus_k + kappa_32;
                result.significand--;
                finished = 1;
            } else {
                result.significand--;
                r = big_divisor;
                do_small_divisor_case = 1;
            }
        }
        
    } else if ( r > deltai ) {
        
        do_small_divisor_case = 1;
        
    } else {
        // r == deltai; compare fractional parts.
        uint32_t two_fl = two_fc - 1;
        
        if ( !interval.include_left_endpoint || exponent < s32_case_fc_pm_half_lower_threshold_32 || exponent > s32_divisibility_check_by_5_threshold_32 ) {
            // If the left endpoint is not included, the condition for
            // success is z^(f) < delta^(f) (odd parity).
            // Otherwise, the inequalities on exponent ensure that
            // x is not an integer, so if z^(f) >= delta^(f) (even parity), we in fact
            // have strict inequality.
            uint8_t _ignore;
            if ( !fptd_compute_mul_parity_32( two_fl, cache, beta, &_ignore ) ) {
                do_small_divisor_case = 1;
            }
            
        } else {
            
            uint8_t x_is_integer;
            uint8_t xi_parity = fptd_compute_mul_parity_32( two_fl, cache, beta, &x_is_integer );
            
            if ( !xi_parity && !x_is_integer ) {
                do_small_divisor_case = 1;
            }
        }
    }
    
    if ( !finished && !do_small_divisor_case ) {
        
        result.exponent = minus_k + kappa_32 + 1;
        
        // We may need to remove trailing zeros.
        fptd_on_trailing_zeroes_32( &result, flags );
        finished = 1;
    }
    
    if ( !finished ) {
        
        //////////////////////////////////////////////////////////////////////
        // Step 3: Find the significand with the smaller divisor
        //////////////////////////////////////////////////////////////////////
        
        // no_trailing_zeroes( &result, flags );
        
        result.significand *= 10;
        result.exponent = minus_k + kappa_32;
        
        if ( flags & floating_point_to_decimal_flags_binary_to_decimal_rounding_do_not_care ) {
            // Normally, we want to compute
            // ret_value.significand += r / small_divisor
            // and return, but we need to take care of the case that the resulting
            // value is exactly the right endpoint, while that is not included in the
            // interval.
            if ( !interval.include_right_endpoint ) {
                // Is r divisible by 10^kappa?
                if ( is_z_integer && fptd_check_divisibility_and_divide_by_pow10_32( &r ) ) {
                    // This should be in the interval.
                    result.significand += r - 1;
                } else {
                    result.significand += r;
                }
                
            } else {
                result.significand += fptd_small_division_by_pow10_32( r );
            }
            
        } else {
            
            uint32_t dist = r - ( deltai / 2 ) + ( small_divisor / 2 );
            uint8_t approx_y_parity = ( ( dist ^ ( small_divisor / 2 ) ) & 1 ) != 0;
            
            // Is dist divisible by 10^kappa?
            uint8_t divisible_by_small_divisor = fptd_check_divisibility_and_divide_by_pow10_32( &dist );
            
            // Add dist / 10^kappa to the significand.
            result.significand += dist;
            
            if ( divisible_by_small_divisor ) {
                // Check z^(f) >= epsilon^(f).
                // We have either yi == zi - epsiloni or yi == (zi - epsiloni) - 1,
                // where yi == zi - epsiloni if and only if z^(f) >= epsilon^(f).
                // Since there are only 2 possibilities, we only need to care about the
                // parity. Also, zi and r should have the same parity since the divisor is
                // an even number.
                uint8_t is_y_integer;
                uint8_t yi_parity = fptd_compute_mul_parity_32( two_fc, cache, beta, &is_y_integer );
                
                if ( yi_parity != approx_y_parity ) {
                    
                    result.significand--;
                    
                } else {
                    // If z^(f) >= epsilon^(f), we might have a tie
                    // when z^(f) == epsilon^(f), or equivalently, when y is an integer.
                    // For tie-to-up case, we can just choose the upper one.
                    uint8_t even_significand_bits = fptd_has_even_significand_bits_32( result.significand );
                    if ( fptd_binary_to_decimal_prefer_round_down( even_significand_bits, flags ) && is_y_integer ) {
                        result.significand--;
                    }
                }
            }
        }
    }
    
    return result;
}

fptd_stu floating_point_decimal_32_t fptd_compute_left_closed_directed_32( uint32_t two_fc, int32_t exponent, uint32_t flags ) {
    
    //////////////////////////////////////////////////////////////////////
    // Step 1: Schubfach multiplier calculation
    //////////////////////////////////////////////////////////////////////
    
    floating_point_decimal_32_t result = { 0 };
    
    // Compute k and beta.
    int32_t minus_k = fptd_floor_log10_pow2( exponent ) - kappa_32;
    uint64_t cache = fptd_get_cache_32( -minus_k );
    int32_t beta = exponent + fptd_floor_log2_pow10( -minus_k );
    
    // Compute xi and deltai.
    // 10^kappa <= deltai < 10^(kappa + 1)
    uint32_t deltai = fptd_compute_delta_32( cache, beta );
    uint8_t is_x_integer;
    uint32_t xi= fptd_compute_mul_32( two_fc << beta, cache, &is_x_integer );
    
    // Deal with the unique exceptional cases
    // 29711844 * 2^-82
    // = 6.1442653300000000008655037797566933477355632930994033813476... * 10^-18
    // and 29711844 * 2^-81
    // = 1.2288530660000000001731007559513386695471126586198806762695... * 10^-17
    // for binary32.
    
    if ( exponent <= -80 ) {
        is_x_integer = 0;
    }
    
    if ( !is_x_integer ) {
        xi++;
    }
    
    //////////////////////////////////////////////////////////////////////
    // Step 2: Try larger divisor; remove trailing zeros if necessary
    //////////////////////////////////////////////////////////////////////
    
    uint32_t big_divisor = u32_power_10_kappa_plus_1_32;
    
    // Using an upper bound on xi, we might be able to optimize the division
    // better than the compiler; we are computing xi / big_divisor here.
    result.significand = fptd_divide_by_pow10_32( xi, kappa_32 + 1, ( 1u << ( significand_bit_count_32 + 1 ) ) * big_divisor - 1 );
    uint32_t r = ( uint32_t ) ( xi - big_divisor * result.significand );
    
    if ( r != 0 ) {
        result.significand++;
        r = big_divisor - r;
    }
    
    uint8_t do_small_divisor_case = 0;
    
    if ( r > deltai ) {
        do_small_divisor_case = 1;
    } else if ( r == deltai ) {
        // Compare the fractional parts.
        // This branch is never taken for the exceptional cases
        // 2f_c = 29711482, e = -81
        // (6.1442649164096937243516663440523473127541365101933479309082... * 10^-18)
        // and 2f_c = 29711482, e = -80
        // (1.2288529832819387448703332688104694625508273020386695861816... * 10^-17).
        uint8_t is_z_integer;
        uint8_t zi_parity = fptd_compute_mul_parity_32( two_fc + 2, cache, beta, &is_z_integer );
        
        if ( zi_parity || is_z_integer ) {
            do_small_divisor_case = 1;
        }
    }
    
    if ( !do_small_divisor_case ) {
        
        // The ceiling is inside, so we are done.
        result.exponent = minus_k + kappa_32 + 1;
        fptd_on_trailing_zeroes_32( &result, flags );
        
    } else {
        
        //////////////////////////////////////////////////////////////////////
        // Step 3: Find the significand with the smaller divisor
        //////////////////////////////////////////////////////////////////////
        
        result.significand *= 10;
        result.significand -= fptd_small_division_by_pow10_32( r );
        result.exponent = minus_k + kappa_32;
        // no_trailing_zeroes( &result, flags );
    }
    
    return result;
}

fptd_stu floating_point_decimal_32_t fptd_compute_right_closed_directed_32( uint32_t two_fc, int32_t exponent, uint8_t shorter_interval, uint32_t flags ) {
    
    //////////////////////////////////////////////////////////////////////
    // Step 1: Schubfach multiplier calculation
    //////////////////////////////////////////////////////////////////////
    
    floating_point_decimal_32_t result = { 0 };
    
    // Compute k and beta.
    int32_t minus_k = fptd_floor_log10_pow2( exponent - ( shorter_interval ? 1 : 0 ) ) - kappa_32;
    uint64_t cache = fptd_get_cache_32( -minus_k );
    int32_t beta = exponent + fptd_floor_log2_pow10( -minus_k );
    
    // Compute zi and deltai.
    // 10^kappa <= deltai < 10^(kappa + 1)
    uint32_t deltai = ( shorter_interval ) ? fptd_compute_delta_32( cache, beta - 1 ) : fptd_compute_delta_32( cache, beta );
    uint8_t _ignore;
    uint32_t zi = fptd_compute_mul_32( two_fc << beta, cache, &_ignore );
    
    
    //////////////////////////////////////////////////////////////////////
    // Step 2: Try larger divisor; remove trailing zeros if necessary
    //////////////////////////////////////////////////////////////////////
    
    uint32_t big_divisor = u32_power_10_kappa_plus_1_32;
    
    // Using an upper bound on zi, we might be able to optimize the division better than
    // the compiler; we are computing zi / big_divisor here.
    result.significand = fptd_divide_by_pow10_32( zi, kappa_32 + 1, ( 1u << ( significand_bit_count_32 + 1 ) ) * big_divisor - 1 );
    uint32_t r = ( uint32_t ) ( zi - big_divisor * result.significand );
    
    uint8_t do_small_divisor_case = 0;
    
    if ( r > deltai ) {
        do_small_divisor_case = 1;
    } else if ( r == deltai ) {
        // Compare the fractional parts.
        if ( !fptd_compute_mul_parity_32( two_fc - ( shorter_interval ? 1 : 2 ), cache, beta, &_ignore ) ) {
            do_small_divisor_case = 1;
        }
    }
    
    if ( !do_small_divisor_case ) {
        
        // The floor is inside, so we are done.
        result.exponent = minus_k + kappa_32 + 1;
        fptd_on_trailing_zeroes_32( &result, flags );
        
    } else {
        
        //////////////////////////////////////////////////////////////////////
        // Step 3: Find the significand with the small divisor
        //////////////////////////////////////////////////////////////////////
        
        result.significand *= 10;
        result.significand += fptd_small_division_by_pow10_32( r );
        result.exponent = minus_k + kappa_32;
        // no_trailing_zeroes( &result, flags );
    }
    
    return result;
}

floating_point_decimal_32_t floating_point_to_decimal_32( f32 f, uint32_t flags ) {
    
    _assert( floating_point_to_decimal_is_finite_32( f ) );
    _assert( f != 0.0f );
    _assert( f != -0.0f );
    
    floating_point_decimal_32_t result = { 0 };
    
    uint32_t u32_f = *( uint32_t* ) &f;
    int32_t exponent = ( int32_t ) ( ( u32_f & exponent_mask_32 ) >> significand_bit_count_32 );
    uint32_t signed_significand = ( u32_f & ( ~exponent_mask_32 ) );
    
    uint8_t has_even_siginificand_bits = fptd_has_even_significand_bits_32( signed_significand );
    uint8_t is_negative = floating_point_to_decimal_is_negative_32( signed_significand );
    
    fptd_interval_provider_t interval = fptd_get_interval_provider( has_even_siginificand_bits, is_negative, flags );
    
    uint32_t two_fc = ( signed_significand << 1 );
    
    if ( interval.tag == fptd_interval_tag_to_nearest ) {
        
        if ( exponent != 0 ) { // Is the input a normal number?
            
            exponent += exponent_bias_32 - significand_bit_count_32;
            
            if ( two_fc != 0 ) {
                two_fc |= ( 1u << ( significand_bit_count_32 + 1 ) );
            }
            
        } else { // Is the input a subnormal number?
            
            exponent = min_exponent_32 - significand_bit_count_32;
        }
        
        if ( two_fc == 0 ) {
            result = fptd_compute_nearest_shorter_32( exponent, interval.shorter, flags );
        } else {
            result = fptd_compute_nearest_normal_32( two_fc, exponent, interval.normal, flags );
        }
        
    } else if ( interval.tag == fptd_interval_tag_left_closed_directed ) {
        
        if ( exponent != 0 ) { // Is the input a normal number?
            
            exponent += exponent_bias_32 - significand_bit_count_32;
            two_fc |= ( 1u << ( significand_bit_count_32 + 1 ) );
            
        } else { // Is the input a subnormal number?
            
            exponent = min_exponent_32 - significand_bit_count_32;
        }
        
        result = fptd_compute_left_closed_directed_32( two_fc, exponent, flags );
        
    } else if ( interval.tag  == fptd_interval_tag_right_closed_directed ) {
        
        uint8_t shorter_interval = 0;
        
        if ( exponent != 0 ) { // Is the input a normal number?
            
            if ( two_fc == 0 && exponent != 1 ) {
                shorter_interval = 1;
            }
            
            exponent += exponent_bias_32 - significand_bit_count_32;
            two_fc |= ( 1u << ( significand_bit_count_32 + 1 ) );
            
        } else { // Is the input a subnormal number?
            
            exponent = min_exponent_32 - significand_bit_count_32;
        }
        
        result = fptd_compute_right_closed_directed_32( two_fc, exponent, shorter_interval, flags );
        
    } else {
        invalid_code_path( );
    }
    
    result.is_negative = is_negative;
    
    return result;
}

#undef kappa_32

#undef significand_bit_count_32
// #undef exponent_bit_count_32
#undef min_exponent_32
// #undef max_exponent_32
#undef exponent_bias_32
// #undef decimal_digits_32
#undef sign_mask_32
#undef exponent_mask_32
#undef significand_mask_32

// #undef s32_floor_log2_pow10_kappa_plus_1_32
#undef u32_power_10_kappa_32
#undef u32_power_10_kappa_plus_1_32

#undef s32_divisibility_check_by_5_threshold_32
#undef s32_case_fc_pm_half_lower_threshold_32
#undef s32_shorter_interval_tie_lower_threshold_32
#undef s32_shorter_interval_tie_upper_threshold_32

#undef carrier_bit_count_32

#endif /* NOTE simon: no_32 */

#if !defined ( FLOATING_POINT_TO_DECIMAL_NO_64 )

#define kappa_64 2

#define fptd_u64_max 0xffffffffffffffff

#define significand_bit_count_64 52
// #define exponent_bit_count_64 11
#define min_exponent_64 -1022
// #define max_exponent_64 1023
#define exponent_bias_64 -1023
// #define decimal_digits_64 17
#define sign_mask_64 0x8000000000000000ull
#define exponent_mask_64 0x7ff0000000000000ull
#define significand_mask_64 0xfffffffffffffull

// #define s32_floor_log2_pow10_kappa_plus_1_64 9
#define u32_power_10_kappa_64 100
#define u32_power_10_kappa_plus_1_64 1000

#define s32_divisibility_check_by_5_threshold_64 86
#define s32_case_fc_pm_half_lower_threshold_64 -2
#define s32_shorter_interval_tie_lower_threshold_64 -77
#define s32_shorter_interval_tie_upper_threshold_64 -77

#define carrier_bit_count_64 64
// _ctassert( carrier_bit_count_64 >= significand_bit_count_64 + 2 + s32_floor_log2_pow10_kappa_plus_1_64 );

uint8_t floating_point_to_decimal_is_nonzero_64( uint64_t f ) {
    uint8_t result = ( f & ( ~sign_mask_64 ) ) != 0;
    return result;
}

uint8_t floating_point_to_decimal_is_positive_64( uint64_t f ) {
    uint8_t result = ( f < sign_mask_64 );
    return result;
}

uint8_t floating_point_to_decimal_is_negative_64( uint64_t f ) {
    uint8_t result = ( f >= sign_mask_64 );
    return result;
}

uint8_t floating_point_to_decimal_is_finite_64( double f ) {
    
    uint64_t u64_f = *( uint64_t* ) &f;
    int32_t exponent = ( int32_t ) ( ( u64_f & exponent_mask_64 ) >> significand_bit_count_64 );
    uint8_t result = ( exponent != 0x7ff );
    
    return result;
}

fptd_stu uint8_t fptd_has_even_significand_bits_64( uint64_t signed_significand ) {
    uint8_t result = ( ( signed_significand % 2 ) == 0 );
    return result;
}

typedef struct floating_point_decimal_64_t {
    
    int32_t exponent;
    uint64_t significand;
    uint8_t is_negative;
    uint8_t may_have_trailing_zeroes;
    
} floating_point_decimal_64_t;

fptd_stu uint8_t fptd_check_divisibility_and_divide_by_pow10_64( uint32_t* n ) {
    
    uint8_t result = 0;
    
    _ctassert( kappa_64 + 1 <= s32_floor_log10_pow2_31 );
    _assert( ( *n ) <= u32_power_10_kappa_plus_1_64 );
    
    ( *n ) *= 656;
    uint32_t mask = ( 1u << 16 ) - 1;
    result = ( ( ( *n ) & mask ) < 656 );
    ( *n ) = ( *n ) >> 16;
    
    return result;
}

fptd_stu uint32_t fptd_small_division_by_pow10_64( uint32_t n ) {
    
    _ctassert( kappa_64 + 1 <= s32_floor_log10_pow2_31 );
    _assert( n <= u32_power_10_kappa_plus_1_64 );
    
    uint32_t result = ( n * 656 ) >> 16;
    
    return result;
}

fptd_stu uint64_t fptd_divide_by_pow10_64( uint64_t n, int32_t kappa, uint64_t n_max ) {
    
    _assert( kappa == 3 );
    uint64_t result = 0;
    
    if ( n_max <= 15534100272597517998ull ) {
        result = fptd_math_u64_multiply_high( n, 2361183241434822607ull ) >> 7;
    } else {
        
#if 0
        /* NOTE simon: 10^kappa */
        uint64_t divisor = 1;
        
        for ( int32_t i = 0; i < kappa; i++ ) {
            divisor *= 10;
        }
#else
        uint64_t divisor = 10 * 10 * 10;
#endif
        
        result = n / divisor;
    }
    
#if 0
    
    /* NOTE simon: If this function is called with kappa != 3 we should do this instead. */
    
    if ( kappa == 3 && n_max <= 15534100272597517998ull ) {
        result = fptd_math_u64_multiply_high( n, 2361183241434822607ull ) >> 7;
    } else {
        uint64_t divisor = compute_power_64( 10, kappa );
        result /= divisor;
    }
#endif
    
    return result;
}

#define cache_min_k_64 -292
#define cache_max_k_64 326
#define cache_bit_count_64 128

#if !defined( FLOATING_POINT_TO_DECIMAL_USE_COMPRESSED_CACHE )

fptd_stu fptd_u128_t fptd_cache_64[ ] = {
    { 0x25e8e89c13bb0f7b, 0xff77b1fcbebcdc4f }, { 0x77b191618c54e9ad, 0x9faacf3df73609b1 },
    { 0xd59df5b9ef6a2418, 0xc795830d75038c1d }, { 0x4b0573286b44ad1e, 0xf97ae3d0d2446f25 },
    { 0x4ee367f9430aec33, 0x9becce62836ac577 }, { 0x229c41f793cda740, 0xc2e801fb244576d5 },
    { 0x6b43527578c11110, 0xf3a20279ed56d48a }, { 0x830a13896b78aaaa, 0x9845418c345644d6 },
    { 0x23cc986bc656d554, 0xbe5691ef416bd60c }, { 0x2cbfbe86b7ec8aa9, 0xedec366b11c6cb8f },
    { 0x7bf7d71432f3d6aa, 0x94b3a202eb1c3f39 }, { 0xdaf5ccd93fb0cc54, 0xb9e08a83a5e34f07 },
    { 0xd1b3400f8f9cff69, 0xe858ad248f5c22c9 }, { 0x23100809b9c21fa2, 0x91376c36d99995be },
    { 0xabd40a0c2832a78b, 0xb58547448ffffb2d }, { 0x16c90c8f323f516d, 0xe2e69915b3fff9f9 },
    { 0xae3da7d97f6792e4, 0x8dd01fad907ffc3b }, { 0x99cd11cfdf41779d, 0xb1442798f49ffb4a },
    { 0x40405643d711d584, 0xdd95317f31c7fa1d }, { 0x482835ea666b2573, 0x8a7d3eef7f1cfc52 },
    { 0xda3243650005eed0, 0xad1c8eab5ee43b66 }, { 0x90bed43e40076a83, 0xd863b256369d4a40 },
    { 0x5a7744a6e804a292, 0x873e4f75e2224e68 }, { 0x711515d0a205cb37, 0xa90de3535aaae202 },
    { 0x0d5a5b44ca873e04, 0xd3515c2831559a83 }, { 0xe858790afe9486c3, 0x8412d9991ed58091 },
    { 0x626e974dbe39a873, 0xa5178fff668ae0b6 }, { 0xfb0a3d212dc81290, 0xce5d73ff402d98e3 },
    { 0x7ce66634bc9d0b9a, 0x80fa687f881c7f8e }, { 0x1c1fffc1ebc44e81, 0xa139029f6a239f72 },
    { 0xa327ffb266b56221, 0xc987434744ac874e }, { 0x4bf1ff9f0062baa9, 0xfbe9141915d7a922 },
    { 0x6f773fc3603db4aa, 0x9d71ac8fada6c9b5 }, { 0xcb550fb4384d21d4, 0xc4ce17b399107c22 },
    { 0x7e2a53a146606a49, 0xf6019da07f549b2b }, { 0x2eda7444cbfc426e, 0x99c102844f94e0fb },
    { 0xfa911155fefb5309, 0xc0314325637a1939 }, { 0x793555ab7eba27cb, 0xf03d93eebc589f88 },
    { 0x4bc1558b2f3458df, 0x96267c7535b763b5 }, { 0x9eb1aaedfb016f17, 0xbbb01b9283253ca2 },
    { 0x465e15a979c1cadd, 0xea9c227723ee8bcb }, { 0x0bfacd89ec191eca, 0x92a1958a7675175f },
    { 0xcef980ec671f667c, 0xb749faed14125d36 }, { 0x82b7e12780e7401b, 0xe51c79a85916f484 },
    { 0xd1b2ecb8b0908811, 0x8f31cc0937ae58d2 }, { 0x861fa7e6dcb4aa16, 0xb2fe3f0b8599ef07 },
    { 0x67a791e093e1d49b, 0xdfbdcece67006ac9 }, { 0xe0c8bb2c5c6d24e1, 0x8bd6a141006042bd },
    { 0x58fae9f773886e19, 0xaecc49914078536d }, { 0xaf39a475506a899f, 0xda7f5bf590966848 },
    { 0x6d8406c952429604, 0x888f99797a5e012d }, { 0xc8e5087ba6d33b84, 0xaab37fd7d8f58178 },
    { 0xfb1e4a9a90880a65, 0xd5605fcdcf32e1d6 }, { 0x5cf2eea09a550680, 0x855c3be0a17fcd26 },
    { 0xf42faa48c0ea481f, 0xa6b34ad8c9dfc06f }, { 0xf13b94daf124da27, 0xd0601d8efc57b08b },
    { 0x76c53d08d6b70859, 0x823c12795db6ce57 }, { 0x54768c4b0c64ca6f, 0xa2cb1717b52481ed },
    { 0xa9942f5dcf7dfd0a, 0xcb7ddcdda26da268 }, { 0xd3f93b35435d7c4d, 0xfe5d54150b090b02 },
    { 0xc47bc5014a1a6db0, 0x9efa548d26e5a6e1 }, { 0x359ab6419ca1091c, 0xc6b8e9b0709f109a },
    { 0xc30163d203c94b63, 0xf867241c8cc6d4c0 }, { 0x79e0de63425dcf1e, 0x9b407691d7fc44f8 },
    { 0x985915fc12f542e5, 0xc21094364dfb5636 }, { 0x3e6f5b7b17b2939e, 0xf294b943e17a2bc4 },
    { 0xa705992ceecf9c43, 0x979cf3ca6cec5b5a }, { 0x50c6ff782a838354, 0xbd8430bd08277231 },
    { 0xa4f8bf5635246429, 0xece53cec4a314ebd }, { 0x871b7795e136be9a, 0x940f4613ae5ed136 },
    { 0x28e2557b59846e40, 0xb913179899f68584 }, { 0x331aeada2fe589d0, 0xe757dd7ec07426e5 },
    { 0x3ff0d2c85def7622, 0x9096ea6f3848984f }, { 0x0fed077a756b53aa, 0xb4bca50b065abe63 },
    { 0xd3e8495912c62895, 0xe1ebce4dc7f16dfb }, { 0x64712dd7abbbd95d, 0x8d3360f09cf6e4bd },
    { 0xbd8d794d96aacfb4, 0xb080392cc4349dec }, { 0xecf0d7a0fc5583a1, 0xdca04777f541c567 },
    { 0xf41686c49db57245, 0x89e42caaf9491b60 }, { 0x311c2875c522ced6, 0xac5d37d5b79b6239 },
    { 0x7d633293366b828c, 0xd77485cb25823ac7 }, { 0xae5dff9c02033198, 0x86a8d39ef77164bc },
    { 0xd9f57f830283fdfd, 0xa8530886b54dbdeb }, { 0xd072df63c324fd7c, 0xd267caa862a12d66 },
    { 0x4247cb9e59f71e6e, 0x8380dea93da4bc60 }, { 0x52d9be85f074e609, 0xa46116538d0deb78 },
    { 0x67902e276c921f8c, 0xcd795be870516656 }, { 0x00ba1cd8a3db53b7, 0x806bd9714632dff6 },
    { 0x80e8a40eccd228a5, 0xa086cfcd97bf97f3 }, { 0x6122cd128006b2ce, 0xc8a883c0fdaf7df0 },
    { 0x796b805720085f82, 0xfad2a4b13d1b5d6c }, { 0xcbe3303674053bb1, 0x9cc3a6eec6311a63 },
    { 0xbedbfc4411068a9d, 0xc3f490aa77bd60fc }, { 0xee92fb5515482d45, 0xf4f1b4d515acb93b },
    { 0x751bdd152d4d1c4b, 0x991711052d8bf3c5 }, { 0xd262d45a78a0635e, 0xbf5cd54678eef0b6 },
    { 0x86fb897116c87c35, 0xef340a98172aace4 }, { 0xd45d35e6ae3d4da1, 0x9580869f0e7aac0e },
    { 0x8974836059cca10a, 0xbae0a846d2195712 }, { 0x2bd1a438703fc94c, 0xe998d258869facd7 },
    { 0x7b6306a34627ddd0, 0x91ff83775423cc06 }, { 0x1a3bc84c17b1d543, 0xb67f6455292cbf08 },
    { 0x20caba5f1d9e4a94, 0xe41f3d6a7377eeca }, { 0x547eb47b7282ee9d, 0x8e938662882af53e },
    { 0xe99e619a4f23aa44, 0xb23867fb2a35b28d }, { 0x6405fa00e2ec94d5, 0xdec681f9f4c31f31 },
    { 0xde83bc408dd3dd05, 0x8b3c113c38f9f37e }, { 0x9624ab50b148d446, 0xae0b158b4738705e },
    { 0x3badd624dd9b0958, 0xd98ddaee19068c76 }, { 0xe54ca5d70a80e5d7, 0x87f8a8d4cfa417c9 },
    { 0x5e9fcf4ccd211f4d, 0xa9f6d30a038d1dbc }, { 0x7647c32000696720, 0xd47487cc8470652b },
    { 0x29ecd9f40041e074, 0x84c8d4dfd2c63f3b }, { 0xf468107100525891, 0xa5fb0a17c777cf09 },
    { 0x7182148d4066eeb5, 0xcf79cc9db955c2cc }, { 0xc6f14cd848405531, 0x81ac1fe293d599bf },
    { 0xb8ada00e5a506a7d, 0xa21727db38cb002f }, { 0xa6d90811f0e4851d, 0xca9cf1d206fdc03b },
    { 0x908f4a166d1da664, 0xfd442e4688bd304a }, { 0x9a598e4e043287ff, 0x9e4a9cec15763e2e },
    { 0x40eff1e1853f29fe, 0xc5dd44271ad3cdba }, { 0xd12bee59e68ef47d, 0xf7549530e188c128 },
    { 0x82bb74f8301958cf, 0x9a94dd3e8cf578b9 }, { 0xe36a52363c1faf02, 0xc13a148e3032d6e7 },
    { 0xdc44e6c3cb279ac2, 0xf18899b1bc3f8ca1 }, { 0x29ab103a5ef8c0ba, 0x96f5600f15a7b7e5 },
    { 0x7415d448f6b6f0e8, 0xbcb2b812db11a5de }, { 0x111b495b3464ad22, 0xebdf661791d60f56 },
    { 0xcab10dd900beec35, 0x936b9fcebb25c995 }, { 0x3d5d514f40eea743, 0xb84687c269ef3bfb },
    { 0x0cb4a5a3112a5113, 0xe65829b3046b0afa }, { 0x47f0e785eaba72ac, 0x8ff71a0fe2c2e6dc },
    { 0x59ed216765690f57, 0xb3f4e093db73a093 }, { 0x306869c13ec3532d, 0xe0f218b8d25088b8 },
    { 0x1e414218c73a13fc, 0x8c974f7383725573 }, { 0xe5d1929ef90898fb, 0xafbd2350644eeacf },
    { 0xdf45f746b74abf3a, 0xdbac6c247d62a583 }, { 0x6b8bba8c328eb784, 0x894bc396ce5da772 },
    { 0x066ea92f3f326565, 0xab9eb47c81f5114f }, { 0xc80a537b0efefebe, 0xd686619ba27255a2 },
    { 0xbd06742ce95f5f37, 0x8613fd0145877585 }, { 0x2c48113823b73705, 0xa798fc4196e952e7 },
    { 0xf75a15862ca504c6, 0xd17f3b51fca3a7a0 }, { 0x9a984d73dbe722fc, 0x82ef85133de648c4 },
    { 0xc13e60d0d2e0ebbb, 0xa3ab66580d5fdaf5 }, { 0x318df905079926a9, 0xcc963fee10b7d1b3 },
    { 0xfdf17746497f7053, 0xffbbcfe994e5c61f }, { 0xfeb6ea8bedefa634, 0x9fd561f1fd0f9bd3 },
    { 0xfe64a52ee96b8fc1, 0xc7caba6e7c5382c8 }, { 0x3dfdce7aa3c673b1, 0xf9bd690a1b68637b },
    { 0x06bea10ca65c084f, 0x9c1661a651213e2d }, { 0x486e494fcff30a63, 0xc31bfa0fe5698db8 },
    { 0x5a89dba3c3efccfb, 0xf3e2f893dec3f126 }, { 0xf89629465a75e01d, 0x986ddb5c6b3a76b7 },
    { 0xf6bbb397f1135824, 0xbe89523386091465 }, { 0x746aa07ded582e2d, 0xee2ba6c0678b597f },
    { 0xa8c2a44eb4571cdd, 0x94db483840b717ef }, { 0x92f34d62616ce414, 0xba121a4650e4ddeb },
    { 0x77b020baf9c81d18, 0xe896a0d7e51e1566 }, { 0x0ace1474dc1d122f, 0x915e2486ef32cd60 },
    { 0x0d819992132456bb, 0xb5b5ada8aaff80b8 }, { 0x10e1fff697ed6c6a, 0xe3231912d5bf60e6 },
    { 0xca8d3ffa1ef463c2, 0x8df5efabc5979c8f }, { 0xbd308ff8a6b17cb3, 0xb1736b96b6fd83b3 },
    { 0xac7cb3f6d05ddbdf, 0xddd0467c64bce4a0 }, { 0x6bcdf07a423aa96c, 0x8aa22c0dbef60ee4 },
    { 0x86c16c98d2c953c7, 0xad4ab7112eb3929d }, { 0xe871c7bf077ba8b8, 0xd89d64d57a607744 },
    { 0x11471cd764ad4973, 0x87625f056c7c4a8b }, { 0xd598e40d3dd89bd0, 0xa93af6c6c79b5d2d },
    { 0x4aff1d108d4ec2c4, 0xd389b47879823479 }, { 0xcedf722a585139bb, 0x843610cb4bf160cb },
    { 0xc2974eb4ee658829, 0xa54394fe1eedb8fe }, { 0x733d226229feea33, 0xce947a3da6a9273e },
    { 0x0806357d5a3f5260, 0x811ccc668829b887 }, { 0xca07c2dcb0cf26f8, 0xa163ff802a3426a8 },
    { 0xfc89b393dd02f0b6, 0xc9bcff6034c13052 }, { 0xbbac2078d443ace3, 0xfc2c3f3841f17c67 },
    { 0xd54b944b84aa4c0e, 0x9d9ba7832936edc0 }, { 0x0a9e795e65d4df12, 0xc5029163f384a931 },
    { 0x4d4617b5ff4a16d6, 0xf64335bcf065d37d }, { 0x504bced1bf8e4e46, 0x99ea0196163fa42e },
    { 0xe45ec2862f71e1d7, 0xc06481fb9bcf8d39 }, { 0x5d767327bb4e5a4d, 0xf07da27a82c37088 },
    { 0x3a6a07f8d510f870, 0x964e858c91ba2655 }, { 0x890489f70a55368c, 0xbbe226efb628afea },
    { 0x2b45ac74ccea842f, 0xeadab0aba3b2dbe5 }, { 0x3b0b8bc90012929e, 0x92c8ae6b464fc96f },
    { 0x09ce6ebb40173745, 0xb77ada0617e3bbcb }, { 0xcc420a6a101d0516, 0xe55990879ddcaabd },
    { 0x9fa946824a12232e, 0x8f57fa54c2a9eab6 }, { 0x47939822dc96abfa, 0xb32df8e9f3546564 },
    { 0x59787e2b93bc56f8, 0xdff9772470297ebd }, { 0x57eb4edb3c55b65b, 0x8bfbea76c619ef36 },
    { 0xede622920b6b23f2, 0xaefae51477a06b03 }, { 0xe95fab368e45ecee, 0xdab99e59958885c4 },
    { 0x11dbcb0218ebb415, 0x88b402f7fd75539b }, { 0xd652bdc29f26a11a, 0xaae103b5fcd2a881 },
    { 0x4be76d3346f04960, 0xd59944a37c0752a2 }, { 0x6f70a4400c562ddc, 0x857fcae62d8493a5 },
    { 0xcb4ccd500f6bb953, 0xa6dfbd9fb8e5b88e }, { 0x7e2000a41346a7a8, 0xd097ad07a71f26b2 },
    { 0x8ed400668c0c28c9, 0x825ecc24c873782f }, { 0x728900802f0f32fb, 0xa2f67f2dfa90563b },
    { 0x4f2b40a03ad2ffba, 0xcbb41ef979346bca }, { 0xe2f610c84987bfa9, 0xfea126b7d78186bc },
    { 0x0dd9ca7d2df4d7ca, 0x9f24b832e6b0f436 }, { 0x91503d1c79720dbc, 0xc6ede63fa05d3143 },
    { 0x75a44c6397ce912b, 0xf8a95fcf88747d94 }, { 0xc986afbe3ee11abb, 0x9b69dbe1b548ce7c },
    { 0xfbe85badce996169, 0xc24452da229b021b }, { 0xfae27299423fb9c4, 0xf2d56790ab41c2a2 },
    { 0xdccd879fc967d41b, 0x97c560ba6b0919a5 }, { 0x5400e987bbc1c921, 0xbdb6b8e905cb600f },
    { 0x290123e9aab23b69, 0xed246723473e3813 }, { 0xf9a0b6720aaf6522, 0x9436c0760c86e30b },
    { 0xf808e40e8d5b3e6a, 0xb94470938fa89bce }, { 0xb60b1d1230b20e05, 0xe7958cb87392c2c2 },
    { 0xb1c6f22b5e6f48c3, 0x90bd77f3483bb9b9 }, { 0x1e38aeb6360b1af4, 0xb4ecd5f01a4aa828 },
    { 0x25c6da63c38de1b1, 0xe2280b6c20dd5232 }, { 0x579c487e5a38ad0f, 0x8d590723948a535f },
    { 0x2d835a9df0c6d852, 0xb0af48ec79ace837 }, { 0xf8e431456cf88e66, 0xdcdb1b2798182244 },
    { 0x1b8e9ecb641b5900, 0x8a08f0f8bf0f156b }, { 0xe272467e3d222f40, 0xac8b2d36eed2dac5 },
    { 0x5b0ed81dcc6abb10, 0xd7adf884aa879177 }, { 0x98e947129fc2b4ea, 0x86ccbb52ea94baea },
    { 0x3f2398d747b36225, 0xa87fea27a539e9a5 }, { 0x8eec7f0d19a03aae, 0xd29fe4b18e88640e },
    { 0x1953cf68300424ad, 0x83a3eeeef9153e89 }, { 0x5fa8c3423c052dd8, 0xa48ceaaab75a8e2b },
    { 0x3792f412cb06794e, 0xcdb02555653131b6 }, { 0xe2bbd88bbee40bd1, 0x808e17555f3ebf11 },
    { 0x5b6aceaeae9d0ec5, 0xa0b19d2ab70e6ed6 }, { 0xf245825a5a445276, 0xc8de047564d20a8b },
    { 0xeed6e2f0f0d56713, 0xfb158592be068d2e }, { 0x55464dd69685606c, 0x9ced737bb6c4183d },
    { 0xaa97e14c3c26b887, 0xc428d05aa4751e4c }, { 0xd53dd99f4b3066a9, 0xf53304714d9265df },
    { 0xe546a8038efe402a, 0x993fe2c6d07b7fab }, { 0xde98520472bdd034, 0xbf8fdb78849a5f96 },
    { 0x963e66858f6d4441, 0xef73d256a5c0f77c }, { 0xdde7001379a44aa9, 0x95a8637627989aad },
    { 0x5560c018580d5d53, 0xbb127c53b17ec159 }, { 0xaab8f01e6e10b4a7, 0xe9d71b689dde71af },
    { 0xcab3961304ca70e9, 0x9226712162ab070d }, { 0x3d607b97c5fd0d23, 0xb6b00d69bb55c8d1 },
    { 0x8cb89a7db77c506b, 0xe45c10c42a2b3b05 }, { 0x77f3608e92adb243, 0x8eb98a7a9a5b04e3 },
    { 0x55f038b237591ed4, 0xb267ed1940f1c61c }, { 0x6b6c46dec52f6689, 0xdf01e85f912e37a3 },
    { 0x2323ac4b3b3da016, 0x8b61313bbabce2c6 }, { 0xabec975e0a0d081b, 0xae397d8aa96c1b77 },
    { 0x96e7bd358c904a22, 0xd9c7dced53c72255 }, { 0x7e50d64177da2e55, 0x881cea14545c7575 },
    { 0xdde50bd1d5d0b9ea, 0xaa242499697392d2 }, { 0x955e4ec64b44e865, 0xd4ad2dbfc3d07787 },
    { 0xbd5af13bef0b113f, 0x84ec3c97da624ab4 }, { 0xecb1ad8aeacdd58f, 0xa6274bbdd0fadd61 },
    { 0x67de18eda5814af3, 0xcfb11ead453994ba }, { 0x80eacf948770ced8, 0x81ceb32c4b43fcf4 },
    { 0xa1258379a94d028e, 0xa2425ff75e14fc31 }, { 0x096ee45813a04331, 0xcad2f7f5359a3b3e },
    { 0x8bca9d6e188853fd, 0xfd87b5f28300ca0d }, { 0x775ea264cf55347e, 0x9e74d1b791e07e48 },
    { 0x95364afe032a819e, 0xc612062576589dda }, { 0x3a83ddbd83f52205, 0xf79687aed3eec551 },
    { 0xc4926a9672793543, 0x9abe14cd44753b52 }, { 0x75b7053c0f178294, 0xc16d9a0095928a27 },
    { 0x5324c68b12dd6339, 0xf1c90080baf72cb1 }, { 0xd3f6fc16ebca5e04, 0x971da05074da7bee },
    { 0x88f4bb1ca6bcf585, 0xbce5086492111aea }, { 0x2b31e9e3d06c32e6, 0xec1e4a7db69561a5 },
    { 0x3aff322e62439fd0, 0x9392ee8e921d5d07 }, { 0x09befeb9fad487c3, 0xb877aa3236a4b449 },
    { 0x4c2ebe687989a9b4, 0xe69594bec44de15b }, { 0x0f9d37014bf60a11, 0x901d7cf73ab0acd9 },
    { 0x538484c19ef38c95, 0xb424dc35095cd80f }, { 0x2865a5f206b06fba, 0xe12e13424bb40e13 },
    { 0xf93f87b7442e45d4, 0x8cbccc096f5088cb }, { 0xf78f69a51539d749, 0xafebff0bcb24aafe },
    { 0xb573440e5a884d1c, 0xdbe6fecebdedd5be }, { 0x31680a88f8953031, 0x89705f4136b4a597 },
    { 0xfdc20d2b36ba7c3e, 0xabcc77118461cefc }, { 0x3d32907604691b4d, 0xd6bf94d5e57a42bc },
    { 0xa63f9a49c2c1b110, 0x8637bd05af6c69b5 }, { 0x0fcf80dc33721d54, 0xa7c5ac471b478423 },
    { 0xd3c36113404ea4a9, 0xd1b71758e219652b }, { 0x645a1cac083126ea, 0x83126e978d4fdf3b },
    { 0x3d70a3d70a3d70a4, 0xa3d70a3d70a3d70a }, { 0xcccccccccccccccd, 0xcccccccccccccccc },
    { 0x0000000000000000, 0x8000000000000000 }, { 0x0000000000000000, 0xa000000000000000 },
    { 0x0000000000000000, 0xc800000000000000 }, { 0x0000000000000000, 0xfa00000000000000 },
    { 0x0000000000000000, 0x9c40000000000000 }, { 0x0000000000000000, 0xc350000000000000 },
    { 0x0000000000000000, 0xf424000000000000 }, { 0x0000000000000000, 0x9896800000000000 },
    { 0x0000000000000000, 0xbebc200000000000 }, { 0x0000000000000000, 0xee6b280000000000 },
    { 0x0000000000000000, 0x9502f90000000000 }, { 0x0000000000000000, 0xba43b74000000000 },
    { 0x0000000000000000, 0xe8d4a51000000000 }, { 0x0000000000000000, 0x9184e72a00000000 },
    { 0x0000000000000000, 0xb5e620f480000000 }, { 0x0000000000000000, 0xe35fa931a0000000 },
    { 0x0000000000000000, 0x8e1bc9bf04000000 }, { 0x0000000000000000, 0xb1a2bc2ec5000000 },
    { 0x0000000000000000, 0xde0b6b3a76400000 }, { 0x0000000000000000, 0x8ac7230489e80000 },
    { 0x0000000000000000, 0xad78ebc5ac620000 }, { 0x0000000000000000, 0xd8d726b7177a8000 },
    { 0x0000000000000000, 0x878678326eac9000 }, { 0x0000000000000000, 0xa968163f0a57b400 },
    { 0x0000000000000000, 0xd3c21bcecceda100 }, { 0x0000000000000000, 0x84595161401484a0 },
    { 0x0000000000000000, 0xa56fa5b99019a5c8 }, { 0x0000000000000000, 0xcecb8f27f4200f3a },
    { 0x4000000000000000, 0x813f3978f8940984 }, { 0x5000000000000000, 0xa18f07d736b90be5 },
    { 0xa400000000000000, 0xc9f2c9cd04674ede }, { 0x4d00000000000000, 0xfc6f7c4045812296 },
    { 0xf020000000000000, 0x9dc5ada82b70b59d }, { 0x6c28000000000000, 0xc5371912364ce305 },
    { 0xc732000000000000, 0xf684df56c3e01bc6 }, { 0x3c7f400000000000, 0x9a130b963a6c115c },
    { 0x4b9f100000000000, 0xc097ce7bc90715b3 }, { 0x1e86d40000000000, 0xf0bdc21abb48db20 },
    { 0x1314448000000000, 0x96769950b50d88f4 }, { 0x17d955a000000000, 0xbc143fa4e250eb31 },
    { 0x5dcfab0800000000, 0xeb194f8e1ae525fd }, { 0x5aa1cae500000000, 0x92efd1b8d0cf37be },
    { 0xf14a3d9e40000000, 0xb7abc627050305ad }, { 0x6d9ccd05d0000000, 0xe596b7b0c643c719 },
    { 0xe4820023a2000000, 0x8f7e32ce7bea5c6f }, { 0xdda2802c8a800000, 0xb35dbf821ae4f38b },
    { 0xd50b2037ad200000, 0xe0352f62a19e306e }, { 0x4526f422cc340000, 0x8c213d9da502de45 },
    { 0x9670b12b7f410000, 0xaf298d050e4395d6 }, { 0x3c0cdd765f114000, 0xdaf3f04651d47b4c },
    { 0xa5880a69fb6ac800, 0x88d8762bf324cd0f }, { 0x8eea0d047a457a00, 0xab0e93b6efee0053 },
    { 0x72a4904598d6d880, 0xd5d238a4abe98068 }, { 0x47a6da2b7f864750, 0x85a36366eb71f041 },
    { 0x999090b65f67d924, 0xa70c3c40a64e6c51 }, { 0xfff4b4e3f741cf6d, 0xd0cf4b50cfe20765 },
    { 0xbff8f10e7a8921a5, 0x82818f1281ed449f }, { 0xaff72d52192b6a0e, 0xa321f2d7226895c7 },
    { 0x9bf4f8a69f764491, 0xcbea6f8ceb02bb39 }, { 0x02f236d04753d5b5, 0xfee50b7025c36a08 },
    { 0x01d762422c946591, 0x9f4f2726179a2245 }, { 0x424d3ad2b7b97ef6, 0xc722f0ef9d80aad6 },
    { 0xd2e0898765a7deb3, 0xf8ebad2b84e0d58b }, { 0x63cc55f49f88eb30, 0x9b934c3b330c8577 },
    { 0x3cbf6b71c76b25fc, 0xc2781f49ffcfa6d5 }, { 0x8bef464e3945ef7b, 0xf316271c7fc3908a },
    { 0x97758bf0e3cbb5ad, 0x97edd871cfda3a56 }, { 0x3d52eeed1cbea318, 0xbde94e8e43d0c8ec },
    { 0x4ca7aaa863ee4bde, 0xed63a231d4c4fb27 }, { 0x8fe8caa93e74ef6b, 0x945e455f24fb1cf8 },
    { 0xb3e2fd538e122b45, 0xb975d6b6ee39e436 }, { 0x60dbbca87196b617, 0xe7d34c64a9c85d44 },
    { 0xbc8955e946fe31ce, 0x90e40fbeea1d3a4a }, { 0x6babab6398bdbe42, 0xb51d13aea4a488dd },
    { 0xc696963c7eed2dd2, 0xe264589a4dcdab14 }, { 0xfc1e1de5cf543ca3, 0x8d7eb76070a08aec },
    { 0x3b25a55f43294bcc, 0xb0de65388cc8ada8 }, { 0x49ef0eb713f39ebf, 0xdd15fe86affad912 },
    { 0x6e3569326c784338, 0x8a2dbf142dfcc7ab }, { 0x49c2c37f07965405, 0xacb92ed9397bf996 },
    { 0xdc33745ec97be907, 0xd7e77a8f87daf7fb }, { 0x69a028bb3ded71a4, 0x86f0ac99b4e8dafd },
    { 0xc40832ea0d68ce0d, 0xa8acd7c0222311bc }, { 0xf50a3fa490c30191, 0xd2d80db02aabd62b },
    { 0x792667c6da79e0fb, 0x83c7088e1aab65db }, { 0x577001b891185939, 0xa4b8cab1a1563f52 },
    { 0xed4c0226b55e6f87, 0xcde6fd5e09abcf26 }, { 0x544f8158315b05b5, 0x80b05e5ac60b6178 },
    { 0x696361ae3db1c722, 0xa0dc75f1778e39d6 }, { 0x03bc3a19cd1e38ea, 0xc913936dd571c84c },
    { 0x04ab48a04065c724, 0xfb5878494ace3a5f }, { 0x62eb0d64283f9c77, 0x9d174b2dcec0e47b },
    { 0x3ba5d0bd324f8395, 0xc45d1df942711d9a }, { 0xca8f44ec7ee3647a, 0xf5746577930d6500 },
    { 0x7e998b13cf4e1ecc, 0x9968bf6abbe85f20 }, { 0x9e3fedd8c321a67f, 0xbfc2ef456ae276e8 },
    { 0xc5cfe94ef3ea101f, 0xefb3ab16c59b14a2 }, { 0xbba1f1d158724a13, 0x95d04aee3b80ece5 },
    { 0x2a8a6e45ae8edc98, 0xbb445da9ca61281f }, { 0xf52d09d71a3293be, 0xea1575143cf97226 },
    { 0x593c2626705f9c57, 0x924d692ca61be758 }, { 0x6f8b2fb00c77836d, 0xb6e0c377cfa2e12e },
    { 0x0b6dfb9c0f956448, 0xe498f455c38b997a }, { 0x4724bd4189bd5ead, 0x8edf98b59a373fec },
    { 0x58edec91ec2cb658, 0xb2977ee300c50fe7 }, { 0x2f2967b66737e3ee, 0xdf3d5e9bc0f653e1 },
    { 0xbd79e0d20082ee75, 0x8b865b215899f46c }, { 0xecd8590680a3aa12, 0xae67f1e9aec07187 },
    { 0xe80e6f4820cc9496, 0xda01ee641a708de9 }, { 0x3109058d147fdcde, 0x884134fe908658b2 },
    { 0xbd4b46f0599fd416, 0xaa51823e34a7eede }, { 0x6c9e18ac7007c91b, 0xd4e5e2cdc1d1ea96 },
    { 0x03e2cf6bc604ddb1, 0x850fadc09923329e }, { 0x84db8346b786151d, 0xa6539930bf6bff45 },
    { 0xe612641865679a64, 0xcfe87f7cef46ff16 }, { 0x4fcb7e8f3f60c07f, 0x81f14fae158c5f6e },
    { 0xe3be5e330f38f09e, 0xa26da3999aef7749 }, { 0x5cadf5bfd3072cc6, 0xcb090c8001ab551c },
    { 0x73d9732fc7c8f7f7, 0xfdcb4fa002162a63 }, { 0x2867e7fddcdd9afb, 0x9e9f11c4014dda7e },
    { 0xb281e1fd541501b9, 0xc646d63501a1511d }, { 0x1f225a7ca91a4227, 0xf7d88bc24209a565 },
    { 0x3375788de9b06959, 0x9ae757596946075f }, { 0x0052d6b1641c83af, 0xc1a12d2fc3978937 },
    { 0xc0678c5dbd23a49b, 0xf209787bb47d6b84 }, { 0xf840b7ba963646e1, 0x9745eb4d50ce6332 },
    { 0xb650e5a93bc3d899, 0xbd176620a501fbff }, { 0xa3e51f138ab4cebf, 0xec5d3fa8ce427aff },
    { 0xc66f336c36b10138, 0x93ba47c980e98cdf }, { 0xb80b0047445d4185, 0xb8a8d9bbe123f017 },
    { 0xa60dc059157491e6, 0xe6d3102ad96cec1d }, { 0x87c89837ad68db30, 0x9043ea1ac7e41392 },
    { 0x29babe4598c311fc, 0xb454e4a179dd1877 }, { 0xf4296dd6fef3d67b, 0xe16a1dc9d8545e94 },
    { 0x1899e4a65f58660d, 0x8ce2529e2734bb1d }, { 0x5ec05dcff72e7f90, 0xb01ae745b101e9e4 },
    { 0x76707543f4fa1f74, 0xdc21a1171d42645d }, { 0x6a06494a791c53a9, 0x899504ae72497eba },
    { 0x0487db9d17636893, 0xabfa45da0edbde69 }, { 0x45a9d2845d3c42b7, 0xd6f8d7509292d603 },
    { 0x0b8a2392ba45a9b3, 0x865b86925b9bc5c2 }, { 0x8e6cac7768d7141f, 0xa7f26836f282b732 },
    { 0x3207d795430cd927, 0xd1ef0244af2364ff }, { 0x7f44e6bd49e807b9, 0x8335616aed761f1f },
    { 0x5f16206c9c6209a7, 0xa402b9c5a8d3a6e7 }, { 0x36dba887c37a8c10, 0xcd036837130890a1 },
    { 0xc2494954da2c978a, 0x802221226be55a64 }, { 0xf2db9baa10b7bd6d, 0xa02aa96b06deb0fd },
    { 0x6f92829494e5acc8, 0xc83553c5c8965d3d }, { 0xcb772339ba1f17fa, 0xfa42a8b73abbf48c },
    { 0xff2a760414536efc, 0x9c69a97284b578d7 }, { 0xfef5138519684abb, 0xc38413cf25e2d70d },
    { 0x7eb258665fc25d6a, 0xf46518c2ef5b8cd1 }, { 0xef2f773ffbd97a62, 0x98bf2f79d5993802 },
    { 0xaafb550ffacfd8fb, 0xbeeefb584aff8603 }, { 0x95ba2a53f983cf39, 0xeeaaba2e5dbf6784 },
    { 0xdd945a747bf26184, 0x952ab45cfa97a0b2 }, { 0x94f971119aeef9e5, 0xba756174393d88df },
    { 0x7a37cd5601aab85e, 0xe912b9d1478ceb17 }, { 0xac62e055c10ab33b, 0x91abb422ccb812ee },
    { 0x577b986b314d600a, 0xb616a12b7fe617aa }, { 0xed5a7e85fda0b80c, 0xe39c49765fdf9d94 },
    { 0x14588f13be847308, 0x8e41ade9fbebc27d }, { 0x596eb2d8ae258fc9, 0xb1d219647ae6b31c },
    { 0x6fca5f8ed9aef3bc, 0xde469fbd99a05fe3 }, { 0x25de7bb9480d5855, 0x8aec23d680043bee },
    { 0xaf561aa79a10ae6b, 0xada72ccc20054ae9 }, { 0x1b2ba1518094da05, 0xd910f7ff28069da4 },
    { 0x90fb44d2f05d0843, 0x87aa9aff79042286 }, { 0x353a1607ac744a54, 0xa99541bf57452b28 },
    { 0x42889b8997915ce9, 0xd3fa922f2d1675f2 }, { 0x69956135febada12, 0x847c9b5d7c2e09b7 },
    { 0x43fab9837e699096, 0xa59bc234db398c25 }, { 0x94f967e45e03f4bc, 0xcf02b2c21207ef2e },
    { 0x1d1be0eebac278f6, 0x8161afb94b44f57d }, { 0x6462d92a69731733, 0xa1ba1ba79e1632dc },
    { 0x7d7b8f7503cfdcff, 0xca28a291859bbf93 }, { 0x5cda735244c3d43f, 0xfcb2cb35e702af78 },
    { 0x3a0888136afa64a8, 0x9defbf01b061adab }, { 0x088aaa1845b8fdd1, 0xc56baec21c7a1916 },
    { 0x8aad549e57273d46, 0xf6c69a72a3989f5b }, { 0x36ac54e2f678864c, 0x9a3c2087a63f6399 },
    { 0x84576a1bb416a7de, 0xc0cb28a98fcf3c7f }, { 0x656d44a2a11c51d6, 0xf0fdf2d3f3c30b9f },
    { 0x9f644ae5a4b1b326, 0x969eb7c47859e743 }, { 0x873d5d9f0dde1fef, 0xbc4665b596706114 },
    { 0xa90cb506d155a7eb, 0xeb57ff22fc0c7959 }, { 0x09a7f12442d588f3, 0x9316ff75dd87cbd8 },
    { 0x0c11ed6d538aeb30, 0xb7dcbf5354e9bece }, { 0x8f1668c8a86da5fb, 0xe5d3ef282a242e81 },
    { 0xf96e017d694487bd, 0x8fa475791a569d10 }, { 0x37c981dcc395a9ad, 0xb38d92d760ec4455 },
    { 0x85bbe253f47b1418, 0xe070f78d3927556a }, { 0x93956d7478ccec8f, 0x8c469ab843b89562 },
    { 0x387ac8d1970027b3, 0xaf58416654a6babb }, { 0x06997b05fcc0319f, 0xdb2e51bfe9d0696a },
    { 0x441fece3bdf81f04, 0x88fcf317f22241e2 }, { 0xd527e81cad7626c4, 0xab3c2fddeeaad25a },
    { 0x8a71e223d8d3b075, 0xd60b3bd56a5586f1 }, { 0xf6872d5667844e4a, 0x85c7056562757456 },
    { 0xb428f8ac016561dc, 0xa738c6bebb12d16c }, { 0xe13336d701beba53, 0xd106f86e69d785c7 },
    { 0xecc0024661173474, 0x82a45b450226b39c }, { 0x27f002d7f95d0191, 0xa34d721642b06084 },
    { 0x31ec038df7b441f5, 0xcc20ce9bd35c78a5 }, { 0x7e67047175a15272, 0xff290242c83396ce },
    { 0x0f0062c6e984d387, 0x9f79a169bd203e41 }, { 0x52c07b78a3e60869, 0xc75809c42c684dd1 },
    { 0xa7709a56ccdf8a83, 0xf92e0c3537826145 }, { 0x88a66076400bb692, 0x9bbcc7a142b17ccb },
    { 0x6acff893d00ea436, 0xc2abf989935ddbfe }, { 0x0583f6b8c4124d44, 0xf356f7ebf83552fe },
    { 0xc3727a337a8b704b, 0x98165af37b2153de }, { 0x744f18c0592e4c5d, 0xbe1bf1b059e9a8d6 },
    { 0x1162def06f79df74, 0xeda2ee1c7064130c }, { 0x8addcb5645ac2ba9, 0x9485d4d1c63e8be7 },
    { 0x6d953e2bd7173693, 0xb9a74a0637ce2ee1 }, { 0xc8fa8db6ccdd0438, 0xe8111c87c5c1ba99 },
    { 0x1d9c9892400a22a3, 0x910ab1d4db9914a0 }, { 0x2503beb6d00cab4c, 0xb54d5e4a127f59c8 },
    { 0x2e44ae64840fd61e, 0xe2a0b5dc971f303a }, { 0x5ceaecfed289e5d3, 0x8da471a9de737e24 },
    { 0x7425a83e872c5f48, 0xb10d8e1456105dad }, { 0xd12f124e28f7771a, 0xdd50f1996b947518 },
    { 0x82bd6b70d99aaa70, 0x8a5296ffe33cc92f }, { 0x636cc64d1001550c, 0xace73cbfdc0bfb7b },
    { 0x3c47f7e05401aa4f, 0xd8210befd30efa5a }, { 0x65acfaec34810a72, 0x8714a775e3e95c78 },
    { 0x7f1839a741a14d0e, 0xa8d9d1535ce3b396 }, { 0x1ede48111209a051, 0xd31045a8341ca07c },
    { 0x934aed0aab460433, 0x83ea2b892091e44d }, { 0xf81da84d56178540, 0xa4e4b66b68b65d60 },
    { 0x36251260ab9d668f, 0xce1de40642e3f4b9 }, { 0xc1d72b7c6b42601a, 0x80d2ae83e9ce78f3 },
    { 0xb24cf65b8612f820, 0xa1075a24e4421730 }, { 0xdee033f26797b628, 0xc94930ae1d529cfc },
    { 0x169840ef017da3b2, 0xfb9b7cd9a4a7443c }, { 0x8e1f289560ee864f, 0x9d412e0806e88aa5 },
    { 0xf1a6f2bab92a27e3, 0xc491798a08a2ad4e }, { 0xae10af696774b1dc, 0xf5b5d7ec8acb58a2 },
    { 0xacca6da1e0a8ef2a, 0x9991a6f3d6bf1765 }, { 0x17fd090a58d32af4, 0xbff610b0cc6edd3f },
    { 0xddfc4b4cef07f5b1, 0xeff394dcff8a948e }, { 0x4abdaf101564f98f, 0x95f83d0a1fb69cd9 },
    { 0x9d6d1ad41abe37f2, 0xbb764c4ca7a4440f }, { 0x84c86189216dc5ee, 0xea53df5fd18d5513 },
    { 0x32fd3cf5b4e49bb5, 0x92746b9be2f8552c }, { 0x3fbc8c33221dc2a2, 0xb7118682dbb66a77 },
    { 0x0fabaf3feaa5334b, 0xe4d5e82392a40515 }, { 0x29cb4d87f2a7400f, 0x8f05b1163ba6832d },
    { 0x743e20e9ef511013, 0xb2c71d5bca9023f8 }, { 0x914da9246b255417, 0xdf78e4b2bd342cf6 },
    { 0x1ad089b6c2f7548f, 0x8bab8eefb6409c1a }, { 0xa184ac2473b529b2, 0xae9672aba3d0c320 },
    { 0xc9e5d72d90a2741f, 0xda3c0f568cc4f3e8 }, { 0x7e2fa67c7a658893, 0x8865899617fb1871 },
    { 0xddbb901b98feeab8, 0xaa7eebfb9df9de8d }, { 0x552a74227f3ea566, 0xd51ea6fa85785631 },
    { 0xd53a88958f872760, 0x8533285c936b35de }, { 0x8a892abaf368f138, 0xa67ff273b8460356 },
    { 0x2d2b7569b0432d86, 0xd01fef10a657842c }, { 0x9c3b29620e29fc74, 0x8213f56a67f6b29b },
    { 0x8349f3ba91b47b90, 0xa298f2c501f45f42 }, { 0x241c70a936219a74, 0xcb3f2f7642717713 },
    { 0xed238cd383aa0111, 0xfe0efb53d30dd4d7 }, { 0xf4363804324a40ab, 0x9ec95d1463e8a506 },
    { 0xb143c6053edcd0d6, 0xc67bb4597ce2ce48 }, { 0xdd94b7868e94050b, 0xf81aa16fdc1b81da },
    { 0xca7cf2b4191c8327, 0x9b10a4e5e9913128 }, { 0xfd1c2f611f63a3f1, 0xc1d4ce1f63f57d72 },
    { 0xbc633b39673c8ced, 0xf24a01a73cf2dccf }, { 0xd5be0503e085d814, 0x976e41088617ca01 },
    { 0x4b2d8644d8a74e19, 0xbd49d14aa79dbc82 }, { 0xddf8e7d60ed1219f, 0xec9c459d51852ba2 },
    { 0xcabb90e5c942b504, 0x93e1ab8252f33b45 }, { 0x3d6a751f3b936244, 0xb8da1662e7b00a17 },
    { 0x0cc512670a783ad5, 0xe7109bfba19c0c9d }, { 0x27fb2b80668b24c6, 0x906a617d450187e2 },
    { 0xb1f9f660802dedf7, 0xb484f9dc9641e9da }, { 0x5e7873f8a0396974, 0xe1a63853bbd26451 },
    { 0xdb0b487b6423e1e9, 0x8d07e33455637eb2 }, { 0x91ce1a9a3d2cda63, 0xb049dc016abc5e5f },
    { 0x7641a140cc7810fc, 0xdc5c5301c56b75f7 }, { 0xa9e904c87fcb0a9e, 0x89b9b3e11b6329ba },
    { 0x546345fa9fbdcd45, 0xac2820d9623bf429 }, { 0xa97c177947ad4096, 0xd732290fbacaf133 },
    { 0x49ed8eabcccc485e, 0x867f59a9d4bed6c0 }, { 0x5c68f256bfff5a75, 0xa81f301449ee8c70 },
    { 0x73832eec6fff3112, 0xd226fc195c6a2f8c }, { 0xc831fd53c5ff7eac, 0x83585d8fd9c25db7 },
    { 0xba3e7ca8b77f5e56, 0xa42e74f3d032f525 }, { 0x28ce1bd2e55f35ec, 0xcd3a1230c43fb26f },
    { 0x7980d163cf5b81b4, 0x80444b5e7aa7cf85 }, { 0xd7e105bcc3326220, 0xa0555e361951c366 },
    { 0x8dd9472bf3fefaa8, 0xc86ab5c39fa63440 }, { 0xb14f98f6f0feb952, 0xfa856334878fc150 },
    { 0x6ed1bf9a569f33d4, 0x9c935e00d4b9d8d2 }, { 0x0a862f80ec4700c9, 0xc3b8358109e84f07 },
    { 0xcd27bb612758c0fb, 0xf4a642e14c6262c8 }, { 0x8038d51cb897789d, 0x98e7e9cccfbd7dbd },
    { 0xe0470a63e6bd56c4, 0xbf21e44003acdd2c }, { 0x1858ccfce06cac75, 0xeeea5d5004981478 },
    { 0x0f37801e0c43ebc9, 0x95527a5202df0ccb }, { 0xd30560258f54e6bb, 0xbaa718e68396cffd },
    { 0x47c6b82ef32a206a, 0xe950df20247c83fd }, { 0x4cdc331d57fa5442, 0x91d28b7416cdd27e },
    { 0xe0133fe4adf8e953, 0xb6472e511c81471d }, { 0x58180fddd97723a7, 0xe3d8f9e563a198e5 },
    { 0x570f09eaa7ea7649, 0x8e679c2f5e44ff8f }, { 0x2cd2cc6551e513db, 0xb201833b35d63f73 },
    { 0xf8077f7ea65e58d2, 0xde81e40a034bcf4f }, { 0xfb04afaf27faf783, 0x8b112e86420f6191 },
    { 0x79c5db9af1f9b564, 0xadd57a27d29339f6 }, { 0x18375281ae7822bd, 0xd94ad8b1c7380874 },
    { 0x8f2293910d0b15b6, 0x87cec76f1c830548 }, { 0xb2eb3875504ddb23, 0xa9c2794ae3a3c69a },
    { 0x5fa60692a46151ec, 0xd433179d9c8cb841 }, { 0xdbc7c41ba6bcd334, 0x849feec281d7f328 },
    { 0x12b9b522906c0801, 0xa5c7ea73224deff3 }, { 0xd768226b34870a01, 0xcf39e50feae16bef },
    { 0xe6a1158300d46641, 0x81842f29f2cce375 }, { 0x60495ae3c1097fd1, 0xa1e53af46f801c53 },
    { 0x385bb19cb14bdfc5, 0xca5e89b18b602368 }, { 0x46729e03dd9ed7b6, 0xfcf62c1dee382c42 },
    { 0x6c07a2c26a8346d2, 0x9e19db92b4e31ba9 }, { 0xc7098b7305241886, 0xc5a05277621be293 },
    { 0xb8cbee4fc66d1ea8, 0xf70867153aa2db38 }
};

fptd_stu fptd_u128_t fptd_get_cache_64( int32_t k ) {
    
    fptd_u128_t result;
    _assert( k >= cache_min_k_64 && k <= cache_max_k_64 );
    result = fptd_cache_64[ k - cache_min_k_64 ];
    
    return result;
}

#else

#define compression_ratio 27
#define compressed_cache_size ( ( cache_max_k_64 - cache_min_k_64 + compression_ratio ) / compression_ratio )

fptd_stu fptd_u128_t fptd_cache_64[ compressed_cache_size ] = {
    { 0x25e8e89c13bb0f7b, 0xff77b1fcbebcdc4f },
    { 0xfb0a3d212dc81290, 0xce5d73ff402d98e3 },
    { 0xf42faa48c0ea481f, 0xa6b34ad8c9dfc06f },
    { 0xae5dff9c02033198, 0x86a8d39ef77164bc },
    { 0x3badd624dd9b0958, 0xd98ddaee19068c76 },
    { 0xe5d1929ef90898fb, 0xafbd2350644eeacf },
    { 0xca8d3ffa1ef463c2, 0x8df5efabc5979c8f },
    { 0xcc420a6a101d0516, 0xe55990879ddcaabd },
    { 0xf808e40e8d5b3e6a, 0xb94470938fa89bce },
    { 0xdde7001379a44aa9, 0x95a8637627989aad },
    { 0x5324c68b12dd6339, 0xf1c90080baf72cb1 },
    { 0x0000000000000000, 0xc350000000000000 },
    { 0xf020000000000000, 0x9dc5ada82b70b59d },
    { 0x02f236d04753d5b5, 0xfee50b7025c36a08 },
    { 0xed4c0226b55e6f87, 0xcde6fd5e09abcf26 },
    { 0x84db8346b786151d, 0xa6539930bf6bff45 },
    { 0x0b8a2392ba45a9b3, 0x865b86925b9bc5c2 },
    { 0x1b2ba1518094da05, 0xd910f7ff28069da4 },
    { 0x387ac8d1970027b3, 0xaf58416654a6babb },
    { 0x5ceaecfed289e5d3, 0x8da471a9de737e24 },
    { 0x0fabaf3feaa5334b, 0xe4d5e82392a40515 },
    { 0x3d6a751f3b936244, 0xb8da1662e7b00a17 },
    { 0x0f37801e0c43ebc9, 0x95527a5202df0ccb }
};

fptd_stu uint64_t fptd_cache_pow5[ compression_ratio ] = {
    1, 5, 25, 125, 625, 3125, 15625, 78125, 390625, 1953125, 
    9765625, 48828125, 244140625, 1220703125, 6103515625, 30517578125, 152587890625, 762939453125, 3814697265625, 19073486328125, 
    95367431640625, 476837158203125, 2384185791015625, 11920928955078125, 59604644775390625, 298023223876953125, 1490116119384765625
};

fptd_stu fptd_u128_t fptd_get_cache_64( int32_t k ) {
    
    fptd_u128_t result = { 0 };
    _assert( k >= cache_min_k_64 && k <= cache_max_k_64 );
    
    int32_t cache_index = ( int32_t ) ( ( ( uint32_t ) ( k - cache_min_k_64 ) ) / compression_ratio );
    int32_t kb = cache_index * compression_ratio + cache_min_k_64;
    int32_t offset = k - kb;
    
    fptd_u128_t base_cache = fptd_cache_64[ cache_index ];
    
    if ( offset == 0 ) {
        result = base_cache;
    } else {
        
        int32_t alpha = floor_log2_pow10( kb + offset ) - floor_log2_pow10( kb ) - offset;
        _assert( alpha > 0 && alpha < 64 );
        uint64_t pow5 = cache_pow5[ offset ];
        fptd_u128_t recovered_cache = fptd_math_u64_multiply( base_cache.high, pow5 );
        fptd_u128_t middle_low = fptd_math_u64_multiply( base_cache.low, pow5 );
        
        recovered_cache = fptd_math_u128_add_u64( recovered_cache, middle_low.high );
        uint64_t high_to_middle = recovered_cache.high << ( 64 - alpha );
        uint64_t middle_to_low = recovered_cache.low << ( 64 - alpha );
        
        recovered_cache.high = ( recovered_cache.low >> alpha ) | high_to_middle;
        recovered_cache.low = ( middle_low.low >> alpha ) | middle_to_low;
        
        _assert( recovered_cache.low + 1 != 0 );
        recovered_cache.low += 1;
        
        result = recovered_cache;
    }
    
    return result;
}

#undef compression_ratio
#undef compressed_cache_size

#endif

#undef cache_min_k_64
#undef cache_max_k_64
#undef cache_bit_count_64

fptd_stu int32_t fptd_remove_trailing_zeroes_64( uint64_t* n ) {
    
    int32_t s;
    // Divide by 10^8 and reduce to 32-bits if divisible.
    // Since ret_value.significand <= (2^53 * 1000 - 1) / 1000 < 10^16,
    // n is at most of 16 digits.
    
    // This magic number is ceil(2^90 / 10^8).
    uint64_t magic_number = 12379400392853802749ull;
    fptd_u128_t nm = fptd_math_u64_multiply( ( *n ), magic_number );
    
    uint64_t mask = ( 1ull << ( 90 - 64 ) ) - 1;
    // Is n is divisible by 10^8?
    if ( ( nm.high & mask ) == 0 && nm.low < magic_number ) {
        
        // If yes, work with the quotient.
        uint32_t n32 = ( uint32_t ) ( nm.high >> ( 90 - 64 ) );
        
#define mod_inv_5 0xcccccccdu
#define mod_inv_25 mod_inv_5 * mod_inv_5
        
        s = 8;
        
        while ( 1 ) {
            
            uint32_t q = fptd_math_rotate_right_u32( n32 * mod_inv_25, 2 );
            
            if ( q <= fptd_u32_max / 100 ) {
                n32 = q;
                s += 2;
            } else {
                break;
            }
        }
        
        uint32_t q = fptd_math_rotate_right_u32( n32 * mod_inv_5, 1 );
        
        if ( q <= fptd_u32_max / 10 ) {
            n32 = q;
            s |= 1;
        }
        
        ( *n ) = n32;
        
#undef mod_inv_5
#undef mod_inv_25
        
    } else {
        
        // If n is not divisible by 10^8, work with n itself.
#define mod_inv_5 0xcccccccccccccccdull
#define mod_inv_25 mod_inv_5 * mod_inv_5
        
        s = 0;
        
        while ( 1 ) {
            
            uint64_t q = fptd_math_rotate_right_u64( ( *n ) * mod_inv_25, 2 );
            
            if ( q <= fptd_u64_max / 100 ) {
                ( *n ) = q;
                s += 2;
            } else {
                break;
            }
        }
        
        uint64_t q = fptd_math_rotate_right_u64( ( *n ) * mod_inv_5, 1 );
        
        if ( q <= fptd_u64_max / 10 ) {
            ( *n ) = q;
            s |= 1;
        }
        
#undef mod_inv_5
#undef mod_inv_25
        
    }
    
    return s;
}

fptd_stu uint64_t fptd_compute_mul_64( uint64_t u, fptd_u128_t cache, uint8_t* is_integer ) {
    
    fptd_u128_t r = fptd_math_multiply_u64_u128_high( u, cache );
    uint64_t result = r.high;
    ( *is_integer ) = ( r.low == 0 );
    
    return result;
}

fptd_stu uint32_t fptd_compute_delta_64( fptd_u128_t cache, int32_t beta ) {
    
    uint32_t result = ( uint32_t ) ( cache.high >> ( carrier_bit_count_64 - 1 - beta ) );
    return result;
}

fptd_stu uint8_t fptd_compute_mul_parity_64( uint64_t two_f, fptd_u128_t cache, int32_t beta, uint8_t* is_integer ) {
    
    _assert( beta >= 1 );
    _assert( beta < 64 );
    
    fptd_u128_t r = fptd_math_multiply_u64_u128_low( two_f, cache );
    uint8_t parity = ( ( r.high >> ( 64 - beta ) ) & 1 ) != 0;
    ( *is_integer ) = ( ( r.high << beta ) | ( r.low >> ( 64 - beta ) ) ) == 0;
    
    return parity;
}

fptd_stu uint64_t fptd_compute_left_endpoint_for_shorter_interval_case_64( fptd_u128_t cache, int32_t beta ) {
    
    uint64_t result = ( cache.high - ( cache.high >> ( significand_bit_count_64 + 2 ) ) ) >> ( carrier_bit_count_64 - significand_bit_count_64 - 1 - beta );
    return result;
}

fptd_stu uint64_t fptd_compute_right_endpoint_for_shorter_interval_case_64( fptd_u128_t cache, int32_t beta ) {
    
    uint64_t result = ( cache.high + ( cache.high >> ( significand_bit_count_64 + 1 ) ) ) >> ( carrier_bit_count_64 - significand_bit_count_64 - 1 - beta );
    return result;
}

fptd_stu uint64_t fptd_compute_round_up_for_shorter_interval_case_64( fptd_u128_t cache, int32_t beta ) {
    
    uint64_t result = ( ( cache.high >> ( carrier_bit_count_64 - significand_bit_count_64 - 2 - beta ) ) + 1 ) / 2;
    return result;
}

fptd_stu void fptd_on_trailing_zeroes_64( floating_point_decimal_64_t* result, uint32_t flags ) {
    
    uint32_t trailing_zero = flags & floating_point_to_decimal_flags_trailing_zero_mask;
    
    if ( trailing_zero == floating_point_to_decimal_flags_trailing_zero_remove ) {
        result->exponent += fptd_remove_trailing_zeroes_64( &result->significand );
    } else if ( trailing_zero == floating_point_to_decimal_flags_trailing_zero_report ) {
        result->may_have_trailing_zeroes = 1;
    }
}

#if 0
fptd_stu void fptd_no_trailing_zeroes_64( floating_point_decimal_64_t* result, uint32_t flags ) {
    
    if ( flags & floating_point_to_decimal_flags_trailing_zero_report ) {
        result->may_have_trailing_zeroes = 0;
    }
}
#endif

fptd_stu floating_point_decimal_64_t fptd_compute_nearest_shorter_64( int32_t exponent, fptd_interval_t interval, uint32_t flags ) {
    
    floating_point_decimal_64_t result = { 0 };
    
    // Compute k and beta.
    int32_t minus_k = fptd_floor_log10_pow2_minus_log10_4_over_3( exponent );
    int32_t beta = exponent + fptd_floor_log2_pow10( -minus_k );
    
    // Compute xi and zi.
    fptd_u128_t cache = fptd_get_cache_64( -minus_k );
    
    uint64_t xi = fptd_compute_left_endpoint_for_shorter_interval_case_64( cache, beta );
    uint64_t zi = fptd_compute_right_endpoint_for_shorter_interval_case_64( cache, beta );
    
    // If we don't accept the right endpoint and
    // if the right endpoint is an integer, decrease it.
    if ( !interval.include_right_endpoint && fptd_is_right_endpoint_integer_shorter_interval( exponent ) ) {
        zi--;
    }
    
    // If we don't accept the left endpoint or
    // if the left endpoint is not an integer, increase it.
    if ( !interval.include_left_endpoint || !fptd_is_left_endpoint_integer_shorter_interval( exponent ) ) {
        xi++;
    }
    
    // Try bigger divisor.
    result.significand = zi / 10;
    
    // If succeed, remove trailing zeros if necessary and return.
    if ( result.significand * 10 >= xi ) {
        
        result.exponent = minus_k + 1;
        fptd_on_trailing_zeroes_64( &result, flags );
        
    } else {
        
        // Otherwise, compute the round-up of y.
        
        // no_trailing_zeroes( &result, flags );
        
        result.significand = fptd_compute_round_up_for_shorter_interval_case_64( cache, beta );
        result.exponent = minus_k;
        
        // When tie occurs, choose one of them according to the rule.
        uint8_t even_significand_bits = fptd_has_even_significand_bits_64( result.significand );
        uint8_t prefer_round_down = fptd_binary_to_decimal_prefer_round_down( even_significand_bits, flags );
        
        if ( prefer_round_down && exponent >= s32_shorter_interval_tie_lower_threshold_64 && exponent <= s32_shorter_interval_tie_upper_threshold_64 ) {
            result.significand--;
        } else if ( result.significand < xi ) {
            result.significand++;
        }
    }
    
    return result;
}

fptd_stu floating_point_decimal_64_t fptd_compute_nearest_normal_64( uint64_t two_fc, int32_t exponent, fptd_interval_t interval, uint32_t flags ) {
    
    //////////////////////////////////////////////////////////////////////
    // Step 1: Schubfach multiplier calculation
    //////////////////////////////////////////////////////////////////////
    
    floating_point_decimal_64_t result = { 0 };
    
    // Compute k and beta.
    int32_t minus_k = fptd_floor_log10_pow2( exponent ) - kappa_64;
    fptd_u128_t cache = fptd_get_cache_64( -minus_k );
    int32_t beta = exponent + fptd_floor_log2_pow10( -minus_k );
    
    // Compute zi and deltai.
    // 10^kappa <= deltai < 10^(kappa + 1)
    uint32_t deltai = fptd_compute_delta_64( cache, beta );
    // For the case of binary32, the result of integer check is not correct for
    // 29711844 * 2^-82
    // = 6.1442653300000000008655037797566933477355632930994033813476... * 10^-18
    // and 29711844 * 2^-81
    // = 1.2288530660000000001731007559513386695471126586198806762695... * 10^-17,
    // and they are the unique counterexamples. However, since 29711844 is even,
    // this does not cause any problem for the endpoints calculations; it can only
    // cause a problem when we need to perform integer check for the center.
    // Fortunately, with these inputs, that branch is never executed, so we are fine.
    uint8_t is_z_integer;
    uint64_t zi = fptd_compute_mul_64( ( two_fc | 1 ) << beta, cache, &is_z_integer );
    
    //////////////////////////////////////////////////////////////////////
    // Step 2: Try larger divisor; remove trailing zeros if necessary
    //////////////////////////////////////////////////////////////////////
    
    uint32_t big_divisor = u32_power_10_kappa_plus_1_64;
    uint32_t small_divisor = u32_power_10_kappa_64;
    
    // Using an upper bound on zi, we might be able to optimize the division
    // better than the compiler; we are computing zi / big_divisor here.
    result.significand = fptd_divide_by_pow10_64( zi, kappa_64 + 1, ( 1ull << ( significand_bit_count_64 + 1 ) ) * big_divisor - 1 );
    uint32_t r = ( uint32_t ) ( zi - big_divisor * result.significand );
    
    uint8_t finished = 0;
    uint8_t do_small_divisor_case = 0;
    
    if ( r < deltai ) {
        
        // Exclude the right endpoint if necessary.
        if ( r == 0 && is_z_integer && !interval.include_right_endpoint ) {
            
            if ( flags & floating_point_to_decimal_flags_binary_to_decimal_rounding_do_not_care ) {
                result.significand *= 10;
                result.exponent = minus_k + kappa_64;
                result.significand--;
                // no_trailing_zeroes( &result );
                finished = 1;
            } else {
                result.significand--;
                r = big_divisor;
                do_small_divisor_case = 1;
            }
        }
        
    } else if ( r > deltai ) {
        
        do_small_divisor_case = 1;
        
    } else {
        // r == deltai; compare fractional parts.
        uint64_t two_fl = two_fc - 1;
        
        if ( !interval.include_left_endpoint || exponent < s32_case_fc_pm_half_lower_threshold_64 || exponent > s32_divisibility_check_by_5_threshold_64 ) {
            // If the left endpoint is not included, the condition for
            // success is z^(f) < delta^(f) (odd parity).
            // Otherwise, the inequalities on exponent ensure that
            // x is not an integer, so if z^(f) >= delta^(f) (even parity), we in fact
            // have strict inequality.
            uint8_t _ignore;
            if ( !fptd_compute_mul_parity_64( two_fl, cache, beta, &_ignore ) ) {
                do_small_divisor_case = 1;
            }
            
        } else {
            
            uint8_t x_is_integer;
            uint8_t xi_parity = fptd_compute_mul_parity_64( two_fl, cache, beta, &x_is_integer );
            
            if ( !xi_parity && !x_is_integer ) {
                do_small_divisor_case = 1;
            }
        }
    }
    
    if ( !finished && !do_small_divisor_case ) {
        
        result.exponent = minus_k + kappa_64 + 1;
        
        // We may need to remove trailing zeros.
        fptd_on_trailing_zeroes_64( &result, flags );
        finished = 1;
    }
    
    if ( !finished ) {
        
        //////////////////////////////////////////////////////////////////////
        // Step 3: Find the significand with the smaller divisor
        //////////////////////////////////////////////////////////////////////
        
        // no_trailing_zeroes( &result, flags );
        
        result.significand *= 10;
        result.exponent = minus_k + kappa_64;
        
        if ( flags & floating_point_to_decimal_flags_binary_to_decimal_rounding_do_not_care ) {
            // Normally, we want to compute
            // ret_value.significand += r / small_divisor
            // and return, but we need to take care of the case that the resulting
            // value is exactly the right endpoint, while that is not included in the
            // interval.
            if ( !interval.include_right_endpoint ) {
                // Is r divisible by 10^kappa?
                if ( is_z_integer && fptd_check_divisibility_and_divide_by_pow10_64( &r ) ) {
                    // This should be in the interval.
                    result.significand += r - 1;
                } else {
                    result.significand += r;
                }
                
            } else {
                result.significand += fptd_small_division_by_pow10_64( r );
            }
            
        } else {
            
            uint32_t dist = r - ( deltai / 2 ) + ( small_divisor / 2 );
            uint8_t approx_y_parity = ( ( dist ^ ( small_divisor / 2 ) ) & 1 ) != 0;
            
            // Is dist divisible by 10^kappa?
            uint8_t divisible_by_small_divisor = fptd_check_divisibility_and_divide_by_pow10_64( &dist );
            
            // Add dist / 10^kappa to the significand.
            result.significand += dist;
            
            if ( divisible_by_small_divisor ) {
                // Check z^(f) >= epsilon^(f).
                // We have either yi == zi - epsiloni or yi == (zi - epsiloni) - 1,
                // where yi == zi - epsiloni if and only if z^(f) >= epsilon^(f).
                // Since there are only 2 possibilities, we only need to care about the
                // parity. Also, zi and r should have the same parity since the divisor is
                // an even number.
                uint8_t is_y_integer;
                uint8_t yi_parity = fptd_compute_mul_parity_64( two_fc, cache, beta, &is_y_integer );
                
                if ( yi_parity != approx_y_parity ) {
                    
                    result.significand--;
                    
                } else {
                    // If z^(f) >= epsilon^(f), we might have a tie
                    // when z^(f) == epsilon^(f), or equivalently, when y is an integer.
                    // For tie-to-up case, we can just choose the upper one.
                    uint8_t even_significand_bits = fptd_has_even_significand_bits_64( result.significand );
                    if ( fptd_binary_to_decimal_prefer_round_down( even_significand_bits, flags ) && is_y_integer ) {
                        result.significand--;
                    }
                }
            }
        }
    }
    
    return result;
}

fptd_stu floating_point_decimal_64_t fptd_compute_left_closed_directed_64( uint64_t two_fc, int32_t exponent, uint32_t flags ) {
    
    //////////////////////////////////////////////////////////////////////
    // Step 1: Schubfach multiplier calculation
    //////////////////////////////////////////////////////////////////////
    
    floating_point_decimal_64_t result = { 0 };
    
    // Compute k and beta.
    int32_t minus_k = fptd_floor_log10_pow2( exponent ) - kappa_64;
    fptd_u128_t cache = fptd_get_cache_64( -minus_k );
    int32_t beta = exponent + fptd_floor_log2_pow10( -minus_k );
    
    // Compute xi and deltai.
    // 10^kappa <= deltai < 10^(kappa + 1)
    uint32_t deltai = fptd_compute_delta_64( cache, beta );
    uint8_t is_x_integer;
    uint64_t xi= fptd_compute_mul_64( two_fc << beta, cache, &is_x_integer );
    
    if ( !is_x_integer ) {
        xi++;
    }
    
    //////////////////////////////////////////////////////////////////////
    // Step 2: Try larger divisor; remove trailing zeros if necessary
    //////////////////////////////////////////////////////////////////////
    
    uint32_t big_divisor = u32_power_10_kappa_plus_1_64;
    
    // Using an upper bound on xi, we might be able to optimize the division
    // better than the compiler; we are computing xi / big_divisor here.
    result.significand = fptd_divide_by_pow10_64( xi, kappa_64 + 1, ( 1ull << ( significand_bit_count_64 + 1 ) ) * big_divisor - 1 );
    uint32_t r = ( uint32_t ) ( xi - big_divisor * result.significand );
    
    if ( r != 0 ) {
        result.significand++;
        r = big_divisor - r;
    }
    
    uint8_t do_small_divisor_case = 0;
    
    if ( r > deltai ) {
        do_small_divisor_case = 1;
    } else if ( r == deltai ) {
        // Compare the fractional parts.
        // This branch is never taken for the exceptional cases
        // 2f_c = 29711482, e = -81
        // (6.1442649164096937243516663440523473127541365101933479309082... * 10^-18)
        // and 2f_c = 29711482, e = -80
        // (1.2288529832819387448703332688104694625508273020386695861816... * 10^-17).
        uint8_t is_z_integer;
        uint8_t zi_parity = fptd_compute_mul_parity_64( two_fc + 2, cache, beta, &is_z_integer );
        
        if ( zi_parity || is_z_integer ) {
            do_small_divisor_case = 1;
        }
    }
    
    if ( !do_small_divisor_case ) {
        
        // The ceiling is inside, so we are done.
        result.exponent = minus_k + kappa_64 + 1;
        fptd_on_trailing_zeroes_64( &result, flags );
        
    } else {
        
        //////////////////////////////////////////////////////////////////////
        // Step 3: Find the significand with the smaller divisor
        //////////////////////////////////////////////////////////////////////
        
        result.significand *= 10;
        result.significand -= fptd_small_division_by_pow10_64( r );
        result.exponent = minus_k + kappa_64;
        // no_trailing_zeroes( &result, flags );
    }
    
    return result;
}

fptd_stu floating_point_decimal_64_t fptd_compute_right_closed_directed_64( uint64_t two_fc, int32_t exponent, uint8_t shorter_interval, uint32_t flags ) {
    
    //////////////////////////////////////////////////////////////////////
    // Step 1: Schubfach multiplier calculation
    //////////////////////////////////////////////////////////////////////
    
    floating_point_decimal_64_t result = { 0 };
    
    // Compute k and beta.
    int32_t minus_k = fptd_floor_log10_pow2( exponent - ( shorter_interval ? 1 : 0 ) ) - kappa_64;
    fptd_u128_t cache = fptd_get_cache_64( -minus_k );
    int32_t beta = exponent + fptd_floor_log2_pow10( -minus_k );
    
    // Compute zi and deltai.
    // 10^kappa <= deltai < 10^(kappa + 1)
    uint32_t deltai = ( shorter_interval ) ? fptd_compute_delta_64( cache, beta - 1 ) : fptd_compute_delta_64( cache, beta );
    uint8_t _ignore;
    uint64_t zi = fptd_compute_mul_64( two_fc << beta, cache, &_ignore );
    
    
    //////////////////////////////////////////////////////////////////////
    // Step 2: Try larger divisor; remove trailing zeros if necessary
    //////////////////////////////////////////////////////////////////////
    
    uint32_t big_divisor = u32_power_10_kappa_plus_1_64;
    
    // Using an upper bound on zi, we might be able to optimize the division better than
    // the compiler; we are computing zi / big_divisor here.
    result.significand = fptd_divide_by_pow10_64( zi, kappa_64 + 1, ( 1ull << ( significand_bit_count_64 + 1 ) ) * big_divisor - 1 );
    uint32_t r = ( uint32_t ) ( zi - big_divisor * result.significand );
    
    uint8_t do_small_divisor_case = 0;
    
    if ( r > deltai ) {
        do_small_divisor_case = 1;
    } else if ( r == deltai ) {
        // Compare the fractional parts.
        if ( !fptd_compute_mul_parity_64( two_fc - ( shorter_interval ? 1 : 2 ), cache, beta, &_ignore ) ) {
            do_small_divisor_case = 1;
        }
    }
    
    if ( !do_small_divisor_case ) {
        
        // The floor is inside, so we are done.
        result.exponent = minus_k + kappa_64 + 1;
        fptd_on_trailing_zeroes_64( &result, flags );
        
    } else {
        
        //////////////////////////////////////////////////////////////////////
        // Step 3: Find the significand with the small divisor
        //////////////////////////////////////////////////////////////////////
        
        result.significand *= 10;
        result.significand += fptd_small_division_by_pow10_64( r );
        result.exponent = minus_k + kappa_64;
        // no_trailing_zeroes( &result, flags );
    }
    
    return result;
}

floating_point_decimal_64_t floating_point_to_decimal_64( f64 f, uint32_t flags ) {
    
    _assert( floating_point_to_decimal_is_finite_64( f ) );
    _assert( f != 0.0 );
    _assert( f != -0.0 );
    
    floating_point_decimal_64_t result = { 0 };
    
    uint64_t u64_f = *( uint64_t* ) &f;
    int32_t exponent = ( int32_t ) ( ( u64_f & exponent_mask_64 ) >> significand_bit_count_64 );
    uint64_t signed_significand = ( u64_f & ( ~exponent_mask_64 ) );
    
    uint8_t has_even_siginificand_bits = fptd_has_even_significand_bits_64( signed_significand );
    uint8_t is_negative = floating_point_to_decimal_is_negative_64( signed_significand );
    
    fptd_interval_provider_t interval = fptd_get_interval_provider( has_even_siginificand_bits, is_negative, flags );
    
    uint64_t two_fc = ( signed_significand << 1 );
    
    if ( interval.tag == fptd_interval_tag_to_nearest ) {
        
        if ( exponent != 0 ) { // Is the input a normal number?
            
            exponent += exponent_bias_64 - significand_bit_count_64;
            
            if ( two_fc != 0 ) {
                two_fc |= ( 1ull << ( significand_bit_count_64 + 1 ) );
            }
            
        } else { // Is the input a subnormal number?
            
            exponent = min_exponent_64 - significand_bit_count_64;
        }
        
        if ( two_fc == 0 ) {
            result = fptd_compute_nearest_shorter_64( exponent, interval.shorter, flags );
        } else {
            result = fptd_compute_nearest_normal_64( two_fc, exponent, interval.normal, flags );
        }
        
    } else if ( interval.tag == fptd_interval_tag_left_closed_directed ) {
        
        if ( exponent != 0 ) { // Is the input a normal number?
            
            exponent += exponent_bias_64 - significand_bit_count_64;
            two_fc |= ( 1ull << ( significand_bit_count_64 + 1 ) );
            
        } else { // Is the input a subnormal number?
            
            exponent = min_exponent_64 - significand_bit_count_64;
        }
        
        result = fptd_compute_left_closed_directed_64( two_fc, exponent, flags );
        
    } else if ( interval.tag  == fptd_interval_tag_right_closed_directed ) {
        
        uint8_t shorter_interval = 0;
        
        if ( exponent != 0 ) { // Is the input a normal number?
            
            if ( two_fc == 0 && exponent != 1 ) {
                shorter_interval = 1;
            }
            
            exponent += exponent_bias_64 - significand_bit_count_64;
            two_fc |= ( 1ull << ( significand_bit_count_64 + 1 ) );
            
        } else { // Is the input a subnormal number?
            
            exponent = min_exponent_64 - significand_bit_count_64;
        }
        
        result = fptd_compute_right_closed_directed_64( two_fc, exponent, shorter_interval, flags );
        
    } else {
        invalid_code_path( );
    }
    
    result.is_negative = is_negative;
    
    return result;
}

#undef kappa_64

#undef significand_bit_count_64
// #undef exponent_bit_count_64
#undef min_exponent_64
// #undef max_exponent_64
#undef exponent_bias_64
// #undef decimal_digits_64
#undef sign_mask_64
#undef exponent_mask_64
#undef significand_mask_64

// #undef s32_floor_log2_pow10_kappa_plus_1_64
#undef u32_power_10_kappa_64
#undef u32_power_10_kappa_plus_1_64

#undef s32_divisibility_check_by_5_threshold_64
#undef s32_case_fc_pm_half_lower_threshold_64
#undef s32_shorter_interval_tie_lower_threshold_64
#undef s32_shorter_interval_tie_upper_threshold_64

#undef carrier_bit_count_64

#endif /* NOTE simon: no_64 */

#undef fptd_stu
#undef s32_floor_log10_pow2_31
#undef s32_case_shorter_interval_left_endpoint_lower_threshold
#undef s32_case_shorter_interval_left_endpoint_upper_threshold
#undef s32_case_shorter_interval_right_endpoint_lower_threshold
#undef s32_case_shorter_interval_right_endpoint_upper_threshold

#define FLOATING_POINT_TO_DECIMAL_H 1

#endif
