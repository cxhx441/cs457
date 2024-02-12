#version 330 compatibility

out vec3 vST;

void
main( void ){
	vST = gl_MultiTexCoord0.st;
	gl_Position = gl_ModelViewMatrix * gl_Vertex;
}
