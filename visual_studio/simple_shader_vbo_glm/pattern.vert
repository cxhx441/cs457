#version 330 compatibility

uniform mat4 uM, uV, uP, uMV, uMVP;
uniform mat3 uN;

out vec2 vST;
out vec3 vMC;
out vec3 vEye, vLight, vNormal;

const vec3 LIGHTPOS = vec3(0, 0, 10);

void setFragLighting(vec4);

void
main( )
{
    vMC = gl_Vertex.xyz;
    vST = gl_MultiTexCoord0.st;

    setFragLighting(gl_Vertex);

    gl_Position = uMVP * gl_Vertex;
}

///*
void setFragLighting(vec4 vertex){
    vec4 viewVertPosition = (uMV * vertex);
    vEye = vec3(0) - viewVertPosition.xyz;
    vLight = LIGHTPOS - viewVertPosition.xyz;


    mat3 n = uN;
    //mat3 n = mat3(transpose(inverse(uMV)));
    vNormal = normalize(n * gl_Normal);
}
//*/
