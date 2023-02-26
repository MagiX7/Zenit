#version 460 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;
layout(binding = 1) uniform sampler2D inputTexture;

uniform vec2 offset;
uniform float angle;
uniform vec2 scale;

const float PI = 3.14159265359;

mat2 rotate(float a)
{
	a *= PI / 180.;
	float s = sin(a);
	float c = cos(a);
	return mat2(c, -s, s, c);
}

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 resolution = imageSize(imgOutput);

	vec2 uv = (pixelCoords.xy - resolution.xy) / resolution.y;
	
	uv -= offset;
	uv *= scale;
	uv *= rotate(angle);

	vec3 color = texture2D(inputTexture, uv).rgb;

	imageStore(imgOutput, pixelCoords, vec4(color, 1));
}