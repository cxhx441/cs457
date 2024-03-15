//SHADER VERTEX
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

uniform mat4 uMV, uMVP;
uniform mat3 uN;

out vec3 vN;
out vec3 vL;
out vec3 vE;

vec3 LIGHTPOS = vec3(0., 10., 0.);
vec4 ECPosition;
	
void main()
{
	ECPosition = uMV * position;
	vN = normalize(uN * normal);
	vL = normalize(LIGHTPOS - ECPosition.xyz);
	vE = normalize(vec3(0, 0, 0) - ECPosition.xyz);
	gl_Position = uMVP * position;
}

//SHADER FRAGMENT
#version 330 core
layout(location = 0) out vec4 color;

uniform vec3 uPlateColor;

in vec3 vN;
in vec3 vL;
in vec3 vE;

void main()
{
	vec3 Normal = normalize(vN);
	vec3 Light  = normalize(vL);
	vec3 Eye    = normalize(vE);

	vec3 ambient = 0.2 * uPlateColor;

	float dd = max( dot(Normal,Light), 0. );       // only do diffuse if the light can see the point
	vec3 diffuse = 0.4 * dd * uPlateColor;

	float ss = 0.;
	if( dot(Normal,Light) > 0. )	      // only do specular if the light can see the point
	{
		vec3 ref = normalize(  reflect( -Light, Normal )  );
		ss = pow( max( dot(Eye,ref),0. ), 128 );
	}
	vec3 specular = 0.4 * ss * vec3(1);

	color = vec4( ambient + diffuse + specular,  1. );
}
