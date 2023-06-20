#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 4) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform int cells;

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 resolution = imageSize(imgOutput);
	vec2 uv = (pixelCoords - 0.5 * resolution.xy) / resolution.y;

	uv *= cells;
	uv = floor(uv);
	vec3 color = vec3(mod(uv.x + uv.y, 2.));

	imageStore(imgOutput, pixelCoords, vec4(color, 1));
}