#version 430 core
out vec4 fColor;

in vec4 gl_FragCoord;

uniform int paintMode;

void 
main() 
{ 
	if(paintMode ==0)
		fColor = vec4(0.0, 1.0, 1.0, 0.0);
	else
		fColor = vec4(0.0, 0.0, 0.0, 1.0);
} 