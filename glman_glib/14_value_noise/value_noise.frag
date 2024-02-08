#version 330 compatibility

uniform float uKa, uKd, uKs; // ambient, diffuse, specular
uniform float uShininess;
uniform vec4 uColor;
uniform sampler3D u3DNoiseUnit;
uniform float uNoiseFreq, uNoiseAmp;
uniform int uNoiseOctaves;
uniform float uTimeMultiplier;
uniform float Timer;

in vec3  vN; // normal vector
in vec3  vL; // point to light vector
in vec3  vE; // point to eye vector
in vec2  vST;
//in vec3 vMC;

const vec4 WHITE = vec4 (1.);

float whiteNoise( vec2 point )
{
	float random = dot( point, vec2(123.4, 234.5) );
	random = sin(random);
	random *= 143758.f ;
	random = sin(random + Timer*uTimeMultiplier);
	random = fract(random);
	return random;
}

float
value_noise(vec2 uv, float freq, float amp){
	uv = uv * freq;
	vec2 gridId = floor(uv);
	vec2 gridUv = fract(uv);

	// grid corners
	vec2 bL = gridId + vec2(0., 0.);
	vec2 bR = gridId + vec2(1., 0.);
	vec2 tL = gridId + vec2(0., 1.);
	vec2 tR = gridId + vec2(1., 1.);

	// random vector for each corner
	float randBL = whiteNoise(bL);
	float randBR = whiteNoise(bR);
	float randTL = whiteNoise(tL);
	float randTR = whiteNoise(tR);

	// smooth out gridUv
	gridUv = smoothstep(0., 1., gridUv);
	//gridUv = cubic(vec2(0, 0), vec2(1, 0), vec2(0, 1), vec2(1, 1), gridUv);
	// gridUv = quintic(gridUv);

	// bilinear interpolation
	float bottom = mix(randBL, randBR, gridUv.x);
	float top = mix(randTL, randTR, gridUv.x);
	float valueNoise = mix(bottom, top, gridUv.y);

	return valueNoise * amp;

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

		color += value_noise(uv, freq, amp);
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
