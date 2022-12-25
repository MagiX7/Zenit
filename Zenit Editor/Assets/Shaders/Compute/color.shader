#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform vec3 color;

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	imageStore(imgOutput, pixelCoords, vec4(color, 1));
}