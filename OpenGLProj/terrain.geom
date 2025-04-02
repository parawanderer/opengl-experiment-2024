#version 330
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 gNormal;

uniform mat3 normalMatrix;

in vec3 FragPos[];

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

// ref: https://learnopengl.com/Advanced-OpenGL/Geometry-Shader
void main() {
	//vec3 normal = normalMatrix * GetNormal();
	vec3 side2 = FragPos[0] - FragPos[2];
    vec3 side0 = FragPos[1] - FragPos[2];
    vec3 normal = normalize(normalMatrix * cross(side0, side2));

	gNormal = normal;
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	gNormal = normal;
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	gNormal = normal;
    gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}