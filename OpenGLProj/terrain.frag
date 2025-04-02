#version 330 core
struct Material {
    //vec3 ambient;
    //vec3 diffuse;
    sampler2D ambient;
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient; // usually set to low value
    vec3 diffuse; // usually set to exact color of the light (e.g. bright white)
    vec3 specular; // usually also kept at vec3(1.0) to shine at full intensity
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main() 
{
    // ambient
    //vec3 ambient = light.ambient * material.ambient;
    vec3 ambient = light.ambient * vec3(texture(material.ambient, TexCoord));
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = light.diffuse * (diff * material.diffuse);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));  

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  
        
    vec3 result = ambient + diffuse + specular;
    //FragColor = texture(texture1, TexCoord); //vec4(result, 1.0);
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(normalize(Normal) * 0.5, 1.0) + vec4(0.5);
}