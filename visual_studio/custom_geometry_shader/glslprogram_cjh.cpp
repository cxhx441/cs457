#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif


#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "glut.h"

void 
check_gl_errors( const char* caller)
{
	unsigned int glerr = glGetError();
	if( glerr == GL_NO_ERROR )
		return;
	fprintf( stderr, "GL Error discovered from caller ‘%s‘: ", caller );
	switch( glerr )
	{
		case GL_INVALID_ENUM:
		fprintf( stderr, "Invalid enum.\n" );
		break;
		case GL_INVALID_VALUE:
		fprintf( stderr, "Invalid value.\n" );
		break;
		case GL_INVALID_OPERATION:
		fprintf( stderr, "Invalid Operation.\n" );
		break;
		case GL_STACK_OVERFLOW:
		fprintf( stderr, "Stack overflow.\n" );
		break;
		case GL_STACK_UNDERFLOW:
		fprintf(stderr, "Stack underflow.\n" );
		break;
		case GL_OUT_OF_MEMORY:
		fprintf( stderr, "Out of memory.\n" );
		break;
		default:
		fprintf( stderr, "Unknown OpenGL error: %d (0x%0x)\n", glerr, glerr );
	}

} 


// Function to read a shader from a file
std::string readShaderFile(const char* filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Function to compile a shader
GLuint compileShader(GLenum shaderType, const char* source) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        std::cerr << "Shader compilation error:\n" << infoLog << std::endl;
    }

    return shader;
}

void
read_compile_link_validate_shader(GLuint program, char *filename, char *shader_type)
{
	std::string ShaderSource = readShaderFile(filename);
    const char* ShaderSourcePtr = ShaderSource.c_str();

	int status;
	int logLength;
	GLuint compiledShader;
	if (shader_type == "vertex")
		compiledShader = compileShader( GL_VERTEX_SHADER, ShaderSourcePtr );
	else if (shader_type == "geometry")
		compiledShader = compileShader( GL_GEOMETRY_SHADER, ShaderSourcePtr);
	else if (shader_type == "tess_control")
		compiledShader = compileShader( GL_TESS_CONTROL_SHADER, ShaderSourcePtr);
	else if (shader_type == "tess_evaluation")
		compiledShader = compileShader( GL_TESS_EVALUATION_SHADER, ShaderSourcePtr);
	else if (shader_type == "fragment")
		compiledShader = compileShader( GL_FRAGMENT_SHADER, ShaderSourcePtr);
	else if (shader_type == "compute")
		compiledShader = compileShader( GL_COMPUTE_SHADER, ShaderSourcePtr);
	else
	{
		fprintf(stderr, "invalid shader_type: %s\n", shader_type);
		exit(0);
	}
		
    glAttachShader(program, compiledShader);

	glLinkProgram(program);
	check_gl_errors( "Shader Program 1" );
	glGetProgramiv( program, GL_LINK_STATUS, &status );
	if( status == GL_FALSE )
	{
		fprintf( stderr, "Link failed.\n" );
		glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logLength );
		GLchar *log = new GLchar [logLength];
		glGetProgramInfoLog( program, logLength, NULL, log );
		fprintf( stderr, "\n%s\n", log );
		delete [ ] log;
		exit( 1 );
	}
	check_gl_errors( "Shader Program 2" );

	glGetProgramiv( program, GL_VALIDATE_STATUS, &status );
	fprintf( stderr, "Program is %s.\n", status == GL_FALSE ? "invalid" : "valid" );

}

void
set_uniform_variable(GLuint program, char* name, int val )
{
	GLint loc = glGetUniformLocation(program, name);
	if (loc < 0)
	{
		fprintf(stderr, "cannot find uniform variable %s\n", name);
	}
	glUniform1i( loc, val );
};

void
set_uniform_variable(GLuint program, char* name, float val )
{
	GLint loc = glGetUniformLocation(program, name);
	if (loc < 0)
	{
		fprintf(stderr, "cannot find uniform variable %s\n", name);
	}
	glUniform1f( loc, val );
};

void
set_uniform_variable(GLuint program, char* name, float val0, float val1, float val2 )
{
	GLint loc = glGetUniformLocation(program, name);
	if (loc < 0)
	{
		fprintf(stderr, "cannot find uniform variable %s\n", name);
	}
	glUniform3f( loc, val0, val1, val2 );
};

void
set_uniform_variable(GLuint program, char* name, float val[3])
{
	GLint loc = glGetUniformLocation(program, name);
	if (loc < 0)
	{
		fprintf(stderr, "cannot find uniform variable %s\n", name);
	}
	glUniform3fv(loc, 1, val);
};

