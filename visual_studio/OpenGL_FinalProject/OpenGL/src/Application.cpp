#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <iostream>
#include <format>
#include <string>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "glsl_reader.h"
#include "Texture.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

// multiplication factors for input interaction:
const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;
const float MINSCALE = 0.05f;
const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;
const int LEFT   = 0; // per https://www.glfw.org/docs/3.3/group__buttons.html
const int RIGHT  = 1;
const int MIDDLE = 2;

// non-constant global variables:
int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		NowProjection;		    // ORTHO or PERSP
float	Scale = 1.f;			// scaling factor
float	Xrot, Yrot;				// rotation angles in degrees
double Xmouse, Ymouse;
int PressedButton = -1;

// for mvp
glm::vec3 eye(0.5, 3., 3.f);
glm::vec3 look(0., 0., 0.);
glm::vec3 up(0., 1., 0.);
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normal;

// for animate
float r = 0.0f;
float increment = 0.05f;
double cur_time_sec; 
double cur_time_ms; 

// for imgui
bool show_demo_window = true;
bool show_another_window = true;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
glm::vec3 translation(0., 0., 0.);
glm::vec3 scaler(1., 1., 1.);
float rotater = 0;
float framerate;


// FOR PLATE SHADER
glm::vec3 uPlateColor = glm::vec3(0.f);
float uPlateDim = 4.f;
// FOR SAND COMPUTE/RENDER SHADER
float uCubeSize =  0.005f;
//float uGravityMetersPerSec = -.3f;
float uBounceFactor = 0.235f;
float uChladniResAmp = 1.0f;
float lastChladniResAmp = uChladniResAmp;
float uPlateNormalScale = 0.1f;
bool uUseChladniNormals = true;
bool uUseChladniNormalsForLighting = false;
float uGravityMetersPerSec = -9.8f;
float uSpawnHeight =  0.75f;
float uDeathHeight = -10.5f;
float uPlateHeight =  0.0f;
int uChladni_N = 1;
int uChladni_M = 2;
float uChladni_DX = 0;
float uChladni_DZ = 0;
bool uQuickRespawn = false;
bool uPulseMode = false;
GLuint SandShaderProgram;
GLuint ComputeSandShaderProgram;
int num_parts_dim = 128; // 128
int NUM_PARTICLES = num_parts_dim * num_parts_dim * num_parts_dim;   // total number of particles to move
#define WORK_GROUP_SIZE 128 // # work-items per work-group
struct position { float x,  y,  z,  w; };
struct velocity { float vx, vy, vz, vw; };
struct rotation { float rx, ry, rz, rw; };
struct rotationSpeed { float s; };
struct color    { float r,  g,  b,  a; };
GLuint posSSbo;
GLuint velSSbo;
GLuint rotSSbo;
GLuint rotSpeedSSbo;
GLuint colSSbo;
void initSand();


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void update_viewport_size(GLFWwindow* window);
void imgui_show_example_frame();
void imgui_show_sand_frame();
glm::mat4 mv() { return view * model; }
glm::mat4 mvp() { return projection * view * model; }
glm::mat3 n(glm::mat4 mat) { return glm::mat3(glm::inverseTranspose(mat)); }

