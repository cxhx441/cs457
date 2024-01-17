#version 330 compatibility

// uniform float uAmp; // amplitude of sine wave
// uniform float uFreq; // frequency of sine wave

out vec3 vN; // normal vector
out vec3 vL1; // point to light vector
out vec3 vL2; // point to light vector
out vec3 vL3; // point to light vector
out vec3 vE; // point to eye vector
out vec2 vST;

const vec3 LIGHTPOS1 = vec3( 10., 100., 10. ); // light position
const vec3 LIGHTPOS2 = vec3( -10., 10., 10. ); // light position
const vec3 LIGHTPOS3 = vec3( 0., -10., 10. ); // light position

void
main( )
{

    vST = gl_MultiTexCoord0.st;

  	vec3 ECposition = (gl_ModelViewMatrix * gl_Vertex).xyz; // camera coordiantes of point
	vN = normalize( gl_NormalMatrix * gl_Normal );  // normal vector
	vL1 = LIGHTPOS1 - ECposition.xyz;	                // vector from the point to the light position
	vL2 = LIGHTPOS2 - ECposition.xyz;	                // vector from the point to the light position
	vL3 = LIGHTPOS3 - ECposition.xyz;	                // vector from the point to the light position
	vE = vec3( 0., 0., 0. ) - ECposition;           // vector from the point to eye

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
}
