#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 position;
};

in vec3 Normal;
in vec3 FragPos;
out vec4 FragColor;

uniform vec3 cameraPos;

uniform Material material;
uniform Light light;

void main()
{
	// Ambient lighting

	vec3 ambient = light.ambient * material.ambient;

	// Diffuse lighting

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = (material.diffuse * diff) * light.diffuse;

	// Specular Light
	
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
	vec3 specular = (material.specular * spec) * light.specular;

	// FInal result
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}