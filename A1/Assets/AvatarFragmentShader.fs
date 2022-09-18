#version 330

uniform vec3 colour;
out vec4 fragColor;
in vec3 Normal;
uniform vec3 lightPos; 
in vec3 FragPos;

void main() {
    vec3 lightcolour = vec3(1.0f);
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); 
    fragColor = vec4(max(dot(norm, lightDir), 0.0) * lightcolour * colour, 1);
}
