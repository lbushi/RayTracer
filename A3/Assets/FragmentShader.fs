#version 330

in vec3 vcolour;
out vec4 fragColour;
uniform int picking;
uniform vec3 pickingcolour;
uniform int selected;
void main() {
	if (picking == 0) {
		if (selected == 0) {
			fragColour = vec4(vcolour,1.);
		}
		else {
			fragColour = vec4(pickingcolour, 1.);
		}
	}
	else {
		fragColour = vec4(pickingcolour, 1.);
	} 
}
