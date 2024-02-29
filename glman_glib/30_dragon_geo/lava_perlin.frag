#version 330 compatibility

uniform vec4 uColor;
uniform sampler3D u3DNoiseUnit;
uniform float uNoiseFreq, uNoiseAmp;
uniform int uNoiseOctaves;
uniform float uTimeMultiplier;
uniform float Timer;
uniform bool uTurbulence;
uniform bool uOctaveTurbulence;
uniform bool uRidged;

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

vec2 cubic(vec2 p0, vec2 p1, vec2 p2, vec2 p3, vec2 t)
{
	vec2 a = p3 - p2 - p0 + p1;
	vec2 b = p0 - p1 - a;
	vec2 c = p2 - p0;
	vec2 d = p1;

	return a*t*t*t + b*t*t + c*t + d;
}

float
perlin_noise(vec2 uv, float freq, float amp){
	uv = uv * freq;
	vec2 gridId = floor(uv);
	vec2 gridUv = fract(uv);

	//color = vec3(gridId, 0.);
	//color = vec3(gridUv, 0.);

	// grid corners
	vec2 bL = gridId + vec2(0., 0.);
	vec2 bR = gridId + vec2(1., 0.);
	vec2 tL = gridId + vec2(0., 1.);
	vec2 tR = gridId + vec2(1., 1.);

	// random vector for each corner
	vec2 randBL = randVec2(bL);
	vec2 randBR = randVec2(bR);
	vec2 randTL = randVec2(tL);
	vec2 randTR = randVec2(tR);

	// corner to pixel
	vec2 distBL = gridUv - vec2(0., 0.);
	vec2 distBR = gridUv - vec2(1., 0.);
	vec2 distTL = gridUv - vec2(0., 1.);
	vec2 distTR = gridUv - vec2(1., 1.);

	// dot product between random vector and pixel to corner
	float dotBL = dot(randBL, distBL);
	float dotBR = dot(randBR, distBR);
	float dotTL = dot(randTL, distTL);
	float dotTR = dot(randTR, distTR);

	// smooth out gridUv
	gridUv = smoothstep(0., 1., gridUv);
	//gridUv = cubic(vec2(0, 0), vec2(1, 0), vec2(0, 1), vec2(1, 1), gridUv);
	// gridUv = quintic(gridUv);

	// bilinear interpolation
	float bottom = mix(dotBL, dotBR, gridUv.x);
	float top = mix(dotTL, dotTR, gridUv.x);
	float perlin = mix(bottom, top, gridUv.y);

	return perlin * amp;

}


void
main( )
{
	vec3 color = vec3(0);

	for (int i = 0; i < uNoiseOctaves; i++)
	{
		vec2 uv = vST;
		float freq = uNoiseFreq * pow(2., float(i));
		float amp = uNoiseAmp / pow(2., float(i));

		if (uOctaveTurbulence) {
			color += abs(perlin_noise(uv, freq, amp)) * uColor.rgb;
		}
		else {
			color += perlin_noise(uv, freq, amp) * uColor.rgb;
		}
	}

	if (uTurbulence) color = abs(color);
	if (uRidged) {
		vec3 ridged = 1 - abs(color);
		color = ridged;
	}

	gl_FragColor = vec4( color,  1. );
}
