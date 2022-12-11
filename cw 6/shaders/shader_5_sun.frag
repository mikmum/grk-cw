#version 430 core

uniform vec3 color;
uniform vec3 lightColor;
uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec3 spotDir;
uniform sampler2D colorTexture;

out vec4 outColor;
in vec3 outVertexNormal;
in vec4 modelNormalMatrix;
in vec4 modelPositionMatrix;
in vec2 texCoord;

void main()
{
	vec4 textureColor = texture2D(colorTexture, texCoord);

	vec3 viewDirection = normalize(cameraPos - modelPositionMatrix.xyz);

	float nat = max(dot(spotDir, viewDirection), 0.0);
	outColor = vec4(textureColor.xyz*nat, 1.0);
}