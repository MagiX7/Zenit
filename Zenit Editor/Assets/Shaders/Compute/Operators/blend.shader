﻿#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

layout(binding = 1) uniform sampler2D tex1;
layout(binding = 2) uniform sampler2D tex2;

uniform float contribution;
uniform int mode;

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 resolution = imageSize(imgOutput);
	vec2 uv = (pixelCoords - resolution.xy) / resolution.y;
	
	vec3 base = texture2D(tex1, uv).rgb;
	vec3 blend = texture2D(tex2, uv).rgb;
	vec3 color = vec3(0);

	switch (mode)
	{
		// Burn
		case 0:
		{
			color = 1.0 - (1.0 - blend) / base;
			break;
		}

		// Darken
		case 1:
		{
			color = min(blend, base);
			break;
		}

		// Difference
		case 2:
		{
			color = abs(blend - base);
			break;
		}

		// Dodge
		case 3:
		{
			color = base / (1.0 - blend);
			break;
		}

		// Divide
		case 4:
		{
			color = base / (max(blend, 0.0000001));
			break;
		}

		// Multiply
		case 5:
		{
			color = base * blend;
			break;
		}

		// Negation
		case 6:
		{
			color = 1.0 - abs(1.0 - blend - base);
			break;
		}

		// Subtract
		case 7:
		{
			color = base - blend;
			break;
		}

	}

	color = mix(base, color, contribution);
	imageStore(imgOutput, pixelCoords, vec4(color, 1));
}