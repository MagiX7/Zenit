#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;
layout(binding = 1) uniform sampler2D inputTexture;

uniform vec2 offset;
uniform float tileNum;

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 resolution = imageSize(imgOutput);
	
	vec2 uv = pixelCoords / resolution.xy;
	
	uv *= tileNum;
	//uv = fract(uv);
	//vec2 id = floor(uv);
	uv = floor(uv);
	uv = vec2(mod(uv, offset));

	vec3 color = texture2D(inputTexture, uv, 0).rgb;

	imageStore(imgOutput, pixelCoords, vec4(color, 1));
}
