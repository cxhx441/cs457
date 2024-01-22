#version 330 compatibility

uniform float uKa, uKd, uKs; // ambient, diffuse, specular
uniform vec4  uSpecularColor;
uniform float uShininess;
uniform sampler2D uTexUnitMoonColor ;

in vec3  vN; // normal vector
in vec3  vL; // point to light vector
in vec3  vE; // point to eye vector
in vec2  vST;

void
main( )
{

	vec3 color = texture( uTexUnitMoonColor, vST ).rgb;

    // PER-FRAGMENT LIGHTING
	vec3 Normal = normalize(vN);
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
	vec3 specular = uKs * ss * uSpecularColor.rgb;

	gl_FragColor = vec4( ambient + diffuse + specular,  1. );
}
