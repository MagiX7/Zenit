#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 4) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform float seed;
uniform int scale;

vec2 Noise(vec2 p, float scale)
{
	// Tiling
	p = mod(p, scale);

	vec3 a = fract(p.xyx * vec3(123.34, 234.45, 345.56));
	a += dot(a, a);
	return fract(vec2(a.x * a.y, a.y * a.z));
}

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 resolution = imageSize(imgOutput);
	vec2 uv = (pixelCoords - 0.5 * resolution.xy) / resolution.y;

	float s = scale;
	uv *= s;

	vec4 color = vec4(0, 0, 0, 1);

	vec2 gv = fract(uv) - 0.5; // Grid
	vec2 id = floor(uv); // Id of each quad of the grid

	float minDist = 100;
	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			vec2 offset = vec2(x, y);
			vec2 n = Noise(id + offset, s) * seed;
			vec2 p = offset + sin(n) * 0.5;
			float d = length(gv - p);

			if (d < minDist)
			{
				minDist = d;
			}
		}
	}

	color.xyz = vec3(minDist);
	imageStore(imgOutput, pixelCoords, color);
}