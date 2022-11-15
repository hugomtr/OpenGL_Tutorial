#version 330 core
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture_diff;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;

void main()
{
	// Ambient lighting
	float ambientStrength = 0.005;
	vec3 ambient = ambientStrength * lightColor;

	// Diffuse lighting

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = diff * lightColor;
	
	// Specular Lighting

	float specularStrength = 0.5;
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
	vec3 specular = specularStrength * spec * lightColor;
	
	// Final result
	vec3 result = (ambient + diffuse + specular) * texture(texture_diff,TexCoord).xyz;
	FragColor = vec4(result,1.0f);
}