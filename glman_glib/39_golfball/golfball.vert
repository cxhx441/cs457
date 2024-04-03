#version 330 compatibility

// uniform float uAmp; // amplitude of sine wave
// uniform float uFreq; // frequency of sine wave

out vec3  vColor;
out float vLightIntensity;
out vec2  vST;
out vec3  vMC;

const vec3 LIGHTPOS = vec3( 0., 0., 10. ); // light position

void
main( )
{

    vST = gl_MultiTexCoord0.st;
    vMC = normalize(gl_Vertex.xyz);
    vec3 tnorm = normalize( gl_NormalMatrix * gl_Normal );
    vec3 ECposition = ( gl_ModelViewMatrix * gl_Vertex ).xyz;
    vLightIntensity = abs( dot( normalize(LIGHTPOS - ECposition), tnorm ) );

    vColor = gl_Color.rgb;

    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
