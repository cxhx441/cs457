#version 330 compatibility

uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjectionMat;
uniform mat3 uNormalMat;

out vec3 vN; // normal vector
out vec3 vE; // point to eye vector
out vec3 vMC;
out vec4 vColor;

void
main( )
{
	///*
	vColor = gl_Color;
	vec4 newVertex = gl_Vertex;
	vMC = newVertex.xyz;
  	vec3 CCposition = (gl_ModelViewMatrix * newVertex).xyz; // camera coords
	vE = CCposition - vec3( 0., 0., 0. );
	vN = normalize(gl_NormalMatrix * gl_Normal);
    gl_Position = gl_ModelViewProjectionMatrix * newVertex;
	//*/

	/*
	vColor = gl_Color;
	vec4 newVertex = gl_Vertex;
	//vMC = newVertex.xyz;
  	vec3 CCposition = (uViewMat * uModelMat * newVertex).xyz; // camera coords
	vE = CCposition - vec3( 0., 0., 0. );
	vN = normalize(uNormalMat * gl_Normal);
	//vN = normalize(transpose(inverse(mat3(model))) * gl_Normal);
    gl_Position = UProjectionMat * uViewMat * uModelMat * gl_Vertex;
	*/
}
