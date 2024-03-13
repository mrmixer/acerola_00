
[name: vs_default]

in vec2 position;
in vec2 in_uv;
out vec2 out_uv;

void main( ) {
    gl_Position = vec4( position, 0.0, 1.0 );
    out_uv = in_uv;
}

[name: vs_screen_space_texture]

in vec3 position;
in vec2 in_uv;
out vec2 out_uv;

uniform mat4x4 screen_to_normalized;

void main( ) {
    vec4 tPos = vec4( position, 1.0 ) * screen_to_normalized;
    gl_Position = tPos;
    out_uv = in_uv;
}

[name: vs_screen_space_texture_color]

row_major float4x4 projection : register( b0 );

struct input_t {
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};

struct output_t {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};

output_t main( input_t input ) {
    output_t output;
    output.color = input.color;
    output.position = mul( input.position, projection );
    output.uv = input.uv;
    return output;
}

[name: vs_screen_space_color]

row_major float4x4 projection : register( b0 );

struct input_t {
    float4 position : POSITION;
    float4 color : COLOR;
};

struct output_t {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

output_t main( input_t input ) {
    output_t output;
    output.color = input.color;
    output.position = mul( input.position, projection );
    return output;
}

[name: vs_orthographic]

in vec3 position;
in vec2 in_uv;
in uint color;
out vec2 out_uv;
out vec4 out_color;

uniform mat4x4 projection;

void main( ) {
    out_uv = in_uv;
    out_color = color_unpack( color );
    vec4 transformed_position = projection * vec4( position, 1.0 );
    gl_Position = transformed_position;
}

[name: fs_default]

out vec4 fragment_out_color;
in vec2 out_uv;

uniform sampler2D albedo;

void main( ) {
    fragment_out_color = texture( albedo, out_uv );
}

[name: fs_default_color]

out vec4 fragment_out_color;
uniform vec4 in_color;

void main( ) {
    fragment_out_color = in_color;
}

[name: fs_default_vertex_color]

struct input_t {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main( input_t input ) : SV_TARGET {
    return input.color;
}

[name: fs_default_u8]

out vec4 fragment_out_color;
in vec2 out_uv;

uniform sampler2D albedo;

void main( ) {
    float c = texture( albedo, out_uv ).r;
    fragment_out_color = vec4( 1.0, 1.0, 1.0, c );
}

[name: fs_default_u8_color]

struct input_t {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};

Texture2D albedo : register( t0 );
SamplerState albedo_sampler : register( s0 );

float4 main( input_t input ) : SV_TARGET {
    float alpha = albedo.Sample( albedo_sampler, input.uv ).a;
    /* NOTE simon (16/02/24 14:08:42): During the port from OpenGL to DirectX, text rendering was too bright because
     OpenGL was doing the blending in sRGB space. The modification to alpha here is to get a close approximation
to the text we were getting before.*/
    float4 result = float4( input.color.rgb, input.color.a * pow( alpha, 1.5 ) );
    // float4 result = float4( input.color.rgb, input.color.a * pow( alpha, 1.3 ) );
    // float4 result = float4( input.color.rgb, input.color.a * ( alpha - 0.125 ) );
    // float4 result = float4( input.color.rgb, input.color.a * alpha );
    return result;
}

[name: fs_textured_color]

struct input_t {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};

Texture2D albedo : register( t0 );
SamplerState albedo_sampler : register( s0 );

float4 main( input_t input ) : SV_TARGET {
    float4 texture_color = albedo.Sample( albedo_sampler, input.uv );
    float4 result = input.color * texture_color;
    return result;
}