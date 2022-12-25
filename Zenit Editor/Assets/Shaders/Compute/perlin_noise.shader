#version 460 core

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform vec3 inputColor;
uniform float seed;

uniform int resolution;

const float PI = 3.14159265359;

float rand(vec2 c)
{
	return fract(sin(dot(c.xy, vec2(12.9898, 78.233)*seed)) * 43758.5453);
}

float noise(vec2 p, float freq, float width)
{
	float unit = width / freq;
	vec2 ij = floor(p / unit);
	vec2 xy = mod(p, unit) / unit;
	//xy = 3.*xy*xy-2.*xy*xy*xy;
	xy = .5 * (1. - cos(PI * xy));
	float a = rand((ij + vec2(0., 0.)));
	float b = rand((ij + vec2(1., 0.)));
	float c = rand((ij + vec2(0., 1.)));
	float d = rand((ij + vec2(1., 1.)));
	float x1 = mix(a, b, xy.x);
	float x2 = mix(c, d, xy.x);
	return mix(x1, x2, xy.y);
}

float perlinNoise(vec2 p, int res)
{
	float persistance = .5;
	float n = 0.;
	float normK = 0.;
	float f = 4.;
	float amp = 1.;
	int iCount = 0;
	float width = imageSize(imgOutput).x;
	for (int i = 0; i < 50; i++) {
		n += amp * noise(p, f, width);
		f *= 2.;
		normK += amp;
		amp *= persistance;
		if (iCount == res) break;
		iCount++;
	}
	float nf = n / normK;
	return nf * nf * nf * nf;
}

vec4 gradient(vec2 tex_coords) {
	float s = tex_coords[0];
	float t = tex_coords[1];
	vec3 red = vec3(1.0, 0.0, 0.0);
	float percent;

	percent = s;
	return vec4(red * percent, 1.0);
}

void main()
{
    ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 uv = vec2(pixelCoords);
	//uv = gradient(uv).xy;

	float n = perlinNoise(uv, resolution);

	vec4 color = vec4(vec3(n), 1) * vec4(inputColor, 1);
	//vec4 color = vec4(uv, 0, 1);

    imageStore(imgOutput, pixelCoords, color);
}
