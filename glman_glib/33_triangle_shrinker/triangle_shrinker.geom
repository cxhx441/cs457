#version 330 compatibility
#extension GL_EXT_gpu_shader4: enable
#extension GL_EXT_geometry_shader4: enable
layout( triangles ) in;
layout( triangle_strip, max_vertices=3 )  out;

uniform float uLightX, uLightY, uLightZ;
uniform float uTriPercent;

in vec3 vN[3];
out vec3 gE, gL, gN;

vec3 LIGHTPOS = vec3(uLightX, uLightY, uLightZ);

vec3 v[3];
vec3 cg;
void produceVertex(int i){
        v[i] = cg + (v[i] - cg) * uTriPercent;

        gE = vec3(0) - v[i];
        gL = LIGHTPOS - v[i];
        gN = vN[i];

        gl_Position = gl_ProjectionMatrix * vec4(v[i], 1.0);
        EmitVertex( );
}

void main( )
{
    v[0] = gl_PositionIn[0].xyz;
    v[1] = gl_PositionIn[1].xyz;
    v[2] = gl_PositionIn[2].xyz;

    cg = (v[0] + v[1] + v[2]) / 3.0;

    for( int i = 0; i < 3; i++ )
    {
        produceVertex(i);
    }
}
