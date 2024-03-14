#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <iostream>

#include "glsl_reader.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // set opengl version 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwSwapInterval(1); // synchronize framerate to vsync/framerate

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK) 
        std::cout << "Error: %s\n" << glewGetErrorString(err) << std::endl;
	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << std::endl;
    
	{ // this block is to avoid infinite loop of glCheckError which provides an error when there is no gl context. stops infinite loop when we close the openggl window. 
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

		// generate vao
		unsigned int vao; // vertex array object

		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		VertexArray va;
		VertexBuffer vb(positions, 2 * 4 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);
		vb.Unbind();

		IndexBuffer ib(indices, 2 * 3 * sizeof(unsigned int));
		ib.Unbind();

		//GLuint triShader = glCreateProgram();
		//read_compile_link_validate_shader(triShader, "res/shaders/Basic.vert", "vertex");
		//read_compile_link_validate_shader(triShader, "res/shaders/Basic.frag", "fragment");
		GLuint triShader = create_program_from_one_file("res/shaders/Basic.shader");
		GLCall(glUseProgram(triShader));
		set_uniform_variable(triShader, (char*)"uColor", glm::vec4(0.8f, 0.3f, 0.8f, 1.0f));
		GLCall(glUseProgram(0));


		// clear 
		va.Unbind();
		GLCall(glUseProgram(0));
		vb.Unbind();
		ib.Unbind();
		//GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		//GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

		// for animate
		float r = 0.0f;
		float increment = 0.05;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);

			GLCall(glUseProgram(triShader)); // bind shader
			set_uniform_variable(triShader, (char*)"uColor", glm::vec4(r, 0.3f, 0.8f, 1.0f));

			//GLCall(glBindVertexArray(vao));
			va.Bind();
			ib.Bind();
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0)); // works with nullptr in last argument...because the index buffer is bound?
			if (r > 1.0f) increment = -0.05f;
			else if (r < 0.0f) increment = 0.05f;
			r += increment;
			GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
			GLCall(glUseProgram(0));

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}

    glfwTerminate();
    return 0;
}

