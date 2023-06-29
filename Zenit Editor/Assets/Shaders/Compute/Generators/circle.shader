#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform float blur;
uniform float radius;
uniform vec2 offset;

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 resolution = imageSize(imgOutput);
	vec2 uv = (pixelCoords - 0.5 * resolution.xy) / resolution.y;
	
	uv -= offset;
	float m = smoothstep(radius, radius - blur, length(uv));
	vec3 color = vec3(m);

	imageStore(imgOutput, pixelCoords, vec4(color, 1));
}