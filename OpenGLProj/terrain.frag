#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
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

in vec3 FragPos;
//in vec3 gNormal;

uniform Material material;
uniform Light light;
//uniform vec3 viewPos;


void main() {
	// ambient
    //vec3 ambient = light.ambient * material.ambient;

	// diffuse 
    //vec3 norm = normalize(gNormal);
    //vec3 lightDir = normalize(light.position - FragPos);
    //float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // specular
    //vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 reflectDir = reflect(-lightDir, norm);  
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //vec3 specular = light.specular * (spec * material.specular);  
        
    //vec3 result = ambient + diffuse + specular;
    //FragColor = vec4(result, 1.0);

    float normY = (FragPos.y + 16.0) / 64.0;
    FragColor = vec4(normY, normY, normY, 1.0) * vec4(0.8, 0.68, 0.43, 1.0);
}