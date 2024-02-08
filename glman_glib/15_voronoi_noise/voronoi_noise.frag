#version 330 compatibility

uniform float uKa, uKd, uKs; // ambient, diffuse, specular
uniform float uShininess;
uniform vec4 uColor;
uniform sampler3D u3DNoiseUnit;
uniform float uNoiseFreq, uNoiseAmp;
uniform int uNoiseOctaves;
uniform float uTimeMultiplier;
uniform float Timer;
uniform bool uSmooth, uAnimate, uClouds;

in vec3  vN; // normal vector
in vec3  vL; // point to light vector
in vec3  vE; // point to eye vector
in vec2  vST;
//in vec3 vMC;

const vec4 WHITE = vec4 (1.);

vec2 randVec2( vec2 gridCorner )
{
	float x = dot( gridCorner, vec2(123.4, 234.5) );
	float y = dot( gridCorner, vec2(234.5, 345.6) );
	vec2 gradient = vec2( x, y );
	gradient = sin(gradient);
	gradient *= 143758.f ;
	gradient = sin(gradient + Timer*uTimeMultiplier);
	return gradient;
}


float
voronoi_noise(vec2 uv, float freq, float amp){
	uv = uv * freq;
	vec2 gridId = floor(uv);
	vec2 gridUv = fract(uv);

	gridUv -= 0.5; // center gridUv

	// get adj grid ids and min dist:
	float minDist = 1000.;
	for (float i = -1; i <= 1; i++) {
		for (float j = -1; j <= 1; j++) {
			vec2 adjGridId = vec2(i, j);
			vec2 noise = randVec2(gridId + adjGridId);
			if (uAnimate) adjGridId += noise * 0.5;
			float dist = length(gridUv - adjGridId);
			minDist = min(minDist, dist);
		}
	}

	// // smooth out gridUv
	if (uClouds) minDist = 1. - minDist;
	if (uSmooth) minDist = smoothstep(0., 1., minDist);
	// //gridUv = cubic(vec2(0, 0), vec2(1, 0), vec2(0, 1), vec2(1, 1), gridUv);
	// // gridUv = quintic(gridUv);

	return minDist * amp;

}


void
main( )
{
	vec3 color = uColor.rgb;

	for (int i = 0; i < uNoiseOctaves; i++)
	{
		vec2 uv = vST;
		float freq = uNoiseFreq * pow(2., float(i));
		float amp = uNoiseAmp / pow(2., float(i));

		color += voronoi_noise(uv, freq, amp);
	}

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
	vec3 specular = uKs * ss * WHITE.rgb;

	gl_FragColor = vec4( ambient + diffuse + specular,  1. );
}
