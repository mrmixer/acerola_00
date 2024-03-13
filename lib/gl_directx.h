#if !defined( GL_DIRECTX_H )

#include "shaders_hlsl.h"

u8 const gl_primitive_restart_index_u8 = u8_max;
u16 const gl_primitive_restart_index_u16 = u16_max;
u32 const gl_primitive_restart_index_u32 = u32_max;

/* NOTE simon (07/02/24 15:55:26): Those functions are provided by the gl.h header. */
stu u8 gl_vertex_format_compute_vertex_size_byte( gl_vertex_format_t vertex_format );
stu void internal_gl_push_index( void* i_array, umm* i_array_index, umm value, umm index_size );
stu void internal_gl_push_primitive_restart( void* i_array, umm* i_array_index, umm index_size );
stu u32 gl_text_to_vertex_array( void* text, umm byte_count, f32 x, f32 y, u32 previous_codepoint, font_collection_t* collection, u32 font_index, f32* v_array, u32* i_array, umm index, vec4 color, gl_vertex_format_t vertex_format );

#if 0
/* TODO simon (07/02/24 17:14:44): Those are values that goes into SwapChain_Present and SwapChain_Present1 */
stu void gl_set_swap_interval( s32 interval ) {
}

stu s32 gl_get_swap_interval( void ) {
    s32 result = 0;
    return result;
}
#endif

stu void gl_swap_buffers( window_t* window ) {
    profiler_event_start( gl_swap_buffers );
    /* TODO simon (07/02/24 16:09:14): Use Present1 to know if the window is occluded and not render in those cases. */
    IDXGISwapChain_Present( window->gl_platform.swap_chain, 1, 0 );
    profiler_event_end( gl_swap_buffers );
}

typedef enum gl_error_t {
    gl_error_none,
    gl_error_window_is_null,
    gl_error_create_device_failed,
    gl_error_query_device_failed,
    gl_error_query_device_context_failed,
    gl_error_create_dxgi_factory_failed,
    gl_error_create_swap_chain_failed,
    gl_error_device1_create_blend_state_failed,
    gl_error_device1_create_rasterizer_state_failed,
    gl_error_device1_create_depth_stencil_state_failed,
    gl_error_device1_create_sampler_state_failed,
    gl_error_frame_start_render_target_view_is_null,
    gl_error_frame_start_depth_stencil_view_is_null,
    gl_error_vertex_shader_compile_failed,
    gl_error_device1_create_vertex_shader_failed,
    gl_error_device1_create_input_layout_failed,
    gl_error_pixel_shader_compile_failed,
    gl_error_device1_create_pixel_shader_failed,
    gl_error_device1_create_texture_2d_failed,
    gl_error_device1_create_shader_resource_view_failed,
    gl_error_create_buffer_invalid_type,
    gl_error_device1_create_buffer_failed,
    gl_error_buffer_map_already_mapped,
    gl_error_device_context1_map_failed,
    gl_error_unmap_not_mapped,
    gl_error_swap_chain_resize_buffers_failed,
    gl_error_swap_chain_get_buffer_failed,
    gl_error_device1_create_render_target_view_failed,
    gl_error_device1_create_depth_stencil_view_failed,
    
} gl_error_t;

stu void gl_initialize( window_t* window, u32 flags, u32* error ) {
    
    gl_platform_t* gl_platform = 0;
    
    ID3D11Device* base_device = 0;
    ID3D11DeviceContext* base_device_context = 0;
    IDXGIFactory2* dxgi_factory = 0;
    
    ID3D11BlendState* blend_state = 0;
    ID3D11RasterizerState1* rasterizer_state = 0;
    ID3D11DepthStencilState* depth_stencil_state = 0;
    
    breakable {
        
        breakable_check( error );
        
        if ( !window ) {
            log_l( "[ERROR] gl_initialize Window was null.\n" );
            set_error( error, gl_error_window_is_null );
            break;
        }
        
        gl_platform = &window->gl_platform;
        
        D3D_FEATURE_LEVEL feature_levels[ ] = { D3D_FEATURE_LEVEL_11_1 };
        
        u32 device_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        
#if DEBUG
        device_flags |= D3D11_CREATE_DEVICE_DEBUG;/* NOTE simon (25/01/24 16:27:21): We can't give an exe with that flag to non dev. */
        /* NOTE simon (25/01/24 17:20:08): device_flags |= D3D11_CREATE_DEVICE_DEBUGGABLE doesn't seem to work on AMD cards. */
#endif
        
        HRESULT hr = D3D11CreateDevice( 0, D3D_DRIVER_TYPE_HARDWARE, 0, device_flags, feature_levels, array_count( feature_levels ), D3D11_SDK_VERSION, &base_device, 0, &base_device_context );
        
        if ( FAILED( hr ) ) {
            log_l( "[WARNING] gl_initialize D3D11CreateDevice failed to create a hardware device.\n" );
            
            hr = D3D11CreateDevice( 0, D3D_DRIVER_TYPE_WARP, 0, device_flags, feature_levels, array_count( feature_levels ), D3D11_SDK_VERSION, &base_device, 0, &base_device_context );
        }
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_initialize D3D11CreateDevice failed to create a WARP device.\n" );
            set_error( error, gl_error_create_device_failed );
            break;
        }
        
        hr = ID3D11Device_QueryInterface( base_device, &IID_ID3D11Device1, &gl_platform->device );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_initialize Query ID3D11Device1 failed.\n" );
            set_error( error, gl_error_query_device_failed );
            break;
        }
        
        hr = ID3D11DeviceContext_QueryInterface( base_device_context, &IID_ID3D11DeviceContext1, &gl_platform->context );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_initialize Query ID3D11DeviceContext1 failed.\n" );
            set_error( error, gl_error_query_device_context_failed );
            break;
        }
        
#if DEBUG
        ID3D11InfoQueue* info;
        hr = ID3D11Device_QueryInterface( base_device, &IID_ID3D11InfoQueue, &info );
        
        if ( FAILED( hr ) ) {
            log_l( "[WARNING] gl_initialize QueryInterface for InfoQueue failed.\n" );
        } else {
            ID3D11InfoQueue_SetBreakOnSeverity( info, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE );
            ID3D11InfoQueue_SetBreakOnSeverity( info, D3D11_MESSAGE_SEVERITY_ERROR, TRUE );
            ID3D11InfoQueue_Release( info );
        }
        
        hr = DXGIGetDebugInterface1( 0, &IID_IDXGIDebug1, &gl_platform->dxgi_debug );
        
        if ( FAILED( hr ) ) {
            log_l( "[WARNING] gl_initialize DXGIGetDebugInterface1 failed.\n" );
        }
