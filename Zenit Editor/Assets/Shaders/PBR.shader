#type vertex
#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangents;
layout(location = 4) in vec3 biTangents;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec3 vWorldPosition;
out vec2 vTexCoords;
out vec3 vNormals;
out mat3 TBN;
out mat4 vModel;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1);
	vTexCoords = texCoords;
	vNormals = normalize((model * vec4(normals, 0.0)).xyz);
	vWorldPosition = (model * vec4(position, 1)).xyz;
	vModel = model;

	vec3 N = normalize(normals);
	vec3 T = tangents;
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(T, N);
	TBN = mat3(T, B, N);
}

#type fragment
#version 430 core

layout(location = 0) uniform sampler2D diffuseTexture;
layout(location = 1) uniform sampler2D normalsTexture;
layout(location = 2) uniform sampler2D metallicTexture;
layout(location = 3) uniform sampler2D roughnessTexture;
layout(location = 4) uniform sampler2D ambientOcclussionTexture;
layout(location = 5) uniform samplerCube irradianceMap;
layout(location = 6) uniform samplerCube skyboxPrefilterMap;
layout(location = 7) uniform sampler2D skyboxBrdf;

uniform vec3 camPos;
uniform float reflectionLod;
uniform int skyboxReflectionEnabled;
uniform int drawSkybox;

out vec4 fragColor;

in vec3 vWorldPosition;
in vec2 vTexCoords;
in vec3 vNormals;
in mat3 TBN;
in mat4 vModel;

const float PI = 3.14159265359;

struct DirLight
{
	vec3 direction;
	vec3 color;
	float intensity;
};
uniform DirLight dirLight;


vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return  F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a2 = roughness * roughness;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return (a2) / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float denom = NdotV * (1.0 - k) + k;

	return NdotV / denom;
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 CalculateDirLight(DirLight dirLight, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness, out vec3 F0)
{
	vec3 lightDir = normalize(dirLight.direction);
	vec3 halfway = normalize(viewDir + lightDir);

	float NdotL = max(dot(normal, lightDir), 0.0);
	float NdotV = max(dot(normal, viewDir), 0.0);
	float NdotH = max(dot(normal, halfway), 0.0);

	F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	float NDF = DistributionGGX(normal, halfway, roughness);
	float G = GeometrySmith(NdotV, NdotL, roughness);
	vec3 F = FresnelSchlick(max(dot(halfway, viewDir), 0.0), F0);

	vec3 numerator = NDF * F * G;
	float denominator = 4 * max((NdotV * NdotL), 0.0001);
	vec3 specular = numerator / denominator;

	// Lambert Diffuse ========
	vec3 ks = F;
	vec3 kd = vec3(1.0) - ks;
	kd *= (1.0f - metallic);
	// Lambert Diffuse ========

	vec3 radiance = dirLight.color * dirLight.intensity;

	return (kd * (albedo / PI) + specular) * radiance * NdotL;
}

void main()
{
	vec3 normal = texture2D(normalsTexture, vTexCoords).xyz * 2.0 - 1.0;

	if (normal == vec3(1.0))
	{
		normal = normalize(vNormals);
	}
	else
	{
		normal = normalize(TBN * normal);
		normal = normalize((vModel * vec4(normal, 0.0)).xyz);
	}

	vec3 albedo = texture2D(diffuseTexture, vTexCoords).rgb;
	vec3 irradiance = texture(irradianceMap, normal).rgb;
	float metallic = texture2D(metallicTexture, vTexCoords).r;
	float roughness = max(texture2D(roughnessTexture, vTexCoords).r, 0.01f);
	
	float ao = texture2D(ambientOcclussionTexture, vTexCoords).r;
	
	vec3 viewDir = normalize(camPos - vWorldPosition);
	
	vec3 color = vec3(0);
	vec3 F0 = vec3(0);
	color += CalculateDirLight(dirLight, normal, viewDir, albedo, metallic, roughness, F0);
	
	vec3 F = FresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
	vec3 ks = F;
	vec3 kd = 1.0 - ks;
	kd *= 1.0 - metallic;
	vec3 diffuse = irradiance * albedo;

	vec3 ambient = kd * diffuse;
	if (bool(skyboxReflectionEnabled) && bool(drawSkybox))
	{
		vec3 R = reflect(-viewDir, normal);
		vec3 prefilteredColor = textureLod(skyboxPrefilterMap, R, roughness * reflectionLod).rgb;
		vec2 brdf = texture2D(skyboxBrdf, vec2(max(dot(normal, viewDir), 0.0)), roughness).rg;
		vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

		ambient += specular;
	}
	
	color += ambient;

	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / 2.2));


	fragColor = vec4(color, 1);	
}