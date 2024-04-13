#pragma once
#include <GL/glew.h>
#include <string>
#define GLM_FORCE_RADIANS
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
//struct ShaderProgramSource;
//static ShaderProgramSource parse_shader_cherno(const std::string& filepath);
void check_gl_errors(const char* caller);
std::string readShaderFile(const char* filePath);
GLuint compileShader(GLenum shaderType, const char* source);
//GLuint create_program_from_one_file(const std::string& filepath);
void read_compile_link_validate_shader(GLuint program, const char* filename, const char* shader_type);
GLint get_uniform_loc(GLuint program, const char* name);
void set_uniform_variable(GLuint program, const char* name, int val);
void set_uniform_variable(GLuint program, const char* name, float val);
void set_uniform_variable(GLuint program, const char* name, float val0, float val1, float val2);
void set_uniform_variable(GLuint program, const char* name, float val0, float val1, float val2, float val3);
void set_uniform_variable(GLuint program, const char* name, int count, float val[]);
void set_uniform_variable(GLuint program, const char* name, glm::mat3 val);
void set_uniform_variable(GLuint program, const char* name, glm::mat4 val);
void set_uniform_variable(GLuint program, const char* name, glm::vec3 val);
void set_uniform_variable(GLuint program, const char* name, glm::vec4 val);
