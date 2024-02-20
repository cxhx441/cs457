#version 330 compatibility

uniform float uA;
uniform float uP;
uniform float uTol;

in float vX, vY;
in vec3  vColor;
in float vLightIntensity;

const vec3 WHITE = vec3( 1., 1., 1. );

void
main( )
{
    float f = fract( uA*vX );

    float left_edge = 0.5 - uP;
    float right_edge = 0.5 + uP;
    //creates sort of pulse acceptance function
    float t = smoothstep( left_edge-uTol, left_edge+uTol, f ) - \
              smoothstep( right_edge-uTol, right_edge+uTol, f );

    vec3 rgb = vLightIntensity * mix( WHITE, vColor, t );

    gl_FragColor = vec4( rgb, 1. );
}