#endif
        
        hr = CreateDXGIFactory( &IID_IDXGIFactory2, &dxgi_factory );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_initialize CreateDXGIFactory failed.\n" );
            set_error( error, gl_error_create_dxgi_factory_failed );
            break;
        }
        
        DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = { 0 };
        /* NOTE simon (25/01/24 16:57:02): Can't specify SRGB here (FLIP_x doesn't support it). It can be set in the render target view instead. */
        swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swap_chain_desc.SampleDesc.Count = 1;
        swap_chain_desc.SampleDesc.Quality = 0;
        swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc.BufferCount = 2;
        swap_chain_desc.Scaling = DXGI_SCALING_NONE;
        swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        
        hr = IDXGIFactory2_CreateSwapChainForHwnd( dxgi_factory, cast( IUnknown*, gl_platform->device ), window->platform.window_handle, &swap_chain_desc, 0, 0, &gl_platform->swap_chain );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_initialize CreateSwapChainForHwnd failed.\n" );
            set_error( error, gl_error_create_swap_chain_failed );
            break;
        }
        
        if ( flags & gl_flag_alpha_blending ) {
            
            D3D11_BLEND_DESC blend_state_desc = { 0 };
            blend_state_desc.RenderTarget[ 0 ].BlendEnable = TRUE;
            blend_state_desc.RenderTarget[ 0 ].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            blend_state_desc.RenderTarget[ 0 ].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            blend_state_desc.RenderTarget[ 0 ].BlendOp = D3D11_BLEND_OP_ADD;
            blend_state_desc.RenderTarget[ 0 ].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
            blend_state_desc.RenderTarget[ 0 ].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            blend_state_desc.RenderTarget[ 0 ].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            blend_state_desc.RenderTarget[ 0 ].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            
            hr = ID3D11Device1_CreateBlendState( gl_platform->device, &blend_state_desc, &blend_state );
            
            if ( FAILED( hr ) ) {
                log_l( "[ERROR] gl_initialize ID3D11Device1_CreateBlendState failed.\n" );
                set_error( error, gl_error_device1_create_blend_state_failed );
                break;
            }
            
            ID3D11DeviceContext1_OMSetBlendState( gl_platform->context, blend_state, 0, u32_max );
        }
        
        D3D11_RASTERIZER_DESC1 rasterizer_desc = { 0 };
        rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        rasterizer_desc.CullMode = D3D11_CULL_NONE;
        rasterizer_desc.ScissorEnable = TRUE;
        
        if ( flags & gl_flag_backface_culling ) {
            rasterizer_desc.CullMode = D3D11_CULL_BACK;
            rasterizer_desc.DepthClipEnable = TRUE; /* NOTE simon (27/01/24 17:09:49): Default value is TRUE in the doc. */
        }
        
        hr = ID3D11Device1_CreateRasterizerState1( gl_platform->device, &rasterizer_desc, &rasterizer_state );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_initialize ID3D11Device1_CreateRasterizerState1 failed.\n" );
            set_error( error, gl_error_device1_create_rasterizer_state_failed );
            break;
        }
        
        /* NOTE simon (28/01/24 15:48:00): Is it OK to cast the rasterizer state here ? */
        ID3D11DeviceContext1_RSSetState( gl_platform->context, cast( ID3D11RasterizerState*, rasterizer_state ) );
        
        D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = { 0 };
        depth_stencil_desc.DepthEnable = FALSE;
        
        if ( flags & gl_flag_depth_testing ) {
            depth_stencil_desc.DepthEnable = TRUE;
            depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
        }
        
        hr = ID3D11Device1_CreateDepthStencilState( gl_platform->device, &depth_stencil_desc, &depth_stencil_state );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_initialize ID3D11Device1_CreateDepthStencilState.\n" );
            set_error( error, gl_error_device1_create_depth_stencil_state_failed );
            break;
        }
        
        ID3D11DeviceContext1_OMSetDepthStencilState( gl_platform->context, depth_stencil_state, 0 );
        
        gl_sampler_t* samplers = gl_platform->samplers;
        
        D3D11_SAMPLER_DESC point_desc = { 0 };
        point_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        point_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        point_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        point_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        point_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        point_desc.MinLOD = 0;
        point_desc.MaxLOD = D3D11_FLOAT32_MAX;
        
        hr = ID3D11Device1_CreateSamplerState( gl_platform->device, &point_desc, &samplers[ gl_sampler_type_point ].pointer );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_initialize ID3D11Device1_CreateSamplerState failed for point sampler.\n" );
            set_error( error, gl_error_device1_create_sampler_state_failed );
            break;
        }
        
        D3D11_SAMPLER_DESC linear_desc = { 0 };
        linear_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        linear_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        linear_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        linear_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        linear_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        linear_desc.MinLOD = 0;
        linear_desc.MaxLOD = D3D11_FLOAT32_MAX;
        
        hr = ID3D11Device1_CreateSamplerState( gl_platform->device, &linear_desc, &samplers[ gl_sampler_type_linear ].pointer );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_initialize ID3D11Device1_CreateSamplerState failed for linear sampler.\n" );
            set_error( error, gl_error_device1_create_sampler_state_failed );
            break;
        }
        
        D3D11_SAMPLER_DESC anisotropic_desc = { 0 };
        anisotropic_desc.Filter = D3D11_FILTER_ANISOTROPIC;
        anisotropic_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        anisotropic_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        anisotropic_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        anisotropic_desc.MaxAnisotropy = 4;
        anisotropic_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        anisotropic_desc.MinLOD = 0;
        anisotropic_desc.MaxLOD = D3D11_FLOAT32_MAX;
        
        hr = ID3D11Device1_CreateSamplerState( gl_platform->device, &anisotropic_desc, &samplers[ gl_sampler_type_anisotropic_4 ].pointer );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_initialize ID3D11Device1_CreateSamplerState failed for anisotropic 4.\n" );
            set_error( error, gl_error_device1_create_sampler_state_failed );
            break;
        }
        
        anisotropic_desc.MaxAnisotropy = 8;
        
        hr = ID3D11Device1_CreateSamplerState( gl_platform->device, &anisotropic_desc, &samplers[ gl_sampler_type_anisotropic_8 ].pointer );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_initialize ID3D11Device1_CreateSamplerState failed for anisotropic 8.\n" );
            set_error( error, gl_error_device1_create_sampler_state_failed );
            break;
        }
        
        anisotropic_desc.MaxAnisotropy = 16;
        
        hr = ID3D11Device1_CreateSamplerState( gl_platform->device, &anisotropic_desc, &samplers[ gl_sampler_type_anisotropic_16 ].pointer );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_initialize ID3D11Device1_CreateSamplerState failed for anisotropic 16.\n" );
            set_error( error, gl_error_device1_create_sampler_state_failed );
            break;
        }
    }
    
    if ( is_error( error ) && gl_platform ) {
        
        if ( gl_platform->context ) {
            ID3D11DeviceContext1_OMSetBlendState( gl_platform->context, 0, 0, u32_max );
            ID3D11DeviceContext1_RSSetState( gl_platform->context, 0 );
            ID3D11DeviceContext1_OMSetDepthStencilState( gl_platform->context, 0, 0 );
        }
        
        for ( umm i = 0; i < array_count( gl_platform->samplers ); i++ ) {
            
            if ( gl_platform->samplers[ i ].pointer ) {
                ID3D11SamplerState_Release( gl_platform->samplers[ i ].pointer );
                gl_platform->samplers[ i ].pointer = 0;
            }
        }
        
        if ( gl_platform->swap_chain ) {
            ID3D11Device1_Release( gl_platform->swap_chain );
            gl_platform->swap_chain = 0;
        }
        
        if ( gl_platform->context ) {
            ID3D11Device1_Release( gl_platform->context );
            gl_platform->context = 0;
        }
        
        if ( gl_platform->device ) {
            ID3D11Device1_Release( gl_platform->device );
            gl_platform->device = 0;
        }
    }
    
    if ( base_device ) {
        ID3D11Device_Release( base_device );
        base_device = 0;
    }
    
    if ( base_device_context ) {
        ID3D11DeviceContext_Release( base_device_context );
        base_device_context = 0;
    }
    
    if ( dxgi_factory ) {
        IDXGIFactory2_Release( dxgi_factory );
        dxgi_factory = 0;
    }
    
    if ( blend_state ) {
        ID3D11BlendState_Release( blend_state );
        blend_state = 0;
    }
    
    if ( rasterizer_state ) {
        ID3D11RasterizerState1_Release( rasterizer_state );
        rasterizer_state = 0;
    }
    
    if ( depth_stencil_state ) {
        ID3D11DepthStencilState_Release( depth_stencil_state );
        depth_stencil_state = 0;
    }
}

stu void gl_platform_frame_start( gl_t* gl, u32* error ) {
    
    breakable {
        
        breakable_check( error );
        
        gl_platform_t* gl_platform = &gl->window->gl_platform;
        
        if ( !gl_platform->render_target_view ) {
            log_l( "[ERROR] gl_platform_frame_start RenderTargetView is null.\n" );
            set_error( error, gl_error_frame_start_render_target_view_is_null );
            break;
        }
        
        if ( !gl_platform->depth_stencil_view ) {
            log_l( "[ERROR] gl_platform_frame_start DepthStencilView is null.\n" );
            set_error( error, gl_error_frame_start_depth_stencil_view_is_null );
            break;
        }
        
        /* NOTE simon (06/02/24 13:12:09): OMSetRenderTargets needs to be called each frame (mentionned here https://devblogs.microsoft.com/directx/dxgi-flip-model/ ) and in another MSDN article about flip swap chain that I forgot to copy. */
        ID3D11DeviceContext1_OMSetRenderTargets( gl_platform->context, 1, &gl_platform->render_target_view, gl_platform->depth_stencil_view );
    }
}

