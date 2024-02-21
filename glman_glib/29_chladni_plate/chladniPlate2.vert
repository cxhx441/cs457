#version 330 compatibility

//uniform float uTol, uN_F,
uniform float uScale;
uniform int uN, uM;

//out vec2 vST;
//out vec3 vMC;
out vec4 vColor;

const float F_2_PI = 6.28318530718;
const float PI = 3.14159265359;
const int size = 1200;

float chladni_pattern_val(float x, float y, float n, float m, float L)
{
    // return a range of -2 to 2
    return cos(n * PI * x / L) * cos(m * PI * y / L) - cos(m * PI * x / L) * cos(n * PI * y / L);
}

void
main()
{
    vec3 newVert = gl_Vertex.xyz;

    // set st
    //vST = gl_MultiTexCoord0.st;
    vec2 coord = gl_MultiTexCoord0.st;

    float val = chladni_pattern_val(coord.s, coord.t, uN, uM, 1.0);
    val = abs(val) / 2.; // range 0 to 1

    newVert.z = val * uScale;

    vColor = vec4(val, val, val, 1.0);
    gl_Position = gl_ModelViewProjectionMatrix * vec4(newVert, 1.0);
}
