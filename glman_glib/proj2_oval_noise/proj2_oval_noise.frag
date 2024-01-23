#version 330 compatibility

uniform float uKa, uKd, uKs; // ambient, diffuse, specular
uniform vec4  uSphereColor;
uniform vec4  uOvalColor;
uniform vec4  uSpecularColor;
uniform float uShininess;
uniform float uAd, uBd; // oval diameters
uniform float uTol;
uniform sampler3D uNoiseTexUnit3;
uniform float uNoiseAmp; // amplitude of sine wave
uniform float uNoiseFreq; // frequency of sine wave
uniform bool uUseXYZForNoise; // frequency of sine wave

in vec3  vN; // normal vector
in vec3  vL; // point to light vector
in vec3  vE; // point to eye vector
in vec2  vST;
in vec3  vMCposition;

void
main( )
{
    float s = vST.s*2; // distortion correction for sphere.
    float t = vST.t;

    // get noise value
    vec4 nv;
    if (uUseXYZForNoise) {
        nv = texture( uNoiseTexUnit3, uNoiseFreq * vMCposition );  // 1-3
    }
    else {
        nv = texture( uNoiseTexUnit3, uNoiseFreq * vec3(s, t, 0) );  // 1-3
    }
    float n = nv.r + nv.g + nv.b + nv.a; // 1 to 3
    n -= 2; // -1 to 1
    n *= uNoiseAmp; // -uNoiseAmp to uNoiseAmp

    // get correct oval shapes
    // get which part of the grid your on,
    int grid_s = int( s / uAd );
    int grid_t = int( t / uBd );
    // get center of circle in that grid
    float rA = uAd / 2.;
    float rB = uBd / 2.;
    float sc = float( grid_s ) * uAd + rA;
    float tc = float( grid_t ) * uBd + rB;
    float dt = t - tc;
    float ds = s - sc;
    // then check if you're in the circle
    float len = sqrt( ds*ds + dt*dt );
    float newLen = len + n;
    float scale = newLen / len;

    ds *= scale;
    dt *= scale;
    float r_to_center = pow(ds / rA, 2) + pow(dt / rB, 2);
        // r < 1 is inside
        // r == 1 is on edge
        // r > 1 is outside

    // modify correct oval shapes with noise

    float smstep = smoothstep(1. - uTol, 1. + uTol, r_to_center);

    vec3 rgb_prelight = mix( uOvalColor.rgb, uSphereColor.rgb, smstep );

    // PER-FRAGMENT LIGHTING
	vec3 Normal = normalize(vN);
	vec3 Light  = normalize(vL);
	vec3 Eye    = normalize(vE);

	vec3 ambient = uKa * rgb_prelight;

	float dd = max( dot(Normal,Light), 0. );       // only do diffuse if the light can see the point
	vec3 diffuse = uKd * dd * rgb_prelight;

	float ss = 0.;
	if( dot(Normal,Light) > 0. )	      // only do specular if the light can see the point
	{
		vec3 ref = normalize(  reflect( -Light, Normal )  );
		ss = pow( max( dot(Eye,ref),0. ), uShininess );
	}
	vec3 specular = uKs * ss * uSpecularColor.rgb;

	gl_FragColor = vec4( ambient + diffuse + specular,  1. );
}
