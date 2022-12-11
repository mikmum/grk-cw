#version 430 core

uniform vec3 color;
uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec3 spotDir;

out vec4 outColor;
in vec3 outVertexNormal;
in vec4 modelNormalMatrix;
in vec4 modelPositionMatrix;

void main()
{
	vec3 viewDirection = normalize(cameraPos - modelPositionMatrix.xyz);

	float nat = max(dot(spotDir, viewDirection), 0.0); 

	outColor = vec4(color * nat, 1.0);
}