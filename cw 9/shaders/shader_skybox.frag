#version 430 core

uniform samplerCube skybox;

in vec3 texCoord;

out vec4 out_color;

void main()
{
	out_color = texture(skybox,texCoord);
	//out_color = vec4(0.0f, 0.3f, 0.3f, 1.0f);
}