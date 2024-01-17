#version 330 compatibility

// uniform float uAmp; // amplitude of sine wave
// uniform float uFreq; // frequency of sine wave

out vec3  vColor;
out float vLightIntensity;
out vec2  vST;

const vec3 LIGHTPOS = vec3( 0., 0., 10. ); // light position

void
main( )
{

    vST = gl_MultiTexCoord0.st;
    vec3 tnorm = normalize( gl_NormalMatrix * gl_Normal ); // model coord normals to camera coord normals
    vec3 ECposition = ( gl_ModelViewMatrix * gl_Vertex ).xyz; // camera space coords .xyz
    vLightIntensity = abs( dot( normalize(LIGHTPOS - ECposition), tnorm ) );
        // abs(1*1*cos(theta)) where theta is the angle between the light-to-eye and normal vectors.
        // dot_prod = |A||B|Cos(theta). Since A and B are normalized (length made to be 1), we get the 1*1
        // vLightIntensity is ultimately somewhere [0-1].
        // This is as described in the lighting notes. Then we take the abs value of that.

    vColor = gl_Color.rgb;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; // TODO is this even used?
}
