#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float quadratic;
    float linear;
    float constant;
    
    float cutoff;
    float cutoff_big; 

    float flash;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float quadratic;
    float linear;
    float constant;
};

vec3 calc_directional_light(DirLight light,vec3 normal,vec3 viewDir);
vec3 calc_point_light(PointLight light,vec3 normal,vec3 pos,vec3 viewDir);
vec3 calc_spot_light(SpotLight light,vec3 normal,vec3 pos,vec3 viewDir);

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 cameraPos;

#define NR_POINT_LIGHTS 4

uniform Material material;
uniform DirLight dirLight;
uniform PointLight[NR_POINT_LIGHTS] pointLights;
uniform SpotLight spotLight;


void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(cameraPos - FragPos);
    
    // phase 1: Directional lighting
    vec3 result = calc_directional_light(dirLight,norm,viewDir);

    // phase 2: Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += calc_point_light(pointLights[i], norm, FragPos, viewDir);
    
    // // // phase 3: Spot light
    result += calc_spot_light(spotLight, norm, FragPos, viewDir);
    FragColor = vec4(result, 1.0);
}



vec3 calc_directional_light(DirLight light,vec3 normal,vec3 view_dir){
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    // diffuse 
    float diff = max(dot(normal, normalize(-light.direction)), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
    // specular
    vec3 reflectDir = reflect(normalize( - light.direction), normal);  
    float spec = pow(max(dot(view_dir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  

    return (ambient + diffuse + specular);
}



vec3 calc_point_light(PointLight light,vec3 normal,vec3 pos,vec3 view_dir){
    vec3 light_dir = normalize(light.position - pos);

    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    // diffuse
    float diff = max(dot(normal,light_dir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // specular
    vec3 reflect_dir = reflect(normalize(light_dir),normal);
    float spec = pow(max(dot(view_dir, reflect_dir),0.0),material.shininess);
    vec3 specular = spec * light.specular * texture(material.specular, TexCoords).rgb;

    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1 / (light.constant + light.linear*distance + light.quadratic*pow(distance,2));

    return (ambient + diffuse + specular) * attenuation;
}



vec3 calc_spot_light(SpotLight light,vec3 normal,vec3 pos,vec3 view_dir){
    
    vec3 light_dir = normalize(light.position - pos);

    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1 / (light.constant + light.linear*distance + light.quadratic*pow(distance,2));

    float theta = dot(light_dir,normalize(-light.direction));
    float epsilon = light.cutoff - light.cutoff_big;
    float intensity = (theta - light.cutoff_big) / epsilon;
    
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    if (intensity > 0){
        // diffuse
        float diff = max(dot(normal,light_dir), 0.0);
        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

        // specular
        vec3 reflect_dir = reflect(normalize(light_dir),normal);
        float spec = pow(max(dot(view_dir, reflect_dir),0.0),material.shininess);
        vec3 specular = spec * light.specular * texture(material.specular, TexCoords).rgb;

        return (diffuse + specular) * attenuation * intensity * light.flash  +  ambient * attenuation;
    }
    else {
        return ambient * attenuation;
    }
}