// for axes
void initAxes();
unsigned int axes_vao;
unsigned int axes_vbo;
unsigned int axes_ibo;

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
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1200, 1200, "Chladni Plate Simulation", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    //glfwSwapInterval(1); // synchronize framerate to vsync/framerate
    glfwSwapInterval(0); // no vsync

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
	
	// Mouse Interactions
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

    GLenum err = glewInit();
    if (err != GLEW_OK) 
        std::cout << "Error: %s\n" << glewGetErrorString(err) << std::endl;
	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "Status: Using OpenGL " << glGetString(GL_VERSION) << std::endl;


	{ // this block is to avoid infinite loop of glCheckError which provides an error when there is no gl context. stops infinite loop when we close the openggl window. 
		// texture blending
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		GLCall(glBlendEquation(GL_FUNC_ADD)); // default, but being explicit

		initSand();
		initAxes();

		/* SETUP PLATE */
		float plate_positions[] = {
			-0.5f, 0.0f, -0.5f,   0.0f, 0.0f,   0.0f, 1.0f, 0.0f, // 0 vertex.xyz, tex.st, normal.xyz
			 0.5f, 0.0f, -0.5f,   1.0f, 0.0f,   0.0f, 1.0f, 0.0f, // 1
			 0.5f, 0.0f,  0.5f,   1.0f, 1.0f,   0.0f, 1.0f, 0.0f, // 2
			-0.5f, 0.0f,  0.5f,   0.0f, 1.0f,   0.0f, 1.0f, 0.0f, // 3
		};
		// indices of the vertices to be drawn
		unsigned int plate_indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		// generate vao
		VertexArray plate_va;
		VertexBuffer plate_vb(plate_positions, 4 * 8 * sizeof(float));
		VertexBufferLayout plate_layout;
		plate_layout.Push<float>(3); // vertex position, 3 floats
		plate_layout.Push<float>(2); // vertex texture coords, 2 floats
		plate_layout.Push<float>(3); // vertex normal coords, 3 floats
		plate_va.AddBuffer(plate_vb, plate_layout);
		plate_vb.Unbind();

		IndexBuffer plate_ib(plate_indices, 2 * 3 * sizeof(unsigned int));
		plate_ib.Unbind();

		// clear 
		plate_va.Unbind();
		plate_vb.Unbind();
		plate_ib.Unbind();


		/* SETUP SHADERS */
		Shader axes_shader = Shader("res/shaders/Axes.glsl");
		Shader plate_shader = Shader("res/shaders/Plate.glsl");

		Renderer renderer;

		/* SETUP GUI */
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		ImGui::StyleColorsDark(); // Set Dark Style
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460");

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();
			cur_time_sec = glfwGetTime();
			cur_time_ms = 1000.f * cur_time_sec;

			// DEPTH BUFFER
			GLCall(glDrawBuffer(GL_BACK));
			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			GLCall(glEnable(GL_DEPTH_TEST));
			glClearColor(0.15f, 0.15f, 0.35f, 1.0f);
			
			update_viewport_size(window); // update viewport size after scaling window

			//img
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			framerate = io.Framerate;
			//imgui_show_example_frame();
			imgui_show_sand_frame();

			// rotate and uniformly scale scene
			model = glm::mat4(1);
			view = glm::lookAt(eye, look, up);
			//projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1000.f);
			projection = glm::perspective(glm::radians(90.f), 1.f, 0.1f, 1000.f);
			view = glm::rotate(view, glm::radians(Yrot), glm::vec3(0., 1., 0.));
			view = glm::rotate(view, glm::radians(Xrot), glm::vec3(1., 0., 0.));
			view = glm::scale(view, glm::vec3(Scale, Scale, Scale));

			// DRAW PLATE
			plate_shader.Bind();
			//test_shader.SetUniformVec4((char*)"uColor", glm::vec4(r, 0.3f, 0.8f, 1.0f));
			model = glm::scale(glm::mat4(1), glm::vec3(uPlateDim));
			plate_shader.SetUniformVec3("uPlateColor", uPlateColor);
			plate_shader.SetUniform1f("uPlateDim", uPlateDim);
			plate_shader.SetUniform1i("uChladni_N", uChladni_N);
			plate_shader.SetUniform1i("uChladni_M", uChladni_M);
			plate_shader.SetUniform1f("uChladni_DX", uChladni_DX);
			plate_shader.SetUniform1f("uChladni_DZ", uChladni_DZ);
			plate_shader.SetUniform1f("uPlateNormalScale", uPlateNormalScale);
			plate_shader.SetUniform1i("uUseChladniNormalsForLighting", uUseChladniNormalsForLighting);
			plate_shader.SetUniform1f("uChladniResAmp", uChladniResAmp);
			plate_shader.SetUniformMat4("uMVP", mvp());
			plate_shader.SetUniformMat4("uMV", mv());
			plate_shader.SetUniformMat3("uN", n(mv()));
			renderer.Draw(plate_va, plate_ib, plate_shader);

			// DRAW AXES
			axes_shader.Bind();
			model = glm::mat4(1);
			axes_shader.SetUniformMat4("uMVP", mvp());
			GLCall(glBindVertexArray(axes_vao));
			GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, axes_ibo));
			GLCall(glLineWidth(3));
			GLCall(glEnable(GL_LINE_SMOOTH)); // enable antialiasing
			GLCall(glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, 0));
			GLCall(glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_INT, (const GLvoid*) (5 * sizeof(unsigned int)))); 
			GLCall(glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_INT, (const GLvoid*) (7 * sizeof(unsigned int)))); 
			GLCall(glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_INT, (const GLvoid*) (9 * sizeof(unsigned int)))); 
			GLCall(glDrawElements(GL_LINE_STRIP, 4, GL_UNSIGNED_INT, (const GLvoid*) (14 * sizeof(unsigned int)))); 
			GLCall(glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_INT, (const GLvoid*) (18 * sizeof(unsigned int)))); 
			GLCall(glLineWidth(1));
			GLCall(glDisable(GL_LINE_SMOOTH)); // disable antialiasing
			GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

			// COMPUTE SAND
			glEnable( GL_NORMALIZE ); // TODO do i need this? 

			glUseProgram( ComputeSandShaderProgram );
			model = glm::mat4(1);
			normal = glm::mat3(glm::inverseTranspose(model));
			set_uniform_variable(ComputeSandShaderProgram, "uDeltaTime", 1.f / framerate); 
			set_uniform_variable(ComputeSandShaderProgram, "uGravityMetersPerSec", uGravityMetersPerSec);
			set_uniform_variable(ComputeSandShaderProgram, "uSpawnHeight", uSpawnHeight);
			set_uniform_variable(ComputeSandShaderProgram, "uDeathHeight", uDeathHeight);
			set_uniform_variable(ComputeSandShaderProgram, "uPlateDim", uPlateDim);
			set_uniform_variable(ComputeSandShaderProgram, "uBounceFactor", uBounceFactor);
			set_uniform_variable(ComputeSandShaderProgram, "uChladniResAmp", uChladniResAmp);
			set_uniform_variable(ComputeSandShaderProgram, "uChladni_N", uChladni_N);
			set_uniform_variable(ComputeSandShaderProgram, "uChladni_M", uChladni_M);
			set_uniform_variable(ComputeSandShaderProgram, "uChladni_DX", uChladni_DX);
			set_uniform_variable(ComputeSandShaderProgram, "uChladni_DZ", uChladni_DZ);
			set_uniform_variable(ComputeSandShaderProgram, "uQuickRespawn", uQuickRespawn);
			set_uniform_variable(ComputeSandShaderProgram, "uUseChladniNormals", uUseChladniNormals);
			uQuickRespawn = false; // turn off!
			if (uPulseMode) {
				if (uChladniResAmp != 0.f) lastChladniResAmp = uChladniResAmp;
				uChladniResAmp = 0.f;
			}
			else {
				uChladniResAmp = lastChladniResAmp;
			}
			glDispatchCompute( NUM_PARTICLES  / WORK_GROUP_SIZE, 1,  1 );
			glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

			// DRAW SAND
			glUseProgram(SandShaderProgram);
			set_uniform_variable( SandShaderProgram, "uMV",       mv());
			set_uniform_variable( SandShaderProgram, "uMVP",      mvp());
			set_uniform_variable( SandShaderProgram, "uCubeSize", uCubeSize);
			glBindBuffer( GL_ARRAY_BUFFER, posSSbo );
			glVertexPointer( 4, GL_FLOAT, 0, (void *)0 );
			glEnableClientState( GL_VERTEX_ARRAY );
			glDrawArrays( GL_POINTS, 0, NUM_PARTICLES );
			glDisableClientState( GL_VERTEX_ARRAY );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glUseProgram(0);

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


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) 
{
	if( action == GLFW_PRESS ) {
		glfwGetCursorPos(window, &Xmouse, &Ymouse);
		PressedButton = button;
	}
	else PressedButton = -1;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	int direction = yoffset / abs(yoffset);
	Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR * direction;
	Scale = std::max(Scale, MINSCALE);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) 
{
	double dx = xpos - Xmouse;
	double dy = ypos - Ymouse;

	if ( PressedButton == GLFW_MOUSE_BUTTON_RIGHT ) 
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	else if( PressedButton == GLFW_MOUSE_BUTTON_MIDDLE ) 
	{
		//std::cout << "scaler" << std::endl;
		Scale += SCLFACT * (float) ( dx - dy );
		Scale = std::max(Scale, MINSCALE);
		//std::cout << "scale: " << Scale << std::endl;
	}
	Xmouse = xpos;			// new current position
	Ymouse = ypos;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		if (key == GLFW_KEY_D && action == GLFW_PRESS) { uChladni_DX += 1; }
		else if (key == GLFW_KEY_A && action == GLFW_PRESS) { uChladni_DX -= 1; }
		else if (key == GLFW_KEY_W && action == GLFW_PRESS) { uChladni_DZ += 1; }
		else if (key == GLFW_KEY_S && action == GLFW_PRESS) { uChladni_DZ -= 1; }
	}
	else if (key == GLFW_KEY_D && action == GLFW_PRESS) { uChladni_N += 1; }
	else if (key == GLFW_KEY_A && action == GLFW_PRESS) { uChladni_N -= 1; }
	else if (key == GLFW_KEY_W && action == GLFW_PRESS) { uChladni_M += 1; }
	else if (key == GLFW_KEY_S && action == GLFW_PRESS) { uChladni_M -= 1; }
	else if (key == GLFW_KEY_R && action == GLFW_PRESS) { uQuickRespawn = true; }
	else if (key == GLFW_KEY_Z && action == GLFW_PRESS) { uChladni_M = rand()%20, uChladni_N = rand()%20; }
}

