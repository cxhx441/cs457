#version 330 compatibility

uniform sampler2D uImageUnit;
uniform float uBlend;
uniform bool uHorzEdgeBool, uVertEdgeBool;
uniform float uEdgeMagTol;
uniform float uQuantize;
uniform bool uOG;

in vec2 vST;

const vec3 LUMINANCE_COEFFS = vec3( 0.2125,0.7154,0.0721 );
const float PI = 3.14159265359;

float
atan2(float y, float x)
{
    if ( x == 0. )
    {
        if ( y >= 0. ) return PI/2.;
        else return -PI/2.;
    }
    return atan(y, x);
}

vec3
quantize( vec3 rgb, float q )
{
    rgb *= q; // scale up
    rgb += vec3( .5, .5, .5 ); // round
    ivec3 irgb = ivec3( rgb ); // cast to all integers
    rgb = vec3( irgb ); // cast back to floats
    rgb /= q; // scale down
    return rgb;
}

void main()
{
    vec4 rgb_og = texture(uImageUnit, vST);
    vec3 rgb = texture(uImageUnit, vST).xyz;

    // EDGE DETECTION
    // horiz and vert sobel convulutions
    //[-1 -2 -1]
    //[ 0  0  0]
    //[ 1  2  1] horz
    //
    //[-1  0  1]
    //[-2  0  2]
    //[-1  0  1] // vert
    int ResS = textureSize(uImageUnit, 0).x;
    int ResT = textureSize(uImageUnit, 0).y;
    vec2 stp0 = vec2(1./ResS,  0. );
    vec2 st0p = vec2(0.     ,  1./ResT);
    vec2 stpp = vec2(1./ResS, 1./ResT);
    vec2 stpm = vec2(1./ResS, -1./ResT);

    float i00 = dot( texture( uImageUnit, vST ).rgb , LUMINANCE_COEFFS );
    float im1m1 = dot( texture( uImageUnit, vST-stpp ).rgb, LUMINANCE_COEFFS );
    float ip1p1 = dot( texture( uImageUnit, vST+stpp ).rgb, LUMINANCE_COEFFS );
    float im1p1 = dot( texture( uImageUnit, vST-stpm ).rgb, LUMINANCE_COEFFS );
    float ip1m1 = dot( texture( uImageUnit, vST+stpm ).rgb, LUMINANCE_COEFFS );
    float im10  =  dot( texture( uImageUnit, vST-stp0 ).rgb,   LUMINANCE_COEFFS );
    float ip10   =  dot( texture( uImageUnit, vST+stp0 ).rgb,  LUMINANCE_COEFFS );
    float i0m1  =  dot( texture( uImageUnit, vST-st0p ).rgb,   LUMINANCE_COEFFS );
    float i0p1   =  dot( texture( uImageUnit, vST+st0p ).rgb,  LUMINANCE_COEFFS );
    float h = -1.*im1p1 - 2.*i0p1 - 1.*ip1p1 + 1.*im1m1 + 2.*i0m1 + 1.*ip1m1;
    float v = -1.*im1m1 - 2.*im10 - 1.*im1p1 + 1.*ip1m1 + 2.*ip10 + 1.*ip1p1;
    if (!uHorzEdgeBool) h = 0;
    if (!uVertEdgeBool) v = 0;
    float mag = sqrt( h*h + v*v );
    float ang = atan2(v, h); // useful for something??
    vec3 target = vec3( mag,mag,mag );
    rgb = mix( rgb, target, uBlend );

    //from pdf
    if( mag > uEdgeMagTol ) {
        gl_FragColor= vec4( 0., 0., 0., 1. );
    }
    else {
        rgb *= uQuantize; // scale up
        rgb += vec3( .5, .5, .5 ); // round
        ivec3 irgb = ivec3( rgb ); // cast to all integers
        rgb = vec3( irgb ); // cast back to floats
        rgb /= uQuantize; // scale down
        gl_FragColor= vec4( rgb, 1. );
    }

    if (uOG) gl_FragColor = rgb_og;
}
