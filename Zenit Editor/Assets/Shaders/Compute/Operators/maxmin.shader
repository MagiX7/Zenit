#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

layout(binding = 1) uniform sampler2D tex1;
layout(binding = 2) uniform sampler2D tex2;

uniform int type; // Stands to know if it's min or max

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 resolution = imageSize(imgOutput);
	vec2 uv = (pixelCoords - resolution.xy) / resolution.y;
	
	vec3 col1 = texture2D(tex1, uv).rgb;
	vec3 col2 = texture2D(tex2, uv).rgb;

	vec3 color = (min(col1, col2) * (1 - type)) + (max(col1, col2) * type);

	imageStore(imgOutput, pixelCoords, vec4(color, 1));
}