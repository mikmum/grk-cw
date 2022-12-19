#version 430 core

uniform vec3 lightColor;
uniform vec3 spotPos;
uniform vec3 spotDir;
uniform float E;

uniform sampler2D colorTexture;
uniform sampler2D normalSampler;

out vec4 outColor;
in vec3 outVertexNormal;

in vec3 viewDirTS;
in vec3 lightDirTS;
in vec2 vecTex;

in flat float lightDistance;

void main()
{
	vec4 textureColor = texture2D(colorTexture, vecTex);
	vec4 N = texture2D(normalSampler, vecTex);
	N = normalize(2*N - 1);

	vec3 lightDir =  normalize(lightDirTS);
	vec3 vertexNor = N.xyz;
	float diffuse = max(dot(vertexNor, lightDir), 0.0) * 300;
	vec3 viewDirection = normalize(viewDirTS);
	vec3 reflectVector = reflect(lightDir, vertexNor);

	float spec = pow(max(dot(viewDirection,reflectVector), 0.0),1000);
	vec3 calculatedColor = max(lightColor/ pow(lightDistance,2)  * (textureColor.xyz  * diffuse + spec),0);

	 outColor = vec4(1-exp(-calculatedColor*E),1);

}