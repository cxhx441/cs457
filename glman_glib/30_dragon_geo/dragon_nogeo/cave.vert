#version 330 compatibility

flat out vec3 vEye;
flat out vec3 vLight;
flat out vec3 vNormal;

//const vec3 LIGHTPOS = vec3(1000.0, 1000.0, 1000.0);
//const vec3 LIGHTPOS = vec3(0.0, -1.0, 0.0);
vec3 LIGHTPOS = vec3(0);

void
main()
{
    vec3 EyeCoordPosition = (gl_ModelViewMatrix * gl_Vertex).xyz;
    vEye = vec3(0) - EyeCoordPosition.xyz;
    vLight = LIGHTPOS - EyeCoordPosition.xyz;
    vNormal = normalize(gl_NormalMatrix * gl_Normal);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
