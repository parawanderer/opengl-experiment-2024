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



// attenuation test
struct Attenuation {
    float c1;
    float c2;
    float c3;
};

const int MAX_ATTENUATED_LIGHTS = 4;
uniform Light attLights[MAX_ATTENUATED_LIGHTS];
uniform Attenuation attConsts[MAX_ATTENUATED_LIGHTS];
uniform int numAttLights;
//uniform Light light2;
//uniform float c1;
//uniform float c2;
//uniform float c3;


// fog
const float FogEnd = 6000.0;
const float ExpDensityFactor = 1.0;
const vec3 FogColor = vec3(0.537, 0.506, 0.6);


// ref: https://www.youtube.com/watch?v=oQksg57qsRA
// I quite appreciate how this exponential function ends up looking visually on my terrain
float calculateExponentialFog() 
{
    float cameraToPixelDist = length(FragPos - viewPos);
    float distRatio = 4.0 * cameraToPixelDist / FogEnd;
    float fogFactor = exp(-distRatio * ExpDensityFactor * distRatio * ExpDensityFactor);

    return fogFactor;
}

// light source attenuation
float computeAttenuationFactor(float c1, float c2, float c3, float d) {
    return min(1 / (c1 + c2 * d + c3 * d * d), 1.0);
}


void main() 
{
    vec3 norm = normalize(Normal);

    // ambient
    vec3 ambientTex = vec3(texture(material.ambient, TexCoord));
    vec3 ambient = light.ambient * ambientTex;
    
    // diffuse 
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular); 

    vec3 totalAmbient = vec3(0.0);
    vec3 totalDiffuseAtt = vec3(0.0);
    vec3 totalSpecularAtt = vec3(0.0);
    for (int i = 0; i < MAX_ATTENUATED_LIGHTS && i < numAttLights; ++i) {
        // ambient 
        totalAmbient += attLights[i].ambient * ambientTex;

        // diffuse2 with attenuation
        vec3 lightDirAtt = normalize(attLights[i].position - FragPos);
        float diff2 = max(dot(norm, lightDirAtt), 0.0);
        float fatt = computeAttenuationFactor(attConsts[i].c1, attConsts[i].c2, attConsts[i].c3, length(attLights[i].position - FragPos));
        vec3 diffuse2 = fatt * attLights[i].diffuse * diff2 * vec3(texture(material.diffuse, TexCoord));
        totalDiffuseAtt += diffuse2;

        // specular2 with attenuation
        vec3 reflectDir2 = reflect(-lightDirAtt, norm);  
        float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), material.shininess);
        vec3 specular2 = fatt * attLights[i].specular * (spec2 * material.specular); 
        totalSpecularAtt += specular2;
    }
        
    vec3 result = (ambient + totalAmbient) + (diffuse + totalDiffuseAtt) + (specular + totalSpecularAtt);
    vec4 nearFinalResult = vec4(result, 1.0);

    if (FogColor != vec3(0)) {
        float fogFactor = calculateExponentialFog();

        nearFinalResult = mix(vec4(FogColor, 1.0), nearFinalResult, fogFactor);
    }

    FragColor = nearFinalResult;
}