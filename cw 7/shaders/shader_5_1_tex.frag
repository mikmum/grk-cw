#version 430 core

uniform vec3 lightColor;
uniform vec3 spotPos;
uniform vec3 spotDir;
uniform float E;

uniform sampler2D colorTexture;

out vec4 outColor;
in vec3 outVertexNormal;
in vec4 vecNormal;

in vec3 viewDirTS;
in vec3 lightDirTS;
in vec2 vecTex;

void main()
{
	vec4 textureColor = texture2D(colorTexture, vecTex);

	vec3 lightDir =  normalize(lightDirTS);
	vec3 vertexNor = vec3(0,0,1);
	float diffuse = max(dot(vertexNor, -lightDir), 0.0) * 300;
	vec3 viewDirection = normalize(viewDirTS);
	vec3 reflectVector = reflect(lightDir, vertexNor);

	float spec = pow(max(dot(viewDirection,reflectVector), 0.0),1000);
	vec3 calculatedColor = max(lightColor/ pow(distance(lightPos, modelPositionMatrix.xyz),2)  * (textureColor.xyz  * diffuse + spec),0);


	vec3 lightDirTwo =  normalize(modelPositionMatrix.xyz - spotPos);
	float diffuseTwo = max(dot(vertexNor, -lightDirTwo), 0.0) * 10;
	vec3 reflectVectorTwo = reflect(lightDirTwo, vertexNor);
	float specTwo = pow(max(dot(viewDirection,reflectVectorTwo), 0.0),64);
	if( dot(lightDirTwo, normalize(spotDir)) > cos(radians(12.5f)) )
	{
		calculatedColor += lightColor / pow(distance(spotPos, modelPositionMatrix.xyz),2) * (calculatedColor  * diffuseTwo + specTwo);
	}

	 outColor = vec4(1-exp(-calculatedColor*E),1);

}