#version 330 compatibility

out vec3 vN; // normal vector
out vec3 vE; // point to eye vector
out vec3 vMC;

uniform float uAmp, uPeriod, uPhase, uDecay;

const float F_2_PI = 2.0 * 3.14159265;

void
main( )
{
	// sending unmoded vertex
	vMC = gl_Vertex.xyz;

	// displace z
	vec4 newVert = gl_Vertex;
	float r = sqrt(newVert.x * newVert.x + newVert.y * newVert.y);
	float theta = F_2_PI * uPeriod * r + uPhase;
	newVert.z = uAmp * cos(theta) * exp(-uDecay * r);
  	vec4 CCposition = gl_ModelViewMatrix * newVert; // camera coords

	// update normal
	float dzdr = -uAmp * exp(-uDecay * r) * (uDecay * cos(theta) + uPeriod * F_2_PI * sin(theta));
	float drdx = newVert.x / r; // cos(of horizontal?) cos(x)?
	float drdy = newVert.y / r; // sin(of horizontal?) sin(y)?
	float dzdx = dzdr * drdx;
	float dzdy = dzdr * drdy;
	vec3 xtangent = vec3(1, 0, dzdx);
	vec3 ytangent = vec3(0, 1, dzdy);
	vec3 newNormal = normalize(cross(xtangent, ytangent)) ;

	//vMC = vert.xyz;
	//vST = gl_MultiTexCoord0.st;
	vN = normalize( gl_NormalMatrix * newNormal );
	vE = CCposition.xyz - vec3( 0., 0., 4. );

    gl_Position = gl_ModelViewProjectionMatrix * newVert;
}
