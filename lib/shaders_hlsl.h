u8 vs_default[ ] =
"in vec2 position;\n"
"in vec2 in_uv;\n"
"out vec2 out_uv;\n"
"\n"
"void main( ) {\n"
"    gl_Position = vec4( position, 0.0, 1.0 );\n"
"    out_uv = in_uv;\n"
"}\n"
"\n";

u8 vs_screen_space_texture[ ] =
"in vec3 position;\n"
"in vec2 in_uv;\n"
"out vec2 out_uv;\n"
"\n"
"uniform mat4x4 screen_to_normalized;\n"
"\n"
"void main( ) {\n"
"    vec4 tPos = vec4( position, 1.0 ) * screen_to_normalized;\n"
"    gl_Position = tPos;\n"
"    out_uv = in_uv;\n"
"}\n"
"\n";

u8 vs_screen_space_texture_color[ ] =
"row_major float4x4 projection : register( b0 );\n"
"\n"
"struct input_t {\n"
"    float4 position : POSITION;\n"
"    float4 color : COLOR;\n"
"    float2 uv : UV;\n"
"};\n"
"\n"
"struct output_t {\n"
"    float4 position : SV_POSITION;\n"
"    float4 color : COLOR;\n"
"    float2 uv : UV;\n"
"};\n"
"\n"
"output_t main( input_t input ) {\n"
"    output_t output;\n"
"    output.color = input.color;\n"
"    output.position = mul( input.position, projection );\n"
"    output.uv = input.uv;\n"
"    return output;\n"
"}\n"
"\n";

u8 vs_screen_space_color[ ] =
"row_major float4x4 projection : register( b0 );\n"
"\n"
"struct input_t {\n"
"    float4 position : POSITION;\n"
"    float4 color : COLOR;\n"
"};\n"
"\n"
"struct output_t {\n"
"    float4 position : SV_POSITION;\n"
"    float4 color : COLOR;\n"
"};\n"
"\n"
"output_t main( input_t input ) {\n"
"    output_t output;\n"
"    output.color = input.color;\n"
"    output.position = mul( input.position, projection );\n"
"    return output;\n"
"}\n"
"\n";

u8 vs_orthographic[ ] =
"in vec3 position;\n"
"in vec2 in_uv;\n"
"in uint color;\n"
"out vec2 out_uv;\n"
"out vec4 out_color;\n"
"\n"
"uniform mat4x4 projection;\n"
"\n"
"void main( ) {\n"
"    out_uv = in_uv;\n"
"    out_color = color_unpack( color );\n"
"    vec4 transformed_position = projection * vec4( position, 1.0 );\n"
"    gl_Position = transformed_position;\n"
"}\n"
"\n";

u8 fs_default[ ] =
"out vec4 fragment_out_color;\n"
"in vec2 out_uv;\n"
"\n"
"uniform sampler2D albedo;\n"
"\n"
"void main( ) {\n"
"    fragment_out_color = texture( albedo, out_uv );\n"
"}\n"
"\n";

u8 fs_default_color[ ] =
"out vec4 fragment_out_color;\n"
"uniform vec4 in_color;\n"
"\n"
"void main( ) {\n"
"    fragment_out_color = in_color;\n"
"}\n"
"\n";

u8 fs_default_vertex_color[ ] =
"struct input_t {\n"
"    float4 position : SV_POSITION;\n"
"    float4 color : COLOR;\n"
"};\n"
"\n"
"float4 main( input_t input ) : SV_TARGET {\n"
"    return input.color;\n"
"}\n"
"\n";

u8 fs_default_u8[ ] =
"out vec4 fragment_out_color;\n"
"in vec2 out_uv;\n"
"\n"
"uniform sampler2D albedo;\n"
"\n"
"void main( ) {\n"
"    float c = texture( albedo, out_uv ).r;\n"
"    fragment_out_color = vec4( 1.0, 1.0, 1.0, c );\n"
"}\n"
"\n";

u8 fs_default_u8_color[ ] =
"struct input_t {\n"
"    float4 position : SV_POSITION;\n"
"    float4 color : COLOR;\n"
"    float2 uv : UV;\n"
"};\n"
"\n"
"Texture2D albedo : register( t0 );\n"
"SamplerState albedo_sampler : register( s0 );\n"
"\n"
"float4 main( input_t input ) : SV_TARGET {\n"
"    float alpha = albedo.Sample( albedo_sampler, input.uv ).a;\n"
"    /* NOTE simon (16/02/24 14:08:42): During the port from OpenGL to DirectX, text rendering was too bright because\n"
"     OpenGL was doing the blending in sRGB space. The modification to alpha here is to get a close approximation\n"
"to the text we were getting before.*/\n"
"    float4 result = float4( input.color.rgb, input.color.a * pow( alpha, 1.5 ) );\n"
"    // float4 result = float4( input.color.rgb, input.color.a * pow( alpha, 1.3 ) );\n"
"    // float4 result = float4( input.color.rgb, input.color.a * ( alpha - 0.125 ) );\n"
"    // float4 result = float4( input.color.rgb, input.color.a * alpha );\n"
"    return result;\n"
"}\n"
"\n";

u8 fs_textured_color[ ] =
"struct input_t {\n"
"    float4 position : SV_POSITION;\n"
"    float4 color : COLOR;\n"
"    float2 uv : UV;\n"
"};\n"
"\n"
"Texture2D albedo : register( t0 );\n"
"SamplerState albedo_sampler : register( s0 );\n"
"\n"
"float4 main( input_t input ) : SV_TARGET {\n"
"    float4 texture_color = albedo.Sample( albedo_sampler, input.uv );\n"
"    float4 result = input.color * texture_color;\n"
"    return result;\n"
"}";

