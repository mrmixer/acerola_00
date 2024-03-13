#if !defined( GL_DIRECTX_TYPES_H )

#if !defined( COBJMACROS )
#define COBJMACROS
#endif

#if !defined( INITGUID )
#define INITGUID
#endif

#include <d3d11_1.h>
#include <dxgi1_3.h>
#include <d3dcompiler.h>

#if DEBUG
#include <dxgidebug.h>
#endif

typedef struct gl_sampler_t {
    ID3D11SamplerState* pointer;
} gl_sampler_t;

typedef struct gl_shader_t {
    ID3D11VertexShader* vertex;
    ID3D11InputLayout* layout;
    ID3D11PixelShader* pixel;
    b32 valid;
} gl_shader_t;

typedef struct gl_texture_t {
    ID3D11Texture2D* pointer;
    ID3D11ShaderResourceView* view;
    u32 width, height;
    u8 bytes_per_pixel;
} gl_texture_t;

typedef struct gl_buffer_t {
    ID3D11Buffer* pointer;
    b32 mapped;
} gl_buffer_t;

typedef struct gl_platform_t {
    
    ID3D11Device1* device;
    ID3D11DeviceContext1* context;
    IDXGISwapChain1* swap_chain;
    ID3D11RenderTargetView* render_target_view;
    ID3D11DepthStencilView* depth_stencil_view;
    gl_sampler_t samplers[ 5 ];
    
#if DEBUG
    IDXGIDebug1* dxgi_debug;
#endif
    
} gl_platform_t;

#define GL_DIRECTX_TYPES_H
#endif
