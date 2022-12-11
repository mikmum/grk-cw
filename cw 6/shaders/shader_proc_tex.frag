#version 430 core

uniform vec3 color;
uniform vec3 colorTwo;

out vec4 outColor;
in vec4 modelPositionMatrix;

void main()
{
	if( sin(modelPositionMatrix.y) > 0 )
	{
		outColor = vec4(color, 1.0);
	}
	else
	{
		outColor = vec4(colorTwo, 1.0);
	}

}