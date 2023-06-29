#version 430 core
#extension GL_NV_compute_shader_derivatives : enable

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform vec3 inputColor;
uniform float seed;

float N21(vec2 p)
{
    vec2 p2 = fract(vec2(p) * vec2(.1031, .11369));
    p2 += dot(p2, p2.yx + 19.19);
    return fract(vec2((p2.x + p2.y) * p2.y, (p2.x + p2.y) * p2.y)).x;
}

float noise(in vec2 st)
{
    vec2 i = floor(st) * seed;
    return N21(i);
}

void main()
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

    vec2 uv = vec2(pixelCoords * 5.0);
    vec4 color = vec4(vec3(noise(uv)), 1) * vec4(inputColor, 1);

    imageStore(imgOutput, pixelCoords, color);
}
