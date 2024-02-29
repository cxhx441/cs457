#version 330 compatibility

flat out vec3 vEye;
flat out vec3 vLight;
flat out vec3 vNormal;
out vec2 vST;

vec3 LIGHTPOS = vec3(0);

void
main()
{
    vST = gl_MultiTexCoord0.st;
    vec3 EyeCoordPosition = (gl_ModelViewMatrix * gl_Vertex).xyz;
    vEye = vec3(0) - EyeCoordPosition.xyz;
    vLight = LIGHTPOS - EyeCoordPosition.xyz;
    vNormal = normalize(gl_NormalMatrix * gl_Normal);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
