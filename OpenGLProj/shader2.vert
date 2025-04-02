#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    // this matrix multiplication is important to deal with the case of doing non-uniform scaling on an object
    Normal = mat3(transpose(inverse(model))) * aNormal; // to be more efficient you would compute this matrix to the left on the CPU

    gl_Position = projection * view * model * vec4(aPos, 1.0);
} 