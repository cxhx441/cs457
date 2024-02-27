#version 330 compatibility

out vec3 vEye;
out vec3 vLight;
out vec3 vNormal;
out vec2 vST;

//const vec3 LIGHTPOS = vec3(1000.0, 1000.0, 1000.0);
//const vec3 LIGHTPOS = vec3(0.0, -1.0, 0.0);
vec3 LIGHTPOS = (gl_ModelViewMatrix * vec4(0.0, -500.0, 0.0, 1.)).xyz;

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
