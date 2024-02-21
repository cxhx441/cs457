#version 330 compatibility

uniform float uScale, uOffsetS, uOffsetT; // 0 for mandelbrot, 1 for julia
uniform float uTol, uN_F; // 0 for mandelbrot, 1 for julia
uniform int uN, uM; // 0 for mandelbrot, 1 for julia
in vec2 vST;

const float F_2_PI = 6.28318530718;
const float PI = 3.14159265359;
const int size = 1200;

float chladni_pattern_val(float x, float y, float n, float m, float L)
{
    // return a range of -2 to 2
    return cos(n * PI * x / L) * cos(m * PI * y / L) - cos(m * PI * x / L) * cos(n * PI * y / L);
}

void main()
{
    // set st
    vec2 st = mod((vST - vec2(600, 600))*F_2_PI, F_2_PI);
    vec2 coord = vST;

    vec2 frag = mod(gl_FragCoord.xy * F_2_PI, F_2_PI);
    float val = chladni_pattern_val(coord.x, coord.y, uN, uM, 1.0);
    val = abs(val) / 2.; // range 0 to 1

    //float val = chladni_pattern_val(coord.x, coord.y, uN_F, uM, 1.0);

    //gl_FragColor = vec4(step(uTol, abs(val)) * vec3(1.0, 1.0, 1.0), 1.0);
    gl_FragColor = vec4(val, val, val, 1.0);
    //gl_FragColor = vec4(1.0, 0, 1.0, 1.0);
}
