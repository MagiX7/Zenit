#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;
layout(binding = 1) uniform sampler2D inputTexture;


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

float HexDist(vec2 p) {
	p = abs(p);

	float c = dot(p, normalize(vec2(1, 1.73)));
	c = max(c, p.x);

	return c;
}

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 resolution = imageSize(imgOutput);
	
	vec2 uv = (pixelCoords - resolution.xy) / resolution.y;

	vec4 hc = HexCoords(uv);
	//float c = smoothstep(hc.y, )
	vec3 finalColor = color * gv;

	imageStore(imgOutput, pixelCoords, vec4(finalColor, 1));
}
