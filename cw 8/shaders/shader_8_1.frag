#version 430 core

float AMBIENT = 0.1;

uniform vec3 cameraPos;

uniform vec3 color;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 spotlightPos;
uniform vec3 spotlightColor;
uniform vec3 spotlightConeDir;
uniform vec3 spotlightPhi;

uniform float exposition;

in vec3 vecNormal;
in vec3 worldPos;

in vec3 viewDirTS;
in vec3 lightDirTS;
in vec3 spotlightDirTS;
in vec3 test;

out vec4 outColor;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

vec3 phongLight(vec3 lightDir, vec3 lightColor, vec3 normal,vec3 viewDir){
	
	float diffuse=max(0,dot(normal,lightDir));

	vec3 R = reflect(-lightDir, normal);  

	float specular = pow(max(dot(viewDir, R), 0.0), 32);

	vec3 resultColor = color*diffuse*lightColor+lightColor*specular;
	return resultColor;
}

vec3 PBRLight(vec3 lightDir, vec3 lightColor, vec3 normal,vec3 viewDir, float a, vec3 metalness)
{ 
	float kS = calculateSpecularComponent(...); // reflection/specular fraction
	float kD = 1.0 - kS;


	vec3 h = normalize(lightDir + viewDir);
	float cosTheta = dot(lightDir, normal);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, color.rgb, metalness
	k = (a+1)^2/8;

	vec3 F =  fresnelSchlick(cosTheta, F0);

	return (1-F) * color.rgb / 3.14 + DistributionGGX(normal, h, a) * F * GeometrySmit(normal, viewDir, lightDir, float k);
}

void main()
{
	vec3 normal = vec3(0,0,1);
	vec3 viewDir = normalize(viewDirTS);
	vec3 lightDir = normalize(lightPos-worldPos);
	vec3 ambient = AMBIENT*color;
	vec3 attenuatedlightColor = lightColor/pow(length(lightPos-worldPos),2);
	vec3 ilumination;
	ilumination = ambient+phongLight(normalize(lightDirTS),attenuatedlightColor,normal,viewDir);
	//flashlight
	vec3 spotlightDir= normalize(spotlightPos-worldPos);
	float angle_atenuation = clamp((dot(-spotlightDir,spotlightConeDir)-0.8)*3,0,1);

	attenuatedlightColor = spotlightColor/pow(length(spotlightPos-worldPos),2)*angle_atenuation;
	ilumination=ilumination+phongLight(normalize(spotlightDirTS),attenuatedlightColor,normal,viewDir);
	
	outColor = vec4(1.0 - exp(-ilumination*exposition),1);
	//outColor = vec4(test,1);
}
