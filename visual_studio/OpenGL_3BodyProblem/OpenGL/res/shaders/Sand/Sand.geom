#version 430 compatibility
#extension GL_EXT_gpu_shader4: enable 
#extension GL_EXT_geometry_shader4: enable 
layout( points ) in; 
layout( triangle_strip, max_vertices=256 ) out;

layout( std430, binding=6 )  buffer  Rot {
	vec4 Rotations[]; // array of structures
};

//uniform mat3 uN;
uniform mat4 uMV, uMVP;
uniform float uCubeSize;

in vec3 vCol[];
in uint vIndex[];

out vec3 gNormal;
out vec3 gLightDir;
out vec3 gEyeDir;
out vec3 gCol;

vec3 LIGHTPOS = vec3(0., 10., 0.);
vec3 ECPosition;


vec3 cube_center;

//struct Quaternion {
    //float w;
    //vec3 v;
//};

// Function to create a quaternion from an axis and an angle
vec4 quatFromAxisAngle(vec3 axis, float angle) {
    float halfAngle = angle * 0.5;
    return normalize( vec4(axis * sin(halfAngle), cos(halfAngle))) ;
}

// Function to rotate a vector by a quaternion
vec3 quatRotateVec3(vec4 q, vec3 v) {
    return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

vec3 rotate(vec3 vertex, vec3 axis, float angle)
{
    vec4 rotation = quatFromAxisAngle(axis, angle);
    return quatRotateVec3(rotation, vertex.xyz);
}

void
make_face(vec4 c1, vec4 c2, vec4 c3, vec4 c4){

	// rotate
	vec3 axis_of_rot = Rotations[vIndex[0]].xyz;
	float angle_of_rot = Rotations[vIndex[0]].w;
	c1.xyz = rotate(c1.xyz, axis_of_rot, angle_of_rot);
	c2.xyz = rotate(c2.xyz, axis_of_rot, angle_of_rot);
	c3.xyz = rotate(c3.xyz, axis_of_rot, angle_of_rot);
	c4.xyz = rotate(c4.xyz, axis_of_rot, angle_of_rot);
	
	// move to be centered around v
	c1.xyz += cube_center;
	c2.xyz += cube_center;
	c3.xyz += cube_center;
	c4.xyz += cube_center;

	gNormal = cross(c1.xyz - c3.xyz, c2.xyz - c3.xyz);
	if (dot(gNormal, c1.xyz - gl_PositionIn[0].xyz) < 0) gNormal *= -1;
	gNormal = normalize(gl_NormalMatrix * gNormal);

	vec3 c1_light = (uMV * c1).xyz;
	vec3 c2_light = (uMV * c2).xyz;
	vec3 c3_light = (uMV * c4).xyz;
	vec3 c4_light = (uMV * c3).xyz;

	vec4 c1_out = uMVP * c1;
	vec4 c2_out = uMVP * c2;
	vec4 c3_out = uMVP * c4;
	vec4 c4_out = uMVP * c3;


	gl_Position = c1_out;
	//gLightIntensity = max(dot(normalize(LIGHTPOS - c1_light), gNormal), 0.0);
	gLightDir = normalize(LIGHTPOS - c1_light);
	gEyeDir = normalize(vec3(0, 0, 0) - c1_light);
	EmitVertex();
	gl_Position = c2_out;
	//gLightIntensity = max(dot(normalize(LIGHTPOS - c2_light), gNormal), 0.0);
	gLightDir = normalize(LIGHTPOS - c2_light);
	gEyeDir = normalize(vec3(0, 0, 0) - c2_light);
	EmitVertex();
	gl_Position = c3_out;
	//gLightIntensity = max(dot(normalize(LIGHTPOS - c3_light), gNormal), 0.0);
	gLightDir = normalize(LIGHTPOS - c3_light);
	gEyeDir = normalize(vec3(0, 0, 0) - c3_light);
	EmitVertex();
	gl_Position = c4_out;
	//gLightIntensity = max(dot(normalize(LIGHTPOS - c4_light), gNormal), 0.0);
	gLightDir = normalize(LIGHTPOS - c4_light);
	gEyeDir = normalize(vec3(0, 0, 0) - c4_light);
	EmitVertex();
	EndPrimitive();
}
void main(){
	gCol = vCol[0];
	float dt = uCubeSize / 2.f;
	cube_center = gl_PositionIn[0].xyz;

	//vec4 c1 = vec4(v, 0) + vec4(-dt,  dt,  dt, 1);
	//vec4 c2 = vec4(v, 0) + vec4(-dt,  dt, -dt, 1);
	//vec4 c3 = vec4(v, 0) + vec4( dt,  dt, -dt, 1);
	//vec4 c4 = vec4(v, 0) + vec4( dt,  dt,  dt, 1);

	//vec4 c5 = vec4(v, 0) + vec4(-dt, -dt,  dt, 1);
	//vec4 c6 = vec4(v, 0) + vec4(-dt, -dt, -dt, 1);
	//vec4 c7 = vec4(v, 0) + vec4( dt, -dt, -dt, 1);
	//vec4 c8 = vec4(v, 0) + vec4( dt, -dt,  dt, 1);
	
	vec4 c1 = vec4(-dt,  dt,  dt, 1);
	vec4 c2 = vec4(-dt,  dt, -dt, 1);
	vec4 c3 = vec4( dt,  dt, -dt, 1);
	vec4 c4 = vec4( dt,  dt,  dt, 1);

	vec4 c5 = vec4(-dt, -dt,  dt, 1);
	vec4 c6 = vec4(-dt, -dt, -dt, 1);
	vec4 c7 = vec4( dt, -dt, -dt, 1);
	vec4 c8 = vec4( dt, -dt,  dt, 1);

	make_face(c1, c2, c3, c4);
	make_face(c4, c3, c7, c8);
	make_face(c3, c2, c6, c7);
	make_face(c1, c2, c6, c5);
	make_face(c1, c4, c8, c5);
	make_face(c6, c7, c8, c5);
} 
