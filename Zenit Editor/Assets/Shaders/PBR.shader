#type vertex
#version 460 core

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

layout(location = 0) uniform sampler2D diffuseTexture;
layout(location = 1) uniform sampler2D normalsTexture;
//layout(location = 2) uniform sampler2D metallicTexture;
//layout(location = 3) uniform sampler2D roughnessTexture;
layout(location = 4) uniform sampler2D ambientOcclussionTexture;
layout(location = 5) uniform samplerCube skybox;

uniform float metallic;
uniform float roughness;

uniform vec3 camPos;
uniform float skyboxIntensity;
uniform int skyboxReflectionEnabled;
uniform int drawSkybox;

out vec4 fragColor;

in vec3 vPosition;
in vec2 vTexCoords;
in vec3 vNormals;


struct Light
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float intensity;
};
uniform Light dirLight;

vec3 GetSkyboxReflection(float ratio)
{
	// TODO: Change vNormals to normal map
	vec3 I = normalize(vPosition - camPos);
	return reflect(I, normalize(vNormals));
}

vec3 CalculateDirLight(Light light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0), roughness);


	vec3 ambient = light.ambient * vec3(texture(diffuseTexture, vTexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseTexture, vTexCoords));
	vec3 specular = light.specular * spec /** vec3(texture(diffuseTexture, vTexCoords))*/;
	//vec3 specular = light.specular * spec * roughness;
	return (ambient + diffuse + specular) * light.intensity;
}

void main()
{
	float ratio = 1.00 / 1.52;
	vec3 R = GetSkyboxReflection(ratio);
	
	vec3 normal = normalize(vNormals);
	vec3 viewDir = normalize(camPos - vPosition);

	vec3 color = CalculateDirLight(dirLight, normal, viewDir);


	if (bool(skyboxReflectionEnabled) && bool(drawSkybox))
		fragColor = texture2D(diffuseTexture, vTexCoords) * vec4(texture(skybox, R).rgb * skyboxIntensity, 1) * vec4(color, 1);
	else
		fragColor = texture2D(diffuseTexture, vTexCoords) * vec4(color, 1);
}