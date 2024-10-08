#version 330 compatibility

// uniform float uA;
uniform int uCount;
uniform float uTol;

in vec2  vST;
in vec3  vColor;
in float vLightIntensity;

float uDiam = 1. / uCount;

const vec3 WHITE = vec3( 1., 1., 1. );


//circle_equation = (s-s0)^2 + (t-t0)^2 = r^2

void
main( )
{
    float vS = vST.s*2;
    float vT = vST.t;
    // get which part of the grid your on,
    int grid_s = int( vS / uDiam );
    int grid_t = int( vT / uDiam );
    // get center of circle in that grid
    float r = uDiam / 2.;
    float s_center = float( grid_s ) * uDiam + r;
    float t_center = float( grid_t ) * uDiam + r;
    // then check if you're in the circle
    float r_to_center = pow((vS - s_center) / r, 2) + pow((vT - t_center) / r, 2);
    // r < 1 is inside
    // r == 1 is on edge
    // r > 1 is outside

    float t = smoothstep(1. - uTol, 1. + uTol, r_to_center);

     vec3 rgb = vLightIntensity * mix( WHITE, vColor, t );

    gl_FragColor = vec4( rgb, 1. );
}
