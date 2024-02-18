#version 330 compatibility

uniform float uBlend;
uniform sampler2D uImgUnit;

in vec2 vST;

void main()
{
    float ResS = textureSize(uImgUnit, 0).x;
    float ResT = textureSize(uImgUnit, 0).y;

    vec2 stpp = vec2( 1./ResS, 1./ResT);

    vec3 c00 = texture( uImgUnit, vST ).rgb;
    vec3 cp1p1 = texture( uImgUnit, vST + stpp ).rgb;
    vec3 diffs = c00 - cp1p1;

    float max = diffs.r;
    if( abs(diffs.g) > abs(max) ) max = diffs.g;
    if( abs(diffs.b) > abs(max) ) max = diffs.b;

    float gray = clamp( max + .5, 0., 1. );
    vec4 grayVersion = vec4( gray, gray, gray, 1. );
    vec4 colorVersion = vec4( c00, 1. );

    gl_FragColor= mix( grayVersion, colorVersion, uBlend );

}
