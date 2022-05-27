#type vertex
#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 texCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;


out vec3 vPosition;
out vec2 vTexCoords;
out vec3 vNormals;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1);
	vTexCoords = texCoords;
	vNormals = normals;
	vPosition = vec3(model * vec4(position, 1));
}

#type fragment
#version 430 core

layout(location = 0) uniform sampler2D colorTexture;
layout(location = 1) uniform samplerCube skybox;

uniform vec3 camPos;
uniform float skyboxIntensity;
uniform int skyboxReflectionEnabled;

out vec4 fragColor;

in vec3 vPosition;
in vec2 vTexCoords;
in vec3 vNormals;

void main()
{
	float ratio = 1.00 / 1.52;
	vec3 I = normalize(vPosition - camPos);
	vec3 R = reflect(I, normalize(vNormals));

	if (bool(skyboxReflectionEnabled))
		fragColor = texture2D(colorTexture, vTexCoords) * vec4(texture(skybox, R).rgb * skyboxIntensity, 1);
	else
		fragColor = texture2D(colorTexture, vTexCoords);
}