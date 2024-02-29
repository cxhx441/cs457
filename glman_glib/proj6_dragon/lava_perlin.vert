#version 330 compatibility

//out vec3 vMC;
out vec2 vST;


uniform float uAmp, uPeriod, uPhase, uDecay;

const float F_2_PI = 2.0 * 3.14159265;

void
main( )
{
	vec3 vert = gl_Vertex.xyz;
	vST = gl_MultiTexCoord0.st;
    gl_Position = gl_ModelViewProjectionMatrix * vec4 ( vert, 1.0 );
}
