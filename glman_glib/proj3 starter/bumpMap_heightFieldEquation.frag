#version 330 compatibility

uniform float uKa, uKd, uKs; // ambient, diffuse, specular
uniform float uShininess;
uniform vec4 uBaseColor;
uniform float uPhase;
uniform float uFreq;
uniform float uAmp;
uniform float uDecay;
uniform float uOffsetS, uOffsetT;

in vec3  vN; // normal vector
in vec3  vL; // point to light vector
in vec3  vE; // point to eye vector
in vec2  vST;

//const float DELTA = 0.01;
const vec4 WHITE = vec4 (1.);
const float F_2_PI = 2.0 * 3.1415926535897932384626433832795;

void
main( )
{
	vec3 color = uBaseColor.rgb;
	vec3 Normal = normalize(vN);
	float x = vST.s - 0.5 + uOffsetS;
	float y = vST.t - 0.5 + uOffsetT;

	//z = A * Cos(2pi * B * r + C) * e^(-D*r)
	//dz/dr = -A e^(-D r) (D cos(C + 2 B π r) + 2 B π sin(C + 2 B π r))
	float r = sqrt(x * x + y * y);
	//float r = length(vMC.xy - vec2(uOffsetS, uOffsetT));
	float theta = uPhase + uFreq * F_2_PI * r;
	float dzdr = -uAmp * exp(-uDecay * r) * (uDecay * cos(theta) + uFreq * F_2_PI * sin(theta));
	//float dzdr = uAmp * ( -sin(F_2_PI*uFreq*r+uPhase) * F_2_PI*uFreq * exp(-uDecay*r) + cos(F_2_PI*uFreq*r+uPhase) * -uDecay * exp(-uDecay*r));
	float drdx = x / r; // cos(of horizontal?)
	float drdy = y / r; // sin(of horizontal?)
	float dzdx = dzdr * drdx;
	float dzdy = dzdr * drdy;
	vec3 xtangent = vec3(1, 0, dzdx);
	vec3 ytangent = vec3(0, 1, dzdy);
	Normal = normalize(cross(xtangent, ytangent));
	Normal = normalize(vN);

    // PER-FRAGMENT LIGHTING
	vec3 Light  = normalize(vL);
	vec3 Eye    = normalize(vE);

	vec3 ambient = uKa * color;

	float dd = max( dot(Normal,Light), 0. );       // only do diffuse if the light can see the point
	vec3 diffuse = uKd * dd * color;

	float ss = 0.;
	if( dot(Normal,Light) > 0. )	      // only do specular if the light can see the point
	{
		vec3 ref = normalize(  reflect( -Light, Normal )  );
		ss = pow( max( dot(Eye,ref), 0. ), uShininess );
	}
	vec3 specular = uKs * ss * WHITE.rgb;

	gl_FragColor = vec4( ambient + diffuse + specular,  1. );
}
