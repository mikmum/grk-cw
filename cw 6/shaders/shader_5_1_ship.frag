#version 430 core

float AMBIENT = 0.1;

uniform vec3 color;
uniform vec3 lightPos;
uniform sampler2D colorTexture;
uniform sampler2D rust;
uniform sampler2D scratches;
uniform sampler2D shipNormal;
uniform sampler2D rustNormal;

in vec2 vecTex;
in vec3 viewDirTS;
in vec3 lightDirTS;
in flat float lightDistance;

uniform vec3 lightColor;
uniform float E;

out vec4 outColor; 
void main()
{
	vec4 textureColor = texture2D(colorTexture, vecTex);
	vec4 addColor = texture2D(rust, vecTex);
	vec4 scratchesColor = texture2D(scratches, vecTex);
	vec4 shipN = texture2D(shipNormal, vecTex);
	vec4 rustN = texture2D(rustNormal, vecTex);
	vec4 N = mix(shipN, rustN, scratchesColor.r);
	N = normalize(2*N - 1);

	vec4 finalColor = mix(textureColor, addColor, scratchesColor.r);

	vec3 lightDir = normalize(lightDirTS);
	vec3 normal = N.xyz;
	float diffuse=max(0,dot(normal,lightDir));
	outColor = vec4(4*finalColor.xyz*min(1,AMBIENT+diffuse), 1.0);
	//outColor = vec4(normalize(lightDirTS), 1.0);
}