void update_viewport_size(GLFWwindow* window)
{
	GLsizei vx, vy;
	glfwGetWindowSize(window, &vx, &vy);
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = (vx - v) / 2;
	GLint yb = (vy - v) / 2;
	glViewport(xl, yb, v, v);
}

std::string intToStrComma(int value1, int value2) {
    // Use std::to_string to convert integers to strings and concatenate them
    return std::to_string(value1) + "/" + std::to_string(value2);
}

void imgui_show_sand_frame()
{
	//static float f = 0.0f;
	//static int counter = 0;
	ImGui::Begin("Sand");                          // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat("uPlateNormalScale", &uPlateNormalScale, 0.001f, 1.f);
	ImGui::SliderFloat("uCubeSize", &uCubeSize, 0.001f, 0.2f);
	ImGui::SliderFloat("uGravityMetersPerSec", &uGravityMetersPerSec, -10.f, 10.f);
	ImGui::SliderFloat("uSpawnHeight", &uSpawnHeight, -1.f, 10.f);
	ImGui::SliderFloat("uDeathHeight", &uDeathHeight, -5.f, 5.f);
	ImGui::SliderFloat3("uPlateColor", &uPlateColor.x, 0.f, 1.f);
	ImGui::SliderFloat("uPlateDim", &uPlateDim, 0.0f, 4.f);
	//ImGui::SliderFloat("uBounceFactor", &uBounceFactor, 0.0f, 1.f);
	ImGui::SliderFloat("uChladniResAmp", &uChladniResAmp, 0.0f, 5.f);
	if (ImGui::IsItemActive()) {
		lastChladniResAmp = uChladniResAmp;
	}
	ImGui::SliderInt("uChladni_N", &uChladni_N, 0, 40);
	ImGui::SliderInt("uChladni_M", &uChladni_M, 0, 40);
	ImGui::SliderFloat("uChladni_DX", &uChladni_DX, -2.f, 2.f);
	ImGui::SliderFloat("uChladni_DZ", &uChladni_DZ, -2.f, 2.f);
	ImGui::Checkbox("uUseChladniNormalsForLighting", &uUseChladniNormalsForLighting);
	ImGui::Checkbox("uUseChladniNormals", &uUseChladniNormals);
	if (ImGui::Button("uQuickRespawn"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		uQuickRespawn = true;
	ImGui::Checkbox("uSignalMode", &uPulseMode);

	bool doRespawn = false;
	for (int n = 1; n < 10; n++) {
		ImGui::Button(intToStrComma(n, 1).c_str());
		if (ImGui::IsItemActive()) {
			uChladni_N = n; 
			uChladni_M = 1; 
			uQuickRespawn = doRespawn; 
			uChladniResAmp = lastChladniResAmp;
		}
		for (int m = 2; m < 10; m++) {
			ImGui::SameLine(); 
			ImGui::Button(intToStrComma(n, m).c_str());
			if (ImGui::IsItemActive()) {
				uChladni_N = n; 
				uChladni_M = m; 
				uQuickRespawn = doRespawn; 
				uChladniResAmp = lastChladniResAmp;
			}
		}
	}

	if (ImGui::Button("Random (n, m)"))
	{ 
		uChladni_N = rand()%20; 
		uChladni_M = rand()%20; 
		uQuickRespawn = doRespawn; 
	}
	//ImGui::SliderFloat3("Scale", &scaler.x, -2.f, 2.f);
	//ImGui::SliderFloat("Rotation", &rotater, -200.f, 200.f);

	//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//ImGui::Checkbox("Another Window", &show_another_window);

	//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	//	counter++;
	//ImGui::SameLine();
	//ImGui::Text("counter = %d", counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
	ImGui::End();

	//if (show_another_window)
	//{
	//	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	//	ImGui::Text("Hello from another window!");
	//	if (ImGui::Button("Close Me"))
	//		show_another_window = false;
	//	ImGui::End();
	//}
}
void imgui_show_example_frame()
{
	static float f = 0.0f;
	static int counter = 0;
	ImGui::Begin("Chladni Plate Simulation");                          // Create a window called "Hello, world!" and append into it.
	ImGui::SliderFloat3("Translation", &translation.x, -1.f, 1.f);
	ImGui::SliderFloat3("Scale", &scaler.x, -2.f, 2.f);
	ImGui::SliderFloat("Rotation", &rotater, -200.f, 200.f);

	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	ImGui::Checkbox("Another Window", &show_another_window);

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
	ImGui::End();
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}

void initSand() {
	SandShaderProgram = glCreateProgram( );
	read_compile_link_validate_shader(SandShaderProgram, "res/shaders/Sand/Sand.vert", "vertex");
    read_compile_link_validate_shader(SandShaderProgram, "res/shaders/Sand/Sand.geom", "geometry");
	read_compile_link_validate_shader(SandShaderProgram, "res/shaders/Sand/Sand.frag", "fragment");
	
	ComputeSandShaderProgram = glCreateProgram( );
	read_compile_link_validate_shader(ComputeSandShaderProgram, "res/shaders/Sand/Sand.comp", "compute");

	//SETUP SHADER STORAGE BUFFERS OBJECTS FOR COMPUTE SHADER (ssbo)
	glGenBuffers(1, &posSSbo);
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, posSSbo );
	glBufferData( GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(struct position), NULL, GL_STATIC_DRAW );
	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT ; // the invalidate makes a big difference when re-writing
	struct position *points = (struct position *) glMapBufferRange( GL_SHADER_STORAGE_BUFFER, 0, NUM_PARTICLES * sizeof(struct position), bufMask );
	for (int i = 0; i < NUM_PARTICLES; i++) {
		points[i].x = ((rand() % 10000) / 10000.f - 0.5f) * uPlateDim; // -1. to 1 
		points[i].y = ((rand() % 10000) / 10000.f - 0.5f) * 2 + 1;
		points[i].z = ((rand() % 10000) / 10000.f - 0.5f) * uPlateDim;
		points[i].w = 1;
	}
	glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );

	glGenBuffers( 1, &velSSbo);
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, velSSbo );
	glBufferData( GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(struct velocity), NULL, GL_STATIC_DRAW );
	struct velocity *velocities = (struct velocity *) glMapBufferRange( GL_SHADER_STORAGE_BUFFER, 0, NUM_PARTICLES * sizeof(struct velocity), bufMask );
	for( int i = 0; i < NUM_PARTICLES; i++ ) {
		//velocities[i].vx = ((rand() % 100) / 50.f - 1.) / 50.f; // -0.5 to 0.5 / 50
		//velocities[i].vy = ((rand() % 100) / 50.f - 1.) / 100.f;
		//velocities[i].vz = ((rand() % 100) / 50.f - 1.) / 50.f;
		//velocities[i].vw = 1.;
		velocities[i].vx = 0;
		velocities[i].vy = 0;
		velocities[i].vz = 0;
		velocities[i].vw = 1.;
	}
	glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );

	glGenBuffers( 1, &rotSSbo);
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, rotSSbo );
	glBufferData( GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(struct rotation), NULL, GL_STATIC_DRAW );
	struct rotation *rotations = (struct rotation *) glMapBufferRange( GL_SHADER_STORAGE_BUFFER, 0, NUM_PARTICLES * sizeof(struct rotation), bufMask );
	for( int i = 0; i < NUM_PARTICLES; i++ ) {
		rotations[ i ].rx = ((rand() % 100) / 100.f); // 0 to 1 // xyz is axis of rotations
		rotations[ i ].ry = ((rand() % 100) / 100.f);
		rotations[ i ].rz = ((rand() % 100) / 100.f);
		rotations[ i ].rw = (((rand() % 100) / 50.f) - 1) * 6.28; // -2pi to 2pi // can change to 3.14??
		//rotations[i].rw = 0;// -2pi to 2pi // can change to 3.14??
	}
	glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );

	glGenBuffers( 1, &rotSpeedSSbo);
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, rotSpeedSSbo );
	glBufferData( GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(struct rotationSpeed), NULL, GL_STATIC_DRAW );
	struct rotationSpeed *rotationSpeeds = (struct rotationSpeed *) glMapBufferRange( GL_SHADER_STORAGE_BUFFER, 0, NUM_PARTICLES * sizeof(struct rotationSpeed), bufMask );
	for( int i = 0; i < NUM_PARTICLES; i++ ) {
		float amp = ((rand() % 100) / 100.f) - 0.5; // -1 to -0.5 || 0.5 to 1.0
		if (amp < 0)
			amp -= .5;
		else
			amp += .5;
		rotationSpeeds[i].s = amp;
		//rotationSpeeds[i].s = 0;
	}
	glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );


	glGenBuffers( 1, &colSSbo);
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, colSSbo );
	glBufferData( GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(struct color), NULL, GL_STATIC_DRAW );
	struct color *colors = (struct color *) glMapBufferRange( GL_SHADER_STORAGE_BUFFER, 0, NUM_PARTICLES * sizeof(struct color), bufMask );
	for( int i = 0; i < NUM_PARTICLES; i++ ) {
		// RainbowColors
		//colors[ i ].r = ((rand() % 100)) / 100.f;
		//colors[ i ].g = ((rand() % 100)) / 100.f;
		//colors[ i ].b = ((rand() % 100)) / 100.f;
		//colors[ i ].a = 1.;
		//SAND COLORS
		colors[ i ].r = (236.f + (rand()%40 -10.f)) / 255.f;
		colors[ i ].g = (204.f + (rand()%40 -10.f)) / 255.f;
		colors[ i ].b = (162.f + (rand()%40 -10.f)) / 255.f;
		colors[ i ].a = 1.;
	}
	glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, posSSbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, velSSbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, rotSSbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, rotSpeedSSbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, colSSbo);
}

