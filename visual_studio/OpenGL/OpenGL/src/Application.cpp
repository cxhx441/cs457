#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <iostream>

#include "glsl_reader.h"

// for error checking
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "): " << function <<
            " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK) 
        std::cout << "Error: %s\n" << glewGetErrorString(err) << std::endl;
	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << std::endl;

    // vertex positions
    float positions[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
        -0.5f,  0.5f,   
    };
    // indices of the vertices to be drawn
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    unsigned int buffer;
	glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(float), positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0); // position
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int ibo; // index buffer object
	glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//GLuint triShader = glCreateProgram();
	//read_compile_link_validate_shader(triShader, "res/shaders/Basic.vert", "vertex");
	//read_compile_link_validate_shader(triShader, "res/shaders/Basic.frag", "fragment");
    GLuint triShader = create_program_from_one_file("res/shaders/Basic.shader");

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(triShader);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0)); // works with nullptr in last argument...because the index buffer is bound?
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glUseProgram(0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

