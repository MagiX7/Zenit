#version 460 core

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
	vec2 uv = (pixelCoords - /*0.5 * */resolution.xy) / resolution.y;
	
	vec3 base = texture2D(tex1, uv).rgb;
	vec3 blend = texture2D(tex2, uv).rgb;
	vec3 color = vec3(0);

	switch (mode)
	{
		// Linear
		case 0:
		{
			color = mix(base, color, contribution);
			break;
		}

		// Burn
		case 1:
		{
			color = 1.0 - (1.0 - blend) / base;
			color = mix(base, color, contribution);
			break;
		}

		// Darken
		case 2:
		{
			color = min(blend, base);
			color = mix(base, color, contribution);
			break;
		}

		// Difference
		case 3:
		{
			color = abs(blend - base);
			color = mix(base, color, contribution);
			break;
		}

		// Dodge
		case 4:
		{
			color = base / (1.0 - blend);
			color = mix(base, color, contribution);
			break;
		}

		// Divide
		case 5:
		{
			color = base / (max(blend, 0.0000001));
			color = mix(base, color, contribution);
			break;
		}

		// Multiply
		case 6:
		{
			color = base * blend;
			color = mix(base, color, contribution);
			break;
		}

		// Negation
		case 7:
		{
			color = 1.0 - abs(1.0 - blend - base);
			color = mix(base, color, contribution);
			break;
		}

		// Subtract
		case 8:
		{
			color = base - blend;
			color = mix(base, color, contribution);
			break;
		}

	}

	imageStore(imgOutput, pixelCoords, vec4(color, 1));
}