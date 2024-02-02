#version 330 compatibility

//uniform float uOffsetS, uOffsetT, uFrequency;
out vec3 vN; // normal vector
out vec3 vL; // point to light vector
out vec3 vE; // point to eye vector
out vec2 vST;

//vec3 LIGHTPOS = (gl_ModelViewMatrix * vec4( 10., 10., 10., 1.)).xyz; // light position stays relative to object
const vec3 LIGHTPOS = vec3( 0., 0., 10.); // light position

void
main( )
{
	vST = gl_MultiTexCoord0.st;
	vN = normalize( gl_NormalMatrix * gl_Normal );  // normal vector

  	vec3 CCposition = (gl_ModelViewMatrix * gl_Vertex).xyz; // camera coords
	vL = LIGHTPOS - CCposition.xyz;
	vE = vec3( 0., 0., 2. ) - CCposition;

	vec3 vert = gl_Vertex.xyz;

    gl_Position = gl_ModelViewProjectionMatrix * vec4 ( vert, 1.0 );
}
