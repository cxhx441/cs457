#version 330 compatibility

out vec3 vN; // normal vector
out vec3 vL; // point to light vector
out vec3 vE; // point to eye vector
//out vec3 vMC;
out vec2 vST;


uniform float uLightX, uLightY, uLightZ;
uniform float uAmp, uPeriod, uPhase, uDecay;

const float F_2_PI = 2.0 * 3.14159265;
//vec3 LIGHTPOS = (gl_ModelViewMatrix * vec4( 10., 10., 10., 1.)).xyz; // light position stays relative to object
//const vec3 LIGHTPOS = vec3( 0., 0., 10.); // light position

void
main( )
{
	vec3 vert = gl_Vertex.xyz;
	//vMC = vert.xyz;
	vST = gl_MultiTexCoord0.st;
	vN = normalize( gl_NormalMatrix * gl_Normal );
  	vec3 CCposition = (gl_ModelViewMatrix * gl_Vertex).xyz; // camera coords
	vL = vec3(uLightX, uLightY, uLightZ) - CCposition.xyz;
	vE = vec3( 0., 0., 8. ) - CCposition;

    gl_Position = gl_ModelViewProjectionMatrix * vec4 ( vert, 1.0 );
}
