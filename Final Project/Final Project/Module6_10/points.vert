#version 150
// Per vertex attributes
in vec3 positionT;
// Uniform
uniform mat4 pv;
in vec3 colorT;
smooth out vec3 color;
void main()
{
	color = colorT;
	gl_Position  = pv * vec4(positionT, 1.0);
}