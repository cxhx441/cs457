#pragma once

#include <string>
#include <unordered_map>

#include <GL/glew.h>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string TesselationControlSource;
	std::string TesselationEvaluationSource;
	std::string GeometrySource;
	std::string FragmentSource;
};

class Shader
{
private: 
	std::string m_FilePath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	// Set uniforms
	GLint GetUniformLoc(char* name);
	void SetUniform1i(char* name, int val);
	void SetUniform1f(char* name, float val);
	void SetUniform3f(char* name, float val0, float val1, float val2);
	void SetUniform4f(char* name, float val0, float val1, float val2, float val3);
	void SetUniformfv(char* name, int count, float val[]);
	void SetUniformMat3(char* name, glm::mat3 val);
	void SetUniformMat4(char* name, glm::mat4 val);
	void SetUniformVec3(char* name, glm::vec3 val);
	void SetUniformVec4(char* name, glm::vec4 val);

private:
	ShaderProgramSource ParseShader_Cherno(const std::string& filepath);
	void CheckGLErrors(const char* caller);
	std::string readShaderFile(const char* filePath);
	GLuint compileShader(GLenum shaderType, const char* source);
	GLuint CreateProgramFromSingleFile(const std::string& filepath);

};
