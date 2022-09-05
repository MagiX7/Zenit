#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

layout(binding = 1) uniform sampler2D tex;
uniform vec3 inputColor;

uniform int isTex;

uniform float min;
uniform float max;

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 resolution = imageSize(imgOutput);
	vec2 uv = (pixelCoords - resolution.xy) / resolution.y;
	
	vec3 color = vec3(0);
	vec3 data = bool(isTex) ? texture2D(tex, uv).rgb : inputColor;
	color = clamp(data, min, max);

	imageStore(imgOutput, pixelCoords, vec4(color, 1));
}