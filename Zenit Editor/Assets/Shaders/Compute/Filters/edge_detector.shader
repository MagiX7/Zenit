#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;
layout(binding = 1) uniform sampler2D inputTexture;

uniform int threshold;

float GetHeight(vec2 uv)
{
	return texture(inputTexture, uv).r;
}

float intensity(in vec4 color)
{
	return sqrt((color.x * color.x) + (color.y * color.y) + (color.z * color.z));
}

// From https://www.shadertoy.com/view/Xdf3Rf
vec3 sobel(float stepx, float stepy, vec2 center)
{
	// Get samples around pixel
	float tleft = intensity(texture2D(inputTexture, center + vec2(-stepx, stepy)));
	float left = intensity(texture2D(inputTexture, center + vec2(-stepx, 0)));
	float bleft = intensity(texture2D(inputTexture, center + vec2(-stepx, -stepy)));
	float top = intensity(texture2D(inputTexture, center + vec2(0, stepy)));
	float bottom = intensity(texture2D(inputTexture, center + vec2(0, -stepy)));
	float tright = intensity(texture2D(inputTexture, center + vec2(stepx, stepy)));
	float right = intensity(texture2D(inputTexture, center + vec2(stepx, 0)));
	float bright = intensity(texture2D(inputTexture, center + vec2(stepx, -stepy)));

	float x = tleft + 2.0 * left + bleft - tright - 2.0 * right - bright;
	float y = -tleft - 2.0 * top - tright + bleft + 2.0 * bottom + bright;
	float color = sqrt((x * x) + (y * y));
	return vec3(color, color, color);
}


void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 resolution = imageSize(imgOutput);
	
	vec2 uv = (pixelCoords - resolution.xy) / resolution.y;

	float h = GetHeight(uv);
	vec2 stepp = 1. / resolution;

	vec2 dxy = h - vec2(
		GetHeight(uv + vec2(stepp.x, 0.)),
		GetHeight(uv + vec2(0., stepp.y))
	);

	vec3 col = sobel(threshold / resolution.x, threshold / resolution.y, uv);

	imageStore(imgOutput, pixelCoords, vec4(col, 1));
}
