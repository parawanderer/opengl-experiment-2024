#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform mat4 view;

uniform mat2 rotate;
// model transform is computed below V

uniform vec3 particleCenterWorld;
uniform vec3 cameraRightWorldSpace;
uniform vec3 cameraUpWorldSpace;
uniform vec2 billboardSize;

//uniform vec3 offset;
uniform vec4 color;

void main() {
	TexCoords = aTexCoords;
	ParticleColor = color;

	vec2 rotatedPos = rotate * aPos; // allow rotating the billboard (in 2D). 
	// This is rotating the 2D flat billboard that we will eventually see on the screen.

	// https://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/
	vec3 positionWorld = particleCenterWorld
						+ cameraRightWorldSpace * rotatedPos.x * billboardSize.x
						+ cameraUpWorldSpace * rotatedPos.y * billboardSize.y;

	gl_Position = projection * view * vec4(positionWorld, 1.0);
}