stu gl_shader_t gl_shader_create( gl_t* gl, gl_vertex_format_t* vertex_format, data_t vertex_shader_source, data_t pixel_shader_source, u32* error ) {
    
    /* NOTE simon (01/02/24 12:49:02): Uniform/constants https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-constants?redirectedfrom=MSDN#Remarks */
    
    gl_shader_t result = { 0 };
    
    ID3DBlob* vs_blob = 0;
    ID3DBlob* vs_error_blob = 0;
    ID3D11VertexShader* vertex_shader = 0;
    ID3D11InputLayout* input_layout = 0;
    
    ID3DBlob* ps_blob = 0;
    ID3DBlob* ps_error_blob = 0;
    ID3D11PixelShader* pixel_shader = 0;
    
    breakable {
        
        breakable_check( error );
        
        ID3D11Device1* device = gl->window->gl_platform.device;
        
        /* NOTE simon (28/01/24 16:13:23): D3DCOMPILE_DEBUG */
        HRESULT hr = D3DCompile( vertex_shader_source.bytes, vertex_shader_source.used, 0, 0, 0, "main", "vs_5_0", 0, 0, &vs_blob, &vs_error_blob );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_shader_create D3DCompile failed for the vertex shader.\n" );
            
            if ( vs_error_blob ) {
                u8* error_message = ID3D10Blob_GetBufferPointer( vs_error_blob );
                umm length = ID3D10Blob_GetBufferSize( vs_error_blob );
                log_p( error_message, length );
                log_l( "\n" );
            }
            
            set_error( error, gl_error_vertex_shader_compile_failed );
            break;
        }
        
        hr = ID3D11Device1_CreateVertexShader( device, ID3D10Blob_GetBufferPointer( vs_blob ), ID3D10Blob_GetBufferSize( vs_blob ), 0, &vertex_shader );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_shader_create CreateVertexShader failed.\n" );
            set_error( error, gl_error_device1_create_vertex_shader_failed );
            break;
        }
        
        /* NOTE simon (02/02/24 17:39:21): In the shader, the semantics name can be anything. We specify here the name to the coresponding input.
         When specifying semantics for the output of the vertex shader, that should map to the input of the pixel shader, we can specify any name, it
doesn't have to be on of the names specified here. */
        
        D3D11_INPUT_ELEMENT_DESC layout_desc[ 3 ] = { 0 };
        u32 input_element_count = 0;
        
        layout_desc[ 0 ].SemanticName = "POSITION";
        
        if ( vertex_format->positions == 2 ) {
            layout_desc[ 0 ].Format = DXGI_FORMAT_R32G32_FLOAT;
        } else if ( vertex_format->positions == 3 ) {
            layout_desc[ 0 ].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        } else if ( vertex_format->positions == 4 ) {
            layout_desc[ 0 ].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
        
        layout_desc[ 0 ].AlignedByteOffset = 0;
        layout_desc[ 0 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        
        input_element_count++;
        
        if ( vertex_format->colors == 1 ) {
            layout_desc[ input_element_count ].SemanticName = "COLOR";
            layout_desc[ input_element_count ].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            layout_desc[ input_element_count ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            layout_desc[ input_element_count ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            input_element_count++;
        } else if ( vertex_format->colors == 4 ) {
            layout_desc[ input_element_count ].SemanticName = "COLOR";
            layout_desc[ input_element_count ].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            layout_desc[ input_element_count ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            layout_desc[ input_element_count ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            input_element_count++;
        }
        
        if ( vertex_format->uvs ) {
            layout_desc[ input_element_count ].SemanticName = "UV";
            layout_desc[ input_element_count ].Format = DXGI_FORMAT_R32G32_FLOAT;
            layout_desc[ input_element_count ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            layout_desc[ input_element_count ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            input_element_count++;
        }
        
        hr = ID3D11Device1_CreateInputLayout( device, layout_desc, input_element_count, ID3D10Blob_GetBufferPointer( vs_blob ), ID3D10Blob_GetBufferSize( vs_blob ), &input_layout );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_shader_create CreateInputLayout failed.\n" );
            set_error( error, gl_error_device1_create_input_layout_failed );
            break;
        }
        
        
        hr = D3DCompile( pixel_shader_source.bytes, pixel_shader_source.used, 0, 0, 0, "main", "ps_5_0", 0, 0, &ps_blob, &ps_error_blob );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_shader_create D3DCompile failed for the pixel shader.\n" );
            
            if ( ps_error_blob ) {
                u8* error_message = ID3D10Blob_GetBufferPointer( ps_error_blob );
                umm length = ID3D10Blob_GetBufferSize( ps_error_blob );
                log_p( error_message, length );
                log_l( "\n" );
            }
            
            set_error( error, gl_error_pixel_shader_compile_failed );
            break;
        }
        
        hr = ID3D11Device1_CreatePixelShader( device, ID3D10Blob_GetBufferPointer( ps_blob ), ID3D10Blob_GetBufferSize( ps_blob ), 0, &pixel_shader );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_shader_create CreatePixelShader failed.\n" );
            set_error( error, gl_error_device1_create_pixel_shader_failed );
            break;
        }
    }
    
    if ( vs_blob ) {
        ID3D10Blob_Release( vs_blob );
        vs_blob = 0;
    }
    
    if ( vs_error_blob ) {
        ID3D10Blob_Release( vs_error_blob );
        vs_error_blob = 0;
    }
    
    if ( ps_blob ) {
        ID3D10Blob_Release( ps_blob );
        ps_blob = 0;
    }
    
    if ( ps_error_blob ) {
        ID3D10Blob_Release( ps_error_blob );
        ps_error_blob = 0;
    }
    
    if ( is_error( error ) ) {
        
        if ( vertex_shader ) {
            ID3D11VertexShader_Release( vertex_shader );
            vertex_shader = 0;
        }
        
        if ( input_layout ) {
            ID3D11InputLayout_Release( input_layout );
            input_layout = 0;
        }
        
        if ( pixel_shader ) {
            ID3D11PixelShader_Release( pixel_shader );
            pixel_shader = 0;
        }
        
    } else {
        
        result.vertex = vertex_shader;
        result.layout = input_layout;
        result.pixel = pixel_shader;
        result.valid = true;
    }
    
    return result;
}

stu void gl_shader_free( gl_shader_t* shader ) {
    
    if ( shader->vertex ) {
        ID3D11VertexShader_Release( shader->vertex );
        shader->vertex = 0;
    }
    
    if ( shader->layout ) {
        ID3D11InputLayout_Release( shader->layout );
        shader->layout = 0;
    }
    
    if ( shader->pixel ) {
        ID3D11PixelShader_Release( shader->pixel );
        shader->pixel = 0;
    }
    
    shader->valid = false;
}

stu gl_texture_t gl_texture_create( gl_t* gl, u8* pixels, u32 width, u32 height, u8 bytes_per_pixel, u32 flags, u32* error ) {
    
    gl_texture_t result = { 0 };
    ID3D11Device1* device = gl->window->gl_platform.device;
    ID3D11DeviceContext1* context = gl->window->gl_platform.context;
    
    _assert( bytes_per_pixel == 1 || bytes_per_pixel == 4 );
    
    breakable {
        
        breakable_check( error );
        
        b32 inverse_input_format = flag_is_set_u32( flags, gl_texture_flag_inverse_input_format );
        b32 generate_mipmaps = flag_is_set_u32( flags, gl_texture_flag_generate_mipmaps );
        
        D3D11_TEXTURE2D_DESC texture_desc = { 0 };
        texture_desc.Width = width;
        texture_desc.Height = height;
        texture_desc.MipLevels = 1;
        texture_desc.ArraySize = 1;
        texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        texture_desc.SampleDesc.Count = 1;
        texture_desc.Usage = D3D11_USAGE_IMMUTABLE;
        texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        
        if ( generate_mipmaps ) {
            texture_desc.MipLevels = 0;
            texture_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        }
        
        if ( inverse_input_format ) {
            texture_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        }
        
        if ( bytes_per_pixel == 1 ) {
            texture_desc.Format = DXGI_FORMAT_A8_UNORM;
        }
        
        D3D11_SUBRESOURCE_DATA texture_data = { 0 };
        texture_data.pSysMem = pixels;
        texture_data.SysMemPitch = width * bytes_per_pixel;
        
        HRESULT hr = ID3D11Device1_CreateTexture2D( device, &texture_desc, &texture_data, &result.pointer );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_texture_create ID3D11Device1_CreateTexture2D failed.\n" );
            set_error( error, gl_error_device1_create_texture_2d_failed );
            break;
        }
        
        hr = ID3D11Device1_CreateShaderResourceView( device, cast( ID3D11Resource*, result.pointer ), 0, &result.view );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_texture_create ID3D11Device1_CreateShaderResourceView failed.\n" );
            set_error( error, gl_error_device1_create_shader_resource_view_failed );
            break;
        }
        
        result.width = width;
        result.height = height;
        result.bytes_per_pixel = bytes_per_pixel;
        
        if ( generate_mipmaps ) {
            ID3D11DeviceContext1_GenerateMips( context, result.view );
        }
    }
    
    if ( is_error( error ) ) {
        
        if ( result.pointer ) {
            ID3D11Texture2D_Release( result.pointer );
            result.pointer = 0;
        }
        
        if ( result.view ) {
            ID3D11ShaderResourceView_Release( result.view );
            result.view = 0;
        }
        
        result = ( gl_texture_t ) { 0 };
    }
    
    return result;
}

stu b32 gl_texture_is_valid( gl_texture_t* texture ) {
    b32 result = texture && texture->pointer && texture->view;
    return result;
}

stu void gl_texture_free( gl_texture_t* texture ) {
    
    if ( texture->pointer ) {
        ID3D11Texture2D_Release( texture->pointer );
        texture->pointer = 0;
    }
    
    if ( texture->view ) {
        ID3D11ShaderResourceView_Release( texture->view );
        texture->view = 0;
    }
}

typedef enum gl_buffer_type_t {
    gl_buffer_type_none,
    gl_buffer_type_vertex,
    gl_buffer_type_index,
    gl_buffer_type_constant,
} gl_buffer_type_t;

stu gl_buffer_t gl_buffer_create( gl_t* gl, u32 type, u32 size_bytes, u32* error ) {
    
    gl_buffer_t result = { 0 };
    
    breakable {
        
        breakable_check( error );
        
        D3D11_BUFFER_DESC buffer_desc = { 0 };
        buffer_desc.ByteWidth = size_bytes;
        buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
        
        if ( type == gl_buffer_type_vertex ) {
            buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        } else if ( type == gl_buffer_type_index ) {
            buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        } else if ( type == gl_buffer_type_constant ) {
            buffer_desc.ByteWidth = ( size_bytes + 0xf ) & 0xfffffff0;
            buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        } else {
            log_l( "[ERROR] gl_buffer_create Invalid buffer type.\n" );
            set_error( error, gl_error_create_buffer_invalid_type );
            break;
        }
        
        buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        
        HRESULT hr = ID3D11Device1_CreateBuffer( gl->window->gl_platform.device, &buffer_desc, 0, &result.pointer );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_buffer_create ID3D11Device1_CreateBuffer failed.\n" );
            set_error( error, gl_error_device1_create_buffer_failed );
            break;
        }
    }
    
    return result;
}

