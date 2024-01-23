#version 330 compatibility

uniform sampler3D uNoiseTexUnit3;
uniform bool uLockPatternOnScreen;
uniform bool uUseXYZForNoise;
uniform bool uDisplacement;
uniform float uNoiseFreq;
uniform float uNoiseAmp;

out vec3 vN; // normal vector
out vec3 vL; // point to light vector
out vec3 vE; // point to eye vector
out vec2 vST;
out vec3 vMCposition;

//vec3 LIGHTPOS = (gl_ModelViewMatrix * vec4( 10., 10., 10., 1.)).xyz; // light position
const vec3 LIGHTPOS = vec3( 10., 10., 10.); // light position

void
main( )
{

    vST = gl_MultiTexCoord0.st;
	vMCposition = gl_Vertex.xyz;
	if (uLockPatternOnScreen) vMCposition = (gl_ModelViewMatrix * gl_Vertex).xyz;


	vec3 vertex = gl_Vertex.xyz;
	if (uDisplacement){
		vec4 nv;
		if (uUseXYZForNoise) {
			nv = texture( uNoiseTexUnit3, uNoiseFreq * vMCposition );  // 1-3
		}
		else {
			nv = texture( uNoiseTexUnit3, uNoiseFreq * vec3(vST.s*2, vST.t, 0) );  // 1-3
		}

		//float n = sum( nv.rgba ); // 1 -> 3
		float n = dot( nv, vec4(1.)); // sum(nv.rgba) // 1 -> 3
		n = (n-2) / 2.; // -0.5 -> 0.5
		n *= uNoiseAmp;
		float scale = (length(vMCposition) + n) / length(vMCposition);
		vertex = gl_Vertex.xyz * scale;
	}


  	vec3 ECposition = (gl_ModelViewMatrix * vec4(vertex, 1.)).xyz;
	vN = normalize( gl_NormalMatrix * gl_Normal );  // normal vector
	vL = LIGHTPOS - ECposition.xyz;	    // vector from the point
										// to the light position
	vE = vec3( 0., 0., 0. ) - ECposition;       // vector from the point to

    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; // TODO is this even used?
    gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex, 1.); // TODO is this even used?
}
