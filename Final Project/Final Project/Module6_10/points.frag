#version 150

out vec4 fragColor;    // Output fragment color
in vec3 color;
void main() 
{
	if (dot(gl_PointCoord-0.5, gl_PointCoord-0.5) > 0.25)
		discard;
	else
		fragColor = vec4(color, 1.0);
	
	
}
