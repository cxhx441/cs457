#version 330 compatibility
#extension GL_EXT_gpu_shader4: enable
#extension GL_EXT_geometry_shader4: enable
layout( triangles ) in;
layout( triangle_strip, max_vertices=73 )  out;

out vec3 gE, gL, gN;
out float gMVC_z;

uniform int uSubdivisionLevel;
uniform float uDiam, uQuantize;
uniform float uLightX, uLightY, uLightZ;

vec3 LIGHTPOS = vec3(uLightX, uLightY, uLightZ);

vec3 V0, V1, V2, V01, V02, CG;
float Diam;

vec3 quantize(vec3, float);
float quantize(float, float);
void produceVertex(float, float);

void
main( )
{
    V0  =   gl_PositionIn[0].xyz;
    V1  =   gl_PositionIn[1].xyz;
    V2  =   gl_PositionIn[2].xyz;
	V01 = V1 - V0;
	V02 = V2 - V0;

    CG = (V0 + V1 + V2) / 3.0;
    CG = quantize(CG, uQuantize);

    /*
    for( int i = 0; i < 3; i++ )
    {
        vec4 vertex = gl_PositionIn[i];
        vec4 viewVertPosition = (gl_ModelViewMatrix * vertex);
        gE = vec3(0) - viewVertPosition.xyz;
        gL = LIGHTPOS - viewVertPosition.xyz;
        gN = normalize(gl_NormalMatrix * cross(gl_PositionIn[1].xyz - gl_PositionIn[0].xyz, gl_PositionIn[2].xyz - gl_PositionIn[0].xyz));
        gl_Position = gl_ModelViewProjectionMatrix * vertex;
        EmitVertex( );
    }
    //*/

    ///*
    //int numLayers = uSubdivisionLevel;
    int numLayers = 1 << uSubdivisionLevel; // 2^uSubdivisionLevel
    float dt = 1. / float( numLayers );
    float t_top = 1.;
    for( int it = 0; it < numLayers; it++ ) {
        float t_bot = t_top - dt;
        float smax_top = 1. - t_top;
        float smax_bot = 1. - t_bot;
        int nums = it + 1;
        float ds_top = smax_top / float( nums - 1 );
        float ds_bot = smax_bot / float( nums );
        float s_top = 0.;
        float s_bot = 0.;
        for( int is = 0; is < nums; is++ ) {
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
    //*/
}


float quantize(float f, float q){
    // snap to nearest q;
    //return float(int(f / q)) * q;
    f *= q;
    int fi = int( f );
    f = float( fi ) / q;
    return f;
}


vec3 quantize(vec3 v, float q){
    return vec3(quantize(v.x, q), quantize(v.y, q), quantize(v.z, q));
}


void produceVertex(float s, float t){
    vec3 v = V0 + s*V01 + t*V02;
    v = v - CG;
    v *= uDiam/2.f / length(v);
    v = v + CG;

    /*
    vec3 v = ?????		// do the vertex (s,t) interpolation
	v = ?????		// make v's cordinates be with respect to the CG
	v = v * ?????		// roughly same code as morphing the cow into a sphere of radius uDiam/2.
	v = ?????		// put v back in the global space (ie, un-do the second line of code)
    */

    // frag lighting
    vec3 viewVertPosition = (gl_ModelViewMatrix * vec4(v, 1)).xyz;
    gE = vec3(0) - viewVertPosition;
    gL = LIGHTPOS - viewVertPosition;
    gN = normalize(gl_NormalMatrix * (v - CG));
    gMVC_z = viewVertPosition.z;

    gl_Position = gl_ModelViewProjectionMatrix * vec4(v, 1);
    EmitVertex();
}
