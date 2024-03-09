#version 330 compatibility
#extension GL_EXT_gpu_shader4: enable
#extension GL_EXT_geometry_shader4: enable
layout( triangles ) in;
layout( triangle_strip, max_vertices=78 )  out;

uniform float uLightX, uLightY, uLightZ;
uniform float uRadius;
uniform int uLevel;

out vec3 gE, gL, gN;

vec3 LIGHTPOS = vec3(uLightX, uLightY, uLightZ);

vec3 v[3];
vec3 v01, v02;
void produceVertex(float s, float t){
    vec3 v = v[0] + s*v01 + t*v02;
    v = normalize(v);
    vec3 n = v;

    vec4 ECposition = vec4(uRadius*v, 1.0);
    gE = vec3(0) - ECposition.xyz;
    gL = LIGHTPOS - ECposition.xyz;
    gN = normalize(gl_NormalMatrix * n);

    gl_Position = gl_ProjectionMatrix * ECposition;
    EmitVertex( );
}

void main( )
{
    v[0] = gl_PositionIn[0].xyz;
    v[1] = gl_PositionIn[1].xyz;
    v[2] = gl_PositionIn[2].xyz;
    v01 = v[1] - v[0];
    v02 = v[2] - v[0];

    int numLayers = 1 << uLevel;
    float dt = 1./ float(numLayers);
    float t_top = 1.;

    for( int it = 0; it < numLayers; it++ )
    {
        float t_bot = t_top - dt;
        float smax_top = 1. - t_top;
        float smax_bot = 1. - t_bot;
        int nums = it + 1;
        float ds_top = smax_top / float( nums - 1 );
        float ds_bot = smax_bot / float( nums );
        float s_top = 0.;
        float s_bot = 0.;
        for( int is = 0; is < nums; is++ )
        {
            produceVertex( s_bot, t_bot );
            produceVertex( s_top, t_top );
            s_top += ds_top;
            s_bot += ds_bot;
        }
        produceVertex( s_bot, t_bot );
        EndPrimitive( );
        t_top = t_bot;
        t_bot -= dt;
    }
}
