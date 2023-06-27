#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;
layout(binding = 1) uniform sampler2D inputTexture;

uniform vec2 center;
uniform float radius;
uniform float angle;

mat2 rotate(float a)
{
	float s = sin(a);
	float c = cos(a);
	return mat2(c, -s, s, c);
}

vec2 Twirl(vec2 uv, vec2 center, float range, float strength) {
	float d = distance(uv, center);
	uv -= center;
	// d = clamp(0.,strength,-strength/range * d + strength);
	d = smoothstep(0., range, range - d) * strength;
	uv *= rotate(d);
	uv += center;
	return uv;
}

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 resolution = imageSize(imgOutput);
	
	vec2 uv = (pixelCoords - resolution.xy) / resolution.y;

	//vec2 texSize = imageSize(imgOutput);
	//vec2 tc = uv * texSize;
	//tc -= center;
	//float dist = length(tc);
	//if (dist < radius)
	//{
	//	float percent = (radius - dist) / radius;
	//	float theta = percent * percent * angle * 8.0;
	//	float s = sin(theta);
	//	float c = cos(theta);
	//	tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
	//}
	////tc += center;
	//vec3 color = texture2D(inputTexture, tc / texSize).rgb;

	vec2 tc = Twirl(uv, center, radius, angle);

	vec3 color = texture2D(inputTexture, tc).rgb;
	
	//color = vec3(tc, 0);

	imageStore(imgOutput, pixelCoords, vec4(color, 1));
}
