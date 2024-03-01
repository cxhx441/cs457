#version 330 compatibility

uniform mat4 uM;
uniform mat4 uV;
uniform mat4 uP;
uniform mat3 uN;
uniform mat4 uMV;
uniform mat4 uMVP;

out vec3 vN; // normal vector
out vec3 vE; // point to eye vector
out vec3 vMC;
out vec4 vColor;

void
main( )
{
	/*
	vColor = gl_Color;
	vec4 newVertex = gl_Vertex;
	vMC = newVertex.xyz;
  	vec3 CCposition = (gl_ModelViewMatrix * newVertex).xyz; // camera coords
	vE = CCposition - vec3( 0., 0., 0. );
	vN = normalize(gl_NormalMatrix * gl_Normal);
    gl_Position = gl_ModelViewProjectionMatrix * newVertex;
	//*/

	///*
	vColor = gl_Color;
	vec4 newVertex = gl_Vertex;
	//vMC = newVertex.xyz;
  	vec3 CCposition = (uMV * newVertex).xyz; // camera coords
	vE = CCposition - vec3( 0., 0., 0. );
	vN = normalize(uN * gl_Normal);
	//vN = normalize(transpose(inverse(mat3(model))) * gl_Normal);
    gl_Position = uMVP * gl_Vertex;
	//*/
}
