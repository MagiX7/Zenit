#type vertex
#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

uniform mat4 viewProjection;

void main()
{
	gl_Position = viewProjection * vec4(position, 1);
}

#type fragment
#version 430 core

out vec4 fragColor;

void main()
{
	fragColor = vec4(1, 0, 0, 1);
}