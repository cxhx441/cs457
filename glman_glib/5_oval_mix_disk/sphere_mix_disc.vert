#version 330 compatibility

uniform float uOffsetS, uOffsetT, uBlend;
out vec3 vN; // normal vector
out vec3 vL; // point to light vector
out vec3 vE; // point to eye vector
out vec2 vST;
out vec3 vColor;

//vec3 LIGHTPOS = (gl_ModelViewMatrix * vec4( 10., 10., 10., 1.)).xyz; // light position
const float F_2_PI = 2.0 * 3.1415926535897932384626433832795;
const vec3 LIGHTPOS = vec3( 10., 10., 10.); // light position

void
main( )
{

	// original model coords of sphere;
	vec4 vertex0 = gl_Vertex;
	vec3 norm0 = gl_Normal;

    vST = gl_MultiTexCoord0.st;
	float radius = 1.0 - vST.t;
	float theta = F_2_PI * vST.s;
	vec4 disc = vec4( radius * cos( theta ), radius * sin( theta ), 0., 1. );
	vec3 circlenorm = vec3( 0., 0., 1. );

	vST += vec2( uOffsetS, uOffsetT );

	// blend between original and disc
	vec4 vertex = mix( vertex0, disc, uBlend );
	vec3 normal = normalize( mix(norm0, circlenorm, uBlend) );

  	vec3 ECposition = (gl_ModelViewMatrix * vertex).xyz;
	vN = normalize( gl_NormalMatrix * normal );  // normal vector
	vL = LIGHTPOS - ECposition.xyz;	    // vector from the point
										// to the light position
	vE = vec3( 0., 0., 0. ) - ECposition;       // vector from the point to

	vColor = gl_Color.rgb;

    gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
