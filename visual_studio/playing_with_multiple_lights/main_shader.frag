#version 330 compatibility

uniform float uKa, uKd, uKs; // ambient, diffuse, specular
uniform vec3  uColor;
uniform vec3  uSpecularColor;
uniform float uShininess;
uniform float uAd, uBd; // oval diameters
uniform float uTol;

in vec3  vN; // normal vector
in vec3  vL1; // point to light vector
in vec3  vL2; // point to light vector
in vec3  vL3; // point to light vector
in vec3  vE; // point to eye vector
in vec2  vST;

const vec3 WHITE = vec3( 1., 1., 1. );

vec3 
ToneMapReinard(vec3 color){
	return color / (vec3(1., 1., 1.) + color);
}

vec3
perFragLight(vec3 normal, vec3 light, vec3 eye, vec3 rgb){
	vec3 n = normalize(normal);
	vec3 l = normalize(light);
	vec3 e = normalize(eye);

	// diffuse
	float dd = max( dot(n,l), 0. );       // only do diffuse if the light can see the point
	vec3 diffuse = uKd * dd * rgb;

	// specular
	float ss = 0.;
	if( dot(n,l) > 0. )	      // only do specular if the light can see the point
	{
		vec3 ref = normalize(  reflect( -l, n )  );
		ss = pow( max( dot(e,ref),0. ), uShininess );
	}
	vec3 specular = uKs * ss * uSpecularColor;

	return diffuse + specular;
}

void
main( )
{
    //float s = vST.s*2; // distortion correction for sphere.
    //float t = vST.t;
    // get which part of the grid your on,
    //int grid_s = int( s / uAd );
    //int grid_t = int( t / uBd );
    // get center of circle in that grid
    //float rA = uAd / 2.;
    //float rB = uBd / 2.;
    //float s_center = float( grid_s ) * uAd + rA;
    //float t_center = float( grid_t ) * uBd + rB;
    // then check if you're in the circle
    //float r_to_center = pow((s - s_center) / rA, 2) + pow((t - t_center) / rB, 2);
        // r < 1 is inside
        // r == 1 is on edge
        // r > 1 is outside

    //float smstep = smoothstep(1. - uTol, 1. + uTol, r_to_center);

    //vec3 rgb = mix( WHITE, uColor, smstep );

    vec3 rgb = uColor;
	vec3 ambient = uKa * rgb;

    // PER-FRAGMENT LIGHTING
	//vec3 Normal = normalize(vN);
	//vec3 Light  = normalize(L);
	//vec3 Eye    = normalize(vE);

	//vec3 ambient = uKa * rgb_prelight;

	//float dd = max( dot(Normal,Light), 0. );       // only do diffuse if the light can see the point
	//vec3 diffuse = uKd * dd * rgb_prelight;

	//float ss = 0.;
	//if( dot(Normal,Light) > 0. )	      // only do specular if the light can see the point
	//{
		//vec3 ref = normalize(  reflect( -Light, Normal )  );
		//ss = pow( max( dot(Eye,ref),0. ), uShininess );
	//}
	//vec3 specular = uKs * ss * uSpecularColor;

	vec3 rgb_l1 = perFragLight(vN, vL1, vE, rgb);
	vec3 rgb_l2 = perFragLight(vN, vL2, vE, rgb);
	vec3 rgb_l3 = perFragLight(vN, vL3, vE, rgb);

	rgb = rgb_l1 + rgb_l2 + rgb_l3 + ambient;
	rgb = ToneMapReinard(rgb);

	// ambient


	//rgb = max(rgb, rgb_l3);
	//gl_FragColor = vec4( ambient + diffuse + specular,  1. );
	gl_FragColor = vec4( rgb,  1. );
}


