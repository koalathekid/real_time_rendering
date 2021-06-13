#version 150

in vec3 vPosition;
uniform mat4 P, V;
out vec3 texcoords;

void main() {
	texcoords = vPosition;

	vec4 pos = P * V * vec4(vPosition, 1.0);

	gl_Position = pos.xyww;
}