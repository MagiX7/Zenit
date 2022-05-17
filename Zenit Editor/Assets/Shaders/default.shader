#type vertex
#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 texCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec2 vTexCoords;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1);
	vTexCoords = texCoords;
}

#type fragment
#version 430 core

layout(location = 0) uniform sampler2D colorTexture;

out vec4 fragColor;

in vec2 vTexCoords;

void main()
{
	fragColor = texture2D(colorTexture, vTexCoords) /** vec4(1, 0, 0, 1)*/;
}