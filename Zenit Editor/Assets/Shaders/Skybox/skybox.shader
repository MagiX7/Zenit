#type vertex
#version 430 core

layout(location = 0) in vec3 position;

out vec3 localPosition;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	mat4 rotView = mat4(mat3(view)); // remove translation from the view matrix
	vec4 clipPos = projection * rotView * vec4(position, 1.0);
	gl_Position = clipPos.xyww;

	localPosition = position;
}

#type fragment
#version 430 core

in vec3 localPosition;
out vec4 fragColor;

uniform samplerCube skybox;

const float PI = 3.14159265359;

void main()
{
	vec3 color = texture(skybox, localPosition).rgb;
	//vec3 color = textureLod(skybox, localPosition, 1.2).rgb;
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / 2.2));

	fragColor = vec4(color, 1);
}