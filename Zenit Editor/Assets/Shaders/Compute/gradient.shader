#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

layout(binding = 1) uniform sampler2D inputTexture;

uniform bool vertical;
uniform bool horizontal;
uniform float distance;

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 resolution = imageSize(imgOutput).xy;

	vec2 uv = pixelCoords / resolution;

	vec3 tex = texture2D(inputTexture, uv).rgb;
	vec3 col = uv.x * tex * float(horizontal) * distance + uv.y * tex * float(vertical) * distance;

	imageStore(imgOutput, pixelCoords, vec4(col, 1));
}