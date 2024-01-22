#version 330 compatibility

uniform float uKa, uKd, uKs; // ambient, diffuse, specular
uniform vec4  uSpecularColor;
uniform float uShininess;
uniform float uTextureBlend;
uniform sampler2D uTexUnit;

in vec3  vN; // normal vector
in vec3  vL; // point to light vector
in vec3  vE; // point to eye vector
in vec2  vST;

void
main( )
{

    // PER-FRAGMENT LIGHTING
	vec3 Normal = normalize(vN);
	vec3 Light  = normalize(vL);
	vec3 Eye    = normalize(vE);


	vec3 texColor = texture(uTexUnit, vST).rgb;

	vec3 ambient = uKa * texColor;

	float dd = max( dot(Normal,Light), 0. );       // only do diffuse if the light can see the point
	vec3 diffuse = uKd * dd * texColor;

	float ss = 0.;
	if( dot(Normal,Light) > 0. )	      // only do specular if the light can see the point
	{
		vec3 ref = normalize(  reflect( -Light, Normal )  );
		ss = pow( max( dot(Eye,ref), 0. ), uShininess );
	}
	vec3 specular = uKs * ss * uSpecularColor.rgb;

	gl_FragColor = vec4( ambient + diffuse + specular,  1. );
}
