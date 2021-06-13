#version 150

out vec4 frag_color;
in vec3 texcoords;
uniform samplerCube skybox;

void main() {
	frag_color = texture(skybox, texcoords);
}