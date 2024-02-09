#version 330 compatibility
uniform bool uPolar;
uniform float uK;
uniform float uTransX;
uniform float uTransY;

out vec3 vColor;

void
main( void ){
	vColor = gl_Color.rgb;

	vec2 pos = ( gl_ModelViewMatrix * gl_Vertex ).xy;
	pos += vec2( uTransX, uTransY );
	float r = length( pos.xy );
	vec4 pos2 = vec4( 0., 0., -5., 1. );

	if( uPolar )
		pos2.xy = pos / ( r + uK );
	else
		pos2.xy = pos / ( pos*pos + uK*uK );

	gl_Position = gl_ProjectionMatrix * pos2;
}
