#version 330 compatibility
#extension GL_EXT_gpu_shader4: enable
#extension GL_EXT_geometry_shader4: enable
layout( triangles ) in;
layout( triangle_strip, max_vertices=3 )  out;

out vec3 gE, gL, gN;

const vec3 LIGHTPOS = vec3(0, 0, 10);

void setFragLighting(vec4);

void
main( )
{
    for( int i = 0; i < 3; i++ )
    {
        setFragLighting(gl_PositionIn[i]);
        gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[i];
        EmitVertex( );
    }
}

///*
void setFragLighting(vec4 vertex){
    vec4 viewVertPosition = (gl_ModelViewMatrix * vertex);
    gE = vec3(0) - viewVertPosition.xyz;
    gL = LIGHTPOS - viewVertPosition.xyz;
    gN = normalize(gl_NormalMatrix * cross(gl_PositionIn[1].xyz - gl_PositionIn[0].xyz, gl_PositionIn[2].xyz - gl_PositionIn[0].xyz));
}
//*/
