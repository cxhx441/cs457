#version 330 compatibility

out vec3 vN; // normal vector
out vec3 vL; // point to light vector
out vec3 vE; // point to eye vector
out vec2 vST;
out vec3 vMCposition;

//vec3 LIGHTPOS = (gl_ModelViewMatrix * vec4( 10., 10., 10., 1.)).xyz; // light position
const vec3 LIGHTPOS = vec3( 10., 10., 10.); // light position

void
main( )
{

    vST = gl_MultiTexCoord0.st;
	vMCposition = gl_Vertex.xyz;

  	vec3 ECposition = (gl_ModelViewMatrix * gl_Vertex).xyz;
	vN = normalize( gl_NormalMatrix * gl_Normal );  // normal vector
	vL = LIGHTPOS - ECposition.xyz;	    // vector from the point
										// to the light position
	vE = vec3( 0., 0., 0. ) - ECposition;       // vector from the point to

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; // TODO is this even used?
}
