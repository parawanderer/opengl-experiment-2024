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
uniform bool doAnimate;

void main()
{
    vec4 aPos4 = vec4(aPos, 1.0);
    TexCoord = aTexCoords;

    if (doAnimate) { // animated model

        vec4 totalPosition = vec4(0.0);
        vec3 totalNormal = vec3(0.0);

        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            if (aBoneIds[i] == -1) continue; // not set in this case.

            if (aBoneIds[i] >= MAX_BONES) {
                totalPosition = aPos4;
                break;
            }

            vec4 localPosition = finalBoneMatrices[aBoneIds[i]] * aPos4;
            totalPosition += localPosition * aWeights[i];
            vec3 localNormal = mat3(finalBoneMatrices[aBoneIds[i]]) * aNormal;
            totalNormal += localNormal;
        }

        FragPos = vec3(model * totalPosition);
        Normal = normalMatrix * aNormal;
        gl_Position = projection * view * model * totalPosition;

    } else { // static model
        FragPos = vec3(model * aPos4);
        Normal = normalMatrix * aNormal;
        gl_Position = projection * view * model * aPos4;
    }
}