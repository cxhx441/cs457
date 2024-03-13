#pragma once

#include <GL/glew.h>

// for error checking
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));

#include <string>

std::string getGLErrorString(GLenum error);
void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);
