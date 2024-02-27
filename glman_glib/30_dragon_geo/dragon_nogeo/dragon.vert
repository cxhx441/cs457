#version 330 compatibility

out vec3 vEye;
out vec3 vLight;
out vec3 vNormal;
out vec2 vST;

const vec3 LIGHTPOS = vec3(1000.0, 1000.0, 1000.0);

void
main()
{
    vST = gl_MultiTexCoord0.st;
    vec3 EyeCoordPosition = (gl_ModelViewMatrix * gl_Vertex).xyz;
    vEye = LIGHTPOS - EyeCoordPosition.xyz;
    vLight = vec3(0) - EyeCoordPosition.xyz;
    vNormal = normalize(gl_NormalMatrix * gl_Normal);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
