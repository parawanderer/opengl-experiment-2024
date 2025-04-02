#version 330 core
struct Material {
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D normal;
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

// TODO: provide these as variables (temp hardcoded values)
uniform float FogEnd = 6000.0;
uniform float ExpDensityFactor = 1.0;
uniform vec3 FogColor = vec3(0.537, 0.506, 0.6);

// ref: https://www.youtube.com/watch?v=oQksg57qsRA
// I quite appreciate how this exponential function ends up looking visually on my terrain
float calculateExponentialFog() 
{
    float cameraToPixelDist = length(FragPos - viewPos);
    float distRatio = 4.0 * cameraToPixelDist / FogEnd;
    float fogFactor = exp(-distRatio * ExpDensityFactor * distRatio * ExpDensityFactor);

    return fogFactor;
}

void main() 
{
    // ambient
    //vec3 ambient = light.ambient * material.ambient;
    vec3 ambient = light.ambient * vec3(texture(material.ambient, TexCoord));
  	
    vec3 norm = normalize(Normal);
    //vec3 norm = texture(material.normal, TexCoord).rgb;
    //norm = normalize(norm * 2.0 - 1.0);
    
    // diffuse 
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
    vec4 nearFinalResult = vec4(result, 1.0);

    if (FogColor != vec3(0)) {
        float fogFactor = calculateExponentialFog();

        nearFinalResult = mix(vec4(FogColor, 1.0), nearFinalResult, fogFactor);
    }

    FragColor = nearFinalResult;
}