#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <iostream>

//#include "glsl_reader.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"


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
    //glfwSwapInterval(1); // synchronize framerate to vsync/framerate
    glfwSwapInterval(0); // no vsync

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
			-0.5f, -0.5f, 0.0f, 0.0f, // 0
			 0.5f, -0.5f, 1.0f, 0.0f, // 1
			 0.5f,  0.5f, 1.0f, 1.0f, // 2
			-0.5f,  0.5f, 0.0f, 1.0f  // 3
		};
		// indices of the vertices to be drawn
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		GLCall(glBlendEquation(GL_FUNC_ADD)); // default, but being explicit

		// generate vao
		VertexArray va;
		VertexBuffer vb(positions, 4 * 4 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2); // vertex position, 2 floats
		layout.Push<float>(2); // vertex texture coords, 2 floats
		va.AddBuffer(vb, layout);
		vb.Unbind();

		IndexBuffer ib(indices, 2 * 3 * sizeof(unsigned int));
		ib.Unbind();

		glm::mat4 model = glm::mat4(1);
		//model = translate(model, glm::vec3(0, 0.5, 0));
		glm::mat4 view = glm::mat4(1);
		//view = glm::translate(view, glm::vec3(-0.5, 0, 0));
		//glm::vec3 eye(0., 0., 3.);
		//glm::vec3 look(0., 0., 0.);
		//glm::vec3 up(0., 1., 0.);
		//view = glm::lookAt(eye, look, up);

		glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
		glm::mat4 mv = view * model;
		glm::mat4 mvp = projection * view * model;
		glm::mat3 normal = glm::mat3(glm::inverseTranspose(mv));
	

		Shader shader = Shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniformVec4("uColor", glm::vec4(0.8f, 0.3f, 0.8f, 1.0f));
		shader.Unbind();
		
		Texture texture("res/textures/dice.png");
		texture.Bind(0);
		shader.Bind();
		shader.SetUniform1i("uTexture", 0);
		shader.SetUniformMat4("uMVP", mvp);
		// clear 
		va.Unbind();
		shader.Unbind();
		vb.Unbind();
		ib.Unbind();

		Renderer renderer;

		// gui setup
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		ImGui::StyleColorsDark(); // Set Dark Style
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460");
		// Our state
		bool show_demo_window = true;
		bool show_another_window = true;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


		// for animate
		float r = 0.0f;
		float increment = 0.05f;
		glm::vec3 translation(0., 0., 0.);
		glm::vec3 scaler(1., 1., 1.);

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();

			//img
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			{
				static float f = 0.0f;
				static int counter = 0;
				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
				ImGui::SliderFloat3("Translation", &translation.x, -1.f, 1.f);
				ImGui::SliderFloat3("Scale", &scaler.x, -2.f, 2.f);

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Another Window", &show_another_window);

				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				ImGui::End();
			}
			if (show_another_window)
			{
				ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
				ImGui::Text("Hello from another window!");
				if (ImGui::Button("Close Me"))
					show_another_window = false;
				ImGui::End();
			}


			shader.Bind();
			model = glm::translate(glm::mat4(1), translation);
			model = glm::scale(model, scaler);
			view = glm::mat4(1);
			mvp = projection * view * model;
			shader.SetUniformVec4((char*)"uColor", glm::vec4(r, 0.3f, 0.8f, 1.0f));
			shader.SetUniformMat4("uMVP", mvp);

			renderer.Draw(va, ib, shader);
			if (r > 1.0f) increment = -0.05f;
			else if (r < 0.0f) increment = 0.05f;
			r += increment;

			// img 
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}

