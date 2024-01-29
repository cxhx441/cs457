#version 330 compatibility

uniform float uKa, uKd, uKs; // ambient, diffuse, specular
uniform float uShininess;
uniform vec4 uBaseColor;


in vec3  vN; // normal vector
in vec3  vL; // point to light vector
in vec3  vE; // point to eye vector
in vec2  vST;

const float DELTA = 0.01;
const vec4 WHITE = vec4 (1.);

void
main( )
{
	vec3 color = uBaseColor.rgb;
	vec3 Normal = normalize(vN);

	// if ( uDoLighting )
	// {
	// 	vec2 stp0     = vec2( DELTA, 0. );
	// 	vec2 st0p     = vec2( 0. , DELTA );
	// 	float west     = texture2D( uDisplaceUnit, vST-stp0 ).r;
	// 	float east     = texture2D( uDisplaceUnit, vST+stp0 ).r;
	// 	float south    = texture2D( uDisplaceUnit, vST-st0p ).r;
	// 	float north    = texture2D( uDisplaceUnit, vST+st0p ).r;
	// 	vec3 stangent = vec3( 2.*DELTA, 0., uNormalScale * ( east - west ) );
	// 	vec3 ttangent = vec3( 0., 2.*DELTA, uNormalScale * ( north - south ) );
	// 	Normal        = normalize( cross( stangent, ttangent ) );
	// }

    // PER-FRAGMENT LIGHTING
	//vec3 Normal = normalize(vN);
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
