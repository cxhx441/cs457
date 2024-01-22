#version 330 compatibility

//uniform float uOffsetS, uOffsetT, uFrequency;
uniform float uHeightScale;
uniform float uSeaLevel;
uniform sampler2D uTexUnitMoonHeight;
out vec3 vN; // normal vector
out vec3 vL; // point to light vector
out vec3 vE; // point to eye vector
out vec2 vST;

//vec3 LIGHTPOS = (gl_ModelViewMatrix * vec4( 10., 10., 10., 1.)).xyz; // light position
const float F_2_PI = 4.0 * 3.1415926535897932384626433832795;
const vec3 LIGHTPOS = vec3( 10., 10., 10.); // light position

const float MOON_RADIUS = 1727400.; // meters

void
main( )
{


	// original model coords of sphere;
	vec4 vertex = gl_Vertex;
	vec3 normal = gl_Normal;
    //vST = (gl_MultiTexCoord0.st + vec2( uOffsetS, uOffsetT )) * uFrequency;
    vST = gl_MultiTexCoord0.st;

	float moonRad = texture( uTexUnitMoonHeight, vST ).r;
	vertex.xyz += vec3 (moonRad, moonRad, moonRad) * normal * uHeightScale;


  	vec3 ECposition = (gl_ModelViewMatrix * vertex).xyz;
	vN = normalize( gl_NormalMatrix * normal );  // normal vector
	vL = LIGHTPOS - ECposition.xyz;	    // vector from the point
										// to the light position
	vE = vec3( 0., 0., 0. ) - ECposition;       // vector from the point to


    gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
