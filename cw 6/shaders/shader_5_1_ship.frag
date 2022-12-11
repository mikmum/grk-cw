#version 430 core

float AMBIENT = 0.1;

uniform vec3 color;
uniform vec3 lightPos;
uniform sampler2D colorTexture;
uniform sampler2D rust;
uniform sampler2D scratches;

in vec3 vecNormal;
in vec3 worldPos;
in vec2 texCoord;

out vec4 outColor;
void main()
{
	vec2 fixedCoord = vec2(texCoord.x, -texCoord.y);
	vec4 textureColor = texture2D(colorTexture, fixedCoord);
	vec4 addColor = texture2D(rust, fixedCoord);
	vec4 scratchesColor = texture2D(scratches, fixedCoord);

	vec4 finalColor = mix(textureColor, addColor, scratchesColor.r);

	vec3 lightDir = normalize(lightPos-worldPos);
	vec3 normal = normalize(vecNormal);
	float diffuse=max(0,dot(normal,lightDir));
	outColor = vec4(finalColor.xyz*min(1,AMBIENT+diffuse), 1.0);
}
