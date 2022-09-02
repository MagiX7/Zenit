#type vertex
#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangents;
layout(location = 4) in vec3 biTangents;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;


out vec3 vPosition;
out vec2 vTexCoords;
out vec3 vNormals;
out mat3 tbnMatrix;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1);
	vTexCoords = texCoords;
	vNormals = normalize((model * vec4(normals, 0.0)).xyz);
	vPosition = vec3(model * vec4(position, 1));

	vec3 T = normalize(tangents);
	vec3 B = normalize(biTangents);
	vec3 N = normalize(normals);
	tbnMatrix = mat3(T, B, N);
}

#type fragment
#version 460 core

layout(location = 0) uniform sampler2D diffuseTexture;
layout(location = 1) uniform sampler2D normalsTexture;
layout(location = 2) uniform sampler2D metallicTexture;
layout(location = 3) uniform sampler2D roughnessTexture;
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
in mat3 tbnMatrix;

const float PI = 3.14159265359;

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

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	//return F0 + (1.0 - F0) * pow(clamp(1.0 - min(cosTheta, 1.0), 0.0, 1.0), 5.0);
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return a2 / denom;
}

float DisneyDistributionGGX(float NdotH, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (NdotH * NdotH) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 2.0;

	float denom = NdotV * (1.0 - k) + k;

	return NdotV / denom;
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 CalculateDirLight(Light light, vec3 normal, vec3 viewDir, vec3 albedo, float metalness, float roughness)
{
	// Lh -> halfway
	// Li -> lightDir
	// Lo -> viewDir
	// cosLh -> NdotH
	// cosLi -> NdotL
	// cosLo -> NdotV

	vec3 lightDir = normalize(-light.direction);
	vec3 halfway = normalize(viewDir + lightDir);

	float NdotL = max(dot(normal, lightDir), 0.0);
	float NdotV = max(dot(normal, viewDir), 0.0);
	float NdotH = max(dot(normal, halfway), 0.0);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);
	


	vec3 F = FresnelSchlick(max(dot(halfway, viewDir), 0.0), F0);
	//float NDF = DistributionGGX(normal, halfway, roughness);
	float NDF = DisneyDistributionGGX(NdotH, roughness);
	float G = GeometrySmith(NdotV, NdotL, roughness);
	
	vec3 ks = F;
	vec3 kd = vec3(1.0) - ks;
	kd *= 1.0f - metallic;
	//vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);
	
	vec3 diffuseBRDF = kd * albedo;
	vec3 specularBRDF = F * NDF * G / max(0.0001, 4 * NdotL * NdotV);
	return diffuseBRDF + specularBRDF * light.intensity * 200 * light.diffuse * NdotL;

	//vec3 specular = (NDF * G * F) / (4 * NdotV * NdotL);
	//return (kd * (albedo * light.diffuse) / PI + specular * light.specular) * light.intensity * NdotL;





	//float diff = max(dot(normal, lightDir), 0);
	//
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0), roughness);
	//
	//
	//vec3 ambient = light.ambient * vec3(texture(diffuseTexture, vTexCoords));
	//vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseTexture, vTexCoords));
	////vec3 specular = light.specular * spec /** vec3(texture(diffuseTexture, vTexCoords))*/;
	////vec3 specular = light.specular * spec * roughness;
	//return (ambient + diffuse + specular) * light.intensity;
}

void main()
{
	float ratio = 1.00 / 1.52;
	vec3 R = GetSkyboxReflection(ratio);
	
	//vec3 normal = normalize(vNormals);
	vec3 normal = normalize(2.0 * texture2D(normalsTexture, vTexCoords).xyz - 1.0);
	normal = normalize(tbnMatrix * normal);
	vec3 viewDir = normalize(camPos - vPosition);

	vec3 albedo = texture2D(diffuseTexture, vTexCoords).rgb;
	float metalness = texture2D(metallicTexture, vTexCoords).r;
	float roughness = texture2D(roughnessTexture, vTexCoords).r;

	vec3 color = albedo * dirLight.diffuse;
	color += CalculateDirLight(dirLight, normal, viewDir, albedo, metalness, roughness);


	if (bool(skyboxReflectionEnabled) && bool(drawSkybox))
		fragColor = /*texture2D(diffuseTexture, vTexCoords) * */vec4(texture(skybox, R).rgb * skyboxIntensity, 1) * vec4(color, 1);
	else
		fragColor =/* texture2D(diffuseTexture, vTexCoords) **/ vec4(color, 1);
}