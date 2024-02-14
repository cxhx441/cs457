#version 330 compatibility

uniform float uPower;
uniform float uRtheta;
uniform float uMosaic;
uniform float uBlend;
uniform sampler2D uImg1Unit;
uniform sampler2D uImg2Unit;

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
    vec2 st = vST;

    // fisheye
    st -= vec2(0.5, 0.5); // set st range to +- 0.5
    float r = sqrt(dot(st, st));
    float r_prime = pow(2*r, uPower);

    // swirl
    float theta = atan2(st.t, st.s);
    float theta_prime = theta - uRtheta * r;

    // get st back to 0-1
    st = r_prime * vec2(cos(theta_prime), sin(theta_prime)); // st +-1
    st += 1.; // st 0-2
    st /= 2.; // st 0-1

    // mosaic
    ivec2 NumIntST = ivec2(st / uMosaic);
    st = NumIntST*uMosaic + vec2(uMosaic/2., uMosaic/2.);

    vec3 color_img1 = texture(uImg1Unit, st).rgb;
    vec3 color_img2 = texture(uImg2Unit, st).rgb;
    vec3 rgb = mix(color_img1, color_img2, uBlend);

    // make pixel black if st outside [0, 1]
    rgb *= 1 - step(1, st.s); // if s > than 1, rgb *= 0
    rgb *= 1 - step(1, st.t); // if t > than 1, rgb *= 0
    rgb *= step(0, st.s); // if s < than 0, rgb *= 0
    rgb *= step(0, st.t); // if t < than 0, rgb *= 0

    gl_FragColor = vec4(rgb, 1.);
}
