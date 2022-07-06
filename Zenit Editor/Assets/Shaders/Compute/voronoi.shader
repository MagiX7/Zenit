#version 430 core

layout(local_size_x = 8, local_size_y = 4, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform float time;

vec2 Noise(vec2 p)
{
	vec3 a = fract(p.xyx * vec3(123.34, 234.45, 345.56));
	a += dot(a, a + 34.45f);
	return fract(vec2(a.x * a.y, a.y * a.z));
}

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 resolution = imageSize(imgOutput);
	vec2 uv = (pixelCoords - 0.5 * resolution.xy) / resolution.y; // -0.5 to 0.5
	uv += 0.5;

	vec4 color = vec4(0, 0, 0, 1);

	float m = 0;
	float minDist = 50000;

	for (int i = 0; i < 50; ++i)
	{
		vec2 n = Noise(vec2(i));
		//vec2 p = sin(n * time * 0.2);
		vec2 p = n;
		float d = length(uv - p);
		m += smoothstep(0.01, 0.009, d);
		
		if (d < minDist)
		{
			minDist = d;
		}
	}

	//color.xyz = vec3(uv.x, uv.y, 0);
	color.xyz = vec3(minDist);

	imageStore(imgOutput, pixelCoords, color);
}