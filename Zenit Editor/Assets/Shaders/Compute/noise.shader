#version 430 core

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform vec3 inputColor;
uniform float seed;

float random(in vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float N21(vec2 p)
{
    vec2 p2 = fract(vec2(p) * vec2(.1031, .11369));
    p2 += dot(p2, p2.yx + 19.19);
    return fract(vec2((p2.x + p2.y) * p2.y, (p2.x + p2.y) * p2.y)).x;
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise(in vec2 st)
{
    float s = seed == 0.0f ? 0.1 : seed;
    vec2 i = floor(st) * s;
    float a = N21(i);
    return a;
}

void main()
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
    vec2 pos = vec2(pixelCoords * 5.0);

    vec4 color = vec4(vec3(noise(pos)), 1) * vec4(inputColor, 1);

    imageStore(imgOutput, pixelCoords, color);
}
