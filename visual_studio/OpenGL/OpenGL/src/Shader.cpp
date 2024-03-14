#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <stdio.h>
#include <fstream>

#include "Renderer.h"

Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererID(0)
{
	m_RendererID = (unsigned int) CreateProgramFromSingleFile(filepath);
}
Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID))
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

ShaderProgramSource Shader::ParseShader_Cherno(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		TESSELLATION_CONTROL = 1,
		TESSELLATION_EVALUATION = 2,
		GEOMETRY = 3,
		FRAGMENT = 4
	};

	std::string line;
	std::stringstream ss[5];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line)) {
		if (line.find("#SHADER") != std::string::npos)
		{
			if (line.find("VERTEX") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("TESSELLATION_CONTROL") != std::string::npos)
				type = ShaderType::TESSELLATION_CONTROL;
			else if (line.find("TESSELLATION_EVALUTION") != std::string::npos)
				type = ShaderType::TESSELLATION_EVALUATION;
			else if (line.find("GEOMETRY") != std::string::npos)
				type = ShaderType::GEOMETRY;
			else if (line.find("FRAGMENT") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else {
			ss[(int)type] << line << '\n';
		};
	}
	return {
			 ss[0].str(),
			 ss[1].str(),
			 ss[2].str(),
			 ss[3].str(),
			 ss[4].str(),
	};

}


void Shader::CheckGLErrors(const char* caller)
{
	unsigned int glerr = glGetError();
	if (glerr == GL_NO_ERROR)
		return;
	fprintf(stderr, "GL Error discovered from caller ‘%s‘: ", caller);
	switch (glerr)
	{
	case GL_INVALID_ENUM:
		fprintf(stderr, "Invalid enum.\n");
		break;
	case GL_INVALID_VALUE:
		fprintf(stderr, "Invalid value.\n");
		break;
	case GL_INVALID_OPERATION:
		fprintf(stderr, "Invalid Operation.\n");
		break;
	case GL_STACK_OVERFLOW:
		fprintf(stderr, "Stack overflow.\n");
		break;
	case GL_STACK_UNDERFLOW:
		fprintf(stderr, "Stack underflow.\n");
		break;
	case GL_OUT_OF_MEMORY:
		fprintf(stderr, "Out of memory.\n");
		break;
	default:
		fprintf(stderr, "Unknown OpenGL error: %d (0x%0x)\n", glerr, glerr);
	}

}


// Function to read a shader from a file
std::string Shader::readShaderFile(const char* filePath) {
	std::ifstream file(filePath);
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

// Function to compile a shader
GLuint Shader::compileShader(GLenum shaderType, const char* source) {
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

GLuint Shader::CreateProgramFromSingleFile(const std::string& filepath)
{
	GLuint program = glCreateProgram();
	ShaderProgramSource shaderSource = ParseShader_Cherno(filepath);

	
	for (int i = 0; i < 5; i++)
	{
		int status;
		int logLength;
		GLuint compiledShader;
		const char* ShaderSourcePtr;
		if (i == 0) {
			ShaderSourcePtr = shaderSource.VertexSource.c_str();
			if (std::strcmp(ShaderSourcePtr, "") == 0) continue;
			compiledShader = compileShader(GL_VERTEX_SHADER, ShaderSourcePtr);
		}
		else if (i == 1) {
			ShaderSourcePtr = shaderSource.TesselationControlSource.c_str();
			if (std::strcmp(ShaderSourcePtr, "") == 0) continue;
			compiledShader = compileShader(GL_TESS_CONTROL_SHADER, ShaderSourcePtr);
		}
		else if (i == 2) {
			ShaderSourcePtr = shaderSource.TesselationEvaluationSource.c_str();
			if (std::strcmp(ShaderSourcePtr, "") == 0) continue;
			compiledShader = compileShader(GL_TESS_EVALUATION_SHADER, ShaderSourcePtr);
		}
		else if (i == 3) {
			ShaderSourcePtr = shaderSource.GeometrySource.c_str();
			if (std::strcmp(ShaderSourcePtr, "") == 0) continue;
			compiledShader = compileShader(GL_GEOMETRY_SHADER, ShaderSourcePtr);
		}
		else if (i == 4) {
			ShaderSourcePtr = shaderSource.FragmentSource.c_str();
			if (std::strcmp(ShaderSourcePtr, "") == 0) continue;
			compiledShader = compileShader(GL_FRAGMENT_SHADER, ShaderSourcePtr);
		}
		else {
			fprintf(stderr, "ERROR in FOR LOOP \n");
			exit(0);
		}

		glAttachShader(program, compiledShader);

		glLinkProgram(program);
		CheckGLErrors("Shader Program 1");
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			fprintf(stderr, "Link failed.\n");
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
			GLchar* log = new GLchar[logLength];
			glGetProgramInfoLog(program, logLength, NULL, log);
			fprintf(stderr, "\n%s\n", log);
			delete[] log;
			exit(1);
		}
		CheckGLErrors("Shader Program 2");

		glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
		fprintf(stderr, "Program is %s.\n", status == GL_FALSE ? "invalid" : "valid");
	}

	return program;
}



GLint Shader::GetUniformLoc(char* name) 
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];
	GLint loc = glGetUniformLocation(m_RendererID, name);
	if (loc < 0)
		fprintf(stderr, "cannot find uniform variable %s\n", name);
	m_UniformLocationCache[name] = loc;
	return loc;
}

void Shader::SetUniform1i(char* name, int val )
{
	GLint loc = GetUniformLoc(name);
	glUniform1i( loc, val );
};

void Shader::SetUniform1f(char* name, float val )
{
	GLint loc = GetUniformLoc(name);
	glUniform1f( loc, val );
};

void Shader::SetUniform3f(char* name, float val0, float val1, float val2 )
{
	GLint loc = GetUniformLoc(name);
	glUniform3f( loc, val0, val1, val2 );
};

void Shader::SetUniform4f(char* name, float val0, float val1, float val2, float val3 )
{
	GLint loc = GetUniformLoc(name);
	glUniform4f( loc, val0, val1, val2, val3 );
};

void Shader::SetUniformfv(char* name, int count, float val[])
{
	GLint loc = GetUniformLoc(name);
	if (count == 2)      glUniform2fv(loc, 1, val);
	else if (count == 3) glUniform3fv(loc, 1, val);
	else if (count == 4) glUniform4fv(loc, 1, val);
};

void Shader::SetUniformMat3( char* name, glm::mat3 val)
{
	GLint loc = GetUniformLoc(name);
	glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val));
};

void Shader::SetUniformMat4( char* name, glm::mat4 val)
{
	GLint loc = GetUniformLoc(name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
};

void Shader::SetUniformVec3( char* name, glm::vec3 val)
{
	GLint loc = GetUniformLoc(name);
	glUniform3fv(loc, 1, glm::value_ptr(val));
};

void Shader::SetUniformVec4( char* name, glm::vec4 val)
{
	GLint loc = GetUniformLoc(name);
	glUniform4fv(loc, 1, glm::value_ptr(val));
};
