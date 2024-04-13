//SHADER VERTEX
#version 330 compatibility
uniform mat4 uMVP;
void main()
{
	gl_Position = uMVP * gl_Vertex;
}


//SHADER FRAGMENT
#version 330 compatibility
void main()
{
	gl_FragColor = vec4(1.f, 1.f, 0.f, 1.f);
}
