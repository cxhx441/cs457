#version 330 compatibility
#extension GL_EXT_gpu_shader4: enable
#extension GL_EXT_geometry_shader4: enable

layout( triangles_adjacency ) in;
layout( triangle_strip, max_vertices=200 )  out;

uniform int uNum;

void
main( )
{
    for( int i = 3; i < 6; i++ )
    {
        gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[i];
        EmitVertex( );
    }
}
