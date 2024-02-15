#version 330 compatibility

uniform float uBlurMix;
uniform sampler2D uImg1Unit;

in vec2 vST;

const vec4 BLACK = vec4( 0., 0., 0., 1. );
const float PI = 3.14159265;

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

void main()
{
    ivec2 ires = textureSize(uImg1Unit, 0);
    float ResS = float(ires.s);
    float ResT = float(ires.t);

    float deltaS = 1./ResS;
    float deltaT = 1./ResT;

    vec3 rgb = texture(uImg1Unit, vST).rgb;

    //3x3 blur convolution kernel
    // 1/16 * [1 2 1]
    //        [2 4 2]
    //        [1 2 1]

    vec3 up_left =    1 * texture(uImg1Unit, vST+vec2(-deltaS, deltaT)).rgb;
    vec3 up =         2 * texture(uImg1Unit, vST+vec2( 0,      deltaT)).rgb;
    vec3 up_right =   1 * texture(uImg1Unit, vST+vec2( deltaS, deltaT)).rgb;

    vec3 left =       2 * texture(uImg1Unit, vST+vec2(-deltaS, 0)).rgb;
    vec3 center =     4 * texture(uImg1Unit, vST+vec2( 0,      0)).rgb;
    vec3 right =      2 * texture(uImg1Unit, vST+vec2( deltaS, 0)).rgb;

    vec3 down_left =  1 * texture(uImg1Unit, vST+vec2(-deltaS, -deltaT)).rgb;
    vec3 down =       2 * texture(uImg1Unit, vST+vec2( 0,      -deltaT)).rgb;
    vec3 down_right = 1 * texture(uImg1Unit, vST+vec2( deltaS, -deltaT)).rgb;

    vec3 blur_3x3 = up_left    + \
                up         + \
                up_right   + \
                left       + \
                center     + \
                right      + \
                down_left  + \
                down       + \
                down_right;

    blur_3x3 /= 16.;

    // 5x5 blur convultion kernel
    // 1/100 * [1 2 4  2 1]
    //         [2 4 8  4 2]
    //         [4 8 16 8 4]
    //         [1 2 4  2 1]
    //         [2 4 8  4 2]
    vec3 r0c0 = 1 * texture(uImg1Unit, vST + vec2(-2*deltaS,  2*deltaT)).rgb;
    vec3 r1c0 = 2 * texture(uImg1Unit, vST + vec2(-2*deltaS,  1*deltaT)).rgb;
    vec3 r2c0 = 4 * texture(uImg1Unit, vST + vec2(-2*deltaS,  0*deltaT)).rgb;
    vec3 r3c0 = 2 * texture(uImg1Unit, vST + vec2(-2*deltaS, -1*deltaT)).rgb;
    vec3 r4c0 = 1 * texture(uImg1Unit, vST + vec2(-2*deltaS, -2*deltaT)).rgb;

    vec3 r0c1 = 2 * texture(uImg1Unit, vST + vec2(-1*deltaS,  2*deltaT)).rgb;
    vec3 r1c1 = 4 * texture(uImg1Unit, vST + vec2(-1*deltaS,  1*deltaT)).rgb;
    vec3 r2c1 = 8 * texture(uImg1Unit, vST + vec2(-1*deltaS,  0*deltaT)).rgb;
    vec3 r3c1 = 4 * texture(uImg1Unit, vST + vec2(-1*deltaS, -1*deltaT)).rgb;
    vec3 r4c1 = 2 * texture(uImg1Unit, vST + vec2(-1*deltaS, -2*deltaT)).rgb;

    vec3 r0c2 = 4 * texture(uImg1Unit, vST + vec2( 0*deltaS,  2*deltaT)).rgb;
    vec3 r1c2 = 8 * texture(uImg1Unit, vST + vec2( 0*deltaS,  1*deltaT)).rgb;
    vec3 r2c2 = 16 * texture(uImg1Unit, vST + vec2(0*deltaS,  0*deltaT)).rgb;
    vec3 r3c2 = 8 * texture(uImg1Unit, vST + vec2( 0*deltaS, -1*deltaT)).rgb;
    vec3 r4c2 = 4 * texture(uImg1Unit, vST + vec2( 0*deltaS, -2*deltaT)).rgb;

    vec3 r0c3 = 2 * texture(uImg1Unit, vST + vec2( 1*deltaS,  2*deltaT)).rgb;
    vec3 r1c3 = 4 * texture(uImg1Unit, vST + vec2( 1*deltaS,  1*deltaT)).rgb;
    vec3 r2c3 = 8 * texture(uImg1Unit, vST + vec2( 1*deltaS,  0*deltaT)).rgb;
    vec3 r3c3 = 4 * texture(uImg1Unit, vST + vec2( 1*deltaS, -1*deltaT)).rgb;
    vec3 r4c3 = 2 * texture(uImg1Unit, vST + vec2( 1*deltaS, -2*deltaT)).rgb;

    vec3 r0c4 = 1 * texture(uImg1Unit, vST + vec2( 2*deltaS,  2*deltaT)).rgb;
    vec3 r1c4 = 2 * texture(uImg1Unit, vST + vec2( 2*deltaS,  1*deltaT)).rgb;
    vec3 r2c4 = 4 * texture(uImg1Unit, vST + vec2( 2*deltaS,  0*deltaT)).rgb;
    vec3 r3c4 = 2 * texture(uImg1Unit, vST + vec2( 2*deltaS, -1*deltaT)).rgb;
    vec3 r4c4 = 1 * texture(uImg1Unit, vST + vec2( 2*deltaS, -2*deltaT)).rgb;

    vec3 blur_5x5 = r0c0 + \
            r1c0 + \
            r2c0 + \
            r3c0 + \
            r4c0 + \
            r0c1 + \
            r1c1 + \
            r2c1 + \
            r3c1 + \
            r4c1 + \
            r0c2 + \
            r1c2 + \
            r2c2 + \
            r3c2 + \
            r4c2 + \
            r0c3 + \
            r1c3 + \
            r2c3 + \
            r3c3 + \
            r4c3 + \
            r0c4 + \
            r1c4 + \
            r2c4 + \
            r3c4 + \
            r4c4;

    blur_5x5 /= 100.;

    gl_FragColor = vec4(mix(rgb, blur_3x3, uBlurMix), 1.);
    //gl_FragColor = vec4(mix(blur_3x3, blur_5x5, uBlurMix), 1.);
}
