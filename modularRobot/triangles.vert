#version 430 core
layout(location = 0) in vec4 vPosition;

uniform mat4 rotateMatrix, windowMatrix;

uniform int paintMode;

void 
main() 
{ 
	//if(paintMode ==0)
		gl_Position = windowMatrix * rotateMatrix * vPosition ;
	//else
	//	gl_Position = windowMatrix * vPosition;
	gl_Position.z =  - gl_Position.z;
}