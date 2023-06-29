#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform vec3 inputColor;
uniform float seed;
uniform int scale;
uniform int numOctaves;

const float PI = 3.14159265359;


float Hash(in vec2 p, in float scale)
{
	// This is tiling part, adjusts with the scale...
	p = mod(p, scale);
	return fract(sin(dot(p, vec2(27.16898, 38.90563))) * fract(seed) * 5151.5473453);
}

//----------------------------------------------------------------------------------------
float Noise(in vec2 p, in float scale)
{
	vec2 f;

	p *= scale;

	f = fract(p);		// Separate integer from fractional
	p = floor(p);

	f = f * f * (3.0 - 2.0 * f);	// Cosine interpolation approximation

	float res = mix(mix(Hash(p, scale),
		Hash(p + vec2(1.0, 0.0), scale), f.x),
		mix(Hash(p + vec2(0.0, 1.0), scale),
			Hash(p + vec2(1.0, 1.0), scale), f.x), f.y);
	return res;
}

//----------------------------------------------------------------------------------------
float fBm(in vec2 p)
{
	float f = 0.0;
	float s = scale;
	p = mod(p, scale);
	float amp = 0.6;

	for (int i = 0; i < numOctaves; i++)
	{
		f += Noise(p, s) * amp;
		amp *= .5;
		// Scale must be multiplied by an integer value...
		s *= 2.;
	}
	// Clamp it just in case....
	return min(f, 1.0);
}



void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 res = imageSize(imgOutput);
    
	vec2 uv = pixelCoords / res;

	float value = fBm(uv);

	vec4 color = vec4(vec3(value), 1) * vec4(inputColor, 1);
	imageStore(imgOutput, pixelCoords, color);
}