stu u8* gl_buffer_map( gl_t* gl, gl_buffer_t* buffer, u32* error ) {
    
    u8* result = 0;
    
    breakable {
        
        breakable_check( error );
        
        if ( buffer->mapped ) {
            log_l( "[ERROR] gl_buffer_map Buffer is already mapped.\n" );
            set_error( error, gl_error_buffer_map_already_mapped );
            break;
        }
        
        D3D11_MAPPED_SUBRESOURCE map = { 0 };
        HRESULT hr = ID3D11DeviceContext1_Map( gl->window->gl_platform.context, cast( ID3D11Resource*, buffer->pointer ), 0, D3D11_MAP_WRITE_DISCARD, 0, &map );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_buffer_map ID3D11DeviceContext1_Map failed.\n" );
            set_error( error, gl_error_device_context1_map_failed );
            break;
        }
        
        result = map.pData;
        buffer->mapped = true;
    }
    
    return result;
}

stu void gl_buffer_unmap( gl_t* gl, gl_buffer_t* buffer, u32* error ) {
    
    breakable {
        
        /* NOTE simon (08/02/24 13:15:56): We don't do breakable check here because we still want to unmap the buffer in case of errors. */
        /* breakable_check( error ); */
        
        if ( !buffer->mapped ) {
            log_l( "[ERROR] gl_buffer_unmap Buffer isn't mapped.\n" );
            set_error( error, gl_error_unmap_not_mapped );
            break;
        }
        
        ID3D11DeviceContext1_Unmap( gl->window->gl_platform.context, cast( ID3D11Resource*, buffer->pointer ), 0 );
        
        buffer->mapped = false;
    }
}

stu void gl_buffer_free( gl_t* gl, gl_buffer_t* buffer ) {
    
    if ( buffer && buffer->pointer ) {
        
        if ( buffer->mapped ) {
            u32 error = 0;
            gl_buffer_unmap( gl, buffer, &error );
        }
        
        ID3D11Buffer_Release( buffer->pointer );
        buffer->pointer = 0;
    }
}

stu void internal_gl_do_scissor( gl_t* gl, gl_command_t* command ) {
    
    D3D11_RECT scissor = { 0, 0, gl->window->width, gl->window->height };
    
    if ( command->scissor.enable ) {
        scissor.left = command->scissor.x;
        scissor.top = gl->window->height - command->scissor.y - command->scissor.height;
        scissor.right = command->scissor.x + command->scissor.width;
        scissor.bottom = gl->window->height - command->scissor.y;
    }
    
    ID3D11DeviceContext1_RSSetScissorRects( gl->window->gl_platform.context, 1, &scissor );
}

