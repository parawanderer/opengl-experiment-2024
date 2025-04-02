#version 330 core
layout (location = 0) in vec2 aPos; // 2D full-screen quad positions
layout (location = 1) in vec2 aTexCoords; // 2D full-screen quad texture sampling coordinates for the given position

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    TexCoords = aTexCoords;
}  