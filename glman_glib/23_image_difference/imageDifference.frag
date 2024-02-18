#version 330 compatibility

in vec2 vST;
uniform sampler2D uImgUnit;
uniform sampler2D uImgEditUnit;
uniform float uOffsetS, uOffsetT;
uniform float uBlend;

void main()
{
    vec2 st = vST;
    vec3 color0 = texture(uImgUnit, st + vec2(uOffsetS, uOffsetT)).rgb;
    vec3 color1 = texture(uImgEditUnit, st).rgb;

    vec3 difference = vec3(1) - step(0.001, abs(color0 - color1));
    vec3 rgb = color0 - color1*difference*uBlend;
    gl_FragColor = vec4(rgb, 1.);
}
