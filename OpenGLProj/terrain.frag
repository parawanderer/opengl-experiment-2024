#version 330 core
out vec4 FragColor;

in float Y;

void main() {
	FragColor = vec4(Y, Y, Y, 0.1);
}