/* NOTE simon (08/02/24 14:55:23): A possible improvement (not sure) would be to just have all command into the command buffer (no batches);
do a pass where we build the index and vertex buffers, and than a second pass to draw, so that we could interleave different primitive types ?
It would probably be less performant as we would have more batches of smaller size, but would be more flexible for the user ? */
stu void gl_render( gl_t* gl, gl_command_buffer_t* command_buffer, u32* error ) {
    
    profiler_event_start( gl_render );
    
    gl_platform_t* gl_platform = &gl->window->gl_platform;
    ID3D11Device1* device = gl_platform->device;
    ID3D11DeviceContext1* context = gl_platform->context;
    
    gl_command_t* buffer_commands = cast( gl_command_t*, command_buffer->commands.bytes );
    
    for ( umm global_command_index = 0; global_command_index < command_buffer->command_count; global_command_index++ ) {
        
        breakable_check( error );
        
        gl_command_t* buffer_command = buffer_commands + global_command_index;
        
        switch ( buffer_command->type ) {
            
            case gl_command_type_viewport: {
                
                f32 y = cast( f32, gl->window->height - buffer_command->viewport.y - buffer_command->viewport.height );
                
                D3D11_VIEWPORT viewport = {
                    .TopLeftX = cast( f32, buffer_command->viewport.x ),
                    .TopLeftY = y,
                    .Width = cast( f32, buffer_command->viewport.width ),
                    .Height = cast( f32, buffer_command->viewport.height ),
                    .MinDepth = 0,
                    .MaxDepth = 1
                };
                
                ID3D11DeviceContext1_RSSetViewports( context, 1, &viewport );
                
            } break;
            
            case gl_command_type_scissor: {
                
                /* TODO simon (30/01/24 13:19:40): Since DX can set several scissors, we might want to group all scissor operation instead of doing them as we go ? */
                internal_gl_do_scissor( gl, buffer_command );
                
            } break;
            
            case gl_command_type_clear: {
                
                vec4 srgb = v4_from_u32_color( buffer_command->clear.color );
                vec4 linear = v4_srgb_to_linear_precise( srgb );
                ID3D11DeviceContext1_ClearRenderTargetView( context, gl_platform->render_target_view, linear.e );
                ID3D11DeviceContext1_ClearDepthStencilView( context, gl_platform->depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0 );
                
            } break;
            
            case gl_command_type_batch: {
                
                u32 batch_error = 0;
                gl_batch_t* batch = buffer_command->batch.batch;
                
                breakable {
                    
                    gl_command_t* batch_commands = cast( gl_command_t*, batch->commands.bytes );
                    
                    if ( !batch->command_count ) {
                        break;
                    }
                    
                    if ( !batch->vertex_buffer.pointer ) {
                        log_l( "[WARNING] batch vertex buffer wasn't created. Skipping this batch.\n" );
                        set_error( &batch_error, 1 );
                        break;
                    }
                    
                    if ( !batch->index_buffer.pointer ) {
                        log_l( "[WARNING] batch index buffer wasn't created. Skipping this batch.\n" );
                        set_error( &batch_error, 2 );
                        break;
                    }
                    
                    if ( batch->uniform.used ) {
                        
                        /* NOTE simon (08/02/24 14:17:04): Constants packing in HLSL https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-rules */
                        
                        if ( batch->batch_constants_buffer.pointer ) {
                            
                            D3D11_BUFFER_DESC constant_buffer_desc = { 0 };
                            ID3D11Buffer_GetDesc( batch->batch_constants_buffer.pointer, &constant_buffer_desc );
                            
                            if ( constant_buffer_desc.ByteWidth < batch->uniform.used ) {
                                gl_buffer_free( gl, &batch->batch_constants_buffer );
                            }
                        }
                        
                        if ( !batch->batch_constants_buffer.pointer ) {
                            batch->batch_constants_buffer = gl_buffer_create( gl, gl_buffer_type_constant, cast( u32, batch->uniform.used ), &batch_error );
                        }
                    }
                    
                    breakable_check( &batch_error );
                    
                    if ( batch->uniform.used && batch->batch_constants_buffer.pointer ) {
                        
                        u8* constants_buffer = gl_buffer_map( gl, &batch->batch_constants_buffer, &batch_error );
                        
                        if ( no_error( &batch_error ) ) {
                            memory_copy_p( constants_buffer, batch->uniform.bytes, cast( u32, batch->uniform.used ) );
                        }
                        
                        gl_buffer_unmap( gl, &batch->batch_constants_buffer, &batch_error );
                    }
                    
                    breakable_check( &batch_error );
                    
                    u32 vertex_format_size_byte = gl_vertex_format_compute_vertex_size_byte( batch->vertex_format );
                    
                    u32 stride = vertex_format_size_byte;
                    u32 offset = 0;
                    ID3D11DeviceContext1_IASetVertexBuffers( context, 0, 1, &batch->vertex_buffer.pointer, &stride, &offset );
                    
                    u32 position_component_count = batch->vertex_format.positions;
                    
                    _assert( batch->vertex_format.index_size == 2 || batch->vertex_format.index_size == 4 );
                    /* NOTE simon (30/01/24 17:21:23): Primitive restart in D3D is called Strip Cutting. */
                    u32 primitive_restart_index = gl_primitive_restart_index_u32;
                    u32 index_size = 4;
                    
                    if ( batch->vertex_format.index_size == 2 ) {
                        ID3D11DeviceContext1_IASetIndexBuffer( context, batch->index_buffer.pointer, DXGI_FORMAT_R16_UINT, 0 );
                        primitive_restart_index = gl_primitive_restart_index_u16;
                        index_size = 2;
                    } else {
                        ID3D11DeviceContext1_IASetIndexBuffer( context, batch->index_buffer.pointer, DXGI_FORMAT_R32_UINT, 0 );
                        primitive_restart_index = gl_primitive_restart_index_u32;
                        index_size = 4;
                    }
                    
                    ID3D11DeviceContext1_IASetInputLayout( context, batch->shader->layout );
                    
                    ID3D11DeviceContext1_VSSetShader( context, batch->shader->vertex, 0, 0 );
                    
                    if ( batch->uniform.used && batch->batch_constants_buffer.pointer ) {
                        ID3D11DeviceContext1_VSSetConstantBuffers( context, 0, 1, &batch->batch_constants_buffer.pointer );
                    }
                    
                    ID3D11DeviceContext1_PSSetShader( context, batch->shader->pixel, 0, 0 );
                    
                    u8* vertex_buffer = gl_buffer_map( gl, &batch->vertex_buffer, &batch_error );
                    u8* index_buffer = gl_buffer_map( gl, &batch->index_buffer, &batch_error );
                    
                    breakable_check( &batch_error );
                    
                    switch ( batch->type ) {
                        
                        case gl_command_type_point: {
                            
                            umm vertex_format_size = vertex_format_size_byte / 4;
                            
                            for ( u32 batch_command_index = 0; batch_command_index < batch->command_count; batch_command_index++ ) {
                                
                                gl_command_t* command = batch_commands + batch_command_index;
                                
                                if ( command->type != gl_command_type_point ) {
                                    continue;
                                }
                                
                                f32* v_input_array = command->point.vertex_array;
                                u32* c_input_array = cast( u32*, command->point.vertex_array );
                                
                                u32 v_array_size = vertex_format_size_byte * ( ( position_component_count == 2 ) ? 4 : 8 ) * command->point.point_count;
                                u32 i_array_size = command->point.point_count * ( ( position_component_count == 2 ) ? 5 : 15 ) * index_size;
                                
                                if ( batch->vertex_used + v_array_size > batch->v_buffer_size || batch->index_used + i_array_size > batch->i_buffer_size ) {
                                    log_l( "[WARNING] gl_command_type_point Not enough space in the vertex/index buffer. Skipping remaining commands.\n" );
                                    batch->command_count = batch_command_index;
                                    break;
                                }
                                
                                f32* v_array = cast( f32*, vertex_buffer + batch->vertex_used );
                                u32* c_array = cast( u32*, v_array );
                                
                                void* i_array = cast( void*, index_buffer + batch->index_used );
                                umm i_array_index = 0;
                                umm v_array_index = 0;
                                
                                f32 radius = command->point.size * 0.5f;
                                
                                vec4 offsets[ 8 ] = {
                                    v4( -radius, -radius, -radius, 1.0f ),
                                    v4( -radius, radius, -radius, 1.0f ),
                                    v4( radius, -radius, -radius, 1.0f ),
                                    v4( radius, radius, -radius, 1.0f ),
                                    
                                    v4( -radius, -radius, radius, 1.0f ),
                                    v4( -radius, radius, radius, 1.0f ),
                                    v4( radius, -radius, radius, 1.0f ),
                                    v4( radius, radius, radius, 1.0f )
                                };
                                
                                if ( position_component_count == 2 ) {
                                    
                                    umm input_index = 0;
                                    
                                    for ( umm point_index = 0; point_index < command->point.point_count; point_index++ ) {
                                        
                                        for ( umm offset_index = 0; offset_index < 4; offset_index++ ) {
                                            
                                            for ( umm i = 0; i < 2; i++ ) {
                                                v_array[ v_array_index++ ] = v_input_array[ input_index + i ] + offsets[ offset_index ].e[ i ];
                                            }
                                            
                                            if ( batch->vertex_format.colors == 1 ) {
                                                c_array[ v_array_index++ ] = c_input_array[ input_index + 2 ];
                                            } else if ( batch->vertex_format.colors == 4 ) {
                                                v_array[ v_array_index++ ] = v_input_array[ input_index + 2 ];
                                                v_array[ v_array_index++ ] = v_input_array[ input_index + 3 ];
                                                v_array[ v_array_index++ ] = v_input_array[ input_index + 4 ];
                                                v_array[ v_array_index++ ] = v_input_array[ input_index + 5 ];
                                            }
                                            
                                            internal_gl_push_index( i_array, &i_array_index, batch->vertex_index++, index_size );
                                        }
                                        
                                        input_index += vertex_format_size;
                                        
                                        internal_gl_push_primitive_restart( i_array, &i_array_index, index_size );
                                    }
                                    
                                } else {
                                    
                                    umm input_index = 0;
                                    
                                    for ( umm point_index = 0; point_index < command->point.point_count; point_index++ ) {
                                        
                                        for ( umm offset_index = 0; offset_index < array_count( offsets ); offset_index++ ) {
                                            
                                            for ( umm i = 0; i < position_component_count; i++ ) {
                                                v_array[ v_array_index++ ] = command->point.vertex_array[ input_index + i ] + offsets[ offset_index ].e[ i ];
                                            }
                                            
                                            if ( batch->vertex_format.colors == 1 ) {
                                                c_array[ v_array_index++ ] = c_input_array[ input_index + position_component_count ];
                                            } else if ( batch->vertex_format.colors == 4 ) {
                                                v_array[ v_array_index++ ] = v_input_array[ input_index + position_component_count + 0 ];
                                                v_array[ v_array_index++ ] = v_input_array[ input_index + position_component_count + 1 ];
                                                v_array[ v_array_index++ ] = v_input_array[ input_index + position_component_count + 2 ];
                                                v_array[ v_array_index++ ] = v_input_array[ input_index + position_component_count + 3 ];
                                            }
                                        }
                                        
                                        input_index += vertex_format_size;
                                        
                                        internal_gl_push_index( i_array, &i_array_index, batch->vertex_index + 0, index_size );
                                        internal_gl_push_index( i_array, &i_array_index, batch->vertex_index + 1, index_size );
                                        internal_gl_push_index( i_array, &i_array_index, batch->vertex_index + 2, index_size );
                                        internal_gl_push_index( i_array, &i_array_index, batch->vertex_index + 3, index_size );
                                        internal_gl_push_index( i_array, &i_array_index, batch->vertex_index + 7, index_size );
                                        internal_gl_push_index( i_array, &i_array_index, batch->vertex_index + 1, index_size );
                                        internal_gl_push_index( i_array, &i_array_index, batch->vertex_index + 5, index_size );
                                        internal_gl_push_index( i_array, &i_array_index, batch->vertex_index + 0, index_size );
                                        internal_gl_push_index( i_array, &i_array_index, batch->vertex_index + 4, index_size );
                                        internal_gl_push_index( i_array, &i_array_index, batch->vertex_index + 2, index_size );
                                        internal_gl_push_index( i_array, &i_array_index, batch->vertex_index + 6, index_size );
                                        internal_gl_push_index( i_array, &i_array_index, batch->vertex_index + 7, index_size );
                                        internal_gl_push_index( i_array, &i_array_index, batch->vertex_index + 4, index_size );
                                        internal_gl_push_index( i_array, &i_array_index, batch->vertex_index + 5, index_size );
                                        
                                        batch->vertex_index += 8;
                                        
                                        internal_gl_push_primitive_restart( i_array, &i_array_index, index_size );
                                    }
                                }
                                
                                batch->vertex_used += v_array_size;
                                batch->index_used += i_array_size;
                                _assert( batch->vertex_used <= batch->v_buffer_size );
                                _assert( batch->index_used <= batch->i_buffer_size );
                            }
                            
                            gl_buffer_unmap( gl, &batch->vertex_buffer, &batch_error );
                            gl_buffer_unmap( gl, &batch->index_buffer, &batch_error );
                            
                            breakable_check( &batch_error );
                            
                            ID3D11DeviceContext1_IASetPrimitiveTopology( context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
                            
                            u32 index_count_per_point = 5;
                            
                            if ( position_component_count == 3 ) {
                                index_count_per_point = 15;
                            }
                            
                            u32 index_count_to_draw = 0;
                            u32 index_count_drawn = 0;
                            
                            for ( umm i = 0; i < batch->command_count; i++ ) {
                                
                                gl_command_t* command = batch_commands + i;
                                
                                if ( command->type == gl_command_type_point ) {
                                    
                                    index_count_to_draw += command->point.point_count * index_count_per_point;
                                    
                                } else if ( command->type == gl_command_type_scissor ) {
                                    
                                    if ( index_count_to_draw ) {
                                        ID3D11DeviceContext1_DrawIndexed( context, index_count_to_draw, index_count_drawn, 0 );
                                        index_count_drawn += index_count_to_draw;
                                        index_count_to_draw = 0;
                                    }
                                    
                                    internal_gl_do_scissor( gl, command );
                                    
                                } else {
                                    invalid_code_path( );
                                }
                            }
                            
                            if ( index_count_to_draw ) {
                                ID3D11DeviceContext1_DrawIndexed( context, index_count_to_draw, index_count_drawn, 0 );
                            }
                            
                        } break;
                        
                        case gl_command_type_line: {
                            
                            for ( u32 batch_command_index = 0; batch_command_index < batch->command_count; batch_command_index++ ) {
                                
                                gl_command_t* command = batch_commands + batch_command_index;
                                
                                if ( command->type != gl_command_type_line ) {
                                    continue;
                                }
                                
                                u32 v_array_size = vertex_format_size_byte * command->line.point_count;
                                u32 i_array_size = ( command->line.point_count + 1 ) * index_size;
                                
                                if ( batch->vertex_used + v_array_size > batch->v_buffer_size || batch->index_used + i_array_size > batch->i_buffer_size ) {
                                    log_l( "[WARNING] gl_command_type_line Not enough space in the vertex/index buffer. Skipping remaining commands.\n" );
                                    batch->command_count = batch_command_index;
                                    break;
                                }
                                
                                void* i_array = cast( void*, index_buffer + batch->index_used );
                                umm i_array_index = 0;
                                
                                for ( umm point_index = 0; point_index < command->line.point_count; point_index++ ) {
                                    internal_gl_push_index( i_array, &i_array_index, batch->vertex_index++, index_size );
                                }
                                
                                internal_gl_push_primitive_restart( i_array, &i_array_index, index_size );
                                
                                batch->vertex_used += v_array_size;
                                batch->index_used += i_array_size;
                                
                                _assert( batch->vertex_used <= batch->v_buffer_size );
                                _assert( batch->index_used <= batch->i_buffer_size );
                            }
                            
                            /* NOTE simon (08/02/24 16:48:26): batch->datas.used can be different from batch->vertex_used if we hit the warning above. */
                            memory_copy_p( vertex_buffer, batch->datas.bytes, batch->vertex_used );
                            
                            gl_buffer_unmap( gl, &batch->vertex_buffer, &batch_error );
                            gl_buffer_unmap( gl, &batch->index_buffer, &batch_error );
                            
                            breakable_check( &batch_error );
                            
                            ID3D11DeviceContext1_IASetPrimitiveTopology( context, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );
                            
                            u32 index_count_to_draw = 0;
                            u32 index_count_drawn = 0;
                            
                            for ( umm i = 0; i < batch->command_count; i++ ) {
                                
                                gl_command_t* command = batch_commands + i;
                                
                                if ( command->type == gl_command_type_line ) {
                                    index_count_to_draw += command->line.point_count + 1;
                                    
                                } else if ( command->type == gl_command_type_scissor ) {
                                    
                                    if ( index_count_to_draw ) {
                                        ID3D11DeviceContext1_DrawIndexed( context, index_count_to_draw, index_count_drawn, 0 );
                                        index_count_drawn += index_count_to_draw;
                                        index_count_to_draw = 0;
                                    }
                                    
                                    internal_gl_do_scissor( gl, command );
                                    
                                } else {
                                    
                                    invalid_code_path( );
                                }
                            }
                            
                            if ( index_count_to_draw ) {
                                ID3D11DeviceContext1_DrawIndexed( context, index_count_to_draw, index_count_drawn, 0 );
                            }
                            
                        } break;
                        
                        /* TODO simon: gl_command_type_triangle. */
                        
                        case gl_command_type_triangle_strip: {
                            
                            if ( batch->vertex_format.uvs && batch->texture && batch->sampler ) {
                                ID3D11DeviceContext1_PSSetShaderResources( context, 0, 1, &batch->texture->view );
                                ID3D11DeviceContext1_PSSetSamplers( context, 0, 1, &batch->sampler->pointer );
                            }
                            
                            for ( u32 batch_command_index = 0; batch_command_index < batch->command_count; batch_command_index++ ) {
                                
                                gl_command_t* command = batch_commands + batch_command_index;
                                
                                if ( command->type != gl_command_type_triangle_strip ) {
                                    continue;
                                }
                                
                                u32 i_array_size = ( 3 + command->triangle_strip.triangle_count ) * index_size;
                                u32 v_array_size = ( 2 + command->triangle_strip.triangle_count ) * vertex_format_size_byte;
                                
                                if ( batch->vertex_used + v_array_size > batch->v_buffer_size || batch->index_used + i_array_size > batch->i_buffer_size ) {
                                    log_l( "[WARNING] gl_command_type_triangle_strip Not enough space in the vertex/index buffer. Skipping remaining commands.\n" );
                                    batch->command_count = batch_command_index;
                                    break;
                                }
                                
                                void* i_array = cast( void*, index_buffer + batch->index_used );
                                umm i_array_index = 0;
                                
                                umm vertex_count = command->triangle_strip.triangle_count + 2;
                                
                                for ( umm vertex_index = 0; vertex_index < vertex_count; vertex_index++ ) {
                                    internal_gl_push_index( i_array, &i_array_index, batch->vertex_index++, index_size );
                                }
                                
                                internal_gl_push_primitive_restart( i_array, &i_array_index, index_size );
                                
                                batch->vertex_used += v_array_size;
                                batch->index_used += i_array_size;
                                _assert( batch->vertex_used <= batch->v_buffer_size );
                                _assert( batch->index_used <= batch->i_buffer_size );
                            }
                            
                            /* NOTE simon (08/02/24 16:48:26): batch->datas.used can be different from batch->vertex_used if we hit the warning above. */
                            memory_copy_p( vertex_buffer, batch->datas.bytes, batch->vertex_used );
                            
                            gl_buffer_unmap( gl, &batch->vertex_buffer, &batch_error );
                            gl_buffer_unmap( gl, &batch->index_buffer, &batch_error );
                            
                            breakable_check( &batch_error );
                            
                            ID3D11DeviceContext1_IASetPrimitiveTopology( context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
                            
                            u32 index_count_to_draw = 0;
                            u32 index_count_drawn = 0;
                            
                            for ( umm i = 0; i < batch->command_count; i++ ) {
                                
                                gl_command_t* command = batch_commands + i;
                                
                                if ( command->type == gl_command_type_triangle_strip ) {
                                    index_count_to_draw += command->triangle_strip.triangle_count + 3;
                                    
                                } else if ( command->type == gl_command_type_scissor ) {
                                    
                                    if ( index_count_to_draw ) {
                                        ID3D11DeviceContext1_DrawIndexed( context, index_count_to_draw, index_count_drawn, 0 );
                                        index_count_drawn += index_count_to_draw;
                                        index_count_to_draw = 0;
                                    }
                                    
                                    internal_gl_do_scissor( gl, command );
                                    
                                } else {
                                    
                                    invalid_code_path( );
                                }
                            }
                            
                            if ( index_count_to_draw ) {
                                ID3D11DeviceContext1_DrawIndexed( context, index_count_to_draw, index_count_drawn, 0 );
                            }
                            
                        } break;
                        
                        case gl_command_type_triangle_strip_textured: {
                            
                            for ( u32 batch_command_index = 0; batch_command_index < batch->command_count; batch_command_index++ ) {
                                
                                gl_command_t* command = batch_commands + batch_command_index;
                                
                                if ( command->type != gl_command_type_triangle_strip_textured ) {
                                    continue;
                                }
                                
                                u32 i_array_size = ( 3 + command->triangle_strip.triangle_count ) * index_size;
                                u32 v_array_size = ( 2 + command->triangle_strip.triangle_count ) * vertex_format_size_byte;
                                
                                if ( batch->vertex_used + v_array_size > batch->v_buffer_size || batch->index_used + i_array_size > batch->i_buffer_size ) {
                                    log_l( "[WARNING] gl_command_type_triangle_strip_textured Not enough space in the vertex/index buffer. Skipping remaining commands.\n" );
                                    batch->command_count = batch_command_index;
                                    break;
                                }
                                
                                void* i_array = cast( void*, index_buffer + batch->index_used );
                                umm i_array_index = 0;
                                
                                umm vertex_count = command->triangle_strip.triangle_count + 2;
                                
                                for ( umm vertex_index = 0; vertex_index < vertex_count; vertex_index++ ) {
                                    internal_gl_push_index( i_array, &i_array_index, batch->vertex_index++, index_size );
                                }
                                
                                internal_gl_push_primitive_restart( i_array, &i_array_index, index_size );
                                
                                batch->vertex_used += v_array_size;
                                batch->index_used += i_array_size;
                                _assert( batch->vertex_used <= batch->v_buffer_size );
                                _assert( batch->index_used <= batch->i_buffer_size );
                            }
                            
                            /* NOTE simon (08/02/24 16:48:26): batch->datas.used can be different from batch->vertex_used if we hit the warning above. */
                            memory_copy_p( vertex_buffer, batch->datas.bytes, batch->vertex_used );
                            
                            gl_buffer_unmap( gl, &batch->vertex_buffer, &batch_error );
                            gl_buffer_unmap( gl, &batch->index_buffer, &batch_error );
                            
                            breakable_check( &batch_error );
                            
                            ID3D11DeviceContext1_IASetPrimitiveTopology( context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
                            
                            u32 index_count_drawn = 0;
                            
                            for ( umm i = 0; i < batch->command_count; i++ ) {
                                
                                gl_command_t* command = batch_commands + i;
                                
                                if ( command->type == gl_command_type_triangle_strip_textured ) {
                                    
                                    u32 index_count_to_draw = command->triangle_strip.triangle_count + 3;
                                    
                                    ID3D11DeviceContext1_PSSetSamplers( context, 0, 1, &command->triangle_strip.sampler->pointer );
                                    ID3D11DeviceContext1_PSSetShaderResources( context, 0, 1, &command->triangle_strip.texture->view );
                                    ID3D11DeviceContext1_DrawIndexed( context, index_count_to_draw, index_count_drawn, 0 );
                                    index_count_drawn += index_count_to_draw;
                                    
                                } else if ( command->type == gl_command_type_scissor ) {
                                    
                                    internal_gl_do_scissor( gl, command );
                                    
                                } else {
                                    
                                    invalid_code_path( );
                                }
                            }
                            
                        } break;
                        
                        case gl_command_type_text: {
                            
                            if ( batch->vertex_format.uvs && batch->texture && batch->sampler ) {
                                ID3D11DeviceContext1_PSSetShaderResources( context, 0, 1, &batch->texture->view );
                                ID3D11DeviceContext1_PSSetSamplers( context, 0, 1, &batch->sampler->pointer );
                            }
                            
                            for ( u32 batch_command_index = 0; batch_command_index < batch->command_count; batch_command_index++ ) {
                                
                                gl_command_t* command = batch_commands + batch_command_index;
                                
                                if ( command->type != gl_command_type_text ) {
                                    continue;
                                }
                                
                                u32 codepoint_count = cast( u32, command->text.string.used );
                                u32 index_count = codepoint_count * 5;
                                u32 v_array_size = vertex_format_size_byte * 4 * codepoint_count;
                                u32 i_array_size = index_count * index_size;
                                
                                if ( batch->vertex_used + v_array_size > batch->v_buffer_size || batch->index_used + i_array_size > batch->i_buffer_size ) {
                                    log_l( "[WARNING] gl_command_type_text Not enough space in the vertex/index buffer. Skipping remaining commands.\n" );
                                    batch->command_count = batch_command_index;
                                    break;
                                }
                                
                                f32* v_array = cast( f32*, vertex_buffer + batch->vertex_used );
                                void* i_array = cast( void*, index_buffer + batch->index_used );
                                
                                command->text.outputed_codepoint_count = gl_text_to_vertex_array( command->text.string.text, command->text.string.used, command->text.position.x, command->text.position.y, command->text.previous_codepoint, batch->collection, command->text.font_index, v_array, i_array, batch->vertex_used / vertex_format_size_byte, command->text.color, batch->vertex_format );
                                
                                /* NOTE simon (16/02/24 17:06:59): I might want modify gl_text_to_vertex_array to make sure it can't overflow. */
                                _assert( command->text.outputed_codepoint_count <= codepoint_count );
                                
                                v_array_size = vertex_format_size_byte * 4 * command->text.outputed_codepoint_count;
                                i_array_size = command->text.outputed_codepoint_count * 5 * index_size;
                                
                                batch->vertex_used += v_array_size;
                                batch->index_used += i_array_size;
                                _assert( batch->vertex_used <= batch->v_buffer_size );
                                _assert( batch->index_used <= batch->i_buffer_size );
                            }
                            
                            gl_buffer_unmap( gl, &batch->vertex_buffer, &batch_error );
                            gl_buffer_unmap( gl, &batch->index_buffer, &batch_error );
                            
                            breakable_check( &batch_error );
                            
                            ID3D11DeviceContext1_IASetPrimitiveTopology( context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
                            
                            u32 index_count_to_draw = 0;
                            u32 index_count_drawn = 0;
                            
                            for ( umm i = 0; i < batch->command_count; i++ ) {
                                
                                gl_command_t* command = batch_commands + i;
                                
                                if ( command->type == gl_command_type_text ) {
                                    index_count_to_draw += command->text.outputed_codepoint_count * 5;
                                } else if ( command->type == gl_command_type_scissor ) {
                                    
                                    if ( index_count_to_draw ) {
                                        ID3D11DeviceContext1_DrawIndexed( context, index_count_to_draw, index_count_drawn, 0 );
                                        index_count_drawn += index_count_to_draw;
                                        index_count_to_draw = 0;
                                    }
                                    
                                    internal_gl_do_scissor( gl, command );
                                    
                                } else {
                                    
                                    invalid_code_path( );
                                }
                            }
                            
                            if ( index_count_to_draw ) {
                                ID3D11DeviceContext1_DrawIndexed( context, index_count_to_draw, index_count_drawn, 0 );
                            }
                            
                        } break;
                        
                        default: {
                            invalid_code_path( );
                        } break;
                    }
                    
                    /* NOTE simon (08/02/24 16:31:51): This allows to redraw the same command buffer if we don't reset it. */
                    batch->vertex_used = 0;
                    batch->vertex_index = 0;
                    batch->index_used = 0;
                }
                
                if ( batch->batch_constants_buffer.mapped ) {
                    gl_buffer_unmap( gl, &batch->batch_constants_buffer, &batch_error );
                }
                
                if ( batch->vertex_buffer.mapped ) {
                    gl_buffer_unmap( gl, &batch->vertex_buffer, &batch_error );
                }
                
                if ( batch->index_buffer.mapped ) {
                    gl_buffer_unmap( gl, &batch->index_buffer, &batch_error );
                }
                
            } break;
            
            default: {
                invalid_code_path( );
            } break;
        }
    }
    
    profiler_event_end( gl_render );
}

stu gl_sampler_t* gl_sampler_get( gl_t* gl, gl_sampler_type_t type ) {
    gl_sampler_t* result = 0;
    result = gl->window->gl_platform.samplers + type;
    return result;
}

stu void gl_resize_frame_buffer( window_t* window, u32* error ) {
    
    gl_platform_t* gl_platform = &window->gl_platform;
    ID3D11Texture2D* frame_buffer = 0;
    ID3D11Texture2D* depth_buffer = 0;
    
    breakable {
        
        breakable_check( error );
        
        if ( gl_platform->render_target_view ) {
            ID3D11RenderTargetView_Release( gl_platform->render_target_view );
            gl_platform->render_target_view = 0;
        }
        
        if ( gl_platform->depth_stencil_view ) {
            ID3D11DepthStencilView_Release( gl_platform->depth_stencil_view );
            gl_platform->depth_stencil_view = 0;
        }
        
        HRESULT hr = IDXGISwapChain_ResizeBuffers( gl_platform->swap_chain, 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0 );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_resize_frame_buffer IDXGISwapChain_ResizeBuffers failed.\n" );
            set_error( error, gl_error_swap_chain_resize_buffers_failed );
            break;
        }
        
        hr = IDXGISwapChain1_GetBuffer( gl_platform->swap_chain, 0, &IID_ID3D11Texture2D, &frame_buffer );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_resize_frame_buffer IDXGISwapChain1_GetBuffer failed.\n" );
            set_error( error, gl_error_swap_chain_get_buffer_failed );
            break;
        }
        
        D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc = { 0 };
        render_target_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        // render_target_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        
        hr = ID3D11Device1_CreateRenderTargetView( gl_platform->device, cast( void*, frame_buffer ), &render_target_view_desc, &gl_platform->render_target_view );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_resize_frame_buffer ID3D11Device1_CreateRenderTargetView failed.\n" );
            set_error( error, gl_error_device1_create_render_target_view_failed );
            break;
        }
        
        D3D11_TEXTURE2D_DESC depth_buffer_desc = { 0 };
        
        ID3D11Texture2D_GetDesc( frame_buffer, &depth_buffer_desc );
        depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        
        
        hr = ID3D11Device1_CreateTexture2D( gl_platform->device, &depth_buffer_desc, 0, &depth_buffer );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_resize_frame_buffer ID3D11Device1_CreateTexture2D failed.\n" );
            set_error( error, gl_error_device1_create_texture_2d_failed );
            break;
        }
        
        hr = ID3D11Device1_CreateDepthStencilView( gl_platform->device, cast( void*, depth_buffer ), 0, &gl_platform->depth_stencil_view );
        
        if ( FAILED( hr ) ) {
            log_l( "[ERROR] gl_resize_frame_buffer ID3D11Device1_CreateDepthStencilView failed.\n" );
            set_error( error, gl_error_device1_create_depth_stencil_view_failed );
            break;
        }
    }
    
    if ( frame_buffer ) {
        ID3D11Texture2D_Release( frame_buffer );
        frame_buffer = 0;
    }
    
    if ( depth_buffer ) {
        ID3D11Texture2D_Release( depth_buffer );
        depth_buffer = 0;
    }
    
    if ( is_error( error ) ) {
        
        if ( gl_platform->render_target_view ) {
            ID3D11RenderTargetView_Release( gl_platform->render_target_view );
            gl_platform->render_target_view = 0;
        }
        
        if ( gl_platform->depth_stencil_view ) {
            ID3D11DepthStencilView_Release( gl_platform->depth_stencil_view );
            gl_platform->depth_stencil_view = 0;
        }
    }
}

