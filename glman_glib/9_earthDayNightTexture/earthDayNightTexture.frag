#version 330 compatibility

uniform float uKa, uKd, uKs; // ambient, diffuse, specular
uniform vec4  uSpecularColor;
uniform float uShininess;
uniform float uBlend;
uniform float uSunLng;
uniform float uOffsetS;
uniform sampler2D uTexUnitDay, uTexUnitNight;

in vec3  vN; // normal vector
in vec3  vL; // point to light vector
in vec3  vE; // point to eye vector
in vec2  vST;

void
main( )
{

	float sunlng = uSunLng;
	float sunlngp = sunlng-180.;
	if (sunlngp < -180. ) sunlngp += 360.; // -180. to 180.

	vec2 nightST = vST;
	vec2 dayST = vST + uOffsetS;
	if ( dayST.s < 0.) dayST.s += 1.;
	float earthlng = 360. * (dayST.s - 0.5); // -180. to 180.

	float delta0 = earthlng - sunlng;
	if( delta0 < -180. ) delta0 += 360.;
	if( delta0 >  180. ) delta0 -= 360.;

	float delta1 = earthlng - sunlngp;
	if( delta1 < -180. ) delta1 += 360.;
	if( delta1 >  180. ) delta1 -= 360.;
	float delta = delta0;
	if( abs(delta1) < abs(delta0) ) delta = -delta1;

	vec3 texColorDay = texture(uTexUnitDay, vST).rgb;
	vec3 texColorNight = texture(uTexUnitNight, vST).rgb;

	float t = smoothstep ( -uBlend, uBlend, delta );
	vec3 color = mix(texColorNight, texColorDay, t);

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
