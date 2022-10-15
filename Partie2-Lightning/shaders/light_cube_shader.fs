#version 330 core
out vec4 FragColor;
uniform vec3 lightColor;

void main()
{
	// Constant white color
	FragColor = vec4(lightColor,1.0f);
}