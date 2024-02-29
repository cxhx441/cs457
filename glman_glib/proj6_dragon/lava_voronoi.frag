#version 330 compatibility

uniform float uNoiseFreq, uNoiseAmp;
uniform int uNoiseOctaves;
uniform float uTimeMultiplier;
uniform float Timer;
uniform bool uSmooth, uAnimate, uClouds;
uniform vec3 uColor;

in vec2 vST;

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

	return minDist * amp;

}


void
main( )
{
	vec3 color = vec3(0.);

	for (int i = 0; i < uNoiseOctaves; i++)
	{
		vec2 uv = vST;
		float freq = uNoiseFreq * pow(2., float(i));
		float amp = uNoiseAmp / pow(2., float(i));

		color += voronoi_noise(uv, freq, amp) * vec3(1, .37, 0);
	}
	gl_FragColor = vec4( color,  1. );
}
