#version 330 compatibility

out vec3 vN; // normal vector
out vec3 vL; // point to light vector
out vec3 vE; // point to eye vector
out vec3 vMC;

uniform float uLightX, uLightY, uLightZ;
uniform float uAmp, uPeriod, uPhase, uDecay;

const float F_2_PI = 2.0 * 3.14159265;
//vec3 LIGHTPOS = (gl_ModelViewMatrix * vec4( 10., 10., 10., 1.)).xyz; // light position stays relative to object
//const vec3 LIGHTPOS = vec3( 0., 0., 10.); // light position

void
main( )
{
	vec3 vert = gl_Vertex.xyz;
	// displace z
	float r = sqrt(vert.x * vert.x + vert.y * vert.y);
	float theta = F_2_PI * uPeriod * r + uPhase;
	vert.z = uAmp * cos(theta) * exp(-uDecay * r);

	// update normal
	float dzdr = -uAmp * exp(-uDecay * r) * (uDecay * cos(theta) + uPeriod * F_2_PI * sin(theta));
	//float dzdr = uAmp * ( -sin(F_2_PI*uPeriod*r+uPhase) * F_2_PI*uPeriod * exp(-uDecay*r) + cos(F_2_PI*uPeriod*r+uPhase) * -uDecay * exp(-uDecay*r));
	float drdx = vert.x / r; // cos(of horizontal?) cos(x)?
	float drdy = vert.y / r; // sin(of horizontal?) sin(y)?
	float dzdx = dzdr * drdx;
	float dzdy = dzdr * drdy;
	vec3 xtangent = vec3(1, 0, dzdx);
	vec3 ytangent = vec3(0, 1, dzdy);
	vec3 normal = normalize(cross(xtangent, ytangent)) ;

	vMC = vert.xyz;
	//vST = gl_MultiTexCoord0.st;
	vN = normalize( gl_NormalMatrix * normal );
  	vec3 CCposition = (gl_ModelViewMatrix * gl_Vertex).xyz; // camera coords
	vL = vec3(uLightX, uLightY, uLightZ) - CCposition.xyz;
	vE = vec3( 0., 0., 8. ) - CCposition;


    gl_Position = gl_ModelViewProjectionMatrix * vec4 ( vert, 1.0 );
}
