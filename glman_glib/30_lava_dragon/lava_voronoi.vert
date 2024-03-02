#version 330 compatibility

out vec2 vST;

void
main( )
{
	vec3 vert = gl_Vertex.xyz;
	vST = gl_MultiTexCoord0.st;
    gl_Position = gl_ModelViewProjectionMatrix * vec4 ( vert, 1.0 );
}
