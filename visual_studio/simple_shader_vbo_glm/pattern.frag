#version 330 compatibility

uniform sampler2D uTexUnit;
in vec2 vST;
in vec3 vMC;

void
main( )
{
    //vec3 color = texture(uTexUnit, vST).rgb;
    vec3 color = vMC;
    gl_FragColor = vec4(color, 1.0);
}


