#pragma once
#include <GL/glew.h>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
struct ShaderProgramSource;
static ShaderProgramSource ParseShader_Cherno(const std::string&);
void check_gl_errors(const char* );
std::string readShaderFile(const char* );
GLuint compileShader(GLenum , const char* ) ;
GLuint create_program_from_one_file(const std::string&);
void read_compile_link_validate_shader(GLuint , const char* , const char* );
void set_uniform_variable(GLuint, char*, int);
void set_uniform_variable(GLuint, char*, float);
void set_uniform_variable(GLuint, char*, float, float, float);
void set_uniform_variable(GLuint, char*, float, float, float, float);
void set_uniform_variable(GLuint, char*, int, float[]);
void set_uniform_variable(GLuint, char*, glm::mat3);
void set_uniform_variable(GLuint, char*, glm::mat4);
void set_uniform_variable(GLuint, char*, glm::vec3);
void set_uniform_variable(GLuint, char*, glm::vec4);
