#version 330 compatibility

uniform mat4 uModelView;
uniform mat4 uProjection;

out vec2 vST;
out vec3 vMC;

void
main( )
{
	//mat4 MV = uModelView;
	//mat4 P = uProjection;
    //mat4 MVP = uProjection * uModelView;

    vMC = gl_Vertex.xyz;
    vST = gl_MultiTexCoord0.st;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    //gl_Position = P * V * M * gl_Vertex;
}
