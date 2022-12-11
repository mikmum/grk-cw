#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;

uniform mat4 transformation;
uniform mat4 modelMatrix;

out vec4 modelPositionMatrix;

void main()
{
	gl_Position = transformation * vec4(vertexPosition, 1.0);
	modelPositionMatrix = vec4(vertexPosition, 1.0);
}