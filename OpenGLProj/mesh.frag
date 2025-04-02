#version 330 core
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
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
    vec3 norm = normalize(Normal);

    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));

    // diffuse
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoord));

    // specular (ignored for now...)
    vec3 specular = vec3(0.0);

    //if (material.texture_specular1) {
    //    vec3 viewDir = normalize(viewPos - FragPos);
    //    vec3 reflectDir = reflect(-lightDir, norm);
    //    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //    specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoord));
    //}

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}