stu void gl_cleanup( gl_t* gl, u32* error ) {
    
    gl_platform_t* gl_platform = &gl->window->gl_platform;
    
#if DEBUG
    if ( gl_platform->dxgi_debug ) {
        OutputDebugString( "[DEBUG] gl_cleanup start report...\n" );
        IDXGIDebug_ReportLiveObjects( gl_platform->dxgi_debug, DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL );
    }
#endif
    
    for ( umm i = 0; i < gl->batch_count; i++ ) {
        
        gl_batch_t* batch = gl->batches + i;
        
        gl_buffer_free( gl, &batch->vertex_buffer );
        gl_buffer_free( gl, &batch->index_buffer );
        gl_buffer_free( gl, &batch->batch_constants_buffer );
    }
    
    for ( umm i = 0; i < array_count( gl->shaders ); i++ ) {
        
        gl_shader_t* shader = gl->shaders + i;
        
        if ( shader->valid ) {
            gl_shader_free( shader );
        }
    }
    
    if ( gl_platform->context ) {
        
        ID3D11DeviceContext1_OMSetRenderTargets( gl_platform->context, 0, 0, 0 );
        
        ID3D11DeviceContext1_OMSetBlendState( gl_platform->context, 0, 0, u32_max );
        ID3D11DeviceContext1_RSSetState( gl_platform->context, 0 );
        ID3D11DeviceContext1_OMSetDepthStencilState( gl_platform->context, 0, 0 );
    }
    
    for ( umm i = 0; i < array_count( gl_platform->samplers ); i++ ) {
        ID3D11SamplerState_Release( gl_platform->samplers[ i ].pointer );
        gl_platform->samplers[ i ].pointer = 0;
    }
    
    if ( gl_platform->depth_stencil_view ) {
        ID3D11DepthStencilView_Release( gl_platform->depth_stencil_view );
        gl_platform->depth_stencil_view = 0;
    }
    
    if ( gl_platform->render_target_view ) {
        ID3D11RenderTargetView_Release( gl_platform->render_target_view );
        gl_platform->render_target_view = 0;
    }
    
    if ( gl_platform->swap_chain ) {
        IDXGISwapChain1_Release( gl_platform->swap_chain );
        gl_platform->swap_chain = 0;
    }
    
    if ( gl_platform->context ) {
        ID3D11DeviceContext1_Release( gl_platform->context );
        gl_platform->context = 0;
    }
    
    if ( gl_platform->device ) {
        ID3D11Device1_Release( gl_platform->device );
        gl_platform->device = 0;
    }
    
#if DEBUG
    
    if ( gl_platform->dxgi_debug ) {
        
        OutputDebugString( "[DEBUG] gl_cleanup end report (no text after this line is success)...\n" );
        IDXGIDebug_ReportLiveObjects( gl_platform->dxgi_debug, DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL );
        
        IDXGIDebug1_Release( gl_platform->dxgi_debug );
        gl_platform->dxgi_debug = 0;
    }
    
#endif
    
}

#define GL_DIRECTX_H
#endif
