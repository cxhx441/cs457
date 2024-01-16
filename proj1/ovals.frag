#version 330 compatibility

uniform float uAd, uBd; // oval diameters
uniform float uTol;

in vec2  vST;
in vec3  vColor;
in float vLightIntensity;

const vec3 WHITE = vec3( 1., 1., 1. );

void
main( )
{
    float vS = vST.s*2; // distortion correction for sphere.
    float vT = vST.t;
    // get which part of the grid your on,
    int grid_s = int( vS / uAd );
    int grid_t = int( vT / uBd );
    // get center of circle in that grid
    float rA = uAd / 2.;
    float rB = uBd / 2.;
    float s_center = float( grid_s ) * uAd + rA;
    float t_center = float( grid_t ) * uBd + rB;
    // then check if you're in the circle
    float r_to_center = pow((vS - s_center) / rA, 2) + pow((vT - t_center) / rB, 2);
        // r < 1 is inside
        // r == 1 is on edge
        // r > 1 is outside

    float t = smoothstep(1. - uTol, 1. + uTol, r_to_center);

    vec3 rgb = vLightIntensity * mix( WHITE, vColor, t );

    gl_FragColor = vec4( rgb, 1. );
}
