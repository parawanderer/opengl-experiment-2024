#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIds; 
layout (location = 4) in vec4 aWeights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normalMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;


const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBoneMatrices[MAX_BONES];

void main()
{
    vec4 totalPosition = vec4(0.0f);

    for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if (aBoneIds[i] == -1) continue;

        if (aBoneIds[i] >= MAX_BONES) {
            totalPosition = vec4(aPos, 1.0f);
            break;
        }

        vec4 localPosition = finalBoneMatrices[aBoneIds[i]] * vec4(aPos, 1.0f);
        totalPosition += localPosition * aWeights[i];
        vec3 localNormal = mat3(finalBoneMatrices[aBoneIds[i]]) * aNormal;
    }

    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalMatrix * aNormal;
    TexCoord = aTexCoords;    

    gl_Position = projection * view * model * totalPosition;
}