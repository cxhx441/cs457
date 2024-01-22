#version 330 compatibility

uniform float uKa, uKd; // ambient, diffuse, specular
uniform vec4  uSpecularColor;
uniform float uShininess;
uniform float uNormalScale;
uniform bool  uDoLighting;
uniform sampler2D uColorUnit, uDispUnit;


in vec3  vN; // normal vector
in vec3  vL; // point to light vector
in vec3  vE; // point to eye vector
in vec2  vST;

const float DELTA = 0.01;

void
main( )
{
	vec3 color = texture( uColorUnit, vST ).rgb;
	vec3 Normal = normalize(vN);

	if ( uDoLighting )
	{
		vec2  stp0     = vec2( DELTA, 0. );
		vec2  st0p     = vec2( 0. , DELTA );
		float west     = texture2D( uDispUnit, vST-stp0 ).r;
		float east     = texture2D( uDispUnit, vST+stp0 ).r;
		float south    = texture2D( uDispUnit, vST-st0p ).r;
		float north    = texture2D( uDispUnit, vST+st0p ).r;
		vec3  stangent = vec3( 2.*DELTA, 0., uNormalScale * ( east - west ) );
		vec3  ttangent = vec3( 0., 2.*DELTA, uNormalScale * ( north - south ) );
		Normal         = normalize( cross( stangent, ttangent ) );
	}

    // PER-FRAGMENT LIGHTING
	//vec3 Normal = normalize(vN);
	vec3 Light  = normalize(vL);
	vec3 Eye    = normalize(vE);

	vec3 ambient = uKa * color;

	float dd = max( dot(Normal,Light), 0. );       // only do diffuse if the light can see the point
	vec3 diffuse = uKd * dd * color;

	gl_FragColor = vec4( ambient + diffuse,  1. );
}
