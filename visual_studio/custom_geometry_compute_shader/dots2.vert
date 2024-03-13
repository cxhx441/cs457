#version 430 compatibility

layout( std430, binding=4 )  buffer  Pos {
	vec4 Positions[]; // array of structures
};

layout( std430, binding=8 )  buffer  Col {
	vec4 Colors[]; // array of structures
};

out vec3 vCol; 
out uint vIndex;

void main(){
	vIndex = gl_VertexID; 
	vCol = Colors[vIndex].rgb;
	gl_Position = Positions[vIndex];
} 
