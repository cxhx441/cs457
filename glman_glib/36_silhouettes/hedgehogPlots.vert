#version 330 compatibility

out vec4 vColor;
out vec3 vTnorm;
void
main()
{
    vColor = gl_Color;
    vTnorm = gl_NormalMatrix * gl_Normal;
    gl_Position = gl_ModelViewMatrix * gl_Vertex;
}
