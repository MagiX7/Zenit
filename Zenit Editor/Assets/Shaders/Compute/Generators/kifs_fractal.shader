#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform float angle;
uniform float reflectionAngle;
uniform int iterations;
uniform float lineThickness;

const float PI = 3.14159265359;

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 resolution = imageSize(imgOutput);
	
	vec2 uv = (pixelCoords - 0.5 * resolution.xy) / resolution.y;
	uv *= 1.5;
	uv.y -= .25;
	vec3 col = vec3(0);

	float a = reflectionAngle * PI;
	vec2 n = vec2(sin(a), cos(a));
	uv.x = abs(uv.x);
	uv -= n * max(0, dot(uv - vec2(0.5, 0), n)) * 2;

	a = angle * PI;
	n = vec2(sin(a), cos(a));
	float scale = 1.0f;
	uv.x += 0.5f;
	for (int i = 0; i < iterations; i++)
	{
		uv *= 3.0f;
		scale *= 3;
		uv.x -= 1.5f;

		uv.x = abs(uv.x);
		uv.x -= 0.5;
		uv -= n * min(0, dot(uv, n)) * 2; // This does the mirror
	}

	float d = length(uv - vec2(clamp(uv.x, -1, 1), 0));
	col += smoothstep((1.0 / resolution.y) * lineThickness, 0.0, d / scale);

	imageStore(imgOutput, pixelCoords, vec4(col, 1));
}
