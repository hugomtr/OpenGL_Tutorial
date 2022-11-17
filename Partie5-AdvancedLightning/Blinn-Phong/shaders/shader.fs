#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D floorTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool blinn;

void main()
{          
    float distance = length(vec3(lightPos - FragPos));
    float attenuation = 1.0 / (distance );
    vec3 color = texture(floorTexture, TexCoords).rgb;
    // ambient
    vec3 ambient = 0.01 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color * attenuation;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = 0.0;
    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = (vec3(color) + vec3(1.0))* attenuation * spec; // assuming bright white light color
    vec3 composante_color = ambient + diffuse + specular;

    float gamma = 2.2;

    FragColor = vec4(pow(composante_color,vec3(1.0/gamma)), 1.0);

}

