#version 330 compatibility

out vec3 vN;

void
main()
{
    vN = normalize(gl_NormalMatrix * gl_Normal);
    gl_Position = gl_ModelViewMatrix * gl_Vertex;
}
