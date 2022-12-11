#version 430 core

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec4 vertexColor;

precision mediump float;
uniform mat4 transformation;
uniform float time;

flat out vec4 color;

out vec4 pos_local; 
out vec4 pos_global;

out flat float out_time;

void main()
{
	//color = vertexColor / time;
	gl_Position = transformation * vertexPosition;
	color = mix(vertexColor, glm::vec4(0.0f, 0.3f, 0.3f, 1.0f), sin(time));
	pos_local = vertexPosition;
	pos_global = transformation * vertexPosition;
	out_time = time;
}
