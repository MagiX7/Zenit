#type vertex
#version 430 core

layout(location = 0) in vec3 position;

out vec3 vTexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 pos = projection * view * vec4(position, 1.0);
	gl_Position = pos.xyww;
	vTexCoords = position;
}

#type fragment
#version 430 core

in vec3 vTexCoords;
out vec4 fragColor;

uniform samplerCube skybox;

void main()
{
	fragColor = texture(skybox, vTexCoords);
}