void initAxes() {
	/* SETUP AXES */
	float axes_positions[] = {
		 1.0f, 0.0f, 0.0f, // lines
		 0.0f, 0.0f, 0.0f,
		 0.0f, 1.0f, 0.0f,
		 0.0f, 0.0f, 0.0f,
		 0.0f, 0.0f, 1.0f,

		 1.1f, -0.05f, 0.0f, // X
		 1.2f,  0.05f, 0.0f,
		 1.1f,  0.05f, 0.0f,
		 1.2f, -0.05f, 0.0f,

		 0.0f,  1.1f,  0.0f, // Y
		 0.0f,  1.16f, 0.0f,
		-0.05f, 1.2f,  0.0f,
		 0.0f,  1.16f, 0.0f,
		 0.05f, 1.2f,  0.0f,

		 0.0f,  0.05f, 1.2f, // Z
		 0.0f,  0.05f, 1.1f,
		 0.0f, -0.05f, 1.2f,
		 0.0f, -0.05f, 1.1f,
		 0.0f,  0.0f,  1.125f,
		 0.0f,  0.0f,  1.175f,
	};
	unsigned int axes_indices[] = {
		0, 1, 2, 3, 4,         // lines
		5, 6, 7, 8,            // X
		9, 10, 11, 12, 13,     // Y
		14, 15, 16, 17, 18, 19 // Z	
	};

	//unsigned int axes_vao;
	GLCall(glGenVertexArrays(1, &axes_vao));
	GLCall(glBindVertexArray(axes_vao));

	//unsigned int axes_vbo;
	GLCall(glGenBuffers(1, &axes_vbo));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, axes_vbo));
	GLCall(glBufferData(GL_ARRAY_BUFFER, 3 * 20 * sizeof(float), axes_positions, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0)); // sets to currently bound vao
	//GLCall(glEnableVertexArrayAttrib(axes_vao, 0)); // should do the same thing? 
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float), 0))
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

	//unsigned int axes_ibo;
	GLCall(glGenBuffers(1, &axes_ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, axes_ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 20 * sizeof(unsigned int), axes_indices, GL_STATIC_DRAW));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
