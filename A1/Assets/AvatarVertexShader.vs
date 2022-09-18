#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
out vec3 Normal;
out vec3 FragPos;
void main() {
	gl_Position = P * V * M * vec4(position, 1.0);
    FragPos = vec3(M * vec4(position, 1.0));
    Normal = mat3(M) * normal;
}
