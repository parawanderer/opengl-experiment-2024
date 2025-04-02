#version 330 core
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    bool has_normal;
    sampler2D texture_normal1;
};

struct Light {
    vec3 ambient; // usually set to low value
    vec3 diffuse; // usually set to exact color of the light (e.g. bright white)
    vec3 specular; // usually also kept at vec3(1.0) to shine at full intensity
};

out vec4 FragColor;


in vec3 Normal;
in vec2 TexCoord;


// WHEN NO NORMAL TEXTURE
in vec3 FragPos; // when *not* using normal textures
in vec3 LightPos; // ^
in vec3 ViewPos; //  ^

// WHEN NORMAL TEXTURE
in vec3 TangentLightPos; // When using normal textures
in vec3 TangentViewPos; // ^
in vec3 TangentFragPos; // ^



uniform Material material;
uniform Light light;


vec3 computeNormalTextureCase() {
    vec3 norm = texture(material.texture_normal1, TexCoord).rgb;
    norm = normalize(norm * 2.0 - 1.0);

    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));


    // diffuse
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoord));


    // specular (ignored for now...) -> because I don't have any good specilar maps right now anyways, I think
    vec3 specular = vec3(0.0);
    // vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    // vec3 reflectDir = reflect(-lightDir, norm);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoord));


    vec3 result = ambient + diffuse + specular;
    return result;
}


vec3 computeRegularCase() {
    vec3 norm = normalize(Normal);

    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));


    // diffuse
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoord));


    // specular: is ignored for now (see above)
    vec3 specular = vec3(0.0);
    // vec3 viewDir = normalize(ViewPos - FragPos);
    // vec3 reflectDir = reflect(-lightDir, norm);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoord));


    vec3 result = ambient + diffuse + specular;
    return result;
}


void main()
{
    vec3 result;

    if (material.has_normal) { // use normal texture
        result = computeNormalTextureCase();
    } else { // compute using vertex normals
        result = computeRegularCase();
    }

    FragColor = vec4(result, 1.0);
}