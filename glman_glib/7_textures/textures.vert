#version 330 compatibility

uniform float uShapeBlend;
uniform bool uCube;
out vec3 vN; // normal vector
out vec3 vL; // point to light vector
out vec3 vE; // point to eye vector
out vec2 vST;
out vec3 vColor;

//vec3 LIGHTPOS = (gl_ModelViewMatrix * vec4( 10., 10., 10., 1.)).xyz; // light position
const float F_2_PI = 4.0 * 3.1415926535897932384626433832795;
const vec3 LIGHTPOS = vec3( 10., 10., 10.); // light position

const float RADIUS = 2.0;

void
main( )
{

	// original model coords of sphere;
	vec4 vertex = gl_Vertex;
	vec4 vertex_sphere = gl_Vertex;
	vertex_sphere.xyz *= RADIUS / length(vertex.xyz); // basically just set it to 4.0.
	if (uCube == true) {
		vertex_sphere.xyz = clamp(vertex_sphere.xyz, -RADIUS/2., RADIUS/2.); // clamp to -4.0 to 4.0
	}
	vec3 normal = gl_Normal;
	// don't change the normal to see lighting effects of the original object.

	// do the same for ST
    vST = gl_MultiTexCoord0.st;

	// blend between original and disc
	vertex.xyz = mix( vertex.xyz, vertex_sphere.xyz, uShapeBlend );
  	vec3 ECposition = (gl_ModelViewMatrix * vertex).xyz;
	vN = normalize( gl_NormalMatrix * normal );  // normal vector
	vL = LIGHTPOS - ECposition.xyz;	    // vector from the point
										// to the light position
	vE = vec3( 0., 0., 0. ) - ECposition;       // vector from the point to

	vColor = gl_Color.rgb;

    gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
