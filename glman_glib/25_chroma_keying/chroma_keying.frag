#version 330 compatibility

in vec2 vST;
uniform sampler2D uGreenUnit;
uniform sampler2D uReplaceUnit;
uniform float uBlend;
uniform float uGreenThreshold;

void main()
{
    vec2 st = vST;
    vec3 rgb = texture(uGreenUnit, st).rgb;
    vec3 replacement_color = texture(uReplaceUnit, st).rgb;

    float gt =uGreenThreshold;
    // replace if r < t,
    //            g > 1-t
    //            b < t
    //if (rgb.r < gt && rgb.g > 1-gt && rgb.b < gt) rgb = mix(rgb, replacement_color, uBlend);
    if (all(lessThan(rgb*vec3(1, -1, 1), vec3(gt, gt-1, gt)))) rgb = mix(rgb, replacement_color, uBlend);
    gl_FragColor = vec4(rgb, 1.);
}
