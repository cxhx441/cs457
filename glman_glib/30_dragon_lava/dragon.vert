#version 330 compatibility

uniform float Timer;

out vec3 vEye;
out vec3 vLight1;
out vec3 vLight2;
out vec3 vNormal;
out vec2 vST;

//const vec3 LIGHTPOS = vec3(1000.0, 1000.0, 1000.0);
//const vec3 LIGHTPOS = vec3(0.0, -1.0, 0.0);
vec3 LIGHTPOS1 = (gl_ModelViewMatrix * vec4(0.0, -1000, 0.0, 1.)).xyz;
vec3 LIGHTPOS2 = (gl_ModelViewMatrix * vec4(0.0, 1., 1.0, 1.)).xyz;

bool check_dragon_part(vec3 xyz, float uX, float uY, float uZ, float uDx, float uDy, float uDz)
{
    return ( uX-uDx <= xyz.x  &&  xyz.x <= uX+uDx  &&   uY-uDy <= xyz.y  &&  xyz.y <= uY+uDy  &&  uZ-uDz <= xyz.z  &&  xyz.z <= uZ+uDz );
}

float uX, uY, uZ, uDx, uDy, uDz;
void
main()
{
    vST = gl_MultiTexCoord0.st;
    vec4 newVertex = gl_Vertex;

	vec3 vXYZ = gl_Vertex.xyz;

    // LEFT WING
    uX = -28.4488;
    uDx = 24.0854;
    uY = 14.0244;
    uDy = 7.122;
    uZ = -7.9268;
    uDz = 19.2073;
    if( check_dragon_part(vXYZ, uX, uY, uZ, uDx, uDy, uDz )) {
        //newVertex.y += sin((uX-vXYZ.x)/4.)*2;
        float wingUpY = -sin( 6.0509 + (uX-vXYZ.x)/8.)*8;
        float wingDownY = sin( 5.16 + (uX-vXYZ.x)/(12.195))*8;
        float wingPosY = mix(wingDownY, wingUpY, (1 + sin(Timer*20))/2.);
        newVertex.y += wingPosY;
    }
    // RIGHT WING
    uX = 28.4488;
    if( check_dragon_part(vXYZ, uX, uY, uZ, uDx, uDy, uDz )) {
        //newVertex.y += sin( 2.4507 + (uX-vXYZ.x)/4.)*2;
        float wingUpY = -sin( 3.335 + (uX-vXYZ.x)/8.)*8;
        float wingDownY = sin( 4.5122 + (uX-vXYZ.x)/13.415)*8;
        float wingPosY = mix(wingDownY, wingUpY, (1 + sin(Timer*20))/2.);
        wingPosY = wingPosY;
        newVertex.y += wingPosY;
    }
    // HEAD
    uX = -0.0195;
    uDx = 4.5732;
    uY = -20.6537;
    uDy = 50;
    uZ = 25.6098;
    uDz = 5.7927;
    if( check_dragon_part(vXYZ, uX, uY, uZ, uDx, uDy, uDz )) {
        float HeadX = -sin( 0.7317 + (-(uZ-vXYZ.z))/7.9268)*8;
        newVertex.x += HeadX;
    }

    vec3 EyeCoordPosition = (gl_ModelViewMatrix * newVertex).xyz;
    vEye = vec3(0) - EyeCoordPosition.xyz;
    vLight1 = LIGHTPOS1 - EyeCoordPosition.xyz;
    vLight2 = LIGHTPOS2 - EyeCoordPosition.xyz;
    vNormal = normalize(gl_NormalMatrix * gl_Normal);
    gl_Position = gl_ModelViewProjectionMatrix * newVertex;
}
