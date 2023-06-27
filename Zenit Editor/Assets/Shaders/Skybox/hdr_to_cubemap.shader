#type vertex
#version 430 core

layout(location = 0) in vec3 position;

out vec3 localPosition;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * vec4(position, 1.0);
	localPosition = position;
}

#type fragment
#version 430 core

in vec3 localPosition;
out vec4 fragColor;

uniform sampler2D hdrMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(localPosition));
	vec3 color = texture(hdrMap, uv).rgb;

	fragColor = vec4(color, 1);
}