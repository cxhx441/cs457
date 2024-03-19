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
out vec2 vST;
out vec2 vMC;

vec3 LIGHTPOS = vec3(0., 10., 10.);
vec4 ECPosition;
	
void main()
{
	vST = texCoord;
	vMC = position.xz;
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
uniform float uPlateDim;
uniform int uChladni_N, uChladni_M;
uniform float uChladni_DX, uChladni_DZ;
uniform float uPlateNormalScale;
uniform bool uUseChladniNormalsForLighting;
uniform mat3 uN;


in vec3 vN;
in vec3 vL;
in vec3 vE;
in vec2 vST;
in vec2 vMC;

const float PI = 3.14159265359;

float chladni_res_factor(float x, float z, float n, float m, float L)
{
    // range of -2 to 2
	x += uChladni_DX / uPlateDim;
	z += uChladni_DZ / uPlateDim;
    float chladni_val = cos(n * PI * x / L) * cos(m * PI * z / L) + cos(m * PI * x / L) * cos(n * PI * z / L);
    // return a range of 0 to 1;
    return abs(chladni_val) / 2.f;
}

void main()
{
	vec3 Normal = normalize(vN);
	vec3 heightColor = vec3(1.f);
	if (uUseChladniNormalsForLighting){
		float d = 0.001f;
		vec2 xy = vMC;
		float west  = chladni_res_factor(xy.s - d, xy.t    , uChladni_N, uChladni_M, 1.f);
		float east  = chladni_res_factor(xy.s + d, xy.t    , uChladni_N, uChladni_M, 1.f);
		float north = chladni_res_factor(xy.s    , xy.t - d, uChladni_N, uChladni_M, 1.f);
		float south = chladni_res_factor(xy.s    , xy.t + d, uChladni_N, uChladni_M, 1.f);
		vec3 stangent = normalize(vec3(d * 2., uPlateNormalScale*(east-west), 0.    ));
		vec3 ttangent = normalize(vec3(0     , uPlateNormalScale*(south-north), d * 2.));
		Normal = normalize(cross( ttangent, stangent ));
		Normal = normalize(uN * Normal);
		heightColor = vec3(chladni_res_factor(xy.s, xy.t, uChladni_N, uChladni_M, 1.f));
	}
	vec3 Light  = normalize(vL);
	vec3 Eye    = normalize(vE);

	vec3 ambient = 0.2 * heightColor * uPlateColor;

	float dd = max( dot(Normal,Light), 0. );       // only do diffuse if the light can see the point
	vec3 diffuse = 0.4 * dd * heightColor * uPlateColor;

	float ss = 0.;
	if( dot(Normal,Light) > 0. )	      // only do specular if the light can see the point
	{
		vec3 ref = normalize(  reflect( -Light, Normal )  );
		ss = pow( max( dot(Eye,ref),0. ), 128 );
	}
	vec3 specular = 0.4 * ss * vec3(1);

	color = vec4( ambient + diffuse + specular,  1. );
}


