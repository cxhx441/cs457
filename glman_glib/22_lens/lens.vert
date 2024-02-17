#version 330 compatibility

out vec3 vRefractVector;
uniform float uR1, uR2;
uniform bool uSymmetric;

const float ETA = 0.66; // 1./ index refraction of the glass
const vec3 EYE = vec3(0.);

void
main( )
{
	float r1 = uR1;
	float r2 = uR2;
	if (uSymmetric) r2 = r1;

	vec3 P = vec3(gl_ModelViewMatrix * gl_Vertex);
	vec3 EyeToPoint = normalize(P - EYE);

	vec3 Center1 = vec3(0., 0., P.z - r1);
	vec3 Normal1;
	if (r1 >=0. ) Normal1 = normalize(P - Center1);
	else Normal1 = normalize(Center1 - P);

	vec3 Center2 = vec3(0., 0., P.z + r2);
	vec3 Normal2;
	if (r2 >= 0. ) Normal2 = normalize(Center2 - P);
	else Normal2 = normalize(P - Center2);

	vec3 v1 = refract(EyeToPoint, Normal1, ETA); //eta = in/out
	v1 = normalize(v1);

	vRefractVector = refract(v1, Normal2, 1./ETA); //eta = in/out

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
