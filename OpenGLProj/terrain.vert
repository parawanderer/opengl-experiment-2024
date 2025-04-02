#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normalMatrix;

uniform vec3 viewPos;
uniform vec3 lightPos;

const int MAX_ATTENUATED_LIGHTS = 4;
uniform vec3 attLightPos[MAX_ATTENUATED_LIGHTS];
uniform int numAttLights;


out vec3 FragPosWorld;
out vec3 ViewPosWorld;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

out vec3 LightPos;
out vec3 ViewPos;

out vec3 attLightPosT[MAX_ATTENUATED_LIGHTS];
out vec3 attLightPosWorld[MAX_ATTENUATED_LIGHTS];

void main() {
	vec3 norm = normalMatrix * aNormal; // this matrix multiplication is important to deal with the case of doing non-uniform scaling on an object

	// https://learnopengl.com/Advanced-Lighting/Normal-Mapping
	vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(norm, 0.0)));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N); 


	// WORLD COORDINATES
	FragPosWorld = vec3(model * vec4(aPos, 1.0));
	ViewPosWorld = viewPos;


	// TANGENT COORDINATES
	FragPos = TBN * vec3(model * vec4(aPos, 1.0));
    Normal = norm; // I'm not sure why my normal is suddenly incorrect now that I try to compute things this way?
	TexCoord = aTexCoord;

	LightPos = TBN * lightPos;
	ViewPos = TBN * viewPos;
	

	gl_Position = projection * view * model * vec4(aPos, 1.0);


	// attenuated lights
	for (int i = 0; i < MAX_ATTENUATED_LIGHTS && i < numAttLights; ++i) {
		attLightPosT[i] = TBN * attLightPos[i];
		attLightPosWorld[i] = attLightPos[i];
	}
}