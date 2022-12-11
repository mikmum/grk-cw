#version 430 core

out vec4 out_color;
uniform vec3 in_color;


float calculateZ()
{
	float z_n = 2.0 * gl_FragCoord.z - 1.0;
	float n = 0.05;
	float f = 20.;
	float z = (2.0 * n * f) / ( z_n * (n - f) + n + f);
	return z/f;
}

void main()
{
	//vec3 color = in_color;
	//color.r = calculateZ();
	//color.g = calculateZ();
	//color.b = calculateZ();
	vec4 color = mix(vec4(in_color,1), glm::vec4(0.0f, 0.3f, 0.3f, 1.0f), calculateZ());
	out_color = color;
}


