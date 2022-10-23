#version 460 core
//#extension GL_NV_compute_shader_derivatives : enable

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;
layout(binding = 1) uniform sampler2D inputTexture;

uniform float zoom;
uniform float bumpness;

float GetHeight(vec2 uv)
{
	return texture(inputTexture, uv).r;
}

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 resolution = imageSize(imgOutput);
	
	vec2 uv = (pixelCoords - /*0.5 * */resolution.xy) / resolution.y;
	uv *= zoom;

	vec3 col = vec3(0);

	float h = GetHeight(uv);
	vec2 stepp = 1 / resolution;
	
	vec2 dxy = h - vec2(
		GetHeight(uv + vec2(stepp.x, 0)),
		GetHeight(uv + vec2(0, stepp.y))
	);

	col = normalize(vec3(dxy * bumpness / stepp, 1.)) * 0.5 + 0.5;

	imageStore(imgOutput, pixelCoords, vec4(col, 1));
}