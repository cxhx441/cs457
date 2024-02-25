#version 330 compatibility

uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjectionMat;

out vec2 vST;

void
main( )
{
	mat4 M = uModelMat;
	mat4 V = uViewMat;
	mat4 P = uProjectionMat;

    vST = gl_MultiTexCoord0.st;
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_Position = P * V * M * gl_Vertex;
}
