#version 330 compatibility

out vec3 vN; // normal vector
out vec3 vL; // point to light vector
out vec3 vE; // point to eye vector
//out vec3 vMC;
out vec2 vST;


uniform float uLightX, uLightY, uLightZ;
uniform float uAmp, uPeriod, uPhase, uDecay;

const float PI = 3.14159265;

void
main( )
{
	vec4 pos = gl_ModelViewMatrix * gl_Vertex;
	float lenxy = length(pos.xy);

	float phi = atan(lenxy, -pos.z);
	pos.xy = (phi / (PI/2.)) * (pos.xy / lenxy);

	//vMC = vert.xyz;
	vST = gl_MultiTexCoord0.st;
	vN = normalize( gl_NormalMatrix * gl_Normal );
  	vec3 CCposition = (gl_ModelViewMatrix * gl_Vertex).xyz; // camera coords
	vL = vec3(uLightX, uLightY, uLightZ) - CCposition.xyz;
	vE = vec3( 0., 0., 8. ) - CCposition;

    gl_Position = gl_ProjectionMatrix * pos;
}
