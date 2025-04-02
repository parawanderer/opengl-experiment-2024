#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in ivec4 aBoneIds; // for animations
layout (location = 4) in vec4 aWeights;  // ^
layout (location = 5) in vec3 aTangent; // for normal mapping (https://learnopengl.com/Advanced-Lighting/Normal-Mapping)
layout (location = 6) in vec3 aBiTangent; // ^

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 Normal;
out vec2 TexCoord;

// WHEN NO NORMAL TEXTURE
out vec3 FragPos;
out vec3 LightPos;
out vec3 ViewPos;

// WHEN NORMAL TEXTURE
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBoneMatrices[MAX_BONES];
uniform bool doAnimate;



void main()
{
    vec4 aPos4 = vec4(aPos, 1.0);
    TexCoord = aTexCoords;

    vec3 norm;
    vec3 fragPos;
    
    if (doAnimate) { 
        // animated model
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

        fragPos = vec3(model * totalPosition);
        norm = normalMatrix * totalNormal;
        gl_Position = projection * view * model * totalPosition;


    } else { 
        // static model
        
        fragPos = vec3(model * aPos4);
        norm = normalMatrix * aNormal;
        gl_Position = projection * view * model * aPos4;
    }


    // https://learnopengl.com/Advanced-Lighting/Normal-Mapping
    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(norm, 0.0)));
    T = normalize(T - dot(T, N) * N); // re-orthogonalize T with respect to N
    vec3 B = cross(N, T); // then retrieve perpendicular vector B with the cross product of T and N
    mat3 TBN = mat3(T, B, N); 


    // NORMAL TEXTURE CASE (TODO: probably better not to compute this if we know there's no normal texture)
    TangentLightPos = TBN * lightPos;
    TangentViewPos = TBN * viewPos;
    TangentFragPos = TBN * fragPos;


    // NO NORMAL TEXTURE CASE
    Normal = norm;
    LightPos = lightPos;
    ViewPos = viewPos;
    FragPos = fragPos;
}