#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

out vec4 FragColor;

in vec3 FragPos;

uniform Material material;

void main() {
    float normY = (FragPos.y + 16.0) / 64.0;
    FragColor = vec4(normY, normY, normY, 1.0) * vec4(material.ambient, 1.0);
}