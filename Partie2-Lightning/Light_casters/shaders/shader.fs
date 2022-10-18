#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 direction_spot;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float quadratic;
    float linear;
    float constant;

    float cutoff;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 cameraPos;
uniform Material material;
uniform Light light;

void main()
{
    vec3 lightDir = normalize(light.position - FragPos);
    float theta = dot(lightDir,normalize(-light.direction_spot));
    vec3 result;


    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
        

    // attenuation of the light given distance   
    float d = length(light.position - FragPos);
    float attenuation = 1 / (light.constant + light.linear*d + light.quadratic*pow(d,2));


    if (theta > light.cutoff){

        // diffuse 
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
        
        // specular
        vec3 viewDir = normalize(cameraPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  

        result = (diffuse + specular + ambient) * attenuation;
    }
    else {
        result = ambient*attenuation;
    }
    FragColor = vec4(result, 1